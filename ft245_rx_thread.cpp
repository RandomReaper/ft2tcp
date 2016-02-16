#include "ft245_rx_thread.h"
#include "ft245_duplex.h"
#include <stdio.h>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>

static int read_callback(uint8_t *buffer, int length, void *userdata)
{
	Ft245RxThread *self = (Ft245RxThread *)userdata;

	if (length)
	{
		emit self->rx(QByteArray((char *)buffer, length));
	}

	if (self->_stop)
	{
		return 1;
	}

	return 0;
}

void Ft245RxThread::doWork(struct ftdi_context *_ftdi)
{
	int ret;

	_stop = false;
	duplex = ftdi_duplex_start(_ftdi, read_callback, this, 8, 8);

	if (!duplex)
	{
		emit stopped();
		return;
	}

	while (true)
	{
		ret = ftdi_duplex_poll(duplex);
		if (ret)
		{
			qDebug() << "ftdi_duplex_poll returned" << ret;
			ftdi_duplex_stop(duplex);
			QThread::sleep (1);
			break;
		}
		if (_stop)
		{
			break;
		}
		QCoreApplication::processEvents();
	}
	_stop = true;
	QThread::sleep (1);
	ftdi_duplex_stop(duplex);
	emit stopped();
}

void Ft245RxThread::stop(void)
{
	_stop = true;
}

void Ft245RxThread::tx(const QByteArray &data)
{
	int ret = ftdi_duplex_write(duplex, data.data(), data.length());
	if (ret < 0)
	{
		qDebug() << "ftdi_duplex_write failed with status" << ret;
		return;
	}
}
