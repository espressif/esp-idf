****************************************************
Setup Linux Toolchain from Scratch (Legacy GNU Make)
****************************************************
:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst
                     
.. note::
    
    Standard process for installing the toolchain is described :doc:`here <linux-setup>`. See :ref:`Customized Setup of Toolchain <get-started-customized-setup-legacy>` section for some of the reasons why installing the toolchain from scratch may be necessary.

Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages:

- Ubuntu and Debian::

    sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python-pyelftools libffi-dev libssl-dev

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial python2-cryptography python2-future python2-pyparsing python2-pyelftools

.. note::

    Some older (pre-2014) Linux distributions may use ``pyserial`` version 2.x which is not supported by ESP-IDF.
    In this case please install a supported version via ``pip`` as it is described in section
    :ref:`get-started-get-packages-legacy`.

Compile the Toolchain from Source
=================================

- Install dependencies:

  - CentOS 7::

        sudo yum install gawk gperf grep gettext ncurses-devel python python-devel automake bison flex texinfo help2man libtool

  - Ubuntu pre-16.04::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool

  - Ubuntu 16.04 or newer::

        sudo apt-get install gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin

  - Debian 9::

        sudo apt-get install gawk gperf grep gettext libncurses-dev python python-dev automake bison flex texinfo help2man libtool libtool-bin

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

Toolchain will be built in ``~/esp/crosstool-NG/builds/xtensa-esp32-elf``. Follow :ref:`instructions for standard setup <setup-linux-toolchain-add-it-to-path-legacy>` to add the toolchain to your ``PATH``.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-legacy`.
