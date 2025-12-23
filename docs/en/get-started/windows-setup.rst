************************************************
Installation of ESP-IDF and Tools on Windows
************************************************

:link_to_translation:`zh_CN:[中文]`

This section describes how to install ESP-IDF and its required tools on Windows using the Espressif Installation Manager (EIM).

.. note::

    This document describes the default and recommended way to install ESP-IDF v6.0 and newer versions. ESP-IDF also supports the :doc:`legacy method for updating ESP-IDF tools on Windows <windows-setup-update-legacy>`.


Step 1: Install the EIM
=======================

Install the EIM Graphical User Interface (GUI) or Command Line Interface (CLI) via `WinGet <https://learn.microsoft.com/en-us/windows/package-manager/winget/>`__:

- GUI:
    .. code-block:: bash

        winget install Espressif.EIM

- CLI:
    .. code-block:: bash

        winget install Espressif.EIM-CLI

.. note::

    Installing via WinGet makes it easier to keep EIM up to date.

    Alternatively, download the EIM installer for Windows from the `Espressif Download Page <https://dl.espressif.com/dl/eim/>`__, which provides both online and offline installers available in both CLI and GUI versions.


Step 2: Install ESP-IDF Using EIM
=================================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    windows-setup-update-legacy
