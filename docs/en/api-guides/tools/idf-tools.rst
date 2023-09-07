Downloadable IDF Tools
==========================

:link_to_translation:`zh_CN:[中文]`

The ESP-IDF build process relies on a number of tools: cross-compiler toolchains, CMake build system, and others.

Installing the tools using an OS-specific package manager (e.g., apt, yum, brew, etc.) is the preferred method, when the required version of the tool is available. This recommendation is reflected in the :doc:`Get Started <../../get-started/index>`. For example, on Linux and macOS, it is recommended to install CMake using an OS package manager.

However, some of the tools are specific to ESP-IDF and are not available in OS package repositories. Furthermore, different ESP-IDF versions require different tool versions for proper operation. To solve these two problems, ESP-IDF provides a set of scripts that can download and install the correct tool versions and set up the environment accordingly.

The rest of the document refers to these downloadable tools simply as "tools". Other kinds of tools used in ESP-IDF are:

* Python scripts bundled with ESP-IDF such as ``idf.py``
* Python packages installed from PyPI

The following sections explain the installation method and provide the list of tools installed on each platform.

.. note::

    This document is provided for advanced users who need to customize their installation, users who wish to understand the installation process, and ESP-IDF developers.

    If you are looking for instructions on how to install the tools, see :doc:`Get Started <../../get-started/index>`.


Tools Metadata File
-------------------

The list of tools and tool versions required for each platform is located in :idf_file:`tools/tools.json`. The schema of this file is defined by :idf_file:`tools/tools_schema.json`.

This file is used by the :idf_file:`tools/idf_tools.py` script when installing the tools or setting up the environment variables.

.. _idf-tools-path:

Tools Installation Directory
----------------------------

The ``IDF_TOOLS_PATH`` environment variable specifies the location where the tools are to be downloaded and installed. If not set, the default location will be ``HOME/.espressif`` on Linux and macOS, and ``%USER_PROFILE%\.espressif`` on Windows.

Inside the ``IDF_TOOLS_PATH`` directory, the tools installation scripts create the following directories and files:

- ``dist`` — where the archives of the tools are downloaded.
- ``tools`` — where the tools are extracted. The tools are extracted into subdirectories: ``tools/TOOL_NAME/VERSION/``. This arrangement allows different versions of tools to be installed side by side.
- ``idf-env.json`` — user install options, such as targets and features, are stored in this file. Targets are selected chip targets for which tools are installed and kept up-to-date. Features determine the Python package set which should be installed. These options will be discussed later.
- ``python_env`` —  not related to the tools; virtual Python environments are installed in the sub-directories. Note that the Python environment directory can be placed elsewhere by setting the ``IDF_PYTHON_ENV_PATH`` environment variable.
- ``espidf.constraints.*.txt`` — one constraint file for each ESP-IDF release containing Python package version requirements.

GitHub Assets Mirror
--------------------

Most of the tools downloaded by the tools installer are GitHub Release Assets, which are files attached to a software release on GitHub.

If GitHub downloads are inaccessible or slow to access, a GitHub assets mirror can be configured.

To use Espressif's download server, set the environment variable ``IDF_GITHUB_ASSETS`` to ``dl.espressif.com/github_assets``, or ``dl.espressif.cn/github_assets`` for faster download in China. When the install process is downloading a tool from ``github.com``, the URL will be rewritten to use this server instead.

Any mirror server can be used provided the URL matches the ``github.com`` download URL format. For any GitHub asset URL that the install process downloads, it will replace ``https://github.com`` with ``https://${IDF_GITHUB_ASSETS}``.

.. note:: The Espressif download server currently does not mirror everything from GitHub, but only files attached as Assets to some releases, as well as source archives for some releases.


``idf_tools.py`` Script
-----------------------

The :idf_file:`tools/idf_tools.py` script bundled with ESP-IDF performs several functions:

* ``install``: Download the tool into the ``${IDF_TOOLS_PATH}/dist`` directory and extract it into ``${IDF_TOOLS_PATH}/tools/TOOL_NAME/VERSION``.

  The ``install`` command accepts the list of tools to install in the ``TOOL_NAME`` or ``TOOL_NAME@VERSION`` format. If ``all`` is given, all the tools, including required and optional ones, are installed. If no argument or ``required`` is given, only the required tools are installed.

* ``download``: Similar to ``install`` but doesn't extract the tools. An optional ``--platform`` argument may be used to download the tools for the specific platform.

