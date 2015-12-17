#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

signals:
    void rx(const QByteArray &data);

public slots:
    void tx(const QByteArray &data);

private slots:
    void disconnected();
    void newConnection();
    void rxReady();

private:
    QTcpServer *server;
    QList<QTcpSocket*> clients;
};

#endif /* TCPSERVER_H */
