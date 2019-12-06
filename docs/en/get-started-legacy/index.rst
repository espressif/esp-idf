*****************************
Get Started (Legacy GNU Make)
*****************************

:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst

This document is intended to help you set up the software development environment for the hardware based on Espressif ESP32.

After that, a simple example will show you how to use ESP-IDF (Espressif IoT Development Framework) for menu configuration, then how to build and flash firmware onto an ESP32 board.

.. include:: /_build/inc/version-note.inc

Introduction
============

ESP32 is a system on a chip that integrates the following features:

* Wi-Fi (2.4 GHz band)
* Bluetooth 4.2
* Dual high performance cores
* Ultra Low Power co-processor
* Several peripherals

Powered by 40 nm technology, ESP32 provides a robust, highly integrated platform, which helps meet the continuous demands for efficient power usage, compact design, security, high performance, and reliability.

Espressif provides basic hardware and software resources to help application developers realize their ideas using the ESP32 series hardware. The software development framework by Espressif is intended for development of Internet-of-Things (IoT) applications with Wi-Fi, Bluetooth, power management and several other system features.

What You Need
=============

Hardware:

* An **ESP32** board
* **USB cable** - USB A / micro USB B
* **Computer** running Windows, Linux, or macOS

Software:

* **Toolchain** to build the **Application** for ESP32
* **ESP-IDF** that essentially contains API (software libraries and source code) for ESP32 and scripts to operate the **Toolchain**
* **Text editor** to write programs (**Projects**) in C, e.g., `Eclipse <https://www.eclipse.org/>`_


.. figure:: ../../_static/what-you-need.png
    :align: center
    :alt: Development of applications for ESP32
    :figclass: align-center

    Development of applications for ESP32


Development Board Overviews
===========================

If you have one of ESP32 development boards listed below, you can click on the link to learn more about its hardware.

.. toctree::
    :maxdepth: 1

    ESP32-DevKitC <../hw-reference/get-started-devkitc>
    ESP-WROVER-KIT <../hw-reference/get-started-wrover-kit>
    ESP32-PICO-KIT <../hw-reference/get-started-pico-kit>
    ESP32-Ethernet-Kit <../hw-reference/get-started-ethernet-kit>

.. _get-started-step-by-step-legacy:

Installation Step by Step
=========================

This is a detailed roadmap to walk you through the installation process.

Setting up Development Environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-setup-toolchain-legacy` for :doc:`Windows <windows-setup>`, :doc:`Linux <linux-setup>` or :doc:`macOS <macos-setup>`
* :ref:`get-started-get-esp-idf-legacy`
* :ref:`get-started-setup-path-legacy`
* :ref:`get-started-get-packages-legacy`

Creating Your First Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-start-project-legacy`
* :ref:`get-started-connect-legacy`
* :ref:`get-started-configure-legacy`
* :ref:`get-started-build-and-flash-legacy`
* :ref:`get-started-monitor-legacy`


.. _get-started-setup-toolchain-legacy:

Step 1. Set up the Toolchain
============================

The toolchain is a set of programs for compiling code and building applications.

The quickest way to start development with ESP32 is by installing a prebuilt toolchain. Pick up your OS below and follow the provided instructions. 

.. toctree::
    :hidden:

    Windows <windows-setup>
    Linux <linux-setup>
    macOS <macos-setup>

+-----------------+---------------+---------------+
| |windows-logo|  | |linux-logo|  | |macos-logo|  |
+-----------------+---------------+---------------+
| Windows_        | Linux_        | `macOS`_      |
+-----------------+---------------+---------------+

.. |windows-logo| image:: ../../_static/windows-logo.png
    :target: ../get-started-legacy/windows-setup.html

.. |linux-logo| image:: ../../_static/linux-logo.png
    :target: ../get-started-legacy/linux-setup.html

.. |macos-logo| image:: ../../_static/macos-logo.png
    :target: ../get-started-legacy/macos-setup.html

.. _Windows: ../get-started-legacy/windows-setup.html
.. _Linux: ../get-started-legacy/linux-setup.html
.. _macOS: ../get-started-legacy/macos-setup.html

.. note::

    This guide uses the directory ``~/esp`` on Linux and macOS or ``%userprofile%\esp`` on Windows as an installation folder for ESP-IDF. You can use any directory, but you will need to adjust paths for the commands respectively. Keep in mind that ESP-IDF does not support spaces in paths.