* ``export``: Lists the environment variables that need to be set to use the installed tools. For most of the tools, setting the ``PATH`` environment variable is sufficient, but some tools require extra environment variables.

  The environment variables can be listed in either ``shell`` or ``key-value`` formats, which can be set using the ``--format`` parameter:

  - ``export`` optional parameters:

    - ``--unset``: Creates a statement that unsets specific global variables and restores the environment to its state before calling ``export.{sh/fish}``.
    - ``--add_paths_extras``: Adds extra ESP-IDF-related paths of ``$PATH`` to ``${IDF_TOOLS_PATH}/esp-idf.json``, which is used to remove global variables when the active ESP-IDF environment is deactivated. For example, while processing the ``export.{sh/fish}`` script, if new paths are added to the global variable ``$PATH``, this option saves these new paths to the ``${IDF_TOOLS_PATH}/esp-idf.json`` file.

  - ``shell``: Produces output suitable for evaluation in the shell. For example, produce the following output on Linux and macOS:

    .. code-block:: none

        export PATH="/home/user/.espressif/tools/tool/v1.0.0/bin:$PATH"

    Produce the following output on Windows:

    .. code-block:: none

        set "PATH=C:\Users\user\.espressif\tools\v1.0.0\bin;%PATH%"

    .. note::

        Exporting environment variables in Powershell format is not supported at the moment. ``key-value`` format may be used instead.

    The output of this command may be used to update the environment variables if the shell supports it. For example

    .. code-block:: none

        eval $($IDF_PATH/tools/idf_tools.py export)

  - ``key-value``: Produces output in the ``VARIABLE=VALUE`` format that is suitable for parsing by other scripts

    .. code-block:: none

        PATH=/home/user/.espressif/tools/tool/v1.0.0:$PATH

    Note that the script consuming this output has to perform expansion of ``$VAR`` or ``%VAR%`` patterns found in the output.

* ``list``: Lists the known versions of the tools, and indicates which ones are installed.

  The following option is available to customize the output.

  - ``--outdated``: Lists only outdated versions of tools installed in ``IDF_TOOLS_PATH``.

* ``check``: For each tool, checks whether the tool is available in the system path and in ``IDF_TOOLS_PATH``.

* ``install-python-env``: Creates a Python virtual environment in the ``${IDF_TOOLS_PATH}/python_env`` directory or directly in the directory set by the ``IDF_PYTHON_ENV_PATH`` environment variable, and install the required Python packages there.

  * An optional ``--features`` argument allows one to specify a comma-separated list of features to be added or removed.

    #. A feature that begins with ``-`` will be removed, and features with ``+`` or without any sign will be added. Example syntax for removing feature ``XY`` is ``--features=-XY``, and for adding feature ``XY`` is ``--features=+XY`` or ``--features=XY``. If both removing and adding options are provided with the same feature, no operation is performed.

    #. For each feature, a requirements file must exist. For example, feature ``XY`` is a valid feature if ``${IDF_PATH}/tools/requirements/requirements.XY.txt`` is an existing file with a list of Python packages to be installed.

    #. There is one mandatory ``core`` feature ensuring the core functionality of ESP-IDF, e.g., build, flash, monitor, debug in console. There can be an arbitrary number of optional features.

    #. The selected list of features is stored in ``idf-env.json``.

    #. The requirement files contain a list of the desired Python packages to be installed and the ``espidf.constraints.*.txt`` file downloaded from https://dl.espressif.com and stored in ``${IDF_TOOLS_PATH}``, which contains the package version requirements for a given ESP-IDF version.

  .. note::

    Although **it is not recommended**, the download and use of constraint files can be disabled with the ``--no-constraints`` argument or setting the ``IDF_PYTHON_CHECK_CONSTRAINTS`` environment variable to ``no``.

* ``check-python-dependencies``: Checks if all required Python packages are installed. Packages from ``${IDF_PATH}/tools/requirements/requirements.*.txt`` files selected by the feature list of ``idf-env.json`` are checked with the package versions specified in the ``espidf.constraints.*.txt`` file.

  .. note::

    The constraint file is downloaded with the ``install-python-env`` command. Similar to the ``install-python-env`` command, the use of constraint files can be disabled with the ``--no-constraints`` argument or setting the ``IDF_PYTHON_CHECK_CONSTRAINTS`` environment variable to ``no``.

* ``uninstall``: Prints and removes tools that are currently not used by the active ESP-IDF version.

  - ``--dry-run``: Prints installed unused tools.
  - ``--remove-archives``: Additionally removes all older versions of previously downloaded installation packages.

.. _idf-tools-install:

Install Scripts
---------------

