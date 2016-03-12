#include "ft245.h"
#include <QDebug>

static int read_callback(uint8_t *buffer, int length, void *userdata)
{
	Ft245 *self = (Ft245 *)userdata;

	if (length)
	{
		emit self->rx(buffer, length);
	}

	if (self->_stop)
	{
		return 1;
	}

	return 0;
}

void Ft245::rx_callback(const quint8 *data, quint64 size)
{
	emit rx(data, size);
}

void Ft245::poll()
{
	int ret = ftdi_duplex_poll(duplex);
	if (ret < 0)
	{
		fatal("ftdi_duplex_poll", __FILE__, __LINE__ );
	}
}

int Ft245::open(void)
{
	struct ftdi_version_info info = ftdi_get_library_version();
	QList<uint16_t> supported_pid = QList<uint16_t>() << 0x6010 << 0x6014;

	qDebug() << "ftdi_get_library_version" << info.major << "." << info.minor << "." << info.micro;

	ftdi = ftdi_new();

	if (!ftdi)
	{
		fatal("ftdi_new", __FILE__, __LINE__ );

		return -1;
	}

	if (ftdi_set_interface(ftdi, INTERFACE_A))
	{
		fatal("ftdi_set_interface failed", __FILE__, __LINE__ );
		close();
		return -1;
	}

	uint16_t found_pid = 0;
	foreach(uint16_t pid, supported_pid)
	{
		if (ftdi_usb_open(ftdi, 0x0403, pid) == 0)
		{
			found_pid = pid;
			break;
		}
	}

	if (!found_pid)
	{
		fatal("no devices found", __FILE__, __LINE__ );

		printf("Searched for vid:pid : ");
		foreach(uint16_t pid, supported_pid)
		{
			printf("0403:%04x, ", pid);
		}
		printf("\n");
		close();
		return -1;
	}


	_stop = false;
	duplex = ftdi_duplex_start(ftdi, read_callback, this, 128, 8);
	if (duplex == NULL)
	{
		fatal("ftdi_duplex_start", __FILE__, __LINE__ );
		close();
		return -1;
	}

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(poll()));
	timer->start(0);

	return 0;
}

void Ft245::close(void)
{
	if (!ftdi)
	{
		return;
	}

	if (ftdi_set_bitmode(ftdi,  0xff, BITMODE_RESET) < 0)
	{
		qDebug() << "ftdi_set_bitmode" << __FILE__ << ":" << __LINE__ ;
	}

	ftdi_usb_close(ftdi);
	ftdi_free(ftdi);
	ftdi = NULL;
}

Ft245::Ft245(QObject *parent) : QObject(parent)
{

}

int Ft245::start(void)
{
	return open();
}

void Ft245::fatal(const char *msg, const char *file, int n)
{
	qDebug() << msg << "in file" << file << ", line : " << n;

	emit fatal();
}

void Ft245::tx(const quint8 *data, quint64 size)
{
	int ret = ftdi_duplex_write(duplex, (const char *)data, size);
	if (ret < 0)
	{
		qDebug() << "ftdi_duplex_write failed with status" << ret;
		return;
	}
}

Ft245::~Ft245()
{

}
