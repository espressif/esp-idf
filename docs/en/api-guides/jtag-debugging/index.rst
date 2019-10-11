JTAG Debugging
==============
:link_to_translation:`zh_CN:[中文]`

This document provides a guide to installing OpenOCD for ESP32 and debugging using
GDB. The document is structured as follows:

:ref:`jtag-debugging-introduction`
    Introduction to the purpose of this guide.
:ref:`jtag-debugging-how-it-works`
    Description how ESP32, JTAG interface, OpenOCD and GDB are interconnected and working together to enable debugging of ESP32.
:ref:`jtag-debugging-selecting-jtag-adapter`
    What are the criteria and options to select JTAG adapter hardware.
:ref:`jtag-debugging-setup-openocd`
    Procedure to install OpenOCD and verify that it is installed.
:ref:`jtag-debugging-configuring-esp32-target`
    Configuration of OpenOCD software and set up JTAG adapter hardware that will make together a debugging target.
:ref:`jtag-debugging-launching-debugger`
    Steps to start up a debug session with GDB from :ref:`jtag-debugging-using-debugger-eclipse` and from :ref:`jtag-debugging-using-debugger-command-line`.
:ref:`jtag-debugging-examples`
    If you are not familiar with GDB, check this section for debugging examples provided from :ref:`jtag-debugging-examples-eclipse` as well as from :ref:`jtag-debugging-examples-command-line`.
:ref:`jtag-debugging-building-openocd`
    Procedure to build OpenOCD from sources for :doc:`Windows <building-openocd-windows>`, :doc:`Linux <building-openocd-linux>` and :doc:`MacOS <building-openocd-macos>` operating systems.
:ref:`jtag-debugging-tips-and-quirks`
    This section provides collection of tips and quirks related JTAG debugging of ESP32 with OpenOCD and GDB.


.. _jtag-debugging-introduction:

Introduction
------------

The ESP32 has two powerful Xtensa cores, allowing for a great deal of variety of program architectures. The FreeRTOS OS that comes with ESP-IDF is capable of multi-core preemptive multithreading, allowing for an intuitive way of writing software.

The downside of the ease of programming is that debugging without the right tools is harder: figuring out a bug that is caused by two threads, running even simultaneously on two different CPU cores, can take a long time when all you have are printf statements. A better and in many cases quicker way to debug such problems is by using a debugger, connected to the processors over a debug port.

Espressif has ported OpenOCD to support the ESP32 processor and the multicore FreeRTOS, which will be the foundation of most ESP32 apps, and has written some tools to help with features OpenOCD does not support natively.

This document provides a guide to installing OpenOCD for ESP32 and debugging using GDB under Linux, Windows and MacOS. Except for OS specific installation procedures, the s/w user interface and use procedures are the same across all supported operating systems.

.. note::

    Screenshots presented in this document have been made for Eclipse Neon 3 running on Ubuntu 16.04 LTE. There may be some small differences in what a particular user interface looks like, depending on whether you are using Windows, MacOS or Linux and / or a different release of Eclipse.

.. _jtag-debugging-how-it-works:

How it Works?
-------------

The key software and hardware to perform debugging of ESP32 with OpenOCD over JTAG (Joint Test Action Group) interface is presented below and includes **xtensa-esp32-elf-gdb debugger**, **OpenOCD on chip debugger** and **JTAG adapter** connected to **ESP32** target.

.. figure:: ../../../_static/jtag-debugging-overview.jpg
    :align: center
    :alt: JTAG debugging - overview diagram 
    :figclass: align-center

    JTAG debugging - overview diagram 

Under "Application Loading and Monitoring" there is another software and hardware to compile, build and flash application to ESP32, as well as to provide means to monitor diagnostic messages from ESP32.

Debugging using JTAG and application loading / monitoring is integrated under the `Eclipse <https://www.eclipse.org/>`_ environment, to provide quick and easy transition from writing, compiling and loading the code to debugging, back to writing the code, and so on. All the software is available for Windows, Linux and MacOS platforms.

If the :doc:`ESP-WROVER-KIT <../../hw-reference/modules-and-boards>` is used, then connection from PC to ESP32 is done effectively with a single USB cable thanks to FT2232H chip installed on WROVER, which provides two USB channels, one for JTAG and the second for UART connection.

Depending on user preferences, both `debugger` and `idf.py build` can be operated directly from terminal / command line, instead from Eclipse.


.. _jtag-debugging-selecting-jtag-adapter:

Selecting JTAG Adapter
----------------------

The quickest and most convenient way to start with JTAG debugging is by using :doc:`ESP-WROVER-KIT <../../hw-reference/modules-and-boards>`. Each version of this development board has JTAG interface already build in. No need for an external JTAG adapter and extra wiring / cable to connect JTAG to ESP32. WROVER KIT is using FT2232H JTAG interface operating at 20 MHz clock speed, which is difficult to achieve with an external adapter.

