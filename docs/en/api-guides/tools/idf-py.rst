IDF Frontend - idf.py
*********************

The ``idf.py`` command-line tool provides a front-end for easily managing your project builds, deployment and debugging, and more.
It manages several tools, for example:

- CMake_, which configures the project to be built
- Ninja_ which builds the project
- `esptool.py`_ for flashing the target.

The :ref:`getting started guide <get-started-configure>` contains a brief introduction to how to set up ``idf.py`` to configure, build, and flash projects.

.. important::

  ``idf.py`` should be run in an ESP-IDF "project" directory, i.e. one containing a ``CMakeLists.txt`` file.
  Older style projects with a ``Makefile`` will not work with ``idf.py``.

Commands
========

Start a new project: create-project
------------------------------------

.. code-block:: bash

    idf.py create-project <project name>

This will create a new ESP-IDF project, additionally folder where the project will
be created can be specified by the ``--path`` option.

Create a new component: create-component
----------------------------------------

This command creates a new component, which will have a minimum set of files
necessary for building.

.. code-block:: bash

    idf.py create-component <component name>

The ``-C`` option can be used to specify the directory the component will be
created in.
For more information about components see the :ref:`build system page <component-directories>`.

.. _selecting-idf-target:

Select the Target Chip: set-target
----------------------------------

ESP-IDF supports multiple targets (chips). A full list of supported targets in your version of ESP-IDF can be seen by running ``idf.py --list-targets``.

This sets the current project target:

.. code-block:: bash

    idf.py set-target <target>

.. important::

    ``idf.py set-target`` will clear the build directory and re-generate the ``sdkconfig`` file from scratch. The old ``sdkconfig`` file will be saved as ``sdkconfig.old``.

.. note::

    The behavior of ``idf.py set-target`` command is equivalent to:

    1. clearing the build directory (``idf.py fullclean``)
    2. removing the sdkconfig file (``mv sdkconfig sdkconfig.old``)
    3. configuring the project with the new target (``idf.py -DIDF_TARGET=esp32 reconfigure``)

It is also possible to pass the desired ``IDF_TARGET`` as an environment variable (e.g. ``export IDF_TARGET=esp32s2``) or as a CMake variable (e.g. ``-DIDF_TARGET=esp32s2`` argument to CMake or idf.py). Setting the environment variable is a convenient method if you mostly work with one type of the chip.

To specify the _default_ value of ``IDF_TARGET`` for a given project, add ``CONFIG_IDF_TARGET`` value to ``sdkconfig.defaults``. For example, ``CONFIG_IDF_TARGET="esp32s2"``. This value will be used if ``IDF_TARGET`` is not specified by other method: using an environment variable, CMake variable, or ``idf.py set-target`` command.

If the target has not been set by any of these methods, the build system will default to ``esp32`` target.

Start the graphical configuration tool: menuconfig
--------------------------------------------------

.. code-block:: bash

  idf.py menuconfig

Build the project: build
------------------------

.. code-block:: bash

  idf.py build

Running this command will build the project found in the current directory. This can involve multiple steps:

  - Create the build directory if needed. The sub-directory ``build`` is used to hold build output, although this can be changed with the ``-B`` option.
  - Run CMake_ as necessary to configure the project and generate build files for the main build tool.
  - Run the main build tool (Ninja_ or `GNU Make`). By default, the build tool is automatically detected but it can be explicitly set by passing the ``-G`` option to ``idf.py``.

Building is incremental so if no source files or configuration has changed since the last build, nothing will be done.

Additionally, the command can be run with ``app``, ``bootloader`` and
``partition-table`` arguments to build only the app, bootloader or partition table
as applicable.

Remove the build output: clean
------------------------------

It is possible to remove the project build output files from the build directory by
using:

.. code-block:: bash

  idf.py clean

The project will be fully rebuilt on next build.
Using this does not remove the CMake configuration output inside the build folder.

Delete the entire build contents: fullclean
-------------------------------------------

.. code-block:: bash

  idf.py fullclean

Running this command will delete the entire "build" directory contents.
This includes all CMake configuration output.
The next time the project is built, CMake will configure it from scratch.
Note that this option recursively deletes *all* files in the build directory, so use with care.
Project configuration is not deleted.

Flash the project: flash
------------------------

Running the following command:

.. code-block:: bash

  idf.py flash

will automatically build the project if necessary, and then flash it to the target.
You can use ``-p`` and ``-b`` options to set serial port name and flasher baud rate, respectively.

.. note:: The environment variables ``ESPPORT`` and ``ESPBAUD`` can be used to set default values for the ``-p`` and ``-b`` options, respectively. Providing these options on the command line overrides the default.

