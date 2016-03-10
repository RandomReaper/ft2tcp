
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>

#include <libftdi1/ftdi.h>
#include "ft245_duplex.h"

static void fatal(const char *msg, const char *file, int line)
{
	fprintf(stderr, "%s failed at %s:%d\n", msg, file, line);
}

static void LIBUSB_CALL ftdi_duplex_read_cb(struct libusb_transfer *transfer)
{
	int i;
	struct ftdi_duplex_context *duplex = transfer->user_data;
	int packet_size = duplex->pkt_size;
	uint8_t *ptr = transfer->buffer;
	int len = transfer->actual_length;
	int num_pkt = (len + packet_size - 1) / packet_size;

	if (!transfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
		duplex->status = -1;
		return;
	}

	for (i = 0; i < num_pkt; i++)
	{
		int pkt_len = len;

		if (pkt_len > packet_size)
		{
			pkt_len = packet_size;
		}

		duplex->stop |= duplex->rx_cb(ptr + 2, pkt_len - 2, duplex->cb_data);

		ptr += pkt_len;
		len -= pkt_len;
	}

	if (duplex->stop)
	{
		free(transfer->buffer);
		libusb_free_transfer(transfer);
		return;
	}

	transfer->status = -1;
	duplex->status = libusb_submit_transfer(transfer);
}

static void LIBUSB_CALL ftdi_duplex_write_cb(struct libusb_transfer *transfer)
{
	if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
		return;
	}
	else
	{
		fatal("write xfer failed", __FILE__, __LINE__);
		return;
	}
}

int ftdi_duplex_write(struct ftdi_duplex_context *duplex, const char *_data, int len)
{
	int status;
	struct libusb_transfer *transfer;
	struct ftdi_context *ftdi = duplex->ftdi;

	unsigned char *data = malloc(len);
	if (!data)
	{
		return -1;
	}

	memcpy(data, _data, len);

	transfer = libusb_alloc_transfer(0);
	if (!transfer)
	{
		free(data);
		return -1;
	}

	transfer->flags = LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER;

	libusb_fill_bulk_transfer(transfer, ftdi->usb_dev, ftdi->in_ep,
							  data, len,
							  ftdi_duplex_write_cb,
							  duplex, 0);

	transfer->status = -1;
	status = libusb_submit_transfer(transfer);
	if (status)
	{
		libusb_free_transfer(transfer); // Expected to free data
		return status;
	}

	return 0;
}

static int ft245_sync_compatible(struct ftdi_context *ftdi)
{
	int type;

	if ((ftdi->type != TYPE_2232H) && (ftdi->type != TYPE_232H))
	{
		fatal("Device does not support", __FILE__, __LINE__);
		return -1;
	}

	if (ftdi_read_eeprom(ftdi) < 0)
	{
		fatal("ftdi_read_eeprom", __FILE__, __LINE__);
		return -1;
	}

	if (ftdi_eeprom_decode(ftdi, 0) < 0)
	{
		fatal("ftdi_eeprom_decode", __FILE__, __LINE__);
		return -1;
	}

	if (ftdi_get_eeprom_value(ftdi, CHANNEL_A_TYPE, &type) < 0)
	{
		fatal("ftdi_get_eeprom_value", __FILE__, __LINE__);
		return -1;
	}

	if (type != CHANNEL_IS_FIFO)
	{
		fatal("type != CHANNEL_IS_FIFO, go fix your eeprom for 245FIFO mode", __FILE__, __LINE__);
		return -1;
	}

	return 0;
}

