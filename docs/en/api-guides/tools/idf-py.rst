IDF Frontend - ``idf.py``
*************************

:link_to_translation:`zh_CN:[中文]`

The ``idf.py`` command-line tool provides a front-end for easily managing your project builds, deployment and debugging, and more. It manages several tools, for example:

- CMake_, which configures the project to be built.
- Ninja_, which builds the project.
- `esptool.py`_, which flashes the target.

The :ref:`Step 5. First Steps on ESP-IDF <get-started-configure>` contains a brief introduction on how to set up ``idf.py`` to configure, build, and flash projects.

.. important::

  ``idf.py`` should be run in an ESP-IDF project directory, i.e., a directory containing a ``CMakeLists.txt`` file. Older style projects that contain a ``Makefile`` will not work with ``idf.py``.

Commands
========

Start a New Project: ``create-project``
---------------------------------------

.. code-block:: bash

    idf.py create-project <project name>

This command creates a new ESP-IDF project. Additionally, the folder where the project will be created in can be specified by the ``--path`` option.

Create a New Component: ``create-component``
--------------------------------------------

.. code-block:: bash

    idf.py create-component <component name>

This command creates a new component, which will have a minimum set of files necessary for building. The ``-C`` option can be used to specify the directory the component will be created in. For more information about components see the :ref:`Component CMakeLists Files <component-directories>`.

.. _selecting-idf-target:

Select the Target Chip: ``set-target``
--------------------------------------

ESP-IDF supports multiple targets (chips). A full list of supported targets in your version of ESP-IDF can be seen by running ``idf.py --list-targets``.

.. code-block:: bash

    idf.py set-target <target>

This command sets the current project target.

.. important::

    ``idf.py set-target`` will clear the build directory and re-generate the ``sdkconfig`` file from scratch. The old ``sdkconfig`` file will be saved as ``sdkconfig.old``.

.. note::

    The behavior of the ``idf.py set-target`` command is equivalent to:

    1. clearing the build directory (``idf.py fullclean``)
    2. removing the sdkconfig file (``mv sdkconfig sdkconfig.old``)
    3. configuring the project with the new target (``idf.py -DIDF_TARGET=esp32 reconfigure``)

It is also possible to pass the desired ``IDF_TARGET`` as an environment variable (e.g., ``export IDF_TARGET=esp32s2``) or as a CMake variable (e.g., ``-DIDF_TARGET=esp32s2`` argument to CMake or  ``idf.py`` ). Setting the environment variable is a convenient method if you mostly work with one type of the chip.

To specify the default value of ``IDF_TARGET`` for a given project, please add the ``CONFIG_IDF_TARGET`` option to the project's ``sdkconfig.defaults`` file, e.g., ``CONFIG_IDF_TARGET="esp32s2"``. This value of the option will be used if ``IDF_TARGET`` is not specified by other methods, such as using an environment variable, a CMake variable, or the ``idf.py set-target`` command.

If the target has not been set by any of these methods, the build system will default to ``esp32`` target.

Start the Graphical Configuration Tool: ``menuconfig``
------------------------------------------------------

.. code-block:: bash

  idf.py menuconfig

Build the Project: ``build``
----------------------------

.. code-block:: bash

  idf.py build

This command builds the project found in the current directory. This can involve multiple steps:

  - Create the build directory if needed. The sub-directory ``build`` is used to hold build output, although this can be changed with the ``-B`` option.
  - Run CMake_ as necessary to configure the project and generate build files for the main build tool.
  - Run the main build tool (Ninja_ or `GNU Make`). By default, the build tool is automatically detected but it can be explicitly set by passing the ``-G`` option to ``idf.py``.

Building is incremental, so if no source files or configuration has changed since the last build, nothing will be done.

Additionally, the command can be run with ``app``, ``bootloader`` and ``partition-table`` arguments to build only the app, bootloader or partition table as applicable.

Remove the Build Output: ``clean``
----------------------------------

.. code-block:: bash

  idf.py clean

This command removes the project build output files from the build directory, and the project will be fully rebuilt on next build. Using this command does not remove the CMake configuration output inside the build folder.

Delete the Entire Build Contents: ``fullclean``
-----------------------------------------------

.. code-block:: bash

  idf.py fullclean

This command deletes the entire "build" directory contents, which includes all CMake configuration output. The next time the project is built, CMake will configure it from scratch. Note that this option recursively deletes **all** files in the build directory, so use with care. Project configuration is not deleted.

Flash the Project: ``flash``
----------------------------

.. code-block:: bash

  idf.py flash

This command automatically builds the project if necessary, and then flash it to the target. You can use ``-p`` and ``-b`` options to set serial port name and flasher baud rate, respectively.

.. note:: The environment variables ``ESPPORT`` and ``ESPBAUD`` can be used to set default values for the ``-p`` and ``-b`` options, respectively. Providing these options on the command line overrides the default.

``idf.py`` uses the ``write_flash`` command of ``esptool.py`` under the hood to flash the target. You can pass additional arguments to configure the flash writing process using the ``--extra-args`` option. For example, to `write to an external SPI flash chip <https://docs.espressif.com/projects/esptool/en/latest/esptool/advanced-options.html#custom-spi-pin-configuration>`_, use the following command: ``idf.py flash --extra-args="--spi-connection <CLK>,<Q>,<D>,<HD>,<CS>"``. To see the full list of available arguments, run ``esptool.py write_flash --help`` or see the `esptool.py documentation <https://docs.espressif.com/projects/esptool/en/latest/esptool/index.html>`_.

