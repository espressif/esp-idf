Set up of Toolchain for Linux
*****************************

Step 0: Prerequisites
=====================

Install some packages
---------------------

To compile with ESP-IDF you need to get the following packages:

- Ubuntu and Debian::
    
    sudo apt-get install git wget make libncurses-dev flex bison gperf python python-serial 

- Arch::
    
    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial

Step 1: Download binary toolchain for the ESP32
==================================================

ESP32 toolchain for Linux is available for download from Espressif website:

- for 64-bit Linux::

    https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-59.tar.gz

- for 32-bit Linux::

    https://dl.espressif.com/dl/xtensa-esp32-elf-linux32-1.22.0-59.tar.gz

Download this file, then extract it to the location you prefer, for example::

    mkdir -p ~/esp
    cd ~/esp
    tar -xzf ~/Downloads/xtensa-esp32-elf-linux64-1.22.0-59.tar.gz

The toolchain will be extracted into ``~/esp/xtensa-esp32-elf/`` directory.

To use it, you will need to update your ``PATH`` environment variable in ``~/.bash_profile`` file. To make ``xtensa-esp32-elf`` available for all terminal sessions, add the following line to your ``~/.bash_profile`` file::

    export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin

Alternatively, you may create an alias for the above command. This way you can get the toolchain only when you need it. To do this, add different line to your ``~/.bash_profile`` file::

    alias get_esp32="export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin"

Then when you need the toolchain you can type ``get_esp32`` on the command line and the toolchain will be added to your ``PATH``.

Arch Linux Users
----------------

To run the precompiled gdb (xtensa-esp32-elf-gdb) in Arch Linux requires ncurses 5, but Arch uses ncurses 6. Backwards compatibility libraries are available in AUR_ for native and lib32 configurations:
- https://aur.archlinux.org/packages/ncurses5-compat-libs/
- https://aur.archlinux.org/packages/lib32-ncurses5-compat-libs/

(Alternatively, use crosstool-NG to compile a gdb that links against ncurses 6.)


Alternative Step 1: Compile the toolchain from source using crosstool-NG
========================================================================

Instead of downloading binary toolchain from Espressif website (Step 1 above) you may build the toolchain yourself. 

If you can't think of a reason why you need to build it yourself, then probably it's better to stick with the binary version. However, here are some of the reasons why you might want to compile it from source:

- if you want to customize toolchain build configuration

- if you want to hack gcc or newlib or libstdc++

- if you are curious and/or have time to spare

- if you don't trust binaries downloaded from the Internet

In any case, here are the steps to compile the toolchain yourself.

- Install dependencies:

  - Ubuntu::

        sudo apt-get install gawk gperf grep gettext ncurses python python-dev automake bison flex texinfo help2man libtool

  - Debian::

        TODO

  - Arch::

        TODO

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

Open terminal, navigate to the directory you want to clone ESP-IDF and clone it using ``git clone`` command::

    cd ~/esp
    git clone --recursive https://github.com/espressif/esp-idf.git


ESP-IDF will be downloaded into ``~/esp/esp-idf``. 

Note the ``--recursive`` option! If you have already cloned ESP-IDF without this option, run another command to get all the submodules::

    cd ~/esp/esp-idf
    git submodule update --init

**IMPORTANT:** The esp-idf build system does not support spaces in paths to esp-idf or to projects.

Step 3: Starting a project
==========================

ESP-IDF by itself does not build a binary to run on the ESP32. The binary "app" comes from a project in a different directory. Multiple projects can share the same ESP-IDF directory.

The easiest way to start a project is to download the template project from GitHub::

    cd ~/esp
    git clone https://github.com/espressif/esp-idf-template.git myapp

This will download ``esp-idf-template`` project into ``~/esp/myapp`` directory.


Step 4: Building and flashing the application
=============================================

In terminal, go to the application directory which was obtained on the previous step::

    cd ~/esp/myapp

Type a command like this to set the path to ESP-IDF directory:: 

    export IDF_PATH=~/esp/esp-idf

At this point you may configure the serial port to be used for uploading. Run::

    make menuconfig

Then navigate to "Serial flasher config" submenu and change value of "Default serial port" to match the serial port you will use. Also take a moment to explore other options which are configurable in ``menuconfig``.

Special note for Arch Linux users: navigate to "SDK tool configuration" and change the name of "Python 2 interpreter" from ``python`` to ``python2``.

Now you can build and flash the application. Run::

    make flash

This will compile the application and all the ESP-IDF components, generate bootloader, partition table, and application binaries, and flash these binaries to your development board.

Further reading
===============

If you'd like to use the Eclipse IDE instead of running ``make``, check out the Eclipse setup guide in this directory.

.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
