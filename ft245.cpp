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

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_sim_data()));
    timer->start(1000);

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

void Ft245::timer_sim_data()
{
    QString a;
    unsigned int chipid;
    int ret;

    ret = ftdi_read_chipid(ftdi, &chipid);
    if (ret)
    {
        fatal("wahou", __FILE__, __LINE__ );
    }

    ret = ftdi_set_bitmode(ftdi, 0, BITMODE_SYNCFF);

    a.sprintf("ftdi_read_chipid: 0x%08x\n", chipid);
    emit rx(a.toLatin1());
}

void Ft245::tx(const QByteArray &data)
{
   qDebug() << QString(data);
}
