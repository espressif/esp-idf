**********************************************
Standard Setup of Toolchain for Mac OS (CMake)
**********************************************

.. include:: ../cmake-warning.rst

Install Prerequisites
=====================

ESP-IDF will use the version of Python installed by default on Mac OS.

- install pip::

    sudo easy_install pip

- install pyserial::

    pip install --user pyserial

- install CMake & Ninja build:

  - If you have HomeBrew_, you can run::

      brew install cmake ninja

  - If you have MacPorts_, you can run::

      sudo port install cmake ninja

  - Otherwise, consult the CMake_ and Ninja_ home pages for Mac OS installation downloads.

- It is strongly recommended to also install ccache_ for faster builds. If you have HomeBrew_, this can be done via ``brew install ccache`` or ``sudo port install ccache`` on MacPorts_.

.. note::
   If an error like this is shown during any step::

     xcrun: error: invalid active developer path (/Library/Developer/CommandLineTools), missing xcrun at: /Library/Developer/CommandLineTools/usr/bin/xcrun

    Then you will need to install the XCode command line tools to continue. You can install these by running ``xcode-select --install``.

Toolchain Setup
===============

.. include:: /_build/inc/download-links.inc

ESP32 toolchain for macOS is available for download from Espressif website:

|download_link_osx|

Download this file, then extract it in ``~/esp`` directory:

.. include:: /_build/inc/unpack-code-osx.inc

.. _setup-macos-toolchain-add-it-to-path-cmake:

The toolchain will be extracted into ``~/esp/xtensa-esp32-elf/`` directory.

To use it, you will need to update your ``PATH`` environment variable in ``~/.profile`` file. To make ``xtensa-esp32-elf`` available for all terminal sessions, add the following line to your ``~/.profile`` file::

    export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH

Alternatively, you may create an alias for the above command. This way you can get the toolchain only when you need it. To do this, add different line to your ``~/.profile`` file::

    alias get_esp32="export PATH=$HOME/esp/xtensa-esp32-elf/bin:$PATH"

Then when you need the toolchain you can type ``get_esp32`` on the command line and the toolchain will be added to your ``PATH``.

Log off and log in back to make the ``.profile`` changes effective. Run the following command to verify if ``PATH`` is correctly set::

        printenv PATH


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf-cmake`.

Related Documents
=================

.. toctree::
    :maxdepth: 1

    macos-setup-scratch

.. _cmake: https://cmake.org/
.. _ninja: https://ninja-build.org/
.. _ccache: https://ccache.samba.org/
.. _homebrew: https://brew.sh/
.. _MacPorts: https://www.macports.org/install.php
