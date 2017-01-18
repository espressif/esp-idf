Set up of Toolchain for Mac OS
******************************

Step 0: Prerequisites
=====================

- install pip::

    sudo easy_install pip

- install pyserial

    sudo pip install pyserial

Step 1: Download binary toolchain for the ESP32
==================================================

ESP32 toolchain for macOS is available for download from Espressif website:

https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-61-gab8375a-5.2.0.tar.gz

Download this file, then extract it to the location you prefer, for example::

    mkdir -p ~/esp
    cd ~/esp
    tar -xzf ~/Downloads/xtensa-esp32-elf-osx-1.22.0-61-gab8375a-5.2.0.tar.gz

The toolchain will be extracted into ``~/esp/xtensa-esp32-elf/`` directory.

To use it, you will need to update your ``PATH`` environment variable in ``~/.profile`` file. To make ``xtensa-esp32-elf`` available for all terminal sessions, add the following line to your ``~/.profile`` file::

    export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin

Alternatively, you may create an alias for the above command. This way you can get the toolchain only when you need it. To do this, add different line to your ``~/.profile`` file::

    alias get_esp32="export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin"

Then when you need the toolchain you can type ``get_esp32`` on the command line and the toolchain will be added to your ``PATH``.

Alternative Step 1: Compile the toolchain from source using crosstool-NG
========================================================================

Instead of downloading binary toolchain from Espressif website (Step 1 above) you may build the toolchain yourself.

If you can't think of a reason why you need to build it yourself, then probably it's better to stick with the binary version. However, here are some of the reasons why you might want to compile it from source:

- if you want to customize toolchain build configuration

- if you want to use a different GCC version (such as 4.8.5)

- if you want to hack gcc or newlib or libstdc++

- if you are curious and/or have time to spare

- if you don't trust binaries downloaded from the Internet

In any case, here are the steps to compile the toolchain yourself.

- Install dependencies:

  - Install either MacPorts_ or homebrew_ package manager. MacPorts needs a full XCode installation, while homebrew only needs XCode command line tools.
    
    .. _homebrew: http://brew.sh/
    .. _MacPorts: https://www.macports.org/install.php

  - with MacPorts::

        sudo port install gsed gawk binutils gperf grep gettext wget libtool autoconf automake

  - with homebrew::

        brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake

Create a case-sensitive filesystem image::

    hdiutil create ~/esp/crosstool.dmg -volname "ctng" -size 10g -fs "Case-sensitive HFS+"

Mount it::

    hdiutil mount ~/esp/crosstool.dmg

Create a symlink to your work directory::

    cd ~/esp
    ln -s /Volumes/ctng crosstool-NG

Download ``crosstool-NG`` and build it::

    cd ~/esp
    git clone -b xtensa-1.22.x https://github.com/espressif/crosstool-NG.git
    cd crosstool-NG
    ./bootstrap && ./configure --prefix=$PWD && make install

Build the toolchain::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

Toolchain will be built in ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``. Follow instructions given in the previous section to add the toolchain to your ``PATH``.

Step 2: Getting ESP-IDF from github
===================================

Open Terminal.app, navigate to the directory you want to clone ESP-IDF and clone it using ``git clone`` command::

    cd ~/esp
    git clone --recursive https://github.com/espressif/esp-idf.git


ESP-IDF will be downloaded into ``~/esp/esp-idf``.

Note the ``--recursive`` option! If you have already cloned ESP-IDF without this option, run another command to get all the submodules::

    cd ~/esp/esp-idf
    git submodule update --init


Step 3: Starting a project
==========================

ESP-IDF by itself does not build a binary to run on the ESP32. The binary "app" comes from a project in a different directory. Multiple projects can share the same ESP-IDF directory.

The easiest way to start a project is to download the template project from GitHub::

    cd ~/esp
    git clone https://github.com/espressif/esp-idf-template.git myapp

This will download ``esp-idf-template`` project into ``~/esp/myapp`` directory.

**IMPORTANT:** The esp-idf build system does not support spaces in paths to esp-idf or to projects.

Step 4: Building and flashing the application
=============================================

In Terminal.app, go to the application directory which was obtained on the previous step::

    cd ~/esp/myapp

Type a command like this to set the path to ESP-IDF directory::

    export IDF_PATH=~/esp/esp-idf

At this point you may configure the serial port to be used for uploading. Run::

    make menuconfig

Then navigate to "Serial flasher config" submenu and change value of "Default serial port" to match the serial port you will use. Also take a moment to explore other options which are configurable in ``menuconfig``.

If you don't know device name for the  serial port of your development board, run this command two times, first with the board unplugged, then with the board plugged in. The port which appears the second time is the one you need::

    ls /dev/tty.*

Now you can build and flash the application. Run::

    make flash

This will compile the application and all the ESP-IDF components, generate bootloader, partition table, and application binaries, and flash these binaries to your development board.

Further reading
===============

If you'd like to use the Eclipse IDE instead of running ``make``, check out the Eclipse setup guide in this directory.
