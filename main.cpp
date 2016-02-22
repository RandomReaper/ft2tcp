#include <QCoreApplication>
#include "tcpserver.h"
#include "ft245.h"

int main(int argc, char *argv[])
{
	int status;
	QCoreApplication a(argc, argv);

	TcpServer server;
	Ft245 ft245;

	QObject::connect(&ft245, SIGNAL(rx(const QByteArray &)), &server, SLOT(tx(const QByteArray &)));
	QObject::connect(&server, SIGNAL(rx(const QByteArray &)), &ft245, SLOT(tx(const QByteArray &)));

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
