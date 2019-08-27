***********
Get Started
***********

:link_to_translation:`zh_CN:[中文]`

This document is intended to help you set up the software development environment for the hardware based on the ESP32 chip by Espressif.

After that, a simple example will show you how to use ESP-IDF (Espressif IoT Development Framework) for menu configuration, then building, and flashing firmware onto an ESP32 board.

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

* **Toolchain** to compile code for ESP32
* **Build tools** - CMake and Ninja to build a full **Application** for ESP32
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


.. _get-started-step-by-step:

Installation Step by Step
=========================

This is a detailed roadmap to walk you through the installation process.

Setting up Development Environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-get-prerequisites` for :doc:`Windows <windows-setup>`, :doc:`Linux <linux-setup>` or :doc:`macOS <macos-setup>`
* :ref:`get-started-get-esp-idf`
* :ref:`get-started-set-up-tools`
* :ref:`get-started-set-up-env`

Creating Your First Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-start-project`
* :ref:`get-started-connect`
* :ref:`get-started-configure`
* :ref:`get-started-build`
* :ref:`get-started-flash`
* :ref:`get-started-build-monitor`


.. _get-started-get-prerequisites:

Step 1. Install prerequisites
=============================

Some tools need to be installed on the computer before proceeding to the next steps. Follow the links below for the instructions for your OS:

.. toctree::
    :hidden:

    Windows <windows-setup>
    Linux <linux-setup>
    macOS <macos-setup>

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

.. _get-started-get-esp-idf:

Step 2. Get ESP-IDF
===================

To build applications for the ESP32, you need the software libraries provided by Espressif in `ESP-IDF repository <https://github.com/espressif/esp-idf>`_.

To get ESP-IDF, navigate to your installation directory and clone the repository with ``git clone``, following instructions below specific to your operating system.

.. note::

    This guide uses the directory ``~/esp`` on Linux and macOS or ``%userprofile%\esp`` on Windows as an installation folder for ESP-IDF. You can use any directory, but you will need to adjust paths for the commands respectively. Keep in mind that ESP-IDF does not support spaces in paths.

Linux and macOS
~~~~~~~~~~~~~~~

Open Terminal, and run the following commands:

.. include:: /_build/inc/git-clone-bash.inc

ESP-IDF will be downloaded into ``~/esp/esp-idf``.

Consult :doc:`/versions` for information about which ESP-IDF version to use in a given situation.

Windows
~~~~~~~

In addition to installing the tools, :ref:`get-started-windows-tools-installer` for Windows introduced in Step 1 can also download a copy of ESP-IDF.

Consult :doc:`/versions` for information about which ESP-IDF version to use in a given situation.

If you wish to download ESP-IDF without the help of ESP-IDF Tools Installer, refer to these :ref:`instructions <get-esp-idf-windows-command-line>`.

.. _get-started-set-up-tools:

Step 3. Set up the tools
========================

Aside from the ESP-IDF, you also need to install the tools used by ESP-IDF, such as the compiler, debugger, Python packages, etc.

Windows
~~~~~~~

:ref:`get-started-windows-tools-installer` for Windows introduced in Step 1 installs all the required tools.

If you want to install the tools without the help of ESP-IDF Tools Installer, open the Command Prompt and follow these steps:

.. code-block:: batch

    cd %userprofile%\esp\esp-idf
    install.bat

or with Windows PowerShell

.. code-block:: powershell

    cd ~/esp/esp-idf
    ./install.ps1

Linux and macOS
~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/esp-idf
    ./install.sh

Customizing the tools installation path
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The scripts introduced in this step install compilation tools required by ESP-IDF inside the user home directory: ``$HOME/.espressif`` on Linux and macOS, ``%USERPROFILE%\.espressif`` on Windows. If you wish to install the tools into a different directory, set the environment variable ``IDF_TOOLS_PATH`` before running the installation scripts. Make sure that your user has sufficient permissions to read and write this path.

If changing the ``IDF_TOOLS_PATH``, make sure it is set to the same value every time the Install script (``install.bat``, ``install.ps1`` or ``install.sh``) and an Export script (``export.bat``, ``export.ps1`` or ``export.sh``) are executed.

.. _get-started-set-up-env:

Step 4. Set up the environment variables
========================================

