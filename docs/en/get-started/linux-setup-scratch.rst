******************************************
Setup Linux Toolchain from Scratch
******************************************

:link_to_translation:`zh_CN:[中文]`

The following instructions are alternative to downloading binary toolchain from Espressif website. To quickly setup the binary toolchain, instead of compiling it yourself, backup and proceed to section :doc:`linux-setup`.

Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages:

- CentOS 7::

    sudo yum install git wget ncurses-devel flex bison gperf python pyserial python-pyelftools cmake ninja-build ccache

- Ubuntu and Debian::

    sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache libffi-dev libssl-dev

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial python2-click python2-cryptography python2-future python2-pyparsing python2-pyelftools cmake ninja ccache

.. note::
    CMake version 3.5 or newer is required for use with ESP-IDF. Older Linux distributions may require updating, enabling of a "backports" repository, or installing of a "cmake3" package rather than "cmake".

Compile the Toolchain from Source
=================================

- Install dependencies:

  - CentOS 7::

        sudo yum install gawk gperf grep gettext ncurses-devel python python-devel automake bison flex texinfo help2man libtool make

  - Ubuntu pre-16.04::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool make

  - Ubuntu 16.04 or newer::

        sudo apt-get install gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin make

  - Debian 9::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool libtool-bin make

  - Arch::

        TODO

Create the working directory and go into it::

  mkdir -p ~/esp
  cd ~/esp

Download ``crosstool-NG`` and build it:

.. include:: /_build/inc/scratch-build-code.inc

Build the toolchain::

    ./ct-ng xtensa-esp32-elf
    ./ct-ng build
    chmod -R u+w builds/xtensa-esp32-elf

Toolchain will be built in ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``. Follow `instructions for standard setup <setup-linux-toolchain-add-it-to-path>`_ to add the toolchain to your ``PATH``.


Next Steps
==========

To carry on with development environment setup, proceed to :ref:`get-started-get-esp-idf`.
