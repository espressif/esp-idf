*********************************************************
Standard Setup of Toolchain for Windows (Legacy GNU Make)
*********************************************************
:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst
                     
Introduction
============

Windows doesn't have a built-in "make" environment, so as well as installing the toolchain you will need a GNU-compatible environment. We use the MSYS2_ environment to provide this. You don't need to use this environment all the time (you can use :doc:`Eclipse <eclipse-setup>` or some other front-end), but it runs behind the scenes.


Toolchain Setup 
===============

The quick setup is to download the Windows all-in-one toolchain & MSYS2 zip file from dl.espressif.com:

https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_gcc8_toolchain-20191231.zip

Unzip the zip file to ``C:\`` (or some other location, but this guide assumes ``C:\``) and it will create an ``msys32`` directory with a pre-prepared environment.

.. important::

    If another toolchain location is used (different than the default ``C:\msys32``), please ensure that the path where the all-in-one toolchain gets unzipped is a plain ASCII, contains no spaces, symlinks or accents.


Check it Out
============

Open a MSYS2 MINGW32 terminal window by running ``C:\msys32\mingw32.exe``. The environment in this window is a bash shell. Create a directory named ``esp`` that is a default location to develop ESP32 applications. To do so, run the following shell command::

    mkdir -p ~/esp

By typing ``cd ~/esp`` you can then move to the newly created directory. If there are no error messages you are done with this step.

.. figure:: ../../_static/msys2-terminal-window.png
    :align: center
    :alt: MSYS2 MINGW32 shell window
    :figclass: align-center

    MSYS2 MINGW32 shell window

Use this window in the following steps setting up development environment for ESP32.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-legacy`.

Updating The Environment
========================

When IDF is updated, sometimes new toolchains are required or new requirements are added to the Windows MSYS2 environment. To move any data from an old version of the precompiled environment to a new one:

- Take the old MSYS2 environment (ie ``C:\msys32``) and move/rename it to a different directory (ie ``C:\msys32_old``).
- Download the new precompiled environment using the steps above.
- Unzip the new MSYS2 environment to ``C:\msys32`` (or another location).
- Find the old ``C:\msys32_old\home`` directory and move this into ``C:\msys32``.
- You can now delete the ``C:\msys32_old`` directory if you no longer need it.

You can have independent different MSYS2 environments on your system, as long as they are in different directories.

There are :ref:`also steps to update the existing environment without downloading a new one <updating-existing-windows-environment-legacy>`, although this is more complex.

Related Documents
=================

.. toctree::
    :maxdepth: 1

    windows-setup-scratch


.. _MSYS2: https://www.msys2.org/