struct ftdi_duplex_context *ftdi_duplex_start(struct ftdi_context *ftdi,
				ftdi_duplex_rx_cb *rx_cb, void *cb_data,
				int pkt_per_xfer, int nr_rx)
{
	struct ftdi_duplex_context *duplex = NULL;
	int i;
	int status;

	if ((status = ft245_sync_compatible(ftdi)) < 0)
	{
		fatal("ft245_sync_compatible", __FILE__, __LINE__);
		goto error;
	}

	/* Setup ftdi */
	if ((status = ftdi_usb_purge_rx_buffer(ftdi)) < 0)
	{
		fatal("ftdi_usb_purge_rx_buffer", __FILE__, __LINE__);
		goto error;
	}

	if ((status = ftdi_set_latency_timer(ftdi, 2)) < 0)
	{
		fatal("ftdi_set_latency_timer", __FILE__, __LINE__);
		goto error;
	}

	if ((status = ftdi_set_bitmode(ftdi,  0xff, BITMODE_RESET)) < 0)
	{
		fatal("ftdi_set_bitmode", __FILE__, __LINE__);
		goto error;
	}

	if ((status = ftdi_usb_purge_buffers(ftdi)) < 0)
	{
		fatal("ftdi_usb_purge_buffers", __FILE__, __LINE__);
		goto error;
	}

	duplex = malloc(sizeof(*duplex));
	if (!duplex)
	{
		fatal("malloc", __FILE__, __LINE__);
		return NULL;
	}

	memset(duplex, 0x0, sizeof(*duplex));

	duplex->ftdi = ftdi;
	duplex->rx_cb = rx_cb;
	duplex->cb_data = cb_data;
	duplex->pkt_size = ftdi->max_packet_size;
	duplex->nr_rx = nr_rx;

	duplex->rx_xfers = calloc(nr_rx, sizeof *duplex->rx_xfers);
	if (!duplex->rx_xfers)
	{
		fatal("calloc", __FILE__, __LINE__);

		status = -1;
		goto error;
	}

	for (i = 0; i < nr_rx; i++)
	{
		duplex->rx_xfers[i] = libusb_alloc_transfer(0);
		if (!duplex->rx_xfers[i])
		{
			fatal("libusb_alloc_transfer", __FILE__, __LINE__);
			status = -1;
			goto error;
		}

		libusb_fill_bulk_transfer(duplex->rx_xfers[i], ftdi->usb_dev, ftdi->out_ep,
								  malloc(pkt_per_xfer * ftdi->max_packet_size),
								  pkt_per_xfer * ftdi->max_packet_size,
								  ftdi_duplex_read_cb,
								  duplex, 0);

		if (!duplex->rx_xfers[i]->buffer)
		{
			fatal("malloc", __FILE__, __LINE__);
			status = -1;
			goto error;
		}

		duplex->rx_xfers[i]->status = -1;
		if ((status = libusb_submit_transfer(duplex->rx_xfers[i])) < 0)
		{
			fatal("libusb_submit_transfer", __FILE__, __LINE__);
			goto error;
		}
	}

	if ((status = ftdi_set_bitmode(ftdi,  0xff, BITMODE_SYNCFF)) < 0)
	{
		fatal("ftdi_set_bitmode", __FILE__, __LINE__);
		goto error;
	}

	return duplex;

error:
	if (duplex)
	{
		if (duplex->rx_xfers)
		{
			for (i = 0 ; i < duplex->nr_rx; i++)
			{
				if (duplex->rx_xfers[i])
				{
					libusb_free_transfer(duplex->rx_xfers[i]);
				}
			}
			free(duplex->rx_xfers);
		}
		free(duplex);
	}
	return NULL;
}

int ftdi_duplex_poll(struct ftdi_duplex_context *duplex)
{
	struct timeval timeout =
	{
		0,
		duplex->ftdi->usb_read_timeout,
	};

	int status;

	do
	{
		status = libusb_handle_events_timeout_completed(duplex->ftdi->usb_ctx, &timeout, NULL);
	} while (status == LIBUSB_ERROR_INTERRUPTED);

	if (duplex->status != LIBUSB_SUCCESS)
	{
		fatal("something failed in the callback", __FILE__, __LINE__);
		return duplex->status;
	}

	if (status == LIBUSB_SUCCESS)
	{
		duplex->status = 0;
		return 0;
	}

	if (status == LIBUSB_ERROR_TIMEOUT)
	{
		duplex->status = 0;
		return 0;
	}

	duplex->status = 0;
	return duplex->status;
}

int ftdi_duplex_stop(struct ftdi_duplex_context *duplex)
{
	struct ftdi_context *ftdi = duplex->ftdi;
	int i;

	if (!duplex)
	{
		return -1;
	}

	if (ftdi_set_bitmode(ftdi,  0xff, BITMODE_RESET) < 0)
	{
		fprintf(stderr,"Can't reset mode\n");
		return -1;
	}

	for (i = 0; i < duplex->nr_rx; i++)
	{
		struct libusb_transfer *transfer = duplex->rx_xfers[i];
		libusb_cancel_transfer(transfer);
	}

	// FIXME : wait until all in an out transfer are done

	for (i = 0; i < duplex->nr_rx; i++)
	{
		struct libusb_transfer *transfer = duplex->rx_xfers[i];
		free(transfer->buffer);
		libusb_free_transfer(transfer);
	}

	free(duplex);

	return 0;
}
