#include "tcpserver.h"
#include <QTimer>

TcpServer::TcpServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server started!";
    }

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_sim_data()));
    timer->start(1000);
}

void TcpServer::write(const QByteArray &data)
{
    foreach(QTcpSocket *s, clients)
    {
        s->write(data);
    }
}

void TcpServer::timer_sim_data()
{
    QString msg;
    msg.sprintf("Hello client count = %d\n", clients.size());
    write(msg.toLatin1());
}

void TcpServer::disconnected()
{
    // client has disconnected, so remove from list
    QTcpSocket* pClient = static_cast<QTcpSocket*>(QObject::sender());
    clients.removeOne(pClient);
}

void TcpServer::newConnection()
{
    // need to grab the socket
    QTcpSocket *socket = server->nextPendingConnection();
    clients.push_back(socket);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    QString msg;
    msg.sprintf("Hello client count = %d\n", clients.size());

    socket->write(msg.toLatin1());
}
