#!/bin/bash

MXE=~/git/mxe/

mkdir -p build/native
mkdir -p build/win64
cd build/native
qmake ../..
make -j9
cd ../..

QMAKE=$MXE/usr/x86_64-w64-mingw32.static/qt5/bin/qmake
PATH=$MXE/usr/bin:$PATH
cd build/win64
qmake ../..
make -j9
cd ../..
