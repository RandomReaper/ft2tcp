#ifndef FT245_DUPLEX_H
#define FT245_DUPLEX_H

#include <libftdi1/ftdi.h>
#include <libusb-1.0/libusb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (ftdi_duplex_rx_cb)(uint8_t *buffer, int length, void *userdata);

struct ftdi_duplex_context
{
	struct ftdi_context *ftdi;
	ftdi_duplex_rx_cb *rx_cb;
	void *cb_data;
	int pkt_size;
	int status;
	int nr_rx;
	int stop;
	struct libusb_transfer **rx_xfers;
};

struct ftdi_duplex_context *ftdi_duplex_start(struct ftdi_context *ftdi,
				ftdi_duplex_rx_cb *rx_cb, void *cb_data,
				int pkt_per_xfer, int nr_rx);

int ftdi_duplex_poll(struct ftdi_duplex_context *duplex);
int ftdi_duplex_stop(struct ftdi_duplex_context *duplex);

int ftdi_duplex_write(struct ftdi_duplex_context *duplex, const char *data, int len);

#ifdef __cplusplus
}
#endif

#endif /* FT245_DUPLEX_H */