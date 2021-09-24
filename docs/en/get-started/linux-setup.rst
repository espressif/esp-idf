*********************************************
Standard Setup of Toolchain for Linux
*********************************************

:link_to_translation:`zh_CN:[中文]`

Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages:

- CentOS 7::

    sudo yum -y update && sudo yum install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache dfu-util

CentOS 7 is still supported but CentOS version 8 is recommended for a better user experience.

- Ubuntu and Debian::

    sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python-pip cmake ninja ccache dfu-util

.. note::
    CMake version 3.5 or newer is required for use with ESP-IDF. Older Linux distributions may require updating, enabling of a "backports" repository, or installing of a "cmake3" package rather than "cmake".

Additional Tips
===============

Permission issues /dev/ttyUSB0
------------------------------

With some Linux distributions you may get the ``Failed to open port /dev/ttyUSB0`` error message when flashing the {IDF_TARGET_NAME}. :ref:`This can be solved by adding the current user to the dialout group<linux-dialout-group>`.

Fixing broken pip on Ubuntu 16.04
=================================

Package ``python3-pip`` could be broken without possibility to upgrade it. Package has to be removed and installed manually using script `get-pip.py <https://bootstrap.pypa.io/get-pip.py>`_.::

    apt remove python3-pip python3-virtualenv; rm -r ~/.local
    rm -r ~/.espressif/python_env && python get-pip.py

Python 2 deprecation
====================

Python 2 reached its `end of life <https://www.python.org/doc/sunset-python-2/>`_ and support for it in ESP-IDF will be removed soon. Please install Python 3.6 or higher. Instructions for popular Linux distributions are listed above.

Next Steps
==========

To carry on with development environment setup, proceed to :ref:`get-started-get-esp-idf`.

.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
