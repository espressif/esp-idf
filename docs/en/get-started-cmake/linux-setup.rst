﻿*********************************************
Standard Setup of Toolchain for Linux (CMake)
*********************************************

.. include:: ../cmake-warning.rst

Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages:

- CentOS 7::

    sudo yum install git wget ncurses-devel flex bison gperf python pyserial cmake ninja-build ccache

- Ubuntu and Debian::

    sudo apt-get install git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial cmake ninja-build ccache

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial cmake ninja ccache

.. note::
    CMake version 3.5 or newer is required for use with ESP-IDF. Older Linux distributions may require updating, enabling of a "backports" repository, or installing of a "cmake3" package rather than "cmake".

Toolchain Setup
===============

.. include:: /_build/inc/download-links.inc

ESP32 toolchain for Linux is available for download from Espressif website:

- for 64-bit Linux:

  |download_link_linux64|

- for 32-bit Linux:

  |download_link_linux32|

1.  Download this file, then extract it in ``~/esp`` directory:

    - for 64-bit Linux:

      .. include:: /_build/inc/unpack-code-linux64.inc

    - for 32-bit Linux:

      .. include:: /_build/inc/unpack-code-linux32.inc

.. _setup-linux-toolchain-add-it-to-path-cmake:

2.  The toolchain will be extracted into ``~/esp/xtensa-esp32-elf/`` directory.

    To use it, you will need to update your ``PATH`` environment variable in ``~/.profile`` file. To make ``xtensa-esp32-elf`` available for all terminal sessions, add the following line to your ``~/.profile`` file::

        export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"

    Alternatively, you may create an alias for the above command. This way you can get the toolchain only when you need it. To do this, add different line to your ``~/.profile`` file::

        alias get_esp32='export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"'

    Then when you need the toolchain you can type ``get_esp32`` on the command line and the toolchain will be added to your ``PATH``.

    .. note::

        If you have ``/bin/bash`` set as login shell, and both ``.bash_profile`` and ``.profile`` exist, then update ``.bash_profile`` instead.

3.  Log off and log in back to make the ``.profile`` changes effective. Run the following command to verify if ``PATH`` is correctly set::

        printenv PATH

    You are looking for similar result containing toolchain's path at the end of displayed string::

        $ printenv PATH
        /home/user-name/bin:/home/user-name/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/home/user-name/esp/xtensa-esp32-elf/bin

    Instead of ``/home/user-name`` there should be a home path specific to your installation.


Permission issues /dev/ttyUSB0
------------------------------

With some Linux distributions you may get the ``Failed to open port /dev/ttyUSB0`` error message when flashing the ESP32. :ref:`This can be solved by adding the current user to the dialout group<linux-dialout-group-cmake>`.


Arch Linux Users
----------------

To run the precompiled gdb (xtensa-esp32-elf-gdb) in Arch Linux requires ncurses 5, but Arch uses ncurses 6.

Backwards compatibility libraries are available in AUR_ for native and lib32 configurations:

- https://aur.archlinux.org/packages/ncurses5-compat-libs/
- https://aur.archlinux.org/packages/lib32-ncurses5-compat-libs/

Before installing these packages you might need to add the author's public key to your keyring as described in the "Comments" section at the links above.

Alternatively, use crosstool-NG to compile a gdb that links against ncurses 6.


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-cmake`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
