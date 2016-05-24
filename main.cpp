/************************************************************************//**
 *
 * \file main.cpp
 * \brief Connect tcpserver with ft245 interface
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

#include <QCoreApplication>
#include "tcpserver.h"
#include "ft245.h"

int main(int argc, char *argv[])
{
	int status;
	QCoreApplication a(argc, argv);

	TcpServer server;
	Ft245 ft245;

	QObject::connect(&ft245, SIGNAL(rx(const quint8 *, quint64)), &server, SLOT(tx(const quint8 *, quint64)));
	QObject::connect(&server, SIGNAL(rx(const quint8 *, quint64)), &ft245, SLOT(tx(const quint8 *, quint64)));

	QObject::connect(&ft245, SIGNAL(fatal()), &server, SLOT(quit()));
	QObject::connect(&ft245, SIGNAL(fatal()), &a, SLOT(quit()));

	status = server.start();
	if (status)
	{
		return status;
	}

	status = ft245.start();
	if (status)
	{
		return status;
	}

	return a.exec();
}
