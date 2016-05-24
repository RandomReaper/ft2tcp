/************************************************************************//**
 *
 * \file ft245.cpp
 * \brief QT FTDI FT245 synchronous fifo interface
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

#ifndef FT245_H
#define FT245_H

#include <QObject>
#include <QTimer>
#include <libftdi1/ftdi.h>
#include "ft245_duplex.h"

class Ft245 : public QObject
{
	Q_OBJECT
public:
	Ft245(QObject *parent = 0);
	virtual ~Ft245();
	int start();
	void rx_callback(const quint8 *data, quint64 size);
	bool _stop;

signals:
	void fatal();
	void rx(const quint8 *data, quint64 size);

public slots:
    void tx(const quint8 *data, quint64 size);
    void poll();

private:
	void fatal(const char *msg, const char *file, int n);
	int open(void);
	void close(void);

	struct ftdi_context *ftdi;
	struct ftdi_duplex_context *duplex;
	QTimer *timer;
};

#endif /* FT245_H */

