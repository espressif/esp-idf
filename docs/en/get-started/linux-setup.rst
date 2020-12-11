*************************************
Standard Setup of Toolchain for Linux
*************************************
:link_to_translation:`zh_CN:[中文]`

Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages:

- CentOS 7::

    sudo yum -y update && sudo yum install git wget flex bison gperf python3

CentOS 7 is still supported but CentOS version 8 is recommended for a better user experience.

- Ubuntu and Debian::

    sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools libffi-dev libssl-dev

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf

.. note::

    Some older Linux distributions may be missing some of the Python packages listed above (or may use ``pyserial`` version 2.x which is not supported by ESP-IDF). It is possible to install these packages via ``pip`` instead - as described in section :ref:`get-started-get-packages`.

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

.. _setup-linux-toolchain-add-it-to-path:

2.  The toolchain will be extracted into ``~/esp/xtensa-esp32-elf/`` directory.

    To use it, you will need to update your ``PATH`` environment variable in ``~/.profile`` file. To make ``xtensa-esp32-elf`` available for all terminal sessions, add the following line to your ``~/.profile`` file::

        export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"

    Alternatively, you may create an alias for the above command. This way you can get the toolchain only when you need it. To do this, add different line to your ``~/.profile`` file::

        alias get_esp32='export PATH="$HOME/esp/xtensa-esp32-elf/bin:$PATH"'

    Then when you need the toolchain you can type ``get_esp32`` on the command line and the toolchain will be added to your ``PATH``.

    .. note::

        If you have ``/bin/bash`` set as login shell, and both ``.bash_profile`` and ``.profile`` exist, then update ``.bash_profile`` instead. In CentOS, ``alias`` should set in ``.bashrc``.

3.  Log off and log in back to make the ``.profile`` changes effective. Run the following command to verify if ``PATH`` is correctly set::

        printenv PATH

    You are looking for similar result containing toolchain's path at the beginning of displayed string::

        $ printenv PATH
        /home/user-name/esp/xtensa-esp32-elf/bin:/home/user-name/bin:/home/user-name/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin

    Instead of ``/home/user-name`` there should be a home path specific to your installation.


Permission issues /dev/ttyUSB0
------------------------------

With some Linux distributions you may get the ``Failed to open port /dev/ttyUSB0`` error message when flashing the ESP32. :ref:`This can be solved by adding the current user to the dialout group<linux-dialout-group>`.

Arch Linux Users
----------------

To run the precompiled gdb (xtensa-esp32-elf-gdb) in Arch Linux requires ncurses 5, but Arch uses ncurses 6. 

Backwards compatibility libraries are available in AUR_ for native and lib32 configurations:

- https://aur.archlinux.org/packages/ncurses5-compat-libs/
- https://aur.archlinux.org/packages/lib32-ncurses5-compat-libs/

Before installing these packages you might need to add the author's public key to your keyring as described in the "Comments" section at the links above.

Alternatively, use crosstool-NG to compile a gdb that links against ncurses 6.

Setting up Python 3 as default for CentOS
-----------------------------------------

CentOS 7 and older is providing Python 2.7 as the default interpreter.
Python 3 is recommended instead and can be installed in old distributions as follows, or please consult the documentation of your operating system for other recommended ways to achieve this::

    sudo yum -y update && sudo yum install python3 python3-pip python3-setuptools

Making Python 3 the default interpreter is possible by running::

    sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10 && alias pip=pip3

Setting up Python 3 as default for Ubuntu and Debian
----------------------------------------------------

Ubuntu (version 18.04 and older) and Debian (version 9 and older) are still providing Python 2.7 as the default interpreter.
Python 3 is recommended instead and can be installed in old distributions as follows, or please consult the documentation of your operating system for other recommended ways to achieve this::

        sudo apt-get install python3 python3-pip python3-setuptools

Making Python 3 the default interpreter is possible by running::

        sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10 && alias pip=pip3

.. note::
    This is system-wide change which may affect all of the applications.

Fixing broken pip on Ubuntu 16.04
=================================

Package ``python3-pip`` could be broken without possibility to upgrade it. 
Package has to be removed and installed manually using script `get-pip.py <https://bootstrap.pypa.io/get-pip.py>`_.::

    apt remove python3-pip python3-virtualenv; rm -r ~/.local
    rm -r ~/.espressif/python_env && python get-pip.py

Python 2 deprecation
====================

Python 2 reached its `end of life <https://www.python.org/doc/sunset-python-2/>`_ and support for it in ESP-IDF will be removed soon. Please install Python 3.6 or higher. Instructions for popular Linux distributions are listed above.

Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
