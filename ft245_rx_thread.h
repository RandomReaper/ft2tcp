#ifndef FT245_RX_THREAD_H
#define FT245_RX_THREAD_H

#include <QObject>
#include <libftdi1/ftdi.h>

class Ft245RxThread : public QObject
{
Q_OBJECT

public slots:
	void doWork(struct ftdi_context *ftdi);

signals:
	void rx(const QByteArray &data);
	void stopped();

public:
	bool _stop;
    bool _pause;
    void stop(void);
    void tx(const QByteArray &data);

private:
    struct ftdi_context *ftdi;
    bool _paused;
};

#endif /* FT245_RX_THREAD_H */
