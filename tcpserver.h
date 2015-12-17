#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

signals:

public slots:
    void disconnected();
    void write(const QByteArray &data);

private slots:
    void newConnection();
    void timer_sim_data();

private:
    QTcpServer *server;
    QList<QTcpSocket*> clients;
};

#endif /* TCPSERVER_H */
