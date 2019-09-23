**********************************************
Standard Setup of Toolchain for Mac OS
**********************************************

:link_to_translation:`zh_CN:[中文]`

Install Prerequisites
=====================

ESP-IDF will use the version of Python installed by default on macOS.

- install pip::

    sudo easy_install pip

- install pyserial::

    pip install --user pyserial

- install CMake & Ninja build:

  - If you have HomeBrew_, you can run::

      brew install cmake ninja

  - If you have MacPorts_, you can run::

      sudo port install cmake ninja

  - Otherwise, consult the CMake_ and Ninja_ home pages for macOS installation downloads.

- It is strongly recommended to also install ccache_ for faster builds. If you have HomeBrew_, this can be done via ``brew install ccache`` or ``sudo port install ccache`` on MacPorts_.

.. note::
   If an error like this is shown during any step::

     xcrun: error: invalid active developer path (/Library/Developer/CommandLineTools), missing xcrun at: /Library/Developer/CommandLineTools/usr/bin/xcrun

   Then you will need to install the XCode command line tools to continue. You can install these by running ``xcode-select --install``.

Next Steps
==========

To carry on with development environment setup, proceed to :ref:`get-started-get-esp-idf`.

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
