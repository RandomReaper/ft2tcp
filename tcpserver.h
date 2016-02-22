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
	void rx(const QByteArray &data);

public slots:
	void tx(const QByteArray &data);
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
