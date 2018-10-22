﻿***********
Get Started
***********
:link_to_translation:`zh_CN:[中文]`

This document is intended to help users set up the software environment for development of applications using hardware based on the Espressif ESP32. Through a simple example we would like to illustrate how to use ESP-IDF (Espressif IoT Development Framework), including the menu based configuration, compiling the ESP-IDF and firmware download to ESP32 boards. 

.. include:: /_build/inc/version-note.inc

Introduction
============

ESP32 integrates Wi-Fi (2.4 GHz band) and Bluetooth 4.2 solutions on a single chip, along with dual high performance cores, Ultra Low Power co-processor and several peripherals. Powered by 40 nm technology, ESP32 provides a robust, highly integrated platform to meet the continuous demands for efficient power usage, compact design, security, high performance, and reliability.

Espressif provides the basic hardware and software resources that help application developers to build their ideas around the ESP32 series hardware. The software development framework by Espressif is intended for rapidly developing Internet-of-Things (IoT) applications, with Wi-Fi, Bluetooth, power management and several other system features. 


What You Need
=============

To develop applications for ESP32 you need:

* **PC** loaded with either Windows, Linux or Mac operating system
* **Toolchain** to build the **Application** for ESP32
* **ESP-IDF** that essentially contains API for ESP32 and scripts to operate the **Toolchain**
* A text editor to write programs (**Projects**) in C, e.g. `Eclipse <https://www.eclipse.org/>`_
* The **ESP32** board itself and a **USB cable** to connect it to the **PC**

.. figure:: ../../_static/what-you-need.png
    :align: center
    :alt: Development of applications for ESP32
    :figclass: align-center

    Development of applications for ESP32

Preparation of development environment consists of three steps:

1. Setup of **Toolchain**
2. Getting of **ESP-IDF** from GitHub
3. Installation and configuration of **Eclipse**

You may skip the last step, if you prefer to use different editor.

Having environment set up, you are ready to start the most interesting part - the application development. This process may be summarized in four steps:

1. Configuration of a **Project** and writing the code
2. Compilation of the **Project** and linking it to build an **Application**
3. Flashing (uploading) of the **Application** to **ESP32**
4. Monitoring / debugging of the **Application**

See instructions below that will walk you through these steps.


Guides
======

If you have one of ESP32 development boards listed below, click on provided links to get you up and running.

.. toctree::
    :maxdepth: 1

    ESP32 DevKitC <get-started-devkitc>
    ESP-WROVER-KIT <get-started-wrover-kit>
    ESP32-PICO-KIT <get-started-pico-kit>

If you have different board, move to sections below.


.. _get-started-setup-toolchain:

Setup Toolchain
===============

The quickest way to start development with ESP32 is by installing a prebuilt toolchain. Pick up your OS below and follow provided instructions. 

.. toctree::
    :hidden:

    Windows <windows-setup>
    Linux <linux-setup> 
    MacOS <macos-setup> 

+-------------------+-------------------+-------------------+
| |windows-logo|    | |linux-logo|      | |macos-logo|      |
+-------------------+-------------------+-------------------+
| `Windows`_        | `Linux`_          | `Mac OS`_         |
+-------------------+-------------------+-------------------+

.. |windows-logo| image:: ../../_static/windows-logo.png
    :target: ../get-started/windows-setup.html

.. |linux-logo| image:: ../../_static/linux-logo.png
    :target: ../get-started/linux-setup.html

.. |macos-logo| image:: ../../_static/macos-logo.png
    :target: ../get-started/macos-setup.html

.. _Windows: ../get-started/windows-setup.html
.. _Linux: ../get-started/linux-setup.html
.. _Mac OS: ../get-started/macos-setup.html

.. note::

    We are using ``~/esp`` directory to install the prebuilt toolchain, ESP-IDF and sample applications. You can use different directory, but need to adjust respective commands.

Depending on your experience and preferences, instead of using a prebuilt toolchain, you may want to customize your environment. To set up the system your own way go to section :ref:`get-started-customized-setup`.

Once you are done with setting up the toolchain then go to section :ref:`get-started-get-esp-idf`.


.. _get-started-get-esp-idf:

Get ESP-IDF
===========

.. highlight:: bash

Besides the toolchain (that contains programs to compile and build the application), you also need ESP32 specific API / libraries. They are provided by Espressif in `ESP-IDF repository <https://github.com/espressif/esp-idf>`_.

.. include:: /_build/inc/git-clone.inc

Consult :doc:`/versions` for information about which version of ESP-IDF to use in a given situation.

.. note::

    Do not miss the ``--recursive`` option. If you have already cloned ESP-IDF without this option, run another command to get all the submodules::

        cd ~/esp/esp-idf
        git submodule update --init --recursive


