#!/bin/bash
mkdir build
cd build
/usr/lib/x86_64-linux-gnu/qt5/bin/qmake ..
make -j9