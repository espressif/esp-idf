*****************************************
Building OpenOCD from Sources for Windows
*****************************************

:link_to_translation:`zh_CN:[中文]`

.. note::

    This document outlines how to build a binary of OpenOCD from its source files instead of downloading the pre-built binary. For a quick setup, users can download a pre-built binary of OpenOCD from `Espressif GitHub <https://github.com/espressif/openocd-esp32/releases>`_ instead of compiling it themselves (see :ref:`jtag-debugging-setup-openocd` for more details).

.. note::

    All code snippets in this document are assumed to be running in an MSYS2 shell with the MINGW32 subsystem.


Install Dependencies
====================

Install packages that are required to compile OpenOCD:

.. code-block:: bash

    pacman -S --noconfirm --needed autoconf automake git make \
    mingw-w64-i686-gcc \
    mingw-w64-i686-toolchain \
    mingw-w64-i686-libtool \
    mingw-w64-i686-pkg-config \
    mingw-w64-cross-winpthreads-git \
    p7zip


Download Sources of OpenOCD
===========================

The sources for the {IDF_TARGET_NAME}-enabled variant of OpenOCD are available from Espressif's GitHub under https://github.com/espressif/openocd-esp32. These source files can be pulled via Git using the following commands:

.. code-block:: bash

    cd ~/esp
    git clone --recursive https://github.com/espressif/openocd-esp32.git

The clone of sources should be now saved in ``~/esp/openocd-esp32`` directory.


Downloading libusb
==================

The libusb library is also required when building OpenOCD. The following commands will download a particular release of libusb and uncompress it to the current directory.

.. code-block:: bash

    wget https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.7z
    7z x -olibusb ./libusb-1.0.22.7z

We now need to export the following variables such that the libusb library gets linked into the OpenOCD build.

.. code-block:: bash

    export CPPFLAGS="$CPPFLAGS -I${PWD}/libusb/include/libusb-1.0"
    export LDFLAGS="$LDFLAGS -L${PWD}/libusb/MinGW32/.libs/dll"


Build OpenOCD
=============

The following commands will configure OpenOCD then build it.

.. code-block:: bash

    cd ~/esp/openocd-esp32
    export CPPFLAGS="$CPPFLAGS -D__USE_MINGW_ANSI_STDIO=1 -Wno-error"; export CFLAGS="$CFLAGS -Wno-error"
    ./bootstrap
    ./configure --disable-doxygen-pdf --enable-ftdi --enable-jlink --enable-ulink --build=i686-w64-mingw32 --host=i686-w64-mingw32
    make
    cp ../libusb/MinGW32/dll/libusb-1.0.dll ./src
    cp /opt/i686-w64-mingw32/bin/libwinpthread-1.dll ./src


Once the build is completed, the OpenOCD binary will be placed in ``~/esp/openocd-esp32/src/``.

You can then optionally call ``make install``. This will copy the OpenOCD binary to a user specified location.

- This location can be specified when OpenOCD is configured, or by setting ``export DESTDIR="/custom/install/dir"`` before calling ``make install``.
- If you have an existing OpenOCD (from e.g., another development platform), you may want to skip this call as your existing OpenOCD may get overwritten.

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

For greater convenience, all of commands called throughout the OpenOCD build process have been listed in the code snippet below. Users can copy this code snippet into a shell script then execute it:

.. code-block:: bash

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

To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-configuring-target`.