Shell-specific user-facing installation scripts are provided in the root directory of ESP-IDF repository to facilitate tools installation. These are:

* ``install.bat`` for Windows Command Prompt
* ``install.ps1`` for Powershell
* ``install.sh`` for Bash
* ``install.fish`` for Fish

Apart from downloading and installing the tools in ``IDF_TOOLS_PATH``, these scripts prepare a Python virtual environment, and install the required packages into that environment.

These scripts accept optionally a comma-separated list of chip targets and ``--enable-*`` arguments for enabling features. These arguments are passed to the ``idf_tools.py`` script which stores them in ``idf-env.json``. Therefore, chip targets and features can be enabled incrementally.

To install tools for all chip targets, run the scripts without any optional arguments using ``idf_tools.py install --targets=all``. Similarly, to install Python packages for core ESP-IDF functionality, run ``idf_tools.py install-python-env --features=core``.

It is also possible to install tools for specific chip targets. For example, ``install.sh esp32`` installs tools only for ESP32. See :ref:`Step 3. Set up the Tools <get-started-set-up-tools>` for more examples.

``install.sh --enable-XY`` enables feature ``XY`` (by running ``idf_tools.py install-python-env --features=core,XY``).

.. _idf-tools-export:

Export Scripts
--------------

Since the installed tools are not permanently added to the user or system ``PATH`` environment variable, an extra step is required to use them in the command line. The following scripts modify the environment variables in the current shell to make the correct versions of the tools available:

* ``export.bat`` for Windows Command Prompt
* ``export.ps1`` for Powershell
* ``export.sh`` for Bash
* ``export.fish`` for Fish

.. note::

    To modify the shell environment in Bash, ``export.sh`` must be "sourced" by using the ``. ./export.sh`` command. Please ensure to include the leading dot and space.

    ``export.sh`` may be used with shells other than Bash (such as zsh). However, in this case, it is required to set the ``IDF_PATH`` environment variable before running the script. When used in Bash, the script guesses the ``IDF_PATH`` value from its own location.

In addition to calling ``idf_tools.py``, these scripts list the directories that have been added to the ``PATH``.

Other Installation Methods
--------------------------

Depending on the environment, more user-friendly wrappers for ``idf_tools.py`` are provided:

* :ref:`ESP-IDF Tools Installer <get-started-windows-tools-installer>` can download and install the tools. Internally the installer uses ``idf_tools.py``.
* `ESP-IDF Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md>`_ includes a menu item to set up the tools. Internally the plugin calls ``idf_tools.py``.
* `VSCode ESP-IDF Extension <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_ includes an onboarding flow. This flow helps set up the tools. Although the extension does not rely on ``idf_tools.py``, the same installation method is used.

Custom Installation
-------------------

Although the methods above are recommended for ESP-IDF users, they are not a must for building ESP-IDF applications. ESP-IDF build system expects that all the necessary tools are installed somewhere, and made available in the ``PATH``.

.. _idf-tools-uninstall:

Uninstall ESP-IDF
-----------------

Uninstalling ESP-IDF requires removing both the tools and the environment variables that have been configured during the installation.

* Windows users using the :ref:`Windows ESP-IDF Tools Installer <get-started-windows-tools-installer>` can simply run the uninstall wizard to remove ESP-IDF.
* To remove an installation performed by running the supported :ref:`install scripts <idf-tools-install>`, simply delete the :ref:`tools installation directory <idf-tools-path>` including the downloaded and installed tools. Any environment variables set by the :ref:`export scripts <idf-tools-export>` are not permanent and will not be present after opening a new environment.
* When dealing with a custom installation, in addition to deleting the tools as mentioned above, you may also need to manually revert any changes to environment variables or system paths that were made to accommodate the ESP-IDF tools (e.g., ``IDF_PYTHON_ENV_PATH`` or ``IDF_TOOLS_PATH``). If you manually copied any tools, you would need to track and delete those files manually.
* If you installed any plugins like the `ESP-IDF Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md>`_ or `VSCode ESP-IDF Extension <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_, you should follow the specific uninstallation instructions described in the documentation of those components.

.. note::

  Uninstalling the ESP-IDF tools does not remove any project files or your code. Be mindful of what you are deleting to avoid losing any work. If you are unsure about a step, refer back to the installation instructions.

  These instructions assume that the tools were installed following the procedures in this provided document. If you've used a custom installation method, you might need to adapt these instructions accordingly.

.. _idf-tools-list:

List of ESP-IDF Tools
---------------------

.. include-build-file:: idf-tools-inc.rst
