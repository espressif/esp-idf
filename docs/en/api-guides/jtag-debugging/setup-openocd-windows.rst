**************************
Set up OpenOCD for Windows
**************************

IDF Tools Installer
===================

If you are using CMake build system and followed the :doc:`/get-started-cmake/windows-setup` with the ``ESP-IDF Tools Installer`` V1.2 or newer, then by default you will already have ``openocd`` installed.

``ESP-IDF Tools Installer`` adds ``openocd` to the ``PATH`` so that it can be run from any directory.

Set up OpenOCD
==============


OpenOCD for Windows is available for download from Github:

https://github.com/espressif/openocd-esp32/releases

Download latest release archive with `win32` in its name, for example `openocd-esp32-macos-0.10.0-win32-20180418.zip`.

Extract the downloaded file in ``~/esp/`` directory.

    cd ~/esp
    unzip /c/Users/<user>/Downloads/openocd-esp32-win32-<version>.zip

Next Steps
==========

To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-configuring-esp32-target`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    building-openocd-windows
