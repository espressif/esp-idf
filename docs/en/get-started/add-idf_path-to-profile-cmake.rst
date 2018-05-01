Add IDF_PATH & idf.py PATH to User Profile
==========================================

.. note::
   The CMake-based build system is currently in preview release. Documentation may have missing gaps, and you may enocunter bugs (please report these). The original (non-cmake) version of this doc is :doc:`here<add-idf_path-to-profile>`.

To use the CMake-based build system and the idf.py tool, two modifications need to be made to system environment variables:

- ``IDF_PATH`` needs to be set to the path of the directory containing ESP-IDF.
- System ``PATH`` variable to include the directory containing the ``idf.py`` tool (part of ESP-IDF).

To preserve setting of these variables between system restarts, add them to the user profile by following the instructions below.

.. note::
   If you only plan to use ``idf.py`` from the command line, and don't need integration with any IDEs, then you can skip setting ``IDF_PATH``. ``idf.py`` will use its own IDF directory as the default ``IDF_PATH``.

.. _add-paths-to-profile-windows-cmake:

Windows
-------

To edit Environment Variables on Windows 10, search for "Edit Environment Variables" under the Start menu.

On earlier Windows versions, open the System Control Panel then choose "Advanced" and look for the Environment Variables button.

You can set these environment variables for all users, or only for the current user, depending on whether other users of your computer will be using ESP-IDF.

- Click ``New...`` to add a new environment variable named ``IDF_PATH``. Set the path to directory containing ESP-IDF, for example ``C:\Users\myuser\esp\esp-idf``.
- Locate the ``Path`` environment variable and double-click to edit it. Append the following to the end: ``;%IDF_PATH%\tools``. This will allow you to run ``idf.py`` and other tools from Windows Command Prompt.

If you got here from section :ref:`get-started-setup-path-cmake`, while installing s/w for ESP32 development, then go back to section :ref:`get-started-start-project-cmake`.


.. _add-idf_path-to-profile-linux-macos-cmake:

Linux and MacOS
---------------

Set up ``IDF_PATH`` and add ``idf.py`` to the PATH by adding the following two lines to ``~/.profile`` file::

    export IDF_PATH=~/esp/esp-idf
    export PATH="$PATH:$IDF_PATH/tools"

Log off and log in back to make this change effective.

.. note::

    If you have ``/bin/bash`` set as login shell, and both ``.bash_profile`` and ``.profile`` exist, then update ``.bash_profile`` instead.

Run the following command to check if ``IDF_PATH`` is set::

    printenv IDF_PATH

The path previously entered in ``~/.profile`` file (or set manually) should be printed out.

To verify idf.py is now on the ``PATH``, you can run the following::

  which idf.py

A path like ``${IDF_PATH}/tools/idf.py`` should be printed.

If you do not like to have ``IDF_PATH`` set up permanently, you should enter it manually in terminal window on each restart or logout::

    export IDF_PATH=~/esp/esp-idf

If you got here from section :ref:`get-started-setup-path-cmake`, while installing s/w for ESP32 development, then go back to section :ref:`get-started-start-project-cmake`.
