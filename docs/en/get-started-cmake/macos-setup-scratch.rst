***********************************************
Setup Toolchain for Mac OS from Scratch (CMake)
***********************************************

.. include:: ../cmake-warning.rst

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

      brew install cmake ninja

  - If you have MacPorts, you can run::

      sudo port install cmake ninja

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

.. include:: /_build/inc/scratch-build-code.inc

Build the toolchain::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

Toolchain will be built in ``~/esp/ctng-volume/crosstool-NG/builds/xtensa-esp32-elf``. Follow :ref:`instructions for standard setup <setup-macos-toolchain-add-it-to-path-cmake>` to add the toolchain to your ``PATH``.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-cmake`.
