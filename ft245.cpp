#include "ft245.h"
#include "ft245_rx_thread.h"
#include <QDebug>

void Ft245::rx_callback(const QByteArray &data)
{
    emit rx(data);
}

void Ft245::open(void)
{
    ftdi = ftdi_new();

    if (!ftdi)
    {
        return;
    }

    if (ftdi_set_interface(ftdi, INTERFACE_A))
    {
        fatal("wahou", __FILE__, __LINE__ );
        return close();
    }

    if (ftdi_usb_open(ftdi, 0x0403, 0x6010) < 0)
    {
        fatal("wahou", __FILE__, __LINE__ );
        return close();
    }

    // Fixme, seems already in ftdi_readstream
    if (ftdi_set_bitmode(ftdi, 0, BITMODE_SYNCFF))
    {
        fatal("wahou", __FILE__, __LINE__ );
        return close();
    }

    ft245_rx = new Ft245RxThread;
    ft245_rx->moveToThread(&rx_thread);
    connect(&rx_thread, &QThread::finished, ft245_rx, &QObject::deleteLater);
    connect(this, &Ft245::rx_thread_start, ft245_rx, &Ft245RxThread::doWork);
    connect(this, &Ft245::rx_thread_stop, ft245_rx, &Ft245RxThread::stop);
    connect(ft245_rx, &Ft245RxThread::rx, this, &Ft245::rx);
    rx_thread.start();
    emit rx_thread_start(ftdi);

    return;
}

void Ft245::close(void)
{
    if (!ftdi)
    {
        return;
    }

    emit rx_thread_stop();

    rx_thread.quit();
    rx_thread.wait();

    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);
    ftdi = NULL;
}

Ft245::Ft245(QObject *parent) : QObject(parent)
{
    open();
}

Ft245::~Ft245()
{
    close();
}

void Ft245::fatal(const char *msg, const char *file, int n)
{
    qDebug() << msg << "in file" << file << ", line : " << n;

    emit fatal();
}

void Ft245::tx(const QByteArray &data)
{
   qDebug() << QString(data);
}
