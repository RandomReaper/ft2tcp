/***************************************************************************
                          ftdi_duplex.c  -  description
                             -------------------
    copyright            : (C) 2009 Micah Dowty 2010 Uwe Bonnes
							   2015 Marc Pignat
    email                : opensource@intra2net.com
 ***************************************************************************/

/* Adapted from libftdi ftdi_stream.c*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation;             *
 *                                                                         *
 ***************************************************************************/

/* Adapted from
 * fastftdi.c - A minimal FTDI FT232H interface for which supports bit-bang
 *              mode, but focuses on very high-performance support for
 *              synchronous FIFO mode. Requires libusb-1.0
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>

#include <libftdi1/ftdi.h>
#include "ft245_duplex.h"

static void LIBUSB_CALL
ftdi_duplex_read_cb(struct libusb_transfer *transfer)
{
	struct ftdi_duplex_context *duplex = transfer->user_data;
	int packet_size = duplex->packetsize;

	duplex->activity = 1;
	if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
		int i;
		uint8_t *ptr = transfer->buffer;
		int length = transfer->actual_length;
		int numPackets = (length + packet_size - 1) / packet_size;
		int res = 0;

		for (i = 0; i < numPackets; i++)
		{
			int payloadLen;
			int packetLen = length;

			if (packetLen > packet_size)
				packetLen = packet_size;

			payloadLen = packetLen - 2;

			res = duplex->callback(ptr + 2, payloadLen, duplex->userdata);

			ptr += packetLen;
			length -= packetLen;
		}
		if (res)
		{
			free(transfer->buffer);
			libusb_free_transfer(transfer);
		}
		else
		{
			transfer->status = -1;
			duplex->result = libusb_submit_transfer(transfer);
		}
	}
	else
	{
		fprintf(stderr, "unknown status %d\n",transfer->status);
		duplex->result = LIBUSB_ERROR_IO;
	}
}

static void LIBUSB_CALL
ftdi_duplex_write_cb(struct libusb_transfer *transfer)
{
	struct ftdi_duplex_context *duplex = transfer->user_data;

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
		duplex->activity = 1;
		return;
	}
	else
	{
		//duplex->result = transfer->status;
		printf("%s failed\n", __FUNCTION__);
		return;
	}
}

int ftdi_duplex_write(struct ftdi_duplex_context *duplex, const char *_data, int len)
{
	int err;
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
		return LIBUSB_ERROR_NO_MEM;
	}

	transfer->flags = LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER;

	libusb_fill_bulk_transfer(transfer, ftdi->usb_dev, ftdi->in_ep,
							  data, len,
							  ftdi_duplex_write_cb,
							  duplex, 0);

	transfer->status = -1;
	err = libusb_submit_transfer(transfer);
	if (err)
	{
		libusb_free_transfer(transfer); // Expected to free data
		return err;
	}

	return 0;
}

struct ftdi_duplex_context *
ftdi_duplex_start(struct ftdi_context *ftdi,
				FTDIDuplexCallback *callback, void *userdata,
				int packetsPerTransfer, int numTransfers)
{
	struct ftdi_duplex_context *duplex;
	int bufferSize = packetsPerTransfer * ftdi->max_packet_size;
	int i;
	int err = 0;

	duplex = malloc(sizeof(*duplex));
	if (!duplex)
	{
		fprintf(stderr,"malloc failed\n");
		return NULL;
	}
	duplex->ftdi = ftdi;
	duplex->callback = callback;
	duplex->userdata = userdata;
	duplex->packetsize = ftdi->max_packet_size;
	duplex->activity = 1;
	duplex->result = 0;
	duplex->numTransfers = numTransfers;

	/* Only FT2232H and FT232H know about the synchronous FIFO Mode*/
	if ((ftdi->type != TYPE_2232H) && (ftdi->type != TYPE_232H))
	{
		fprintf(stderr,"Device doesn't support synchronous FIFO mode\n");
		return NULL;
	}

	/* We don't know in what duplex we are, switch to reset*/
	if (ftdi_set_bitmode(ftdi,  0xff, BITMODE_RESET) < 0)
	{
		fprintf(stderr,"Can't reset mode\n");
		return NULL;
	}

	/* Purge anything remaining in the buffers*/
	if (ftdi_usb_purge_buffers(ftdi) < 0)
	{
		fprintf(stderr,"Can't Purge\n");
		return NULL;
	}

	/*
	 * Set up all transfers
	 */

	duplex->transfers = calloc(numTransfers, sizeof *duplex->transfers);
	if (!duplex->transfers)
	{
		err = LIBUSB_ERROR_NO_MEM;
		goto cleanup;
	}

	for (i = 0; i < numTransfers; i++)
	{
		struct libusb_transfer *transfer;

		transfer = libusb_alloc_transfer(0);
		duplex->transfers[i] = transfer;
		if (!transfer)
		{
			err = LIBUSB_ERROR_NO_MEM;
			goto cleanup;
		}

		libusb_fill_bulk_transfer(transfer, ftdi->usb_dev, ftdi->out_ep,
								  malloc(bufferSize), bufferSize,
								  ftdi_duplex_read_cb,
								  duplex, 0);

		if (!transfer->buffer)
		{
			err = LIBUSB_ERROR_NO_MEM;
			goto cleanup;
		}

		transfer->status = -1;
		err = libusb_submit_transfer(transfer);
		if (err)
			goto cleanup;
	}

	/* Start the transfers only when everything has been set up.
	 * Otherwise the transfers start stuttering and the PC not
	 * fetching data for several to several ten milliseconds
	 * and we skip blocks
	 */
	if (ftdi_set_bitmode(ftdi,  0xff, BITMODE_SYNCFF) < 0)
	{
		fprintf(stderr,"Can't set synchronous fifo mode: %s\n",
				ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	return duplex;

	/*
	 * Cancel any outstanding transfers, and free memory.
	 */

cleanup:
	if (duplex)
		free(duplex);
	return NULL;
}

int ftdi_duplex_poll(struct ftdi_duplex_context *duplex)
{
	struct ftdi_context *ftdi = duplex->ftdi;
	struct timeval timeout = { 0, ftdi->usb_read_timeout };

	int err = libusb_handle_events_timeout(ftdi->usb_ctx, &timeout);
	if (err ==  LIBUSB_ERROR_INTERRUPTED)
	{
		err = libusb_handle_events_timeout(ftdi->usb_ctx, &timeout);
	}
	if (!duplex->result)
	{
		duplex->result = err;
	}
	if (duplex->activity == 0)
	{
		printf("duplex->activity : 0\n");
		duplex->result = 1;
	}
	else
	{
		duplex->activity = 0;
	}

	if (err)
	{
		return err;
	}
	else
	{
		return duplex->result;
	}
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

	for (i = 0; i < duplex->numTransfers; i++)
	{
		struct libusb_transfer *transfer = duplex->transfers[i];
		libusb_cancel_transfer(transfer);
	}

	// FIXME : wait until all in an out transfer are done

	for (i = 0; i < duplex->numTransfers; i++)
	{
		struct libusb_transfer *transfer = duplex->transfers[i];
		free(transfer->buffer);
		libusb_free_transfer(transfer);
	}

	free(duplex);

	return 0;
}

