#include <QCoreApplication>
#include "tcpserver.h"
#include "ft245.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TcpServer server;
    Ft245 ft245;

    QObject::connect(&ft245, SIGNAL(rx(const QByteArray &)), &server, SLOT(tx(const QByteArray &)));

    return a.exec();
}