.. _get-started-setup-path:

Setup Path to ESP-IDF
=====================

The toolchain programs access ESP-IDF using ``IDF_PATH`` environment variable. This variable should be set up on your PC, otherwise projects will not build. Setting may be done manually, each time PC is restarted. Another option is to set up it permanently by defining ``IDF_PATH`` in user profile. To do so, follow instructions specific to :ref:`Windows <add-idf_path-to-profile-windows>` , :ref:`Linux and MacOS <add-idf_path-to-profile-linux-macos>` in section :doc:`add-idf_path-to-profile`.

.. _get-started-get-packages:

Install the Required Python Packages
====================================

Python packages required by ESP-IDF are located in the ``$IDF_PATH/requirements.txt`` file. You can install them by running::

    python -m pip install --user -r $IDF_PATH/requirements.txt

.. note::

    Please invoke that version of the Python interpreter which you will be using with ESP-IDF. The version of the
    interpreter can be checked by running command ``python --version`` and depending on the result, you might want to
    use ``python2``, ``python2.7`` or similar instead of ``python``, e.g.::

        python2.7 -m pip install --user -r $IDF_PATH/requirements.txt

.. _get-started-start-project:

Start a Project
===============

Now you are ready to prepare your application for ESP32. To start off quickly, we will use :example:`get-started/hello_world` project from :idf:`examples` directory in IDF.

Copy :example:`get-started/hello_world` to ``~/esp`` directory::

    cd ~/esp
    cp -r $IDF_PATH/examples/get-started/hello_world .

You can also find a range of example projects under the :idf:`examples` directory in ESP-IDF. These example project directories can be copied in the same way as presented above, to begin your own projects.

.. important::

    The esp-idf build system does not support spaces in paths to esp-idf or to projects.


.. _get-started-connect:

Connect
=======

You are almost there. To be able to proceed further, connect ESP32 board to PC, check under what serial port the board is visible and verify if serial communication works. If you are not sure how to do it, check instructions in section :doc:`establish-serial-connection`. Note the port number, as it will be required in the next step.


.. _get-started-configure:

Configure
=========

Being in terminal window, go to directory of ``hello_world`` application by typing ``cd ~/esp/hello_world``. Then start project configuration utility ``menuconfig``::

    cd ~/esp/hello_world
    make menuconfig

If previous steps have been done correctly, the following menu will be displayed: 

.. figure:: ../../_static/project-configuration.png
    :align: center
    :alt: Project configuration - Home window
    :figclass: align-center

    Project configuration - Home window

In the menu, navigate to ``Serial flasher config`` > ``Default serial port`` to configure the serial port, where project will be loaded to. Confirm selection by pressing enter, save configuration by selecting ``< Save >`` and then exit application by selecting ``< Exit >``.

.. note::

   On Windows, serial ports have names like COM1. On MacOS, they start with ``/dev/cu.``. On Linux, they start with ``/dev/tty``.
   (See :doc:`establish-serial-connection` for full details.)

Here are couple of tips on navigation and use of ``menuconfig``:

* Use up & down arrow keys to navigate the menu.
* Use Enter key to go into a submenu, Escape key to go out or to exit.
* Type ``?`` to see a help screen. Enter key exits the help screen.
* Use Space key, or ``Y`` and ``N`` keys to enable (Yes) and disable (No) configuration items with checkboxes "``[*]``"
* Pressing ``?`` while highlighting a configuration item displays help about that item.
* Type ``/`` to search the configuration items.

.. note::

    If you are **Arch Linux** user, navigate to ``SDK tool configuration`` and change the name of ``Python 2 interpreter`` from ``python`` to ``python2``.


.. attention::

    When using ESP32-DevKitC board with ESP32-SOLO-1 module, enable single core mode (:ref:`CONFIG_FREERTOS_UNICORE`) in menuconfig before flashing example applications.


.. _get-started-build-flash:

Build and Flash
===============

Now you can build and flash the application. Run::

    make flash

This will compile the application and all the ESP-IDF components, generate bootloader, partition table, and application binaries, and flash these binaries to your ESP32 board.

.. highlight:: none

