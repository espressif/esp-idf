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

Inside ``IDF_TOOLS_PATH``, the scripts performing tools installation create the following directories and files:

- ``dist`` — where the archives of the tools are downloaded.
- ``tools`` — where the tools are extracted. The tools are extracted into subdirectories: ``tools/TOOL_NAME/VERSION/``. This arrangement allows different versions of tools to be installed side by side.
- ``idf-env.json`` — user install options (targets, features) are stored in this file. Targets are selected chip targets for which tools are installed and kept up-to-date. Features determine the Python package set which should be installed. These options will be discussed later.
- ``python_env`` —  not tools related; virtual Python environments are installed in the sub-directories.
- ``espidf.constraints.*.txt`` — one constraint file for each ESP-IDF release containing Python package version requirements.

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

  - ``export`` optional parameters:
    
    - ``--unset`` Creates statement that unset some global variables, so the environment gets to the state it was before calling ``export.{sh/fish}``.
    - ``--add_paths_extras`` Adds extra ESP-IDF-related paths of ``$PATH`` to ``${IDF_TOOLS_PATH}/esp-idf.json``, which is used to remove global variables when the active ESP-IDF environment is deactivated. Example: While processing ``export.{sh/fish}`` script, new paths are added to global variable ``$PATH``. This option is used to save these new paths to the ``${IDF_TOOLS_PATH}/esp-idf.json``.

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

* ``install-python-env``: Create a Python virtual environment in the ``${IDF_TOOLS_PATH}/python_env`` directory and install there the required Python packages. An optional ``--features`` argument allows one to specify a comma-separated list of features. For each feature a requirements file must exist. For example, feature ``XY`` is a valid feature if ``${IDF_PATH}/tools/requirements/requirements.XY.txt`` is an existing file with a list of Python packages to be installed. There is one mandatory ``core`` feature ensuring core functionality of ESP-IDF (build, flash, monitor, debug in console). There can be an arbitrary number of optional features. The selected list of features is stored in ``idf-env.json``. The requirement files contain a list of the desired Python packages to be installed and ``espidf.constraints.*.txt`` downloaded from https://dl.espressif.com and stored in ``${IDF_TOOLS_PATH}`` the package version requirements for a given ESP-IDF version.

* ``check-python-dependencies``: Checks if all required Python packages are installed. Packages from ``${IDF_PATH}/tools/requirements/requirements.*.txt`` files selected by the feature list of ``idf-env.json`` are checked with the package versions specified in the ``espidf.constraints.*.txt`` file. The constraint file will be downloaded from https://dl.espressif.com if this step hasn't been done already in the last day.

* ``uninstall``: Print and remove tools, that are currently not used by active ESP-IDF version.

  - ``--dry-run`` Print installed unused tools.
  - ``--remove-archives`` Additionally remove all older versions of previously downloaded installation packages.
  
.. _idf-tools-install:

Install scripts
---------------

Shell-specific user-facing scripts are provided in the root of ESP-IDF repository to facilitate tools installation. These are:

* ``install.bat`` for Windows Command Prompt
* ``install.ps1`` for Powershell
* ``install.sh`` for Bash
* ``install.fish`` for Fish

Aside from downloading and installing the tools into ``IDF_TOOLS_PATH``, these scripts prepare a Python virtual environment, and install the required packages into that environment.

These scripts accept optionally a comma separated list of chip targets and ``--enable-*`` arguments for enabling features. These arguments are passed to the ``idf_tools.py`` script which stores them in ``idf-env.json``. Therefore, chip targets and features can be enabled incrementally.

Running the scripts without any optional arguments will install tools for all chip targets (by running ``idf_tools.py install --targets=all``) and Python packages for core ESP-IDF functionality (by running ``idf_tools.py install-python-env --features=core``).

Or for example, ``install.sh esp32`` will install tools only for ESP32. See the :ref:`Getting Started Guide <get-started-set-up-tools>` for more examples.

``install.sh --enable-XY`` will enable feature ``XY`` (by running ``idf_tools.py install-python-env --features=core,XY``).

.. _idf-tools-export:

Export scripts
--------------

Since the installed tools are not permanently added into the user or system ``PATH`` environment variable, an extra step is required to use them in the command line. The following scripts modify the environment variables in the current shell to make the correct versions of the tools available:

* ``export.bat`` for Windows Command Prompt
* ``export.ps1`` for Powershell
* ``export.sh`` for Bash
* ``export.fish`` for Fish

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
