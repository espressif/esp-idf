************************************************
Installation of ESP-IDF and Tools on Windows
************************************************

:link_to_translation:`zh_CN:[中文]`

This section describes how to install ESP-IDF and its required tools on Windows using the Espressif Installation Manager (EIM).

.. note::

    This document describes the default and recommended way to install ESP-IDF v6.0 and newer versions. ESP-IDF also supports the :doc:`legacy method for updating ESP-IDF tools on Windows <windows-setup-update-legacy>`.


Step 1: Install the Prerequisites (Optional)
============================================

During ESP-IDF installation, the EIM automatically checks for required prerequisites and prompts you to install any missing prerequisites.

If automatic installation fails, you can install these prerequisites manually:

- `Git <https://git-scm.com/install/windows>`_
- `Python <https://www.python.org/downloads/windows/>`_

  .. note::

    Python 3.10 is the minimum supported version for ESP-IDF.

    For the Python version required by the EIM, please refer to the `EIM documentation <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#python-version>`_.


Step 2: Install the EIM
=======================

You can install the EIM using one of the following methods:

- Download the EIM

  .. figure:: ../../_static/get-started-eim-download.drawio.png
     :alt: EIM Download Page
     :align: left
     :width: 35%
     :target: https://dl.espressif.com/dl/eim/

  You can choose either an online or offline installer, available in Graphical User Interface (GUI) or Command Line Interface (CLI) versions.

- Install the EIM with Package Manager `WinGet <https://learn.microsoft.com/en-us/windows/package-manager/winget/>`__

  - Install the Graphical User Interface (GUI) or Command Line Interface (CLI):

    - GUI:

      .. code-block:: bash

          winget install Espressif.EIM
    - CLI:

      .. code-block:: bash

          winget install Espressif.EIM-CLI

  - This method makes it easy to keep EIM up to date with a single command.


Step 3: Install ESP-IDF Using EIM
=================================

.. include:: eim-install-idf.rst

.. toctree::
    :hidden:
    :maxdepth: 1
    :caption: Legacy Installation

    windows-setup-update-legacy
