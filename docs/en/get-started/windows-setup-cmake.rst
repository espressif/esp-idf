***************************************
Setup for cmake on Windows
***************************************

.. note::
      The CMake-based build system is currently in preview release. Documentation may have missing gaps, and you may enocunter bugs (please report these). The original (non-cmake) version of this doc is :doc:`here<windows-setup>`.

.. note::
      The CMake-based build system is only supported on 64-bit versions of Windows.

Introduction
============

Unlike the conventional ESP-IDF make-based build environment, cmake does not require MSYS2 or another GNU-compatible environment (like Cygwin) for building. You can build with cmake from Windows Command Prompt, or from an IDE with cmake support.

ESP-IDF Tools Installer
=======================

The easiest way to install ESP-IDF's prerequisites is to download the ESP-IDF Tools installer from this URL:

https://dl.espressif.com/esp-idf-tools-setup-1.0.exe

The installer will automatically install the ESP32 Xtensa gcc toolchain, Ninja_ build tool, and mconf configuration tool. The installer can also download and run installers for CMake_ and Python_ 2.7 if these are not already installed on the computer.

By default, the installer updates the Windows ``Path`` environment variable so all of these tools can be run from anywhere. If you disable this option, you will need to configure the environment where you are using ESP-IDF (terminal or chosen IDE) with the correct paths.

Note that this installer is for the ESP-IDF Tools package, it doesn't include ESP-IDF itself.

Installing Git
==============

The ESP-IDF tools does not include Git. By default, the getting started guide assumes you will be using Git on the command line. You can download and install a Git command line for Windows (along with the Git-Bash terminal) from https://gitforwindows.org/.

If you prefer to use a different graphical Git client, then you can install one of these instead - you will need to translate the Git commands in the getting started guide for use with your chosen Git client.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    windows-setup-scratch-cmake

Manual Installation
^^^^^^^^^^^^^^^^^^^

Optionall

Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-cmake`.


.. _cmake: https://cmake.org/download/
.. _ninja: https://ninja-build.org/
.. _Python: https://www.python.org/downloads/windows/
