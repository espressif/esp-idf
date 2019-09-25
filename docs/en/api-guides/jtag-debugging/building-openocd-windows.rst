*****************************************
Building OpenOCD from Sources for Windows
*****************************************
:link_to_translation:`zh_CN:[中文]`

The following instructions are alternative to downloading binary OpenOCD from `Espressif GitHub <https://github.com/espressif/openocd-esp32/releases>`_. To quickly setup the binary OpenOCD, instead of compiling it yourself, backup and proceed to section :ref:`jtag-debugging-setup-openocd`.

.. highlight:: bash

.. note::

    Following instructions are assumed to be runned in MSYS2 environment with MINGW32 subsystem!


Install Dependencies
====================

Install packages that are required to compile OpenOCD::

    pacman -S --noconfirm --needed autoconf automake git make \
    mingw-w64-i686-gcc \
    mingw-w64-i686-toolchain \
    mingw-w64-i686-libtool \
    mingw-w64-i686-pkg-config \
    mingw-w64-cross-winpthreads-git \
    p7zip


Download Sources of OpenOCD
===========================

The sources for the ESP32-enabled variant of OpenOCD are available from Espressif GitHub under https://github.com/espressif/openocd-esp32. To download the sources, use the following commands::

    cd ~/esp
    git clone --recursive https://github.com/espressif/openocd-esp32.git


The clone of sources should be now saved in ``~/esp/openocd-esp32`` directory.


Downloading libusb
==================

Build and export variables for a following OpenOCD compilation::

    wget https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.7z
    7z x -olibusb ./libusb-1.0.22.7z
    export CPPFLAGS="$CPPFLAGS -I${PWD}/libusb/include/libusb-1.0"
    export LDFLAGS="$LDFLAGS -L${PWD}/libusb/MinGW32/.libs/dll"



Build OpenOCD
=============

Proceed with configuring and building OpenOCD::

    cd ~/esp/openocd-esp32
    export CPPFLAGS="$CPPFLAGS -D__USE_MINGW_ANSI_STDIO=1 -Wno-error"; export CFLAGS="$CFLAGS -Wno-error"
    ./bootstrap
    ./configure --disable-doxygen-pdf --enable-ftdi --enable-jlink --enable-ulink --build=i686-w64-mingw32 --host=i686-w64-mingw32
    make
    cp ../libusb/MinGW32/dll/libusb-1.0.dll ./src
    cp /opt/i686-w64-mingw32/bin/libwinpthread-1.dll ./src
    

Optionally you can add ``make install`` step at the end. Skip it, if you have an existing OpenOCD (from e.g. another development platform), as it may get overwritten. Also you could use ``export DESTDIR="/custom/install/dir"; make install``.

.. note::

    * Should an error occur, resolve it and try again until the command ``make`` works. 
    * If there is a submodule problem from OpenOCD, please ``cd`` to the ``openocd-esp32`` directory and input ``git submodule update --init``.
    * If the ``./configure`` is successfully run, information of enabled JTAG will be printed under ``OpenOCD configuration summary``. 
    * If the information of your device is not shown in the log, use ``./configure`` to enable it as described in  ``../openocd-esp32/doc/INSTALL.txt``.
    * For details concerning compiling OpenOCD, please refer to ``openocd-esp32/README.Windows``.
    * Don't forget to copy `libusb-1.0.dll` and `libwinpthread-1.dll` into `OOCD_INSTALLDIR/bin` from ``~/esp/openocd-esp32/src``. 

Once ``make`` process is successfully completed, the executable of OpenOCD will be saved in ``~/esp/openocd-esp32/src`` directory.


Full Listing
============

A complete described previously process is provided below for the faster execution, e.g. as a shell script::

    pacman -S --noconfirm --needed autoconf automake git make mingw-w64-i686-gcc mingw-w64-i686-toolchain mingw-w64-i686-libtool mingw-w64-i686-pkg-config mingw-w64-cross-winpthreads-git p7zip
    cd ~/esp
    git clone --recursive https://github.com/espressif/openocd-esp32.git

    wget https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.7z
    7z x -olibusb ./libusb-1.0.22.7z
    export CPPFLAGS="$CPPFLAGS -I${PWD}/libusb/include/libusb-1.0"; export LDFLAGS="$LDFLAGS -L${PWD}/libusb/MinGW32/.libs/dll"

    export CPPFLAGS="$CPPFLAGS -D__USE_MINGW_ANSI_STDIO=1 -Wno-error"; export CFLAGS="$CFLAGS -Wno-error"
    cd ~/esp/openocd-esp32
    ./bootstrap
    ./configure --disable-doxygen-pdf --enable-ftdi --enable-jlink --enable-ulink --build=i686-w64-mingw32 --host=i686-w64-mingw32
    make
    cp ../libusb/MinGW32/dll/libusb-1.0.dll ./src
    cp /opt/i686-w64-mingw32/bin/libwinpthread-1.dll ./src

    # # optional
    # export DESTDIR="$PWD"
    # make install
    # cp ./src/libusb-1.0.dll $DESTDIR/mingw32/bin
    # cp ./src/libwinpthread-1.dll $DESTDIR/mingw32/bin



Next Steps
==========

To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-configuring-esp32-target`.
