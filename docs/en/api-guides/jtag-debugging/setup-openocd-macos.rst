************************
Set up OpenOCD for MacOS
************************
:link_to_translation:`zh_CN:[中文]`

Install libusb
==============

Use `Homebrew <https://brew.sh/>`_ or `Macports <https://www.macports.org/>`_ to install `libusb` package.

Set up OpenOCD
==============

OpenOCD for MacOS is available for download from Github:

https://github.com/espressif/openocd-esp32/releases

Download latest release archive with `macos` in its name, for example `openocd-esp32-macos-0.10.0-esp32-20180418.tar.gz`.

Extract the downloaded file in ``~/esp/`` directory::

    cd ~/esp
    tar -xzf ~/Downloads/openocd-esp32-macos-<version>.tar.gz 


Next Steps
==========

To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-configuring-esp32-target`.


Related Documents
=================

.. toctree::
    :maxdepth: 1

    building-openocd-macos