Similarly to the ``build`` command, the command can be run with ``app``,
``bootloader`` and ``partition-table`` arguments to flash only the app, bootloader
or partition table as applicable.

Hints on how to resolve errors
==============================

``idf.py`` will try to suggest hints on how to resolve errors. It works with a database of hints stored in :idf_file:`tools/idf_py_actions/hints.yml` and the hints will be printed if a match is found for the given error. The monitor, menuconfig, gdb and openocd targets are not supported at the moment by automatic hints on resolving errors.

The ``--no-hints`` argument of ``idf.py`` can be used to turn the hints off in case they are not desired.

Important notes
===============

Multiple ``idf.py`` commands can be combined into one. For example, ``idf.py -p COM4 clean flash monitor`` will clean the source tree, then build the project and flash it to the target before running the serial monitor.

The order of multiple ``idf.py`` commands on the same invocation is not important, they will automatically be executed in the correct order for everything to take effect (i.e. building before flashing, erasing before flashing, etc.).

For commands that are not known to ``idf.py`` an attempt to execute them as a build system target will be made.

The command ``idf.py`` supports `shell autocompletion <https://click.palletsprojects.com/shell-completion/>`_ for bash, zsh and fish shells.

In order to make `shell autocompletion <https://click.palletsprojects.com/shell-completion/>`_ supported, please make sure you have at least Python 3.5 and `click <https://click.palletsprojects.com/>`_ 7.1 or newer (:ref:`see also <get-started-get-prerequisites>`).

To enable autocompletion for ``idf.py`` use the ``export`` command (:ref:`see this <get-started-set-up-env>`). Autocompletion is initiated by pressing the TAB key. Type ``idf.py -`` and press the TAB key to autocomplete options.

The autocomplete support for PowerShell is planned in the future.

Advanced Commands
=================

Open the documentation: docs
----------------------------

Using the following command the documentation for the projects target and version
will be opened in the browser:

.. code-block:: bash

  idf.py docs

Show size: size
---------------

.. code-block:: bash

  idf.py size

Will print app size information including occupied RAM and FLASH and section sizes.

.. code-block:: bash

  idf.py size

Similarly, this will print the same information for each component used in the project.

.. code-block:: bash

  idf.py size-files

Will print size information per source file in the project.

If you define variable ``-DOUTPUT_JSON=1`` when running CMake (or ``idf.py``), the output will be formatted as JSON not as human readable text. See ``idf.py-size`` for more information.

Reconfigure the project: reconfigure
------------------------------------

.. code-block:: bash

  idf.py reconfigure

This command re-runs CMake_ even if it doesn't seem to need re-running.
This isn't necessary during normal usage, but can be useful after adding/removing
files from the source tree, or when modifying CMake cache variables.
For example, ``idf.py -DNAME='VALUE' reconfigure`` can be used to set variable ``NAME`` in CMake cache to value ``VALUE``.

Clean the python byte code: python-clean
----------------------------------------

Generated python byte code can be deleted from the IDF directory using:

.. code-block:: bash

  idf.py python-clean

The byte code may cause issues when switching between IDF and Python versions.
It is advised to run this target after switching versions of Python.

Global Options
==============

To list all available root level options, run ``idf.py --help``. To list options that are specific for a subcommand, run ``idf.py <command> --help``, for example ``idf.py monitor --help``. Here is a list of some useful options:

- ``-C <dir>`` allows overriding the project directory from the default current working directory.
- ``-B <dir>`` allows overriding the build directory from the default ``build`` subdirectory of the project directory.
- ``--ccache`` flag can be used to enable CCache_ when compiling source files, if the CCache_ tool is installed. This can dramatically reduce some build times.

Note that some older versions of CCache may exhibit bugs on some platforms, so if files are not rebuilt as expected then try disabling CCache and build again. CCache can be enabled by default by setting the ``IDF_CCACHE_ENABLE`` environment variable to a non-zero value.

- ``-v`` flag causes both ``idf.py`` and the build system to produce verbose build output. This can be useful for debugging build problems.
- ``--cmake-warn-uninitialized`` (or ``-w``) will cause CMake to print uninitialized variable warnings found in the project directory only. This only controls CMake variable warnings inside CMake itself, not other types of build warnings. This option can also be set permanently by setting the ``IDF_CMAKE_WARN_UNINITIALIZED`` environment variable to a non-zero value.
- ``--no-hints`` flag to disable hints on resolving errors and disable capturing output.

.. _cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool.py: https://github.com/espressif/esptool/#readme
.. _CCache: https://ccache.dev/
