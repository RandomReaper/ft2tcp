#ifndef FT245_H
#define FT245_H

#include <QObject>
#include <QThread>
#include <libftdi1/ftdi.h>
#include "ft245_rx_thread.h"

class Ft245 : public QObject
{
	Q_OBJECT
public:
	Ft245(QObject *parent = 0);
	virtual ~Ft245();
	void rx_callback(const QByteArray &data);

signals:
	void fatal();
	void rx(const QByteArray &data);
	void rx_thread_start(struct ftdi_context *ftdi);
	void rx_thread_stop();

public slots:
	void tx(const QByteArray &data);

private:
	void fatal(const char *msg, const char *file, int n);
	void open(void);
	void close(void);

	struct ftdi_context *ftdi;
	QThread rx_thread;
	Ft245RxThread *ft245_rx;
};

#endif /* FT245_H */

