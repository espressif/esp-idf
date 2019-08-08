********************************************************
Standard Setup of Toolchain for Mac OS (Legacy GNU Make)
********************************************************
:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst
                     
Install Prerequisites
=====================

- install pip::

    sudo easy_install pip

.. note::

    ``pip`` will be used later for installing :ref:`the required Python packages <get-started-get-packages-legacy>`.

Toolchain Setup
===============

.. include:: /_build/inc/download-links.inc

ESP32 toolchain for macOS is available for download from Espressif website:

|download_link_osx|

Download this file, then extract it in ``~/esp`` directory:

.. include:: /_build/inc/unpack-code-osx.inc

.. _setup-macos-toolchain-add-it-to-path-legacy:

The toolchain will be extracted into ``~/esp/xtensa-esp32-elf/`` directory.

To use it, you will need to update your ``PATH`` environment variable in ``~/.profile`` file. To make ``xtensa-esp32-elf`` available for all terminal sessions, add the following line to your ``~/.profile`` file::

    export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH

Alternatively, you may create an alias for the above command. This way you can get the toolchain only when you need it. To do this, add different line to your ``~/.profile`` file::

    alias get_esp32="export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH"

Then when you need the toolchain you can type ``get_esp32`` on the command line and the toolchain will be added to your ``PATH``.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-legacy`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    macos-setup-scratch
