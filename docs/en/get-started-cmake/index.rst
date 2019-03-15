*******************
Get Started (CMake)
*******************

:link_to_translation:`zh_CN:[中文]`

.. include:: ../cmake-warning.rst

.. include:: ../cmake-pending-features.rst

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


.. figure:: ../../_static/what-you-need-cmake.png
    :align: center
    :alt: Development of applications for ESP32
    :figclass: align-center

    Development of applications for ESP32


Development Board Overviews
===========================

If you have one of ESP32 development boards listed below, you can click on the link to learn more about its hardware.

.. toctree::
    :maxdepth: 1

    ESP32-DevKitC <get-started-devkitc>
    ESP-WROVER-KIT <get-started-wrover-kit>
    ESP32-PICO-KIT <get-started-pico-kit>


.. _get-started-step-by-step-cmake:

Installation Step by Step
=========================

This is a detailed roadmap to walk you through the installation process.

Setting up Development Environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-setup-toolchain-cmake` for :doc:`Windows <windows-setup>`, :doc:`Linux <linux-setup>` or :doc:`MacOS <macos-setup>`
* :ref:`get-started-get-esp-idf-cmake`
* :ref:`get-started-setup-path-cmake`
* :ref:`get-started-get-packages-cmake`

Creating Your First Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* :ref:`get-started-start-project-cmake`
* :ref:`get-started-connect-cmake`
* :ref:`get-started-configure-cmake`
* :ref:`get-started-build-cmake`
* :ref:`get-started-flash-cmake`
* :ref:`get-started-build-monitor-cmake`


.. _get-started-setup-toolchain-cmake:

Step 1. Set up the Toolchain
============================

The toolchain is a set of programs for compiling code and building applications.

The quickest way to start development with ESP32 is by installing a prebuilt toolchain. Pick up your OS below and follow the provided instructions. 

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
    :target: ../get-started-cmake/windows-setup.html

.. |linux-logo| image:: ../../_static/linux-logo.png
    :target: ../get-started-cmake/linux-setup.html

.. |macos-logo| image:: ../../_static/macos-logo.png
    :target: ../get-started-cmake/macos-setup.html

.. _Windows: ../get-started-cmake/windows-setup.html
.. _Linux: ../get-started-cmake/linux-setup.html
.. _Mac OS: ../get-started-cmake/macos-setup.html

.. note::

    This guide uses the directory ``~/esp`` on Linux and macOS or ``%userprofile%\esp`` on Windows as an installation folder for ESP-IDF. You can use any directory, but you will need to adjust paths for the commands respectively. Keep in mind that ESP-IDF does not support spaces in paths.

Depending on your experience and preferences, you may want to customize your environment instead of using a prebuilt toolchain. To set up the system your own way go to Section :ref:`get-started-customized-setup-cmake`.


.. _get-started-get-esp-idf-cmake:

Step 2. Get ESP-IDF
===================

Besides the toolchain, you also need ESP32-specific API (software libraries and source code). They are provided by Espressif in `ESP-IDF repository <https://github.com/espressif/esp-idf>`_.

Get ESP-IDF in accordance with your operating system.

To get ESP-IDF, navigate to your installation directory and clone the repository with ``git clone``.

Linux and MacOS
~~~~~~~~~~~~~~~

Open Terminal, and run the following commands:

.. include:: /_build/inc/git-clone-bash.inc

ESP-IDF will be downloaded into ``~/esp/esp-idf``.

Consult :doc:`/versions` for information about which ESP-IDF version to use in a given situation.

Windows
~~~~~~~

.. note::

        Previous versions of ESP-IDF used the **MSYS2 bash terminal** command line. The current cmake-based build system can run in the regular **Windows Command Prompt** which is used here.

        If you use a bash-based terminal or PowerShell, please note that some command syntax will be different to what is shown below.

Open Command Prompt and run the following commands:

.. include:: /_build/inc/git-clone-windows.inc

ESP-IDF will be downloaded into ``%userprofile%\esp\esp-idf``.

Consult :doc:`/versions` for information about which ESP-IDF version to use in a given situation.

.. include:: /_build/inc/git-clone-notes.inc

.. note::

    Do not miss the ``--recursive`` option. If you have already cloned ESP-IDF without this option, run another command to get all the submodules::

        cd esp-idf
        git submodule update --init


.. _get-started-setup-path-cmake:

Step 3. Set Environment Variables
=================================

Set the following environment variables on your computer, so that projects can be built:

