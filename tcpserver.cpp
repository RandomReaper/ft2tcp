/************************************************************************//**
 *
 * \file tcpserver.cpp
 * \brief Simple QT TCP server
 * \author marc at pignat dot org
 *
 ****************************************************************************
 *
 * Copyright 2015,2016 Marc Pignat
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "tcpserver.h"
#include <QDebug>

TcpServer::TcpServer(QObject *parent) :
	QObject(parent)
{
}

int TcpServer::start()
{
	server = new QTcpServer(this);

	// whenever a user connects, it will emit signal
	connect(server, SIGNAL(newConnection()),
			this, SLOT(newConnection()));

	if(!server->listen(QHostAddress::Any, 9999))
	{
		qDebug() << "Server could not start, listen failed";
		return -1;
	}
	else
	{
		qDebug() << "Server started!";
	}

	return 0;
}

void TcpServer::quit()
{
	qDebug() << "server stopped";
	server->close();
}

void TcpServer::tx(const quint8 *data, quint64 size)
{
	// qDebug() << "tx:data.length:" << data.length();
	if (size != 510)
	{
		//qDebug() << "tx:data.length:" << data.length();
	}
	if (size == 0)
	{
		qDebug() << "tx:size:" << size;
	}

	foreach(QTcpSocket *s, clients)
	{
		s->write((const char *)data, size);
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
	quint8 buffer[512];
	for (;;)
	{
		quint64 size = client->read((char *)buffer, sizeof(buffer));
		if (size == 0)
		{
			break;
		}
		else
		emit rx(buffer, size);
	}
}

void TcpServer::newConnection()
{
	// need to grab the socket
	QTcpSocket *socket = server->nextPendingConnection();
	clients.push_back(socket);
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(rxReady()));
}

TcpServer::~TcpServer()
{

}
