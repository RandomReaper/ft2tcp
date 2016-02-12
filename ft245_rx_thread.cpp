#include "ft245_rx_thread.h"
#include <stdio.h>
#include <QDebug>
#include <QThread>

static int read_callback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
	(void)progress;
	Ft245RxThread *self = (Ft245RxThread *)userdata;

    if (length)
    {
        emit self->rx(QByteArray((char *)buffer, length));
    }

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
        qDebug() << "ftdi_readstream returned" << ret;
		exit(1);
    } while (!_stop);
	_stop = true;
	emit stopped();
}

void Ft245RxThread::stop(void)
{
    _stop = true;
}

void Ft245RxThread::tx(const QByteArray &data)
{
    /*
    uint8_t header[] = {0x00,0x00};
    _pause = true;
    while(!_paused)
    {
        QThread::usleep(1);
    }

    ftdi_write_data(ftdi, header, sizeof(header));
    ftdi_write_data(ftdi, (const unsigned char*)data.data(), data.length());
    _pause = false;
    */

    struct ftdi_transfer_control *xfer = ftdi_write_data_submit(ftdi, (unsigned char*)data.data(), data.length());
    if (!xfer)
    {
        qDebug() << "ftdi_write_data_submit failed";
        return;
    }

    int ret = ftdi_transfer_data_done(xfer);
    if (ret < 0)
    {
        qDebug() << "ftdi_transfer_data_done failed with status" << ret;
        return;
    }
}
