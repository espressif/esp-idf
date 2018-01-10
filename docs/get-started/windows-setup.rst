***************************************
Standard Setup of Toolchain for Windows
***************************************

Introduction
============

Windows doesn't have a built-in "make" environment, so as well as installing the toolchain you will need a GNU-compatible environment. We use the MSYS2_ environment to provide this. You don't need to use this environment all the time (you can use :doc:`Eclipse <eclipse-setup>` or some other front-end), but it runs behind the scenes.


Toolchain Setup 
===============

The quick setup is to download the Windows all-in-one toolchain & MSYS2 zip file from dl.espressif.com:

https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20180110.zip

Unzip the zip file to ``C:\`` (or some other location, but this guide assumes ``C:\``) and it will create an ``msys32`` directory with a pre-prepared environment.


Check it Out
============

Open a MSYS2 MINGW32 terminal window by running ``C:\msys32\mingw32.exe``. The environment in this window is a bash shell. 

.. figure:: ../_static/msys2-terminal-window.png
    :align: center
    :alt: MSYS2 MINGW32 shell window
    :figclass: align-center

    MSYS2 MINGW32 shell window

Use this window in the following steps setting up development environment for ESP32.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf`.

Updating The Environment
========================

When IDF is updated, sometimes new toolchains are required or new requirements are added to the Windows MSYS2 environment. To move any data from an old version of the precompiled environment to a new one:

- Take the old MSYS2 environment (ie ``C:\msys32``) and move/rename it to a different directory (ie ``C:\msys32_old``).
- Download the new precompiled environment using the steps above.
- Unzip the new MSYS2 environment to ``C:\msys32`` (or another location).
- Find the old ``C:\msys32_old\home`` directory and move this into ``C:\msys32``.
- You can now delete the ``C:\msys32_old`` directory if you no longer need it.

You can have independent different MSYS2 environments on your system, as long as they are in different directories.

There are :ref:`also steps to update the existing environment without downloading a new one <updating-existing-windows-environment>`, although this is more complex.

Related Documents
=================

.. toctree::
    :maxdepth: 1

    windows-setup-scratch


.. _MSYS2: https://msys2.github.io/
