********************************************
Setup Windows Toolchain from Scratch
********************************************

:link_to_translation:`zh_CN:[中文]`

This is a step-by-step alternative to running the :doc:`ESP-IDF Tools Installer <windows-setup>` for the CMake-based build system. Installing all of the tools by hand allows more control over the process, and also provides the information for advanced users to customize the install.

To quickly setup the toolchain and other tools in standard way, using the ESP-IDF Tools installer, proceed to section :doc:`windows-setup`.

.. note::
   The GNU Make based build system requires the MSYS2_ Unix compatibility environment on Windows. The CMake-based build system does not require this environment.

.. _get-esp-idf-windows-command-line:

Get ESP-IDF
===========

.. note::

        Previous versions of ESP-IDF used the **MSYS2 bash terminal** command line. The current cmake-based build system can run in the regular **Windows Command Prompt** which is used here.

        If you use a bash-based terminal or PowerShell, please note that some command syntax will be different to what is shown below.

Open Command Prompt and run the following commands:

.. include-build-file:: inc/git-clone-windows.inc

ESP-IDF will be downloaded into ``%userprofile%\esp\esp-idf``.

Consult :doc:`/versions` for information about which ESP-IDF version to use in a given situation.

.. include-build-file:: inc/git-clone-notes.inc

.. note::

    Do not miss the ``--recursive`` option. If you have already cloned ESP-IDF without this option, run another command to get all the submodules::

        cd esp-idf
        git submodule update --init


Tools
=====

cmake
^^^^^

Download the latest stable release of CMake_ for Windows and run the installer.

When the installer asks for Install Options, choose either "Add CMake to the system PATH for all users" or "Add CMake to the system PATH for the current user".

Ninja build
^^^^^^^^^^^

.. note::
    Ninja currently only provides binaries for 64-bit Windows. It is possible to use CMake and ``idf.py`` with other build tools, such as mingw-make, on 32-bit windows. However this is currently undocumented.

Download the ninja_ latest stable Windows release from the (`download page <ninja-dl_>`_).

The Ninja for Windows download is a .zip file containing a single ``ninja.exe`` file which needs to be unzipped to a directory which is then :ref:`added to your Path <add-directory-windows-path>` (or you can choose a directory which is already on your Path).


Python
^^^^^^

Download the latest Python_ for Windows installer, and run it.

The "Customise" step of the Python installer gives a list of options. The last option is "Add python.exe to Path". Change this option to select "Will be installed".

Once Python is installed, open a Windows Command Prompt from the Start menu and run the following command::

  pip install --user pyserial

Toolchain Setup
===============

.. include-build-file:: inc/download-links.inc

Download the precompiled Windows toolchain:

|download_link_win32|

Unzip the zip file to ``C:\Program Files`` (or some other location). The zip file contains a single directory ``xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf``.

Next, the ``bin`` subdirectory of this directory must be :ref:`added to your Path <add-directory-windows-path>`. For example, the directory to add may be ``C:\Program Files\xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf\bin``.

.. note::
   If you already have the MSYS2 environment (for use with the "GNU Make" build system) installed, you can skip the separate download and add the directory ``C:\msys32\opt\xtensa-{IDF_TARGET_TOOLCHAIN_NAME}-elf\bin`` to the Path instead, as the toolchain is included in the MSYS2 environment.


.. _add-directory-windows-path:

Adding Directory to Path
========================

To add any new directory to your Windows Path environment variable:

Open the System control panel and navigate to the Environment Variables dialog. (On Windows 10, this is found under Advanced System Settings).

Double-click the ``Path`` variable (either User or System Path, depending if you want other users to have this directory on their path.) Go to the end of the value, and append ``;<new value>``.


Next Steps
==========

To carry on with development environment setup, proceed to :ref:`get-started-set-up-tools`.

.. _CMake: https://cmake.org/download/
.. _ninja: https://ninja-build.org/
.. _ninja-dl: https://github.com/ninja-build/ninja/releases
.. _Python: https://www.python.org/downloads/windows/
.. _MSYS2: https://www.msys2.org/
.. _kconfig-frontends releases page: https://github.com/espressif/kconfig-frontends/releases
.. Note: These two targets may be used from git-clone-notes.inc depending on version, don't remove
.. _Stable version: https://docs.espressif.com/projects/esp-idf/en/stable/
.. _Releases page: https://github.com/espressif/esp-idf/releases
