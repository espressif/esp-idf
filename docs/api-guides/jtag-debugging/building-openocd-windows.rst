*****************************************
Building OpenOCD from Sources for Windows
*****************************************

The following instructions are alternative to downloading binary OpenOCD from Espressif website. To quickly setup the binary OpenOCD, instead of compiling it yourself, backup and proceed to section :doc:`setup-openocd-windows`.


.. highlight:: bash

Download Sources of OpenOCD
===========================

The sources for the ESP32-enabled variant of OpenOCD are available from Espressif GitHub under https://github.com/espressif/openocd-esp32. To download the sources, use the following commands::

    cd ~/esp
    git clone –recursive https://github.com/espressif/openocd-esp32.git

The clone of sources should be now saved in ``~/esp/openocd-esp32`` directory.


Install Dependencies
====================

Install packages that are required to compile OpenOCD:

.. note::

    Install the following packages one by one, check if installation was successful and then proceed to the next package. Resolve reported problems before moving to the next step.

::

	pacman -S libtool
	pacman -S autoconf
	pacman -S automake
	pacman -S texinfo
	pacman -S mingw-w64-i686-libusb-compat-git
	pacman -S pkg-config

.. note::

	Installation of ``pkg-config`` is breaking operation of esp-idf toolchain. After building of OpenOCD it should be uninstalled. It be covered at the end of this instruction. To build OpenOCD again, you will need to run ``pacman -S pkg-config`` once more. This issue does not concern other packages installed in this step (before ``pkg-config``).


Build OpenOCD
=============

Proceed with configuring and building OpenOCD::

    cd ~/esp/openocd-esp32
    ./bootstrap
    ./configure
    make

Optionally you can add ``make install`` step at the end. Skip it, if you have an existing OpenOCD (from e.g. another development platform), as it may get overwritten.

.. note::

    * Should an error occur, resolve it and try again until the command ``make`` works. 
    * If there is a submodule problem from OpenOCD, please ``cd`` to the ``openocd-esp32`` directory and input ``git submodule update --init``.
    * If the ``./configure`` is successfully run, information of enabled JTAG will be printed under ``OpenOCD configuration summary``. 
    * If the information of your device is not shown in the log, use ``./configure`` to enable it as described in  ``../openocd-esp32/doc/INSTALL.txt``.
    * For details concerning compiling OpenOCD, please refer to ``openocd-esp32/README.Windows``.

Once ``make`` process is successfully completed, the executable of OpenOCD will be saved in ``~/esp/openocd-esp32/src/openocd`` directory.

Remove ``pkg-config``, as discussed during installation of dependencies::

	pacman -Rs pkg-config


Next Steps
==========

To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-configuring-esp32-target`.