If you decide to use separate JTAG adapter, look for one that is compatible with both the voltage levels on the ESP32 as well as with the OpenOCD software. The JTAG port on the ESP32 is an industry-standard JTAG port which lacks (and does not need) the TRST pin. The JTAG I/O pins all are powered from the VDD_3P3_RTC pin (which normally would be powered by a 3.3 V rail) so the JTAG adapter needs to be able to work with JTAG pins in that voltage range. 

On the software side, OpenOCD supports a fair amount of JTAG adapters. See http://openocd.org/doc/html/Debug-Adapter-Hardware.html for an (unfortunately slightly incomplete) list of the adapters OpenOCD works with. This page lists SWD-compatible adapters as well; take note that the ESP32 does not support SWD. JTAG adapters that are hardcoded to a specific product line, e.g. ST-LINK debugging adapters for STM32 families, will not work.

The minimal signalling to get a working JTAG connection are TDI, TDO, TCK, TMS and GND. Some JTAG debuggers also need a connection from the ESP32 power line to a line called e.g. Vtar to set the working voltage. SRST can optionally be connected to the CH_PD of the ESP32, although for now, support in OpenOCD for that line is pretty minimal.


.. _jtag-debugging-setup-openocd:

Setup of OpenOCD
----------------

.. highlight:: bash

If you have already set up ESP-IDF with CMake build system according to the :doc:`Getting Started Guide <../../get-started/index>`, then OpenOCD is already installed. After :ref:`setting up the environment <get-started-set-up-env>` in your terminal, you should be able to run OpenOCD. Check this by executing the following command::

    openocd --version

.. highlight:: none

The output should be as follows (although the version may be more recent than listed here)::

    Open On-Chip Debugger  v0.10.0-esp32-20190708 (2019-07-08-11:04)
    Licensed under GNU GPL v2
    For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html

You may also verify that OpenOCD knows where its configuration scripts are located by printing the value of ``OPENOCD_SCRIPTS`` environment variable, by typing ``echo $OPENOCD_SCRIPTS`` (for Linux and macOS) or ``echo %OPENOCD_SCRIPTS%`` (for Windows). If a valid path is printed, then OpenOCD is set up correctly.

If any of these steps do not work, please go back to the :ref:`setting up the tools <get-started-set-up-tools>` section of the Getting Started Guide.

.. note::

    It is also possible to build OpenOCD from source. Please refer to :ref:`jtag-debugging-building-openocd` section for details.

.. _jtag-debugging-configuring-esp32-target:

Configuring ESP32 Target
------------------------

Once OpenOCD is installed, move to configuring ESP32 target (i.e ESP32 board with JTAG interface). You will do it in the following three steps:

* Configure and connect JTAG interface
* Run OpenOCD
* Upload application for debugging


Configure and connect JTAG interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This step depends on JTAG and ESP32 board you are using - see the two cases described below.

.. toctree::
    :maxdepth: 1

    configure-wrover
    configure-other-jtag


.. _jtag-debugging-run-openocd:

Run OpenOCD
^^^^^^^^^^^

Once target is configured and connected to computer, you are ready to launch OpenOCD.

.. highlight:: bash

Open a terminal and set it up for using the ESP-IDF as described in the :ref:`setting up the environment <get-started-set-up-env>` section of the Getting Started Guide. Then run OpenOCD (this command works on Windows, Linux, and macOS)::

    openocd -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg

.. note::

    The files provided after ``-f`` above are specific for ESP-WROVER-KIT with :ref:`esp-modules-and-boards-esp32-wroom-32` module. You may need to provide different files depending on used hardware. For guidance see :ref:`jtag-debugging-tip-openocd-configure-target`.

.. highlight:: none

You should now see similar output (this log is for ESP-WROVER-KIT)::

    user-name@computer-name:~/esp/esp-idf$ openocd -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg
    Open On-Chip Debugger  v0.10.0-esp32-20190708 (2019-07-08-11:04)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    none separate
    adapter speed: 20000 kHz
    force hard breakpoints
    Info : ftdi: if you experience problems at higher adapter clocks, try the command "ftdi_tdo_sample_edge falling"
    Info : clock speed 20000 kHz
    Info : JTAG tap: esp32.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : JTAG tap: esp32.cpu1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
    Info : esp32: Debug controller was reset (pwrstat=0x5F, after clear 0x0F).
    Info : esp32: Core was reset (pwrstat=0x5F, after clear 0x0F).

* If there is an error indicating permission problems, please see the "Permissions delegation" bit in the OpenOCD README file in ``~/esp/openocd-esp32`` directory.
* In case there is an error finding configuration files, e.g. ``Can't find interface/ftdi/esp32_devkitj_v1.cfg``, check the path after ``-s``. This path is used by OpenOCD to look for the files specified after ``-f``. Also check if the file is indeed under provided path. 
* If you see JTAG errors (...all ones/...all zeroes) please check your connections, whether no other signals are connected to JTAG besides ESP32's pins, and see if everything is powered on.


