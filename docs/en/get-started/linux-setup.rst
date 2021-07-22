*********************************************
Standard Setup of Toolchain for Linux
*********************************************

:link_to_translation:`zh_CN:[中文]`

Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages. The command to run depends on which distribution of Linux you are using:

- Ubuntu and Debian::

    sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

- CentOS 7 & 8::

    sudo yum -y update && sudo yum install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache dfu-util libusbx

CentOS 7 is still supported but CentOS version 8 is recommended for a better user experience.

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python-pip cmake ninja ccache dfu-util libusb

.. note::
    - CMake version 3.5 or newer is required for use with ESP-IDF. Older Linux distributions may require updating, enabling of a "backports" repository, or installing of a "cmake3" package rather than "cmake".
    - If you do not see your Linux distribution in the above list then please check its documentation to find out which command to use for package installation.

Additional Tips
===============

Permission issues /dev/ttyUSB0
------------------------------

With some Linux distributions you may get the ``Failed to open port /dev/ttyUSB0`` error message when flashing the {IDF_TARGET_NAME}. :ref:`This can be solved by adding the current user to the dialout group<linux-dialout-group>`.

Python compatibility
====================

ESP-IDF supports Python 3.6 or newer. It is recommended to upgrade your operating system to a recent version satisfying this requirement. Other options include the installation of Python from `sources <https://www.python.org/downloads/>`_ or the use of a Python version management system such as `pyenv <https://github.com/pyenv/pyenv>`_.

Next Steps
==========

To carry on with development environment setup, proceed to :ref:`get-started-get-esp-idf`.


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