Depending on your experience and preferences, you may want to customize your environment instead of using a prebuilt toolchain. To set up the system your own way go to Section :ref:`get-started-customized-setup-legacy`.


.. _get-started-get-esp-idf-legacy:

Step 2. Get ESP-IDF
===================

Besides the toolchain, you also need ESP32-specific API (software libraries and source code). They are provided by Espressif in `ESP-IDF repository <https://github.com/espressif/esp-idf>`_.

To get a local copy of ESP-IDF, navigate to your installation directory and clone the repository with ``git clone``.

Open Terminal, and run the following commands:

.. include:: /_build/inc/git-clone-bash.inc

ESP-IDF will be downloaded into ``~/esp/esp-idf``.

Consult :doc:`/versions` for information about which ESP-IDF version to use in a given situation.

.. include:: /_build/inc/git-clone-notes.inc

.. note::

    Do not miss the ``--recursive`` option. If you have already cloned ESP-IDF without this option, run another command to get all the submodules::

        cd esp-idf
        git submodule update --init


.. _get-started-setup-path-legacy:

Step 3. Set Environment Variables
=================================

The toolchain uses the environment variable ``IDF_PATH`` to access the ESP-IDF directory. This variable should be set up on your computer, otherwise projects will not build.

These variables can be set temporarily (per session) or permanently. Please follow the instructions specific to :ref:`Windows <add-idf_path-to-profile-windows-legacy>` , :ref:`Linux and macOS <add-idf_path-to-profile-linux-macos-legacy>` in Section :doc:`add-idf_path-to-profile`.


.. _get-started-get-packages-legacy:

Step 4. Install the Required Python Packages
============================================

The python packages required by ESP-IDF are located in ``IDF_PATH/requirements.txt``. You can install them by running::

    python -m pip install --user -r $IDF_PATH/requirements.txt

.. note::

    Please check the version of the Python interpreter that you will be using with ESP-IDF. For this, run 
    the command ``python --version`` and depending on the result, you might want to use ``python2``, ``python2.7``
    or similar instead of just ``python``, e.g.::

        python2.7 -m pip install --user -r $IDF_PATH/requirements.txt


.. _get-started-start-project-legacy:

Step 5. Start a Project
=======================

Now you are ready to prepare your application for ESP32. You can start with :example:`get-started/hello_world` project from :idf:`examples` directory in IDF.

Copy :example:`get-started/hello_world` to the ``~/esp`` directory:

Linux and macOS
~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp
    cp -r $IDF_PATH/examples/get-started/hello_world .

Windows
~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp
    xcopy /e /i %IDF_PATH%\examples\get-started\hello_world hello_world

There is a range of example projects in the :idf:`examples` directory in ESP-IDF. You can copy any project in the same way as presented above and run it.

It is also possible to build examples in-place, without copying them first.

.. important::

    The esp-idf build system does not support spaces in the paths to either esp-idf or to projects.

.. _get-started-connect-legacy:

Step 6. Connect Your Device
===========================

Now connect your ESP32 board to the computer and check under what serial port the board is visible.

Serial ports have the following patterns in their names: 

- **Windows**: names like ``COM1``
- **Linux**: starting with ``/dev/tty``
- **macOS**: starting with ``/dev/cu.``

If you are not sure how to check the serial port name, please refer to :doc:`establish-serial-connection` for full details.

.. note::

    Keep the port name handy as you will need it in the next steps.


.. _get-started-configure-legacy:

Step 7. Configure
=================

Navigate to your ``hello_world`` directory from :ref:`get-started-start-project-legacy` and run the project configuration utility ``menuconfig``.

Linux and macOS
~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/hello_world
    make menuconfig

Windows
~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp\hello_world
    make menuconfig

If the previous steps have been done correctly, the following menu appears:

.. figure:: ../../_static/project-configuration.png
    :align: center
    :alt: Project configuration - Home window
    :figclass: align-center

    Project configuration - Home window

In the menu, navigate to ``Serial flasher config`` > ``Default serial port`` to configure the serial port, where project will be loaded to. Confirm selection by pressing enter, save configuration by selecting ``< Save >`` and then exit ``menuconfig`` by selecting ``< Exit >``.

To navigate and use ``menuconfig``, press the following keys:

