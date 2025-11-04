************************************************
Installation of ESP-IDF and Tools on Linux
************************************************

:link_to_translation:`zh_CN:[中文]`

This section describes how to install ESP-IDF and its required tools on Linux distributions (e.g., Ubuntu) using the ESP-IDF Installation Manager (EIM).

.. note::

    This document describes the default and recommended way to install ESP-IDF v6.0 and newer versions. ESP-IDF also supports the :doc:`legacy installation method on Linux <linux-macos-setup-legacy>`, which was the default before ESP-IDF v6.0.


Step 1: Install the Prerequisites (Optional)
============================================

Skip this step if you plan to install EIM using :ref:`APT <install-eim-linux-apt>`.

For other installation methods, install the `required prerequisites <https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html#linux>`_. These prerequisites may vary depending on your Linux distribution.

.. note::

    Python 3.10 is the minimum supported version for ESP-IDF.

    However, for `Offline Installation`_, EIM requires **Python 3.11 or versions later**.


Step 2: Install the EIM
=======================

You can install the EIM using one of the following methods:

- `Debian-Based Linux Installation via APT`_
- `RPM-Based Linux Installation via DNF`_
- `Download the EIM Installer`_

Installing via APT or DNF allows you to easily keep EIM up to date.


.. _install-eim-linux-apt:

Debian-Based Linux Installation via APT
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add the EIM repository to your APT sources list to make it available for installation:

.. code-block:: bash

    echo "deb [trusted=yes] https://dl.espressif.com/dl/eim/apt/ stable main" | sudo tee /etc/apt/sources.list.d/espressif.list

    sudo apt update

Then, install the EIM Command Line Interface (CLI) alone, or together with Graphical User Interface (GUI) via APT:

- GUI and CLI:
    .. code-block:: bash

        sudo apt install eim

- CLI only:
    .. code-block:: bash

        sudo apt install eim-cli


RPM-Based Linux Installation via DNF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add the EIM repository to your DNF sources list to make it available for installation:

.. code-block:: bash

    sudo tee /etc/yum.repos.d/espressif-eim.repo << 'EOF'
    [eim]
    name=ESP-IDF Installation Manager
    baseurl=https://dl.espressif.com/dl/eim/rpm/$basearch
    enabled=1
    gpgcheck=0
    EOF

Then, install the EIM Command Line Interface (CLI) alone, or together with Graphical User Interface (GUI) via DNF:

- GUI and CLI:
    .. code-block:: bash

        sudo dnf install eim

- CLI only:
    .. code-block:: bash

        sudo dnf install eim-cli


Download the EIM Installer
~~~~~~~~~~~~~~~~~~~~~~~~~~

Alternatively, download the EIM installer for Linux from the `Espressif Download Page <https://dl.espressif.com/dl/eim/>`__, which provides both online and offline installers available in both CLI and GUI versions.


Step 3: Install ESP-IDF Using EIM
=================================

.. include:: eim-install-idf.rst
