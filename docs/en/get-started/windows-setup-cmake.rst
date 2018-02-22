***************************************
Setup for cmake on Windows
***************************************

.. note::
      The CMake-based build system is currently in preview release. Documentation may have missing gaps, and you may enocunter bugs (please report these). The original (non-cmake) version of this doc is :doc:`here<windows-setup>`.

.. note::
   Windows support in particular currently requires more manual installation of tools and setup of system Path than is planned for final release.

Introduction
============

Unlike the conventional ESP-IDF make-based build environment, cmake does not require a GNU-compatible environment for building. You can build with cmake from Windows Command Prompt, or from an IDE with cmake support.

Tools
=====

The following software packages need to be installed:

cmake
^^^^^

Download the latest stable release of CMake_ for Windows and run the installer.

When the installer asks for Install Options, choose either "Add CMake to the system PATH for all users" or "Add CMake to the system PATH for the current user".


Ninja build
^^^^^^^^^^^

Download the ninja_ latest stable Windows release from the (`download page <ninja-dl>`_).

The Ninja for Windows download is a .zip file containing a single ``ninja.exe`` file which needs to be unzipped to a directory on your PATH.

An easy directory to place ``ninja.exe`` is the CMake executable directory, as this is already on the PATH. This directory will be something like ``C:\Program Files\CMake\bin`` (look for the file ``cmake.exe`` in this directory to know it's the correct one).

Python 2.x
^^^^^^^^^^

Download the latest Python_ 2.7 for Windows installer, and run it.

The "Customise" step of the Python installer gives a list of options. The last option is "Add python.exe to Path". Change this option to select "Will be installed".

Once Python is installed, open a Windows Command Prompt from the Start menu and run the following command::

  pip install pyserial


Toolchain Setup
===============

Download the precompiled Windows toolchain from dl.espressif.com:

https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip

Unzip the zip file to ``C:\Program Files`` (or some other location). The zip file contains a single directory ``xtensa-esp32-elf``.

Next, you need to add the ``bin`` subdirectory to your PATH. For example, the directory to add may be ``C:\Program Files\xtensa-esp32-elf\bin``.

Open the System control panel and navigate to the Environment Variables dialog. (On Windows 10, this is found under Advanced System Settings).

Double-click the ``Path`` variable (either User or System Path, depending if you want other users to use the toolchain.) Go to the end of the value, and append ``;C:\Program Files\xtensa-esp32-elf\bin`` (or whatever the path to your toolchain ``bin`` directory is).

.. note::
   If you already have the MSYS2 environment (for use with the "GNU Make" build system) installed, you can skip the separate download and add the directory ``C:\msys32\opt\xtensa-esp32-elf\bin`` to the Path instead, as the toolchain is included in the MSYS2 environment.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-cmake`.


.. _cmake: https://cmake.org/download/
.. _ninja: https://ninja-build.org/
.. _ninja-dl: https://github.com/ninja-build/ninja/releases
.. _Python: https://www.python.org/downloads/windows/
