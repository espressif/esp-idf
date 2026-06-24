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

    For the Python version required by the EIM, please refer to the `EIM documentation <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#python-version>`_.


Step 2: Install the EIM
=======================

You can install the EIM using one of the following methods:

- `Download the EIM`_
- `Install the EIM with Package Manager Homebrew`_

Installing EIM with Homebrew allows you to easily keep EIM up to date with a single command.


Download the EIM
~~~~~~~~~~~~~~~~~

.. figure:: ../../_static/get-started-eim-download.drawio.png
   :alt: EIM Download Page
   :align: left
   :width: 35%
   :target: https://dl.espressif.com/dl/eim/

You can choose either an online or offline installer, available in Graphical User Interface (GUI) or Command Line Interface (CLI) versions.


Install the EIM with Package Manager Homebrew
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add the EIM repository to the `Homebrew <https://brew.sh/>`__ to make it available for installation:

.. code-block:: bash

    brew tap espressif/eim

Then, install the EIM Graphical User Interface (GUI) or Command Line Interface (CLI) via Homebrew:

- GUI:
    .. code-block:: bash

        brew install --cask eim-gui

- CLI:
    .. code-block:: bash

        brew install eim


Step 3: Install ESP-IDF Using EIM
=================================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    linux-macos-setup-legacy