- Create ``IDF_PATH`` and assign it the path to the ESP-IDF directory.
- Add to ``PATH`` the path to the ``tools`` directory inside the ``IDF_PATH`` directory.

These variables can be set temporarily (per session) or permanently. Please follow the instructions specific to :ref:`Windows <add-paths-to-profile-windows-cmake>` , :ref:`Linux and MacOS <add-idf_path-to-profile-linux-macos-cmake>` in Section :doc:`add-idf_path-to-profile`.


.. _get-started-get-packages-cmake:

Step 4. Install the Required Python Packages
============================================

The python packages required by ESP-IDF are located in ``IDF_PATH/requirements.txt``. You can install them by running::

    python -m pip install --user -r $IDF_PATH/requirements.txt

.. note::

    Please check the version of the Python interpreter that you will be using with ESP-IDF. For this, run 
    the command ``python --version`` and depending on the result, you might want to use ``python2``, ``python2.7``
    or similar instead of just ``python``, e.g.::

        python2.7 -m pip install --user -r $IDF_PATH/requirements.txt


.. _get-started-start-project-cmake:

Step 5. Start a Project
=======================

Now you are ready to prepare your application for ESP32. You can start with :example:`get-started/hello_world` project from :idf:`examples` directory in IDF.

Copy :example:`get-started/hello_world` to ``~/esp`` directory:

Linux and MacOS
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

.. _get-started-connect-cmake:

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


.. _get-started-configure-cmake:

Step 7. Configure
=================

Navigate to your ``hello_world`` directory from :ref:`get-started-start-project-cmake` and run the project configuration utility ``menuconfig``.

Linux and MacOS
~~~~~~~~~~~~~~~

.. code-block:: bash

    cd ~/esp/hello_world
    idf.py menuconfig

If your default version of Python is 3.x, you may need to run ``python2 idf.py`` instead.

Windows
~~~~~~~

.. code-block:: batch

    cd %userprofile%\esp\hello_world
    idf.py menuconfig

The Python 2.7 installer will try to configure Windows to associate ``.py`` files with Python 2. If a separately installed program, such as Visual Studio Python Tools, has created an association with a different version of Python, then running ``idf.py`` may not work (it opens the file in Visual Studio instead). You can either run ``C:\Python27\python idf.py`` each time instead, or change the association that Windows uses for ``.py`` files.

.. note::

    If you get an error ``idf.py not found``, make sure that the ``PATH`` environment variable was set correctly in :ref:`get-started-setup-path-cmake`. If there is no ``idf.py`` in ``tools``, make sure you have the correct branch for the CMake preview as shown under :ref:`get-started-get-esp-idf-cmake`.

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

.. _get-started-build-cmake:

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


.. _get-started-flash-cmake:

Step 9. Flash onto the Device
=============================

Flash the binaries that you just built onto your ESP32 board by running::

    idf.py -p PORT [-b BAUD] flash

Replace PORT with your ESP32 board's serial port name from :ref:`get-started-connect-cmake`.

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


.. _get-started-build-monitor-cmake:

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

.. code-block:: none

    e���)(Xn@�y.!��(�PW+)��Hn9a؅/9�!�t5��P�~�k��e�ea�5�jA
    ~zY��Y(1�,1�� e���)(Xn@�y.!Dr�zY(�jpi�|�+z5Ymvp

If you have such a problem, do the following:

1. Exit the monitor.
2. Go back to :ref:`menuconfig <get-started-configure-cmake>`.
3. Go to Component config --> ESP32-specific --> Main XTAL frequency, then change :ref:`CONFIG_ESP32_XTAL_FREQ_SEL` to 26MHz.
4. After that, :ref:`build and flash <get-started-flash-cmake>` the application again.

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

You should update ESP-IDF from time to time, as newer versions fix bugs and provide new features. The simplest way to do the update is to delete the existing ``esp-idf`` folder and clone it again, as if performing the initial installation described in :ref:`get-started-get-esp-idf-cmake`.

If downloading to a new path, remember to :doc:`add-idf_path-to-profile` so that the toolchain scripts can find ESP-IDF in its release specific location.

Another solution is to update only what has changed. :ref:`The update procedure depends on the version of ESP-IDF you are using <updating>`.

Related Documents
=================

.. toctree::
    :maxdepth: 1

    add-idf_path-to-profile
    establish-serial-connection
    eclipse-setup
    ../api-guides/tools/idf-monitor
    toolchain-setup-scratch

.. _Stable version: https://docs.espressif.com/projects/esp-idf/en/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases