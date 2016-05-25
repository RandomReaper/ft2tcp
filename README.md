# ft2tcp [![Build Status](https://travis-ci.org/RandomReaper/ft2tcp.svg?branch=master)](https://travis-ci.org/RandomReaper/ft2tcp)
TCP interface to FTDI FT245 synchronous FIFO

Qt/C++ code for [ft245_sync_if.vhd](https://github.com/RandomReaper/pim-vhdl/blob/master/hdl/rtl/ft245_sync_if/ft245_sync_if.vhd).

## Eeprom
The FTDI EEPROM must be configured in fifo mode, see [eeprom configuration](./eeprom-config/README.md).

## Performance
Performance is really close to USB full speed maximum, more than 40 MiB/s (see [perf.txt](./perf.txt))

## Compiling

Tested on unbuntu 16.04 LTS:
```bash
sudo apt-get install git libftdi1-dev build-essential qt5-default
mkdir -p ~/git && cd ~/git
git clone https://github.com/RandomReaper/ft2tcp
cd ft2tcp
mkdir build && cd build
qmake ..
make -j9
```

## Cross compiling for windows
This procedure will compile ```./build/win{32,64}/release/ft2tcp.exe``` **statically linked**.


Tested on unbuntu 16.04 LTS:
1. Install mxe.cc (can take hours)
```
sudo apt-get install git build-essential libftdi1-dev qt5-default automake autopoint bison flex gperf libtool libtool-bin ruby scons intltool p7zip-full libgdk-pixbuf2.0-dev
mkdir -p ~/git && cd ~/git
git clone https://github.com/mxe/mxe.git
cd mxe
echo MXE_TARGETS='x86_64-w64-mingw32.static i686-w64-mingw32.static' >> settings.mk
make qt5 libftdi1
```
2. Compile
```bash
cd ~/git/ft2tcp
./cross-compile-win.sh

```

## Compiling for windows
Since I haven't found any way to create an installable Qt executable (either statically or
dynamically linked) see [Cross compiling for windows](#Cross-compiling-for-windows).

