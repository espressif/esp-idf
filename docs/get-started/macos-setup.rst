**************************************
Standard Setup of Toolchain for Mac OS
**************************************

Install Prerequisites
=====================

- install pip::

    sudo easy_install pip

- install pyserial::

    sudo pip install pyserial


Toolchain Setup
===============

ESP32 toolchain for macOS is available for download from Espressif website:

https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-75-gbaf03c2-5.2.0.tar.gz

Download this file, then extract it in ``~/esp`` directory::

    mkdir -p ~/esp
    cd ~/esp
    tar -xzf ~/Downloads/xtensa-esp32-elf-osx-1.22.0-75-gbaf03c2-5.2.0.tar.gz

.. _setup-macos-toolchain-add-it-to-path:

The toolchain will be extracted into ``~/esp/xtensa-esp32-elf/`` directory.

To use it, you will need to update your ``PATH`` environment variable in ``~/.profile`` file. To make ``xtensa-esp32-elf`` available for all terminal sessions, add the following line to your ``~/.profile`` file::

    export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin

Alternatively, you may create an alias for the above command. This way you can get the toolchain only when you need it. To do this, add different line to your ``~/.profile`` file::

    alias get_esp32="export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin"

Then when you need the toolchain you can type ``get_esp32`` on the command line and the toolchain will be added to your ``PATH``.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    macos-setup-scratch
