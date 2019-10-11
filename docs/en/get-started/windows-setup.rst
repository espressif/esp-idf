***********************************************
Standard Setup of Toolchain for Windows
***********************************************

:link_to_translation:`zh_CN:[中文]`

.. note::
      Currently only 64-bit versions of Windows are supported. 32-bit Windows can use the :doc:`Legacy GNU Make Build System<../get-started-legacy/windows-setup>`.

Introduction
============

ESP-IDF requires some prerequisite tools to be installed so you can build firmware for the ESP32. The prerequisite tools include Python, Git, cross-compilers, menuconfig tool, CMake and Ninja build tools.

For this Getting Started we're going to use the Command Prompt, but after ESP-IDF is installed you can use :doc:`Eclipse <eclipse-setup>` or another graphical IDE with CMake support instead.

.. note::
   Previous versions of ESP-IDF used the :doc:`Legacy GNU Make Build System<../get-started-legacy/windows-setup>` and MSYS2_ Unix compatibility environment. This is no longer required, ESP-IDF can be used from the Windows Command Prompt.

.. _get-started-windows-tools-installer:

ESP-IDF Tools Installer
=======================

The easiest way to install ESP-IDF's prerequisites is to download the ESP-IDF Tools installer from this URL:

https://dl.espressif.com/dl/esp-idf-tools-setup-2.1.exe

The installer includes the cross-compilers, OpenOCD, cmake_ and Ninja_ build tool, and a configuration tool called mconf-idf_. The installer can also download and run installers for Python_ 3.7 and `Git For Windows`_ if they are not already installed on the computer.

The installer also offers to download one of the ESP-IDF release versions.

Using the Command Prompt
========================

For the remaining Getting Started steps, we're going to use the Windows Command Prompt.

ESP-IDF Tools Installer creates a shortcut in the Start menu to launch the ESP-IDF Command Prompt. This shortcut launches the Command Prompt (cmd.exe) and runs ``export.bat`` script to set up the environment variables (``PATH``, ``IDF_PATH`` and others). Inside this command prompt, all the installed tools are available.

Note that this shortcut is specific to the ESP-IDF directory selected in the ESP-IDF Tools Installer. If you have multiple ESP-IDF directories on the computer (for example, to work with different versions of ESP-IDF), you have two options to use them:

1. Create a copy of the shortcut created by the ESP-IDF Tools Installer, and change the working directory of the new shortcut to the ESP-IDF directory you wish to use.

2. Alternatively, run ``cmd.exe``, then change to the ESP-IDF directory you wish to use, and run ``export.bat``. Note that unlike the previous option, this way requires Python and Git to be present in ``PATH``. If you get errors related to Python or Git not being found, use the first option.

Next Steps
==========

If the ESP-IDF Tools Installer has finished successfully, then the development environment setup is complete. Proceed directly to :ref:`get-started-start-project`.

Related Documents
=================

For advanced users who want to customize the install process:

.. toctree::
    :maxdepth: 1

    windows-setup-scratch
    windows-setup-update

.. _MSYS2: https://msys2.github.io/
.. _cmake: https://cmake.org/download/
.. _ninja: https://ninja-build.org/
.. _Python: https://www.python.org/downloads/windows/
.. _Git for Windows: https://gitforwindows.org/
.. _mconf-idf: https://github.com/espressif/kconfig-frontends/releases/
.. _Github Desktop: https://desktop.github.com/
