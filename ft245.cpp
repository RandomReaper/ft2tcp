#include "ft245.h"
#include <libftdi1/ftdi.h>
#include <QTimer>

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
    uint8_t buffer[256];
    memset(buffer, 0x00, sizeof(buffer));
    qDebug() << "before read";
    int ret = ftdi_read_data(ftdi, buffer, sizeof(buffer));
    qDebug() << "after read, ret = " << ret;
    for (size_t i = 0 ; i < sizeof(buffer) ; i++)
    {
        printf("i%03d:0x%02x\n", (int)i, buffer[i]);
    }

    emit rx(QByteArray((char *)buffer, sizeof(buffer)));
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