* Arrow keys for navigation
* ``Enter`` to go into a submenu
* ``Esc`` to go up one level or exit
* ``?`` to see a help screen. Enter key exits the help screen
* ``Space``, or ``Y`` and ``N`` keys to enable (Yes) and disable (No) configuration items with checkboxes "``[*]``"
* ``?`` while highlighting a configuration item to display help about that item
* ``/`` to find configuration items

.. note::

    If you are **Arch Linux** user, navigate to ``SDK tool configuration`` and change the name of ``Python 2 interpreter`` from ``python`` to ``python2``.

.. attention::

    If you use ESP32-DevKitC board with the **ESP32-SOLO-1** module, enable single core mode (:ref:`CONFIG_FREERTOS_UNICORE`) in menuconfig before flashing examples.

.. _get-started-build-and-flash-legacy:

Step 8. Build and Flash
=======================

Build and flash the project by running::

    make flash

This command will compile the application and all ESP-IDF components, then it will generate the bootloader, partition table, and application binaries. After that, these binaries will be flashed onto your ESP32 board.

If there are no issues by the end of the flash process, you will see messages (below) describing progress of the loading process. Then the board will be reset and the "hello_world" application will start up.

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


If you'd like to use the Eclipse IDE instead of running ``make``, check out the :doc:`Eclipse guide <eclipse-setup>`.


.. _get-started-monitor-legacy:

Step 9. Monitor
===============

To check if "hello_world" is indeed running, type ``make monitor``.

This command launches the :doc:`IDF Monitor <../api-guides/tools/idf-monitor>` application::

    $ make monitor
    MONITOR
    --- idf_monitor on /dev/ttyUSB0 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57
    ...

After startup and diagnostic logs scroll up, you should see "Hello world!" printed out by the application.

.. code-block:: none

    ...
    Hello world!
    Restarting in 10 seconds...
    I (211) cpu_start: Starting scheduler on APP CPU.
    Restarting in 9 seconds...
    Restarting in 8 seconds...
    Restarting in 7 seconds...

To exit IDF monitor use the shortcut ``Ctrl+]``.

If IDF monitor fails shortly after the upload, or if instead of the messages above you see a random garbage similar to what is given below, your board is likely using a 26MHz crystal. Most development board designs use 40MHz, so ESP-IDF uses this frequency as a default value.

.. figure:: ../../_static/get-started-garbled-output.png
    :align: center
    :alt: Garbled output
    :figclass: align-center

If you have such a problem, do the following:

1. Exit the monitor.
2. Go back to :ref:`menuconfig <get-started-configure-legacy>`.
3. Go to Component config --> ESP32-specific --> Main XTAL frequency, then change :ref:`CONFIG_ESP32_XTAL_FREQ_SEL` to 26MHz.
4. After that, :ref:`build and flash <get-started-build-and-flash-legacy>` the application again.

.. note::

    You can combine building, flashing and monitoring into one step by running::

       make flash monitor

See also :doc:`IDF Monitor <../api-guides/tools/idf-monitor>` for handy shortcuts and more details on using IDF monitor.

**That's all that you need to get started with ESP32!**

Now you are ready to try some other :idf:`examples`, or go straight to developing your own applications.


Environment Variables
=====================

Some environment variables can be specified whilst calling ``make`` allowing users to **override arguments without the need to reconfigure them using** ``make menuconfig``.

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
    
    You can export environment variables (e.g. ``export ESPPORT=/dev/ttyUSB1``).
    All subsequent calls of ``make`` within the same terminal session will use 
    the exported value given that the variable is not simultaneously overridden.


Updating ESP-IDF
================

You should update ESP-IDF from time to time, as newer versions fix bugs and provide new features. The simplest way to do the update is to delete the existing ``esp-idf`` folder and clone it again, as if performing the initial installation described in :ref:`get-started-get-esp-idf-legacy`.

If downloading to a new path, remember to :doc:`add-idf_path-to-profile` so that the toolchain scripts can find ESP-IDF in its release specific location.

Another solution is to update only what has changed. :ref:`The update procedure depends on the version of ESP-IDF you are using <updating>`.

Related Documents
=================

.. toctree::
    :maxdepth: 1

    add-idf_path-to-profile
    establish-serial-connection
    make-project
    eclipse-setup
    ../api-guides/tools/idf-monitor
    toolchain-setup-scratch

.. _Stable version: https://docs.espressif.com/projects/esp-idf/en/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases
