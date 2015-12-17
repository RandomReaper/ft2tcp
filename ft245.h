#ifndef FT245_H
#define FT245_H

#include <QObject>
#include <QDebug>

class Ft245 : public QObject
{
    Q_OBJECT
public:
    Ft245(QObject *parent = 0);

signals:
    void rx(const QByteArray &data);

public slots:
    void tx(const QByteArray &data);

private slots:
    void timer_sim_data();

};

#endif /* FT245_H */
