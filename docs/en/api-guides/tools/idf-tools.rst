Downloadable Tools
==================

ESP-IDF build process relies on a number of tools: cross-compiler toolchains, CMake build system, and others.

Installing the tools using an OS-specific package manager (like apt, yum, brew, etc.) is the preferred method when the required version of the tool is available. This recommendation is reflected in the Getting Started guide. For example, on Linux and macOS it is recommended to install CMake using an OS package manager.

However, some of the tools are IDF-specific and are not available in OS package repositories. Furthermore, different versions of ESP-IDF require different versions of the tools to operate correctly. To solve these two problems, ESP-IDF provides a set of scripts for downloading and installing the correct versions of tools, and exposing them in the environment.

The rest of the document refers to these downloadable tools simply as "tools". Other kinds of tools used in ESP-IDF are:

* Python scripts bundled with ESP-IDF (such as ``idf.py``)
* Python packages installed from PyPI.

The following sections explain the installation method, and provide the list of tools installed on each platform.

.. note::

    This document is provided for advanced users who need to customize their installation, users who wish to understand the installation process, and ESP-IDF developers.

    If you are looking for instructions on how to install the tools, see the :doc:`Getting Started Guide <../../get-started/index>`.


Tools metadata file
-------------------

The list of tools and tool versions required for each platform is located in :idf_file:`tools/tools.json`. The schema of this file is defined by :idf_file:`tools/tools_schema.json`.

This file is used by :idf_file:`tools/idf_tools.py` script when installing the tools or setting up the environment variables.

.. _idf-tools-path:

Tools installation directory
----------------------------

``IDF_TOOLS_PATH`` environment variable specifies the location where the tools are to be downloaded and installed. If not set, ``IDF_TOOLS_PATH`` defaults to ``HOME/.espressif`` on Linux and macOS, and ``%USER_PROFILE%\.espressif`` on Windows.

Inside ``IDF_TOOLS_PATH``, the scripts performing tools installation create the following directories:

- ``dist`` — where the archives of the tools are downloaded.
- ``tools`` — where the tools are extracted. The tools are extracted into subdirectories: ``tools/TOOL_NAME/VERSION/``. This arrangement allows different versions of tools to be installed side by side.

GitHub Assets Mirror
--------------------

Most of the tools downloaded by the tools installer are GitHub Release Assets, which are files attached to a software release on GitHub.

If GitHub downloads are inaccessible or slow to access, it's possible to configure a GitHub assets mirror.

To use Espressif's download server, set the environment variable ``IDF_GITHUB_ASSETS`` to ``dl.espressif.com/github_assets``. When the install process is downloading a tool from ``github.com``, the URL will be rewritten to use this server instead.

Any mirror server can be used provided the URL matches the ``github.com`` download URL format: the install process will replace ``https://github.com`` with ``https://${IDF_GITHUB_ASSETS}`` for any GitHub asset URL that it downloads.

.. note:: The Espressif download server doesn't currently mirror everything from GitHub, it only mirrors files attached as Assets to some releases as well as source archives for some releases.


``idf_tools.py`` script
-----------------------

:idf_file:`tools/idf_tools.py` script bundled with ESP-IDF performs several functions:

* ``install``: Download the tool into ``${IDF_TOOLS_PATH}/dist`` directory, extract it into ``${IDF_TOOLS_PATH}/tools/TOOL_NAME/VERSION``.
  
  ``install`` command accepts the list of tools to install, in ``TOOL_NAME`` or ``TOOL_NAME@VERSION`` format. If ``all`` is given, all the tools (required and optional ones) are installed. If no argument or ``required`` is given, only the required tools are installed.

* ``download``: Similar to ``install`` but doesn't extract the tools. An optional ``--platform`` argument may be used to download the tools for the specific platform.

* ``export``: Lists the environment variables which need to be set to use the installed tools. For most of the tools, setting ``PATH`` environment variable is sufficient, but some tools require extra environment variables.

  The environment variables can be listed in either of ``shell`` or ``key-value`` formats, set by ``--format`` parameter:

  - ``shell`` produces output suitable for evaluation in the shell. For example,

    ::
  
        export PATH="/home/user/.espressif/tools/tool/v1.0.0/bin:$PATH"

    on Linux and macOS, and

    ::

        set "PATH=C:\Users\user\.espressif\tools\v1.0.0\bin;%PATH%"
    
    on Windows.

    .. note::

        Exporting environment variables in Powershell format is not supported at the moment. ``key-value`` format may be used instead.

    The output of this command may be used to update the environment variables, if the shell supports this. For example::

        eval $($IDF_PATH/tools/idf_tools.py export)

  - ``key-value`` produces output in `VARIABLE=VALUE` format, suitable for parsing by other scripts::

        PATH=/home/user/.espressif/tools/tool/v1.0.0:$PATH
    
    Note that the script consuming this output has to perform expansion of ``$VAR`` or ``%VAR%`` patterns found in the output. 

* ``list``: Lists the known versions of the tools, and indicates which ones are installed.

* ``check``: For each tool, checks whether the tool is available in the system path and in ``IDF_TOOLS_PATH``.

.. _idf-tools-install:

Install scripts
---------------

Shell-specific user-facing scripts are provided in the root of ESP-IDF repository to facilitate tools installation. These are:

* ``install.bat`` for Windows Command Prompt
* ``install.ps1`` for Powershell
* ``install.sh`` for Bash

Aside from downloading and installing the tools into ``IDF_TOOLS_PATH``, these scripts prepare a Python virtual environment, and install the required packages into that environment.

.. _idf-tools-export:

Export scripts
--------------

Since the installed tools are not permanently added into the user or system ``PATH`` environment variable, an extra step is required to use them in the command line. The following scripts modify the environment variables in the current shell to make the correct versions of the tools available:

* ``export.bat`` for Windows Command Prompt
* ``export.ps1`` for Powershell
* ``export.sh`` for Bash

.. note::

    To modify the shell environment in Bash, ``export.sh`` must be "sourced": ``. ./export.sh`` (note the leading dot and space).

    ``export.sh`` may be used with shells other than Bash (such as zsh). However in this case the ``IDF_PATH`` environment variable must be set before running the script. When used in Bash, the script will guess the ``IDF_PATH`` value from its own location.

In addition to calling ``idf_tools.py``, these scripts list the directories which have been added to the ``PATH``.

Other installation methods
--------------------------

Depending on the environment, more user-friendly wrappers for ``idf_tools.py`` are provided:

* :ref:`IDF Tools installer for Windows <get-started-windows-tools-installer>` can download and install the tools. Internally the installer uses ``idf_tools.py``.
* :doc:`Eclipse plugin for ESP-IDF <../../get-started/eclipse-setup>` includes a menu item to set up the tools. Internally the plugin calls ``idf_tools.py``.
* Visual Studio Code extension for ESP-IDF includes an onboarding flow. This flow helps setting up the tools. Although the extension does not rely on ``idf_tools.py``, the same installation method is used.

Custom installation
-------------------

Although the methods above are recommended for ESP-IDF users, they are not a must for building ESP-IDF applications. ESP-IDF build system expects that all the necessary tools are installed somewhere, and made available in the ``PATH``.

.. _idf-tools-list:

List of IDF Tools
-----------------

.. include-build-file:: idf-tools-inc.rst
