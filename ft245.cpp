#include "ft245.h"
#include <QTimer>

static int read_callback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
    (void)progress;
    Ft245 *ft245 = (Ft245 *)userdata;

    printf("%s:%d\n", __FUNCTION__, length);

    ft245->rx_callback(QByteArray((char *)buffer, length));

    return 1;
}

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

    if (ftdi_set_bitmode(ftdi, 0, BITMODE_SYNCFF))
    {
        fatal("wahou", __FILE__, __LINE__ );
        return close();
    }

    if (ftdi_readstream(ftdi, read_callback,  this, 1, 1))
    {
        fatal("wahou", __FILE__, __LINE__ );
        return close();
    }

    emit tx("waou");

    return;
}

void Ft245::close(void)
{
    if (!ftdi)
    {
        return;
    }
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
