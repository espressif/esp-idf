Add IDF_PATH & idf.py PATH to User Profile (CMake)
==================================================

:link_to_translation:`zh_CN:[中文]`

.. include:: ../cmake-warning.rst

To use the CMake-based build system and the idf.py tool, two modifications need to be made to system environment variables:

- ``IDF_PATH`` needs to be set to the path of the directory containing ESP-IDF.
- System ``PATH`` variable to include the directory containing the ``idf.py`` tool (part of ESP-IDF).

To preserve setting of these variables between system restarts, add them to the user profile by following the instructions below.

.. note:: If using an IDE, you can optionally set these environment variables in your IDE's project environment rather than from the command line as described below.

.. note:: If you don't ever use the command line ``idf.py`` tool, but run cmake directly or via an IDE, then it is not necessary to set the ``PATH`` variable - only ``IDF_PATH``. However it can be useful to set both.

.. note:: If you only ever use the command line ``idf.py`` tool, and never use cmake directly or via an IDE, then it is not necessary to set the ``IDF_PATH`` variable - ``idf.py`` will detect the directory it is contained within and set ``IDF_PATH`` appropriately if it is missing.

.. _add-paths-to-profile-windows-cmake:

Windows
-------

To edit Environment Variables on Windows 10, search for "Edit Environment Variables" under the Start menu.

On earlier Windows versions, open the System Control Panel then choose "Advanced" and look for the Environment Variables button.

You can set these environment variables for all users, or only for the current user, depending on whether other users of your computer will be using ESP-IDF.

- Click ``New...`` to add a new system variable named ``IDF_PATH``. Set the path to directory containing ESP-IDF, for example ``C:\Users\user-name\esp\esp-idf``.
- Locate the ``Path`` environment variable and double-click to edit it. Append the following to the end: ``;%IDF_PATH%\tools``. This will allow you to run ``idf.py`` and other tools from Windows Command Prompt.

If you got here from :ref:`get-started-setup-path-cmake`, while installing s/w for ESP32 development, then you can continue with :ref:`get-started-get-packages-cmake`.


.. _add-idf_path-to-profile-linux-macos-cmake:

Linux and MacOS
---------------

Set up ``IDF_PATH`` and add ``idf.py`` to the PATH by adding the following two lines to your ``~/.profile`` file::

    export IDF_PATH=~/esp/esp-idf
    export PATH="$IDF_PATH/tools:$PATH"

.. note::

   ``~/.profile`` means a file named ``.profile`` in your user's home directory (which is abbreviated ``~`` in the shell).

Log off and log in back to make this change effective.

.. note::

    Not all shells use ``.profile``. If you have ``/bin/bash`` and ``.bash_profile`` exists then update this file instead. For ``zsh``, update ``.zprofile``. Other shells may use other profile files (consult the shell's documentation).

Run the following command to check if ``IDF_PATH`` is set::

    printenv IDF_PATH

The path previously entered in ``~/.profile`` file (or set manually) should be printed out.

To verify ``idf.py`` is now on the ``PATH``, you can run the following::

  which idf.py

A path like ``${IDF_PATH}/tools/idf.py`` should be printed.

If you do not like to have ``IDF_PATH`` or ``PATH`` modifications set, you can enter it manually in terminal window on each restart or logout::

    export IDF_PATH=~/esp/esp-idf
    export PATH="$IDF_PATH/tools:$PATH"

If you got here from :ref:`get-started-setup-path-cmake`, while installing s/w for ESP32 development, then you can continue with :ref:`get-started-get-packages-cmake`.
