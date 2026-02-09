************************************************
Installation of ESP-IDF and Tools on macOS
************************************************

:link_to_translation:`zh_CN:[中文]`

This section describes how to install ESP-IDF and its required tools on macOS using the Espressif Installation Manager (EIM).

.. note::

    This document describes the default and recommended way to install ESP-IDF v6.0 and newer versions. ESP-IDF also supports the :doc:`legacy installation method on macOS <linux-macos-setup-legacy>`, which was the default before ESP-IDF v6.0.


Step 1: Install the Prerequisites
=================================

Install the required prerequisites via `Homebrew <https://brew.sh/>`_:

.. code-block:: bash

    brew install libgcrypt glib pixman sdl2 libslirp dfu-util cmake python

.. note::

    Python 3.10 is the minimum supported version for ESP-IDF.

    However, for `Offline Installation`_, EIM requires **Python 3.11 or versions later**.


Step 2: Install the EIM
=======================

Add the EIM repository to the Homebrew to make it available for installation:

.. code-block:: bash

    brew tap espressif/eim

Then, install the EIM Graphical User Interface (GUI) or Command Line Interface (CLI) via Homebrew:

- GUI:
    .. code-block:: bash

        brew install --cask eim-gui

- CLI:
    .. code-block:: bash

        brew install eim

.. note::

    Installing via Homebrew makes it easier to keep EIM up to date.

    Alternatively, download the EIM installer for macOS from the `Espressif Download Page <https://dl.espressif.com/dl/eim/>`__, which provides both online and offline installers available in both CLI and GUI versions.


Step 3: Install ESP-IDF Using EIM
=================================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    linux-macos-setup-legacy