The installed tools are not yet added to the PATH environment variable. To make the tools usable from the command line, some environment variables must be set. ESP-IDF provides another script which does that.

Windows
~~~~~~~

:ref:`get-started-windows-tools-installer` for Windows creates an "ESP-IDF Command Prompt" shortcut in the Start Menu. This shortcut opens the Command Prompt and sets up all the required environment variables. You can open this shortcut and proceed to the next step.

Alternatively, if you want to use ESP-IDF in an existing Command Prompt window, you can run:

.. code-block:: batch

    %userprofile%\esp\esp-idf\export.bat

or with Windows PowerShell

.. code-block:: powershell

    .$HOME/esp/esp-idf/export.ps1

Linux and macOS
~~~~~~~~~~~~~~~

In the terminal where you are going to use ESP-IDF, run:

.. code-block:: bash

    . $HOME/esp/esp-idf/export.sh

Note the space between the leading dot and the path!

You can also automate this step, making ESP-IDF tools available in every terminal, by adding this line to your ``.profile`` or ``.bash_profile`` script.

.. _get-started-start-project:

Step 5. Start a Project
=======================

Now you are ready to prepare your application for ESP32. You can start with :example:`get-started/hello_world` project from :idf:`examples` directory in IDF.

Copy :example:`get-started/hello_world` to ``~/esp`` directory:

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

    The ESP-IDF build system does not support spaces in the paths to either ESP-IDF or to projects.

.. _get-started-connect:

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


.. _get-started-configure:

Step 7. Configure
=================

Navigate to your ``hello_world`` directory from :ref:`get-started-start-project` and run the project configuration utility ``menuconfig``.

Linux and macOS
~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/hello_world
    idf.py menuconfig

If your default version of Python is 3.x, you may need to run ``python2 $(which idf.py) menuconfig`` instead.

Windows
~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp\hello_world
    idf.py menuconfig

If the previous steps have been done correctly, the following menu appears:

.. figure:: ../../_static/project-configuration.png
    :align: center
    :alt: Project configuration - Home window
    :figclass: align-center

    Project configuration - Home window

To navigate and use ``menuconfig``, press the following keys:

* Arrow keys for navigation
* ``Enter`` to go into a submenu
* ``Esc`` to go up one level or exit
* ``?`` to see a help screen. Enter key exits the help screen
* ``Space``, or ``Y`` and ``N`` keys to enable (Yes) and disable (No) configuration items with checkboxes "``[*]``"
* ``?`` while highlighting a configuration item to display help about that item
* ``/`` to find configuration items

.. attention::

    If you use ESP32-DevKitC board with the **ESP32-SOLO-1** module, enable single core mode (:ref:`CONFIG_FREERTOS_UNICORE`) in menuconfig before flashing examples.

.. _get-started-build:

Step 8. Build the Project
=========================

Build the project by running::

    idf.py build

This command will compile the application and all ESP-IDF components, then it will generate the bootloader, partition table, and application binaries.

.. code-block:: none

   $ idf.py build
   Running cmake in directory /path/to/hello_world/build
   Executing "cmake -G Ninja --warn-uninitialized /path/to/hello_world"...
   Warn about uninitialized values.
   -- Found Git: /usr/bin/git (found version "2.17.0")
   -- Building empty aws_iot component due to configuration
   -- Component names: ...
   -- Component paths: ...
   
   ... (more lines of build system output)
   
   [527/527] Generating hello-world.bin
   esptool.py v2.3.1
   
   Project build complete. To flash, run this command:
   ../../../components/esptool_py/esptool/esptool.py -p (PORT) -b 921600 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x10000 build/hello-world.bin  build 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin
   or run 'idf.py -p PORT flash'

If there are no errors, the build will finish by generating the firmware binary .bin file.


.. _get-started-flash:

Step 9. Flash onto the Device
=============================

Flash the binaries that you just built onto your ESP32 board by running::

    idf.py -p PORT [-b BAUD] flash

Replace PORT with your ESP32 board's serial port name from :ref:`get-started-connect`.

You can also change the flasher baud rate by replacing BAUD with the baud rate you need. The default baud rate is ``460800``.

For more information on idf.py arguments, see :ref:`idf.py`.

.. note:: 

    The option ``flash`` automatically builds and flashes the project, so running ``idf.py build`` is not necessary.

