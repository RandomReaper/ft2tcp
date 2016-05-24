/************************************************************************//**
 *
 * \file ft2tcp.pro
 * \brief FTDI FT245 synchronous FIFO to TCP bridge
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

CONFIG += qt console
SOURCES = tcpserver.cpp ft245.cpp main.cpp ft245_duplex.c
HEADERS = tcpserver.h ft245.h ft245_duplex.h

QT -= gui
QT += core network

unix:LIBS += -lftdi1 -lusb-1.0
win32:LIBS += -lftdi1 -lusb-1.0
win32:INCLUDEPATH += ../../../mxe/usr/x86_64-w64-mingw32.static/include/libftdi1/

QMAKE_CFLAGS_RELEASE += -g
QMAKE_CXXFLAGS_RELEASE += -g

