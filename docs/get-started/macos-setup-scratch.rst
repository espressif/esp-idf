***************************************
Setup Toolchain for Mac OS from Scratch
***************************************

Install Prerequisites
=====================

- install pip::

    sudo easy_install pip

- install pyserial::

    sudo pip install pyserial


Compile the Toolchain from Source
=================================

- Install dependencies:

  - Install either MacPorts_ or homebrew_ package manager. MacPorts needs a full XCode installation, while homebrew only needs XCode command line tools.
    
    .. _homebrew: http://brew.sh/
    .. _MacPorts: https://www.macports.org/install.php

  - with MacPorts::

        sudo port install gsed gawk binutils gperf grep gettext wget libtool autoconf automake

  - with homebrew::

        brew install gnu-sed gawk binutils gperftools gettext wget help2man libtool autoconf automake

Create a case-sensitive filesystem image::

    hdiutil create ~/esp/crosstool.dmg -volname "ctng" -size 10g -fs "Case-sensitive HFS+"

Mount it::

    hdiutil mount ~/esp/crosstool.dmg

Create a symlink to your work directory::

    cd ~/esp
    ln -s /Volumes/ctng crosstool-NG

Download ``crosstool-NG`` and build it::

    cd ~/esp
    git clone -b xtensa-1.22.x https://github.com/espressif/crosstool-NG.git
    cd crosstool-NG
    ./bootstrap && ./configure --enable-local && make install

Build the toolchain::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

Toolchain will be built in ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``. Follow :ref:`instructions for standard setup <setup-macos-toolchain-add-it-to-path>` to add the toolchain to your ``PATH``.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf`.