Similarly to the ``build`` command, the command can be run with ``app``, ``bootloader`` and ``partition-table`` arguments to flash only the app, bootloader or partition table as applicable.

Hints on How to Resolve Errors
==============================

``idf.py`` will try to suggest hints on how to resolve errors. It works with a database of hints stored in :idf_file:`tools/idf_py_actions/hints.yml` and the hints will be printed if a match is found for the given error. The menuconfig target is not supported at the moment by automatic hints on resolving errors.

The ``--no-hints`` argument of ``idf.py`` can be used to turn the hints off in case they are not desired.

Important Notes
===============

Multiple ``idf.py`` commands can be combined into one. For example, ``idf.py -p COM4 clean flash monitor`` will clean the source tree, then build the project and flash it to the target before running the serial monitor.

The order of multiple ``idf.py`` commands on the same invocation is not important, as they will automatically be executed in the correct order for everything to take effect (e.g., building before flashing, erasing before flashing).

For commands that are not known to ``idf.py``, an attempt to execute them as a build system target will be made.

The command ``idf.py`` supports `shell autocompletion <https://click.palletsprojects.com/shell-completion/>`_ for bash, zsh and fish shells.

In order to make `shell autocompletion <https://click.palletsprojects.com/shell-completion/>`_ supported, please make sure you have at least Python 3.5 and `click <https://click.palletsprojects.com/>`_ 7.1 or newer (:ref:`Software <get-started-get-prerequisites>`).

To enable autocompletion for ``idf.py``, use the ``export`` command (:ref:`Step 4. Set up the environment variables <get-started-set-up-env>`). Autocompletion is initiated by pressing the TAB key. Type ``idf.py -`` and press the TAB key to autocomplete options.

The autocomplete support for PowerShell is planned in the future.

Advanced Commands
=================

Open the Documentation: ``docs``
--------------------------------

.. code-block:: bash

  idf.py docs

This command opens the documentation for the projects target and ESP-IDF version in the browser.

Show Size: ``size``
-------------------

.. code-block:: bash

  idf.py size

This command prints app size information including the occupied RAM and flash and section (i.e., .bss) sizes.

.. code-block:: bash

  idf.py size-components

Similarly, this command prints the same information for each component used in the project.

.. code-block:: bash

  idf.py size-files

This command prints size information per source file in the project.

Options
^^^^^^^

- ``--format`` specifies the output format with available options: ``text``, ``csv``, ``json``, default being ``text``.
- ``--output-file`` optionally specifies the name of the file to print the command output to instead of the standard output.

Reconfigure the Project: ``reconfigure``
----------------------------------------

.. code-block:: bash

  idf.py reconfigure

This command forces CMake_ to be rerun regardless of whether it is necessary. It is unnecessary during normal usage, but can be useful after adding/removing files from the source tree, or when modifying CMake cache variables. For example, ``idf.py -DNAME='VALUE' reconfigure`` can be used to set variable ``NAME`` in CMake cache to value ``VALUE``.

Clean the Python Byte Code: ``python-clean``
--------------------------------------------

.. code-block:: bash

  idf.py python-clean

This command deletes generated python byte code from the ESP-IDF directory. The byte code may cause issues when switching between ESP-IDF and Python versions. It is advised to run this target after switching versions of Python.

Generate a UF2 Binary: ``uf2``
------------------------------

.. code-block:: bash

  idf.py uf2

This command generates a UF2 (`USB Flashing Format <https://github.com/microsoft/uf2>`_) binary ``uf2.bin`` in the build directory. This file includes all the necessary binaries (bootloader, app, and partition table) for flashing the target.

This UF2 file can be copied to a USB mass storage device exposed by another ESP running the `ESP USB Bridge <https://github.com/espressif/esp-usb-bridge>`_ project. The bridge MCU will use it to flash the target MCU. This is as simple as copying (or "drag-and-dropping") the file to the exposed disk accessed by a file explorer in your machine.

To generate a UF2 binary for the application only (not including the bootloader and partition table), use the ``uf2-app`` command.

.. code-block:: bash

  idf.py uf2-app

Global Options
==============

To list all available root level options, run ``idf.py --help``. To list options that are specific for a subcommand, run ``idf.py <command> --help``, e.g., ``idf.py monitor --help``. Here is a list of some useful options:

- ``-C <dir>`` allows overriding the project directory from the default current working directory.
- ``-B <dir>`` allows overriding the build directory from the default ``build`` subdirectory of the project directory.
- ``--ccache`` enables CCache_ when compiling source files if the CCache_ tool is installed. This can dramatically reduce the build time.

.. important::

    Note that some older versions of CCache_ may exhibit bugs on some platforms, so if files are not rebuilt as expected, try disabling CCache_ and rebuiling the project. To enable CCache_ by default, set the ``IDF_CCACHE_ENABLE`` environment variable to a non-zero value.

- ``-v`` flag causes both ``idf.py`` and the build system to produce verbose build output. This can be useful for debugging build problems.
- ``--cmake-warn-uninitialized`` (or ``-w``)  causes CMake to print uninitialized variable warnings found in the project directory only. This only controls CMake variable warnings inside CMake itself, not other types of build warnings. This option can also be set permanently by setting the ``IDF_CMAKE_WARN_UNINITIALIZED`` environment variable to a non-zero value.
- ``--no-hints`` flag disables hints on resolving errors and disable capturing output.

.. _cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool.py: https://github.com/espressif/esptool/#readme
.. _CCache: https://ccache.dev/
