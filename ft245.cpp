#include "ft245.h"
#include <ftdi.h>
#include <QTimer>

Ft245::Ft245(QObject *parent) : QObject(parent)
{

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_sim_data()));
    timer->start(1000);

    // FIXME init
}

void Ft245::timer_sim_data()
{
    emit rx("hello\n");
}

void Ft245::tx(const QByteArray &data)
{
   // FIXME
}
