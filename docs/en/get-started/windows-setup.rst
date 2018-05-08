***************************************
Standard Setup of Toolchain for Windows
***************************************

.. note::
   This is documentation for the CMake-based build system which is currently in preview release. The documentation may have gaps, and you may encounter bugs (please report either of these). To view documentation for the older GNU Make based build system, switch versions to the 'latest' master branch or a stable release.

.. note::
      The CMake-based build system is only supported on 64-bit versions of Windows.

Introduction
============

ESP-IDF requires some prerequisite tools to be installed so you can build firmware for the ESP32. The prerequisite tools include Git, a cross-compiler and the CMake build tool. We'll go over each one in this document.

For this Getting Started we're going to use a command prompt, but after ESP-IDF is installed you can use :doc:`Eclipse <eclipse-setup>` or another graphical IDE with CMake support instead.

.. note::
   Previous versions of ESP-IDF used a GNU Make based build system, which required the MSYS2_ Unix compatibility environment on Windows. This is no longer required.

ESP-IDF Tools Installer
=======================

The easiest way to install ESP-IDF's prerequisites is to download the ESP-IDF Tools installer from this URL:

https://dl.espressif.com/dl/esp-idf-tools-setup-1.0.exe

The installer will automatically install the ESP32 Xtensa gcc toolchain, Ninja_ build tool, and a customized configuration tool called mconf. The installer can also download and run installers for CMake_ and Python_ 2.7 if these are not already installed on the computer.

By default, the installer updates the Windows ``Path`` environment variable so all of these tools can be run from anywhere. If you disable this option, you will need to configure the environment where you are using ESP-IDF (terminal or chosen IDE) with the correct paths.

Note that this installer is for the ESP-IDF Tools package, it doesn't include ESP-IDF itself.

Installing Git
==============

The ESP-IDF tools installer does not install Git. By default, the getting started guide assumes you will be using Git on the command line. You can download and install a command line Git for Windows (along with the "Git Bash" terminal) from `Git For Windows`_.

If you prefer to use a different graphical Git client, then you can install one such as `Github Desktop`. You will need to translate the Git commands in the Getting Started guide for use with your chosen Git client.

Using a Terminal
================

For the remaining Getting Started steps, we're going to use a terminal command prompt. It doesn't matter which command prompt you use:

- You can use the built-in Windows Command Prompt, under the Start menu. Note that command line instructions in this documentation give "bash" commands for Mac OS or Linux first. The Windows Command Prompt equivalent is given afterwards.
- You can use the "Git Bash" terminal which is part of `Git for Windows`_. This uses the same "bash" command prompt syntax as Mac OS or Linux. You can find it in the Start menu once installed.
- If you have MSYS2_ installed (maybe from a previous ESP-IDF version), then you can also use the MSYS terminal.

Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf`.

Related Documents
=================

For advanced users who want to customize the install process:

.. toctree::
    :maxdepth: 1

    windows-setup-scratch


.. _MSYS2: https://msys2.github.io/
.. _cmake: https://cmake.org/download/
.. _ninja: https://ninja-build.org/
.. _Python: https://www.python.org/downloads/windows/
.. _Git for Windows: https://gitforwindows.org/
.. _mconf: https://github.com/espressif/kconfig-frontends/releases/
.. _Github Desktop: https://desktop.github.com/
