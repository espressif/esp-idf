***********************************************
Standard Setup of Toolchain for Windows
***********************************************

:link_to_translation:`zh_CN:[中文]`

.. only:: esp32

    .. note::
        Currently only 64-bit versions of Windows are supported. 32-bit Windows can use the :doc:`Legacy GNU Make Build System<../get-started-legacy/windows-setup>`.

Introduction
============

ESP-IDF requires some prerequisite tools to be installed so you can build firmware for supported chips. The prerequisite tools include Python, Git, cross-compilers, CMake and Ninja build tools.

For this Getting Started we're going to use the Command Prompt, but after ESP-IDF is installed you can use :doc:`Eclipse <eclipse-setup>` or another graphical IDE with CMake support instead.

.. only:: esp32

    .. note::
        Previous versions of ESP-IDF used the :doc:`Legacy GNU Make Build System<../get-started-legacy/windows-setup>` and MSYS2_ Unix compatibility environment. This is no longer required, ESP-IDF can be used from the Windows Command Prompt.

.. note::
    Limitation: the installation path of Python or ESP-IDF must not contain white spaces or parentheses.

    Limitation: the installation path of Python or ESP-IDF should not contain special characters (non-ASCII) unless the operating system is configured with "Unicode UTF-8" support. System Administrator can enable the support via Control Panel - Change date, time, or number formats - Administrative tab - Change system locale - check the option "Beta: Use Unicode UTF-8 for worldwide language support" - Ok and reboot the computer.

.. _get-started-windows-tools-installer:

ESP-IDF Tools Installer
=======================

The easiest way to install ESP-IDF's prerequisites is to download one of ESP-IDF Tools Installers from this URL: https://dl.espressif.com/dl/esp-idf/?idf=4.2

What is the usecase for Online and Offline Installer
----------------------------------------------------

Online Installer is very small and allows the installation of all available releases of ESP-IDF. The installer will download only necessary dependencies including `Git For Windows`_  during the installation process. The installer stores downloaded files in the cache directory ``%userprofile%\.espressif``

Offline Installer does not require any network connection. The installer contains all required dependencies including `Git For Windows`_ .

Components of the installation
------------------------------

The installer deploys the following components:

- Embedded Python
- Cross-compilers
- OpenOCD
- CMake_ and Ninja_ build tools
- ESP-IDF

The installer also allows reusing the existing directory with ESP-IDF. The recommended directory is ``%userprofile%\Desktop\esp-idf`` where ``%userprofile%`` is your home directory.

Launching ESP-IDF Environment
-----------------------------

At the end of the installation process you can check out option ``Run ESP-IDF PowerShell Environment`` or ``Run ESP-IDF Command Prompt (cmd.exe)``. The installer will launch ESP-IDF environment in selected prompt.

``Run ESP-IDF PowerShell Environment``:

.. figure:: ../../_static/esp-idf-installer-screenshot-powershell.png
    :align: center
    :alt: Completing the ESP-IDF Tools Setup Wizard with Run ESP-IDF PowerShell Environment
    :figclass: align-center

    Completing the ESP-IDF Tools Setup Wizard with Run ESP-IDF PowerShell Environment

.. figure:: ../../_static/esp-idf-installer-powershell.png
    :align: center
    :alt: ESP-IDF PowerShell
    :figclass: align-center

    ESP-IDF PowerShell

``Run ESP-IDF Command Prompt (cmd.exe)``:

.. figure:: ../../_static/esp-idf-installer-screenshot.png
    :align: center
    :alt: Completing the ESP-IDF Tools Setup Wizard with Run ESP-IDF Command Prompt (cmd.exe)
    :figclass: align-center

    Completing the ESP-IDF Tools Setup Wizard with Run ESP-IDF Command Prompt (cmd.exe)

.. figure:: ../../_static/esp-idf-installer-command-prompt.png
    :align: center
    :alt: ESP-IDF Command Prompt
    :figclass: align-center

    ESP-IDF Command Prompt

Using the Command Prompt
========================

For the remaining Getting Started steps, we're going to use the Windows Command Prompt.

ESP-IDF Tools Installer also creates a shortcut in the Start menu to launch the ESP-IDF Command Prompt. This shortcut launches the Command Prompt (cmd.exe) and runs ``export.bat`` script to set up the environment variables (``PATH``, ``IDF_PATH`` and others). Inside this command prompt, all the installed tools are available.

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

    windows-setup-update


.. _MSYS2: https://www.msys2.org/
.. _CMake: https://cmake.org/download/
.. _Ninja: https://ninja-build.org/
.. _Python: https://www.python.org/downloads/windows/
.. _Git for Windows: https://gitforwindows.org/
.. _Github Desktop: https://desktop.github.com/
