#ifndef FT245_RX_THREAD_H
#define FT245_RX_THREAD_H

#include <QObject>
#include <libftdi1/ftdi.h>

class Ft245RxThread : public QObject
{
Q_OBJECT

public slots:
    void doWork(struct ftdi_context *ftdi);
    void stop(void);

signals:
    void rx(const QByteArray &data);

public:
    bool _stop;
};

#endif /* FT245_RX_THREAD_H */