::

    esptool.py v2.0-beta2
    Flashing binaries to serial port /dev/ttyUSB0 (app at offset 0x10000)...
    esptool.py v2.0-beta2
    Connecting........___
    Uploading stub...
    Running stub...
    Stub running...
    Changing baud rate to 921600
    Changed.
    Attaching SPI flash...
    Configuring flash size...
    Auto-detected Flash size: 4MB
    Flash params set to 0x0220
    Compressed 11616 bytes to 6695...
    Wrote 11616 bytes (6695 compressed) at 0x00001000 in 0.1 seconds (effective 920.5 kbit/s)...
    Hash of data verified.
    Compressed 408096 bytes to 171625...
    Wrote 408096 bytes (171625 compressed) at 0x00010000 in 3.9 seconds (effective 847.3 kbit/s)...
    Hash of data verified.
    Compressed 3072 bytes to 82...
    Wrote 3072 bytes (82 compressed) at 0x00008000 in 0.0 seconds (effective 8297.4 kbit/s)...
    Hash of data verified.

    Leaving...
    Hard resetting...

If there are no issues, at the end of build process, you should see messages describing progress of loading process. Finally, the end module will be reset and "hello_world" application will start.

If you'd like to use the Eclipse IDE instead of running ``make``, check out the :doc:`Eclipse guide <eclipse-setup>`.


.. _get-started-build-monitor:

Monitor
=======

To see if "hello_world" application is indeed running, type ``make monitor``. This command is launching :doc:`IDF Monitor <idf-monitor>` application::

    $ make monitor
    MONITOR
    --- idf_monitor on /dev/ttyUSB0 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57
    ...

Several lines below, after start up and diagnostic log, you should see "Hello world!" printed out by the application. ::

    ...
    Hello world!
    Restarting in 10 seconds...
    I (211) cpu_start: Starting scheduler on APP CPU.
    Restarting in 9 seconds...
    Restarting in 8 seconds...
    Restarting in 7 seconds...

To exit the monitor use shortcut ``Ctrl+]``. 

.. note::

    If instead of the messages above, you see a random garbage similar to::

        e���)(Xn@�y.!��(�PW+)��Hn9a؅/9�!�t5��P�~�k��e�ea�5�jA
        ~zY��Y(1�,1�� e���)(Xn@�y.!Dr�zY(�jpi�|�+z5Ymvp

    or monitor fails shortly after upload, your board is likely using 26MHz crystal, while the ESP-IDF assumes default of 40MHz. Exit the monitor, go back to the :ref:`menuconfig <get-started-configure>`, change :ref:`CONFIG_ESP32_XTAL_FREQ_SEL` to 26MHz, then :ref:`build and flash <get-started-build-flash>` the application again. This is found under ``make menuconfig`` under Component config --> ESP32-specific --> Main XTAL frequency.

To execute ``make flash`` and ``make monitor`` in one go, type ``make flash monitor``. Check section :doc:`IDF Monitor <idf-monitor>` for handy shortcuts and more details on using this application.

That's all what you need to get started with ESP32! 

Now you are ready to try some other :idf:`examples`, or go right to developing your own applications.


Environment Variables
=====================

Some environment variables can be specified whilst calling ``make`` allowing users to **override arguments without needing to reconfigure them using** ``make menuconfig``.

+-----------------+--------------------------------------------------------------+
| Variables       | Description & Usage                                          |
+=================+==============================================================+
| ``ESPPORT``     | Overrides the serial port used in ``flash`` and ``monitor``. |
|                 |                                                              |
|                 | Examples: ``make flash ESPPORT=/dev/ttyUSB1``,               |
|                 | ``make monitor ESPPORT=COM1``                                |
+-----------------+--------------------------------------------------------------+
| ``ESPBAUD``     | Overrides the serial baud rate when flashing the ESP32.      |
|                 |                                                              |
|                 | Example: ``make flash ESPBAUD=9600``                         |
+-----------------+--------------------------------------------------------------+
| ``MONITORBAUD`` | Overrides the serial baud rate used when monitoring.         |
|                 |                                                              |
|                 | Example: ``make monitor MONITORBAUD=9600``                   |
+-----------------+--------------------------------------------------------------+

.. note::
    Users can export environment variables (e.g. ``export ESPPORT=/dev/ttyUSB1``).
    All subsequent calls of ``make`` within the same terminal session will use 
    the exported value given that the variable is not simultaneously overridden.

Updating ESP-IDF
================

After some time of using ESP-IDF, you may want to update it to take advantage of new features or bug fixes. The simplest way to do so is by deleting existing ``esp-idf`` folder and cloning it again, exactly as when doing initial installation described in sections :ref:`get-started-get-esp-idf`.

If downloading to a new path, remember to :doc:`add-idf_path-to-profile` so that the toolchain scripts know where to find the ESP-IDF in its release specific location.

Another solution is to update only what has changed. :ref:`The update procedure depends on the version of ESP-IDF you are using <updating>`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    add-idf_path-to-profile
    establish-serial-connection
    make-project
    eclipse-setup
    idf-monitor
    toolchain-setup-scratch

.. _Stable version: https://docs.espressif.com/projects/esp-idf/en/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases

