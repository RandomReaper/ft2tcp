CONFIG += console
SOURCES = tcpserver.cpp ft245.cpp ft245_rx_thread.cpp main.cpp ft245_duplex.c
HEADERS = tcpserver.h ft245.h ft245_rx_thread.h ft245_duplex.h

QT -= gui
QT += core network

unix:LIBS += -lftdi1 -lusb-1.0
win32:LIBS += -lftdi1 -lusb-1.0 -L FIXIT:\Dropbox\drittsekk\30_tools\ft2tcp-compiling-from-windows\lib
win32:INCLUDEPATH += FIXIT:\Dropbox\drittsekk\30_tools\ft2tcp-compiling-from-windows\include
