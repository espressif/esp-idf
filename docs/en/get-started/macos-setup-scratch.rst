***********************************************
Setup Toolchain for macOS from Scratch
***********************************************

:link_to_translation:`zh_CN:[中文]`

Package Manager
===============

To set up the toolchain from scratch, rather than :doc:`downloading a pre-compiled toolchain<macos-setup>`, you will need to install either the MacPorts_ or homebrew_ package manager.

MacPorts needs a full XCode installation, while homebrew only needs XCode command line tools.

    .. _homebrew: https://brew.sh/
    .. _MacPorts: https://www.macports.org/install.php

See :ref:`Customized Setup of Toolchain <get-started-customized-setup>` section for some of the reasons why installing the toolchain from scratch may be necessary.

Install Prerequisites
=====================

- install pip::

    sudo easy_install pip

- install pyserial::

    pip install --user pyserial

- install CMake & Ninja build:

  - If you have HomeBrew, you can run::

      brew install cmake ninja dfu-util

  - If you have MacPorts, you can run::

      sudo port install cmake ninja dfu-util

Compile the Toolchain from Source
=================================

- Install dependencies:

  - with MacPorts::

        sudo port install gsed gawk binutils gperf grep gettext wget libtool autoconf automake make

  - with homebrew::

        brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake make

Create a case-sensitive filesystem image::

    hdiutil create ~/esp/crosstool.dmg -volname "ctng" -size 10g -fs "Case-sensitive HFS+"

Mount it::

    hdiutil mount ~/esp/crosstool.dmg

Create a symlink to your work directory::

    mkdir -p ~/esp
    ln -s /Volumes/ctng ~/esp/ctng-volume

Go into the newly created directory::

    cd ~/esp/ctng-volume

Download ``crosstool-NG`` and build it:

.. include-build-file:: inc/scratch-build-code.inc

Build the toolchain::

    ./ct-ng xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf

Toolchain will be built in ``~/esp/ctng-volume/crosstool-NG/builds/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf``. To use it, you need to add ``~/esp/ctng-volume/crosstool-NG/builds/xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf/bin`` to ``PATH`` environment variable.

Python 2 deprecation
====================

Python 2 reached its `end of life <https://www.python.org/doc/sunset-python-2/>`_ and support for it in ESP-IDF will be removed soon. Please install Python 3.6 or higher. Instructions for macOS are listed above.

Next Steps
==========

To carry on with development environment setup, proceed to :ref:`get-started-get-esp-idf`.
