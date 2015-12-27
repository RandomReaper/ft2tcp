#include "ft245_rx_thread.h"

static int read_callback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
	(void)progress;
	Ft245RxThread *self = (Ft245RxThread *)userdata;

	emit self->rx(QByteArray((char *)buffer, length));

	if (self->_stop)
	{
		return 1;
	}

	return 0;
}

void Ft245RxThread::doWork(struct ftdi_context *ftdi)
{
	_stop = false;
	ftdi_readstream(ftdi, read_callback,  this, 1, 1);
}

void Ft245RxThread::stop(void)
{
	_stop = false;
}
