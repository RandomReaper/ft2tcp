#include "tcpserver.h"
#include <QDebug>

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

}

void TcpServer::tx(const QByteArray &data)
{
	foreach(QTcpSocket *s, clients)
	{
		s->write(data);
	}
}

void TcpServer::disconnected()
{
	QTcpSocket* client = static_cast<QTcpSocket*>(QObject::sender());
	clients.removeOne(client);
}

void TcpServer::rxReady()
{
	QTcpSocket* client = static_cast<QTcpSocket*>(QObject::sender());
	emit rx(client->readAll());
}

void TcpServer::newConnection()
{
	// need to grab the socket
	QTcpSocket *socket = server->nextPendingConnection();
	clients.push_back(socket);
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(rxReady()));
}
