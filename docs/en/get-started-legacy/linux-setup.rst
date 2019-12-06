*******************************************************
Standard Setup of Toolchain for Linux (Legacy GNU Make)
*******************************************************
:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst
                     
Install Prerequisites
=====================

To compile with ESP-IDF you need to get the following packages:

- CentOS 7::

    sudo yum install gcc git wget make flex bison gperf python python2-cryptography

- Ubuntu and Debian::

    sudo apt-get install gcc git wget make flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python-pyelftools

- Arch::

    sudo pacman -S --needed gcc git make flex bison gperf python2-pyserial python2-cryptography python2-future python2-pyparsing python2-pyelftools

.. note::

    Some older Linux distributions may be missing some of the Python packages listed above (or may use ``pyserial`` version 2.x which is not supported by ESP-IDF). It is possible to install these packages via ``pip`` instead - as described in section :ref:`get-started-get-packages-legacy`.

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

.. _setup-linux-toolchain-add-it-to-path-legacy:

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

With some Linux distributions you may get the ``Failed to open port /dev/ttyUSB0`` error message when flashing the ESP32. :ref:`This can be solved by adding the current user to the dialout group<linux-dialout-group-legacy>`.

Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-legacy`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    linux-setup-scratch


.. _AUR: https://wiki.archlinux.org/index.php/Arch_User_Repository