.. _jtag-upload-app-debug:

Upload application for debugging
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Build and upload your application to ESP32 as usual, see :ref:`get-started-build`.

Another option is to write application image to flash using OpenOCD via JTAG with commands like this::

    openocd -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg -c "program_esp32 filename.bin 0x10000 verify exit"

OpenOCD flashing command ``program_esp32`` has the following format:

``program_esp32 <image_file> <offset> [verify] [reset] [exit]``

 - ``image_file`` - Path to program image file.
 - ``offset`` - Offset in flash bank to write image.
 - ``verify`` - Optional. Verify flash contents after writing.
 - ``reset`` - Optional. Reset target after programing.
 - ``exit`` - Optional. Finally exit OpenOCD.

You are now ready to start application debugging. Follow steps described in section below.


.. _jtag-debugging-launching-debugger:

Launching Debugger
------------------

The toolchain for ESP32 features GNU Debugger, in short GDB. It is available with other toolchain programs under filename ``xtensa-esp32-elf-gdb``. GDB can be called and operated directly from command line in a terminal. Another option is to call it from within IDE (like Eclipse, Visual Studio Code, etc.) and operate indirectly with help of GUI instead of typing commands in a terminal. 

Both options of using debugger are discussed under links below. 

* :ref:`jtag-debugging-using-debugger-eclipse`
* :ref:`jtag-debugging-using-debugger-command-line`

It is recommended to first check if debugger works from :ref:`jtag-debugging-using-debugger-command-line` and then move to using :ref:`jtag-debugging-using-debugger-eclipse`.


.. _jtag-debugging-examples:

Debugging Examples
------------------

This section is intended for users not familiar with GDB. It presents example debugging session from :ref:`jtag-debugging-examples-eclipse` using simple application available under :example:`get-started/blink` and covers the following debugging actions:

1. :ref:`jtag-debugging-examples-eclipse-01`
2. :ref:`jtag-debugging-examples-eclipse-02`
3. :ref:`jtag-debugging-examples-eclipse-03`
4. :ref:`jtag-debugging-examples-eclipse-04`
5. :ref:`jtag-debugging-examples-eclipse-05`
6. :ref:`jtag-debugging-examples-eclipse-06`
7. :ref:`jtag-debugging-examples-eclipse-07`

Similar debugging actions are provided using GDB from :ref:`jtag-debugging-examples-command-line`. 

Before proceeding to examples, set up your ESP32 target and load it with :example:`get-started/blink`.


.. _jtag-debugging-building-openocd:

Building OpenOCD from Sources
-----------------------------

Please refer to separate documents listed below, that describe build process.

.. toctree::
    :maxdepth: 1

    Windows <building-openocd-windows>
    Linux <building-openocd-linux>
    MacOS <building-openocd-macos>

The examples of invoking OpenOCD in this document assume using pre-built binary distribution described in section :ref:`jtag-debugging-setup-openocd`.

.. highlight:: bash

To use binaries build locally from sources, change the path to OpenOCD executable to ``src/openocd`` and set the ``OPENOCD_SCRIPTS`` environment variable so that OpenOCD can find the configuration files. For Linux and macOS::

    cd ~/esp/openocd-esp32
    export OPENOCD_SCRIPTS=$PWD/tcl

For Windows::

    cd %USERPROFILE%\esp\openocd-esp32
    set "OPENOCD_SCRIPTS=%CD%\tcl"

Example of invoking OpenOCD build locally from sources, for Linux and macOS::

    src/openocd -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg

and Windows::

    src\openocd -f interface\ftdi\esp32_devkitj_v1.cfg -f board\esp-wroom-32.cfg


.. _jtag-debugging-tips-and-quirks:

Tips and Quirks
---------------

This section provides collection of links to all tips and quirks referred to from various parts of this guide.

* :ref:`jtag-debugging-tip-breakpoints`
* :ref:`jtag-debugging-tip-where-breakpoints`
* :ref:`jtag-debugging-tip-flash-mappings`
* :ref:`jtag-debugging-tip-why-next-works-as-step`
* :ref:`jtag-debugging-tip-code-options`
* :ref:`jtag-debugging-tip-freertos-support`
* :ref:`jtag-debugging-tip-code-flash-voltage`
* :ref:`jtag-debugging-tip-optimize-jtag-speed`
* :ref:`jtag-debugging-tip-debugger-startup-commands`
* :ref:`jtag-debugging-tip-openocd-configure-target`
* :ref:`jtag-debugging-tip-reset-by-debugger`
* :ref:`jtag-debugging-tip-jtag-pins-reconfigured`
* :ref:`jtag-debugging-tip-reporting-issues`


Related Documents
-----------------

.. toctree::
    :maxdepth: 1

    using-debugger
    debugging-examples
    tips-and-quirks
    ../app_trace
