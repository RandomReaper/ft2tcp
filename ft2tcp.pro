CONFIG += qt console
SOURCES = main.cpp
#HEADERS = main.h

QT -= gui
QT += core network

unix:LIBS += -lftdi -lusb-1.0
win32:LIBS += -lftdi1 -lusb-1.0
win32:INCLUDEPATH += ../../../mxe/usr/x86_64-w64-mingw32.static/include/libftdi1/