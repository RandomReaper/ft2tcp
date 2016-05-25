#!/bin/bash
mkdir build
cd build
/usr/lib/x86_64-linux-gnu/qt5/bin/qmake -spec /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++ ..
make -j9