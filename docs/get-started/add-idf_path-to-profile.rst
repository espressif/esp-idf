Add IDF_PATH to User Profile
============================

To preserve setting of ``IDF_PATH`` environment variable between system restarts, add it to the user profile, following instructions below.


.. _add-idf_path-to-profile-windows:

Windows
-------

The user profile scripts are contained in ``C:/msys32/etc/profile.d/`` directory. They are executed every time you open an MSYS2 window.

#. Create a new script file in ``C:/msys32/etc/profile.d/`` directory. Name it ``export_idf_path.sh``.

#. Identify the path to ESP-IDF directory. It is specific to your system configuration and may look something like ``C:\msys32\home\user-name\esp\esp-idf``

#. Add the ``export`` command to the script file, e.g.::

       export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

   Remember to replace back-slashes with forward-slashes in the original Windows path.

#. Save the script file.

#. Close MSYS2 window and open it again. Check if ``IDF_PATH`` is set, by typing::

       printenv IDF_PATH

   The path previusly entered in the script file should be printed out.

If you do not like to have ``IDF_PATH`` set up permanently in user profile, you should enter it manually on opening of an MSYS2 window::

    export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

If you got here from section :ref:`get-started-setup-path`, while installing s/w for ESP32 development, then go back to section :ref:`get-started-start-project`. 


.. _add-idf_path-to-profile-linux-macos:

Linux and MacOS
---------------

Set up ``IDF_PATH`` by adding the following line to ``~/.profile`` file::

    export IDF_PATH=~/esp/esp-idf

Log off and log in back to make this change effective. 

.. note::

    If you have ``/bin/bash`` set as login shell, and both ``.bash_profile`` and ``.profile`` exist, then update ``.bash_profile`` instead.

Run the following command to check if ``IDF_PATH`` is set::

    printenv IDF_PATH

The path previously entered in ``~/.profile`` file (or set manually) should be printed out.

If you do not like to have ``IDF_PATH`` set up permanently, you should enter it manually in terminal window on each restart or logout::

    export IDF_PATH=~/esp/esp-idf

If you got here from section :ref:`get-started-setup-path`, while installing s/w for ESP32 development, then go back to section :ref:`get-started-start-project`.
