***************************************
Building OpenOCD from Sources for Linux
***************************************
:link_to_translation:`zh_CN:[中文]`

The following instructions are alternative to downloading binary OpenOCD from `Espressif GitHub <https://github.com/espressif/openocd-esp32/releases>`_. To quickly setup the binary OpenOCD, instead of compiling it yourself, backup and proceed to section :ref:`jtag-debugging-setup-openocd`.


.. highlight:: bash

Download Sources of OpenOCD
===========================

The sources for the ESP32-enabled variant of OpenOCD are available from Espressif GitHub under https://github.com/espressif/openocd-esp32. To download the sources, use the following commands::

    cd ~/esp
    git clone --recursive https://github.com/espressif/openocd-esp32.git

The clone of sources should be now saved in ``~/esp/openocd-esp32`` directory.


Install Dependencies
====================

Install packages that are required to compile OpenOCD.

.. note::

    Install the following packages one by one, check if installation was successful and then proceed to the next package. Resolve reported problems before moving to the next step.

::

    sudo apt-get install make
    sudo apt-get install libtool
    sudo apt-get install pkg-config
    sudo apt-get install autoconf
    sudo apt-get install automake
    sudo apt-get install texinfo
    sudo apt-get install libusb-1.0

.. note::

    * Version of pkg-config should be 0.2.3 or above.
    * Version of autoconf should be 2.6.4 or above.
    * Version of automake should be 1.9 or above.
    * When using USB-Blaster, ASIX Presto, OpenJTAG and FT2232 as adapters, drivers libFTDI and FTD2XX need to be downloaded and installed.
    * When using CMSIS-DAP, HIDAPI is needed.


Build OpenOCD
=============

Proceed with configuring and building OpenOCD::

    cd ~/esp/openocd-esp32
    ./bootstrap
    ./configure
    make

Optionally you can add ``sudo make install`` step at the end. Skip it, if you have an existing OpenOCD (from e.g. another development platform), as it may get overwritten.

.. note::

    * Should an error occur, resolve it and try again until the command ``make`` works. 
    * If there is a submodule problem from OpenOCD, please ``cd`` to the ``openocd-esp32`` directory and input ``git submodule update --init``.
    * If the ``./configure`` is successfully run, information of enabled JTAG will be printed under ``OpenOCD configuration summary``. 
    * If the information of your device is not shown in the log, use ``./configure`` to enable it as described in  ``../openocd-esp32/doc/INSTALL.txt``.
    * For details concerning compiling OpenOCD, please refer to ``openocd-esp32/README``.

Once ``make`` process is successfully completed, the executable of OpenOCD will be saved in ``~/openocd-esp32/bin`` directory.


Next Steps
==========

To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-configuring-esp32-target`.
