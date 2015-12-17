#ifndef FT245_H
#define FT245_H

#include <QObject>
#include <QDebug>
#include <ftdi.h>
#include <QThread>

class Ft245 : public QObject
{
    Q_OBJECT
public:
    Ft245(QObject *parent = 0);
    virtual ~Ft245();

signals:
    void fatal();
    void rx(const QByteArray &data);

public slots:
    void tx(const QByteArray &data);

private slots:
    void timer_sim_data();

private:
    void fatal(const char *msg, const char *file, int n);
    void open(void);
    void close(void);

    struct ftdi_context *ftdi;
    QThread workerThread;
};

#endif /* FT245_H */
