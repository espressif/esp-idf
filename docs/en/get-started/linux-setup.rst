*********************************************
Standard Setup of Toolchain for Linux
*********************************************

:link_to_translation:`zh_CN:[中文]`

Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages:

- CentOS 7::

    sudo yum install git wget flex bison gperf python pyserial python-pyelftools cmake ninja-build ccache

- Ubuntu and Debian::

    sudo apt-get install git wget flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python2-pip python2-pyserial python2-click python2-cryptography python2-future python2-pyparsing python2-pyelftools cmake ninja ccache

.. note::
    CMake version 3.5 or newer is required for use with ESP-IDF. Older Linux distributions may require updating, enabling of a "backports" repository, or installing of a "cmake3" package rather than "cmake".

Additional Tips
===============

Permission issues /dev/ttyUSB0
------------------------------

With some Linux distributions you may get the ``Failed to open port /dev/ttyUSB0`` error message when flashing the ESP32. :ref:`This can be solved by adding the current user to the dialout group<linux-dialout-group>`.

Next Steps
==========

To carry on with development environment setup, proceed to :ref:`get-started-get-esp-idf`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