.. code-block:: none

    Running esptool.py in directory [...]/esp/hello_world
    Executing "python [...]/esp-idf/components/esptool_py/esptool/esptool.py -b 460800 write_flash @flash_project_args"...
    esptool.py -b 460800 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello-world.bin
    esptool.py v2.3.1
    Connecting....
    Detecting chip type... ESP32
    Chip is ESP32D0WDQ6 (revision 1)
    Features: WiFi, BT, Dual Core
    Uploading stub...
    Running stub...
    Stub running...
    Changing baud rate to 460800
    Changed.
    Configuring flash size...
    Auto-detected Flash size: 4MB
    Flash params set to 0x0220
    Compressed 22992 bytes to 13019...
    Wrote 22992 bytes (13019 compressed) at 0x00001000 in 0.3 seconds (effective 558.9 kbit/s)...
    Hash of data verified.
    Compressed 3072 bytes to 82...
    Wrote 3072 bytes (82 compressed) at 0x00008000 in 0.0 seconds (effective 5789.3 kbit/s)...
    Hash of data verified.
    Compressed 136672 bytes to 67544...
    Wrote 136672 bytes (67544 compressed) at 0x00010000 in 1.9 seconds (effective 567.5 kbit/s)...
    Hash of data verified.
    
    Leaving...
    Hard resetting via RTS pin...

If there are no issues by the end of the flash process, the module will be reset and the “hello_world” application will be running.

.. (Not currently supported) If you'd like to use the Eclipse IDE instead of running ``idf.py``, check out the :doc:`Eclipse guide <eclipse-setup>`.


.. _get-started-build-monitor:

Step 10. Monitor
================

To check if "hello_world" is indeed running, type ``idf.py -p PORT monitor`` (Do not forget to replace PORT with your serial port name).

This command launches the :doc:`IDF Monitor <../api-guides/tools/idf-monitor>` application::

    $ idf.py -p /dev/ttyUSB0 monitor
    Running idf_monitor in directory [...]/esp/hello_world/build
    Executing "python [...]/esp-idf/tools/idf_monitor.py -b 115200 [...]/esp/hello_world/build/hello-world.elf"...
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

If IDF monitor fails shortly after the upload, or, if instead of the messages above, you see random garbage similar to what is given below, your board is likely using a 26MHz crystal. Most development board designs use 40MHz, so ESP-IDF uses this frequency as a default value.

.. figure:: ../../_static/get-started-garbled-output.png
    :align: center
    :alt: Garbled output
    :figclass: align-center

If you have such a problem, do the following:

1. Exit the monitor.
2. Go back to :ref:`menuconfig <get-started-configure>`.
3. Go to Component config --> ESP32-specific --> Main XTAL frequency, then change :ref:`CONFIG_ESP32_XTAL_FREQ_SEL` to 26MHz.
4. After that, :ref:`build and flash <get-started-flash>` the application again.

.. note::

    You can combine building, flashing and monitoring into one step by running::

       idf.py -p PORT flash monitor

See also:

- :doc:`IDF Monitor <../api-guides/tools/idf-monitor>` for handy shortcuts and more details on using IDF monitor.
- :ref:`idf.py` for a full reference of ``idf.py`` commands and options.

**That's all that you need to get started with ESP32!**

Now you are ready to try some other :idf:`examples`, or go straight to developing your own applications.

Updating ESP-IDF
================

You should update ESP-IDF from time to time, as newer versions fix bugs and provide new features. The simplest way to do the update is to delete the existing ``esp-idf`` folder and clone it again, as if performing the initial installation described in :ref:`get-started-get-esp-idf`.

Another solution is to update only what has changed. :ref:`The update procedure depends on the version of ESP-IDF you are using <updating>`.

After updating ESP-IDF, execute the Install script again, in case the new ESP-IDF version requires different versions of tools. See instructions at :ref:`get-started-set-up-tools`.

Once the new tools are installed, update the environment using the Export script. See instructions at :ref:`get-started-set-up-env`.

Related Documents
=================

.. toctree::
    :maxdepth: 1

    establish-serial-connection
    eclipse-setup
    ../api-guides/tools/idf-monitor
    toolchain-setup-scratch
    ../get-started-legacy/index

.. _Stable version: https://docs.espressif.com/projects/esp-idf/en/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases
