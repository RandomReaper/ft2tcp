#!/bin/bash

MXE=~/git/mxe/

mkdir -p build/native
mkdir -p build/win64
mkdir -p build/win32
cd build/native
qmake ../../ft2tcp.pro
make -j9
cd ../..

QMAKE=$MXE/usr/x86_64-w64-mingw32.static/qt5/bin/qmake
PATH=$MXE/usr/bin:$PATH
cd build/win64
$QMAKE ../../ft2tcp.pro
make -j9
cd ../..


QMAKE=$MXE/usr/i686-w64-mingw32.static/qt5/bin/qmake
cd build/win32
$QMAKE ../../ft2tcp.pro
make -j9
cd ../..

