/************************************************************************//**
 *
 * \file tcpserver.h
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

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

class TcpServer : public QObject
{
	Q_OBJECT
public:
	int start();
	explicit TcpServer(QObject *parent = 0);
	virtual ~TcpServer();

signals:
	void rx(const quint8 *data, quint64 size);

public slots:
	void tx(const quint8 *data, quint64 size);
	void quit();

private slots:
	void disconnected();
	void newConnection();
	void rxReady();

private:
	void open();
	QTcpServer *server;
	QList<QTcpSocket*> clients;
};

#endif /* TCPSERVER_H */
