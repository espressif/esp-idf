*********************************************************
Setup Toolchain for Mac OS from Scratch (Legacy GNU Make)
*********************************************************
:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst
                     
.. note::
    
    Standard process for installing the toolchain is described :doc:`here <macos-setup>`. See :ref:`Customized Setup of Toolchain <get-started-customized-setup-legacy>` section for some of the reasons why installing the toolchain from scratch may be necessary.

Install Prerequisites
=====================

- install pip::

    sudo easy_install pip

.. note::

    ``pip`` will be used later for installing :ref:`the required Python packages <get-started-get-packages-legacy>`.

Compile the Toolchain from Source
=================================

- Install dependencies:

  - Install either MacPorts_ or homebrew_ package manager. MacPorts needs a full XCode installation, while homebrew only needs XCode command line tools.
    
    .. _homebrew: https://brew.sh/
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

Toolchain will be built in ``~/esp/ctng-volume/crosstool-NG/builds/xtensa-esp32-elf``. Follow :ref:`instructions for standard setup <setup-macos-toolchain-add-it-to-path-legacy>` to add the toolchain to your ``PATH``.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-legacy`.
