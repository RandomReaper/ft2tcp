#ifndef FT245_H
#define FT245_H

#include <QObject>
#include <QTimer>
#include <libftdi1/ftdi.h>
#include "ft245_duplex.h"

class Ft245 : public QObject
{
	Q_OBJECT
public:
	Ft245(QObject *parent = 0);
	virtual ~Ft245();
	int start();
	void rx_callback(const quint8 *data, quint64 size);
	bool _stop;

signals:
	void fatal();
	void rx(const quint8 *data, quint64 size);
	void rx_thread_start(struct ftdi_context *ftdi);
	void rx_thread_stop();

public slots:
    void tx(const quint8 *data, quint64 size);
	void rx_thread_stopped();
    void poll();

private:
	void fatal(const char *msg, const char *file, int n);
	int open(void);
	void close(void);

	struct ftdi_context *ftdi;
	struct ftdi_duplex_context *duplex;
	QTimer *timer;
};

#endif /* FT245_H */

