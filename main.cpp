#include <QCoreApplication>
#include "tcpserver.h"
#include "ft245.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	TcpServer server;
	Ft245 ft245;

	QObject::connect(&ft245, SIGNAL(rx(const QByteArray &)), &server, SLOT(tx(const QByteArray &)));
	QObject::connect(&server, SIGNAL(rx(const QByteArray &)), &ft245, SLOT(tx(const QByteArray &)));

	QObject::connect(&ft245, SIGNAL(fatal()), &a, SLOT(quit()));

	return a.exec();
}
