#include "ft245_rx_thread.h"
#include <stdio.h>
#include <QDebug>
#include <QThread>

static int read_callback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
	(void)progress;
	Ft245RxThread *self = (Ft245RxThread *)userdata;

	emit self->rx(QByteArray((char *)buffer, length));

    if (self->_stop || self->_pause)
	{
		return 1;
	}

	return 0;
}

void Ft245RxThread::doWork(struct ftdi_context *_ftdi)
{
    ftdi = _ftdi;
    int ret;
	_stop = false;
    do
    {
        _pause = false;
        _paused = false;
        ret = ftdi_readstream(ftdi, read_callback, this, 8, 8);
        _paused = _pause;
        while(_pause)
        {
            QThread::usleep(1);
        }
    } while (!_stop);
    qDebug() << "ftdi_readstream returned" << ret;
	_stop = true;
	emit stopped();
}

void Ft245RxThread::stop(void)
{
    _stop = true;
}

void Ft245RxThread::tx(const QByteArray &data)
{
    _pause = true;
    while(!_paused)
    {
        QThread::usleep(1);
    }
    ftdi_write_data(ftdi, (const unsigned char*)data.data(), data.length());
    _pause = false;
}
