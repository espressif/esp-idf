IDF Frontend - ``idf.py``
*************************

:link_to_translation:`zh_CN:[中文]`

The ``idf.py`` command-line tool provides a front-end for easily managing your project builds, deployment and debugging, and more. It manages several tools, for example:

- CMake_, which configures the project to be built.
- Ninja_, which builds the project.
- `esptool`_, which flashes the target.

:ref:`Configure Your Project for Windows, <get-started-configure>` :ref:`Linux, or macOS <get-started-configure-linux-macos>` contains a brief introduction on how to set up ``idf.py`` to configure, build, and flash projects.

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

  - Create the build directory if needed. The sub-directory "build" is used to hold build output, although this can be changed with the ``-B`` option.
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

This command deletes the entire build directory contents, which includes all CMake configuration output. The next time the project is built, CMake will configure it from scratch. Note that this option recursively deletes **all** files in the build directory, so use with care. Project configuration is not deleted.

Flash the Project: ``flash``
----------------------------

.. code-block:: bash

  idf.py flash

This command automatically builds the project if necessary, and then flash it to the target. You can use ``-p`` and ``-b`` options to set serial port name and flasher baud rate, respectively.

.. note:: The environment variables ``ESPPORT`` and ``ESPBAUD`` can be used to set default values for the ``-p`` and ``-b`` options, respectively. Providing these options on the command line overrides the default.

``idf.py`` uses the ``write-flash`` command of ``esptool`` under the hood to flash the target. You can pass additional arguments to configure the flash writing process using the ``--extra-args`` option. For example, to `write to an external SPI flash chip <https://docs.espressif.com/projects/esptool/en/latest/esptool/advanced-options.html#custom-spi-pin-configuration>`_, use the following command: ``idf.py flash --extra-args="--spi-connection <CLK>,<Q>,<D>,<HD>,<CS>"``. To see the full list of available arguments, run ``esptool write-flash --help`` or see the `esptool documentation <https://docs.espressif.com/projects/esptool/en/latest/esptool/index.html>`_.

Similarly to the ``build`` command, the command can be run with ``app``, ``bootloader`` and ``partition-table`` arguments to flash only the app, bootloader or partition table as applicable.

.. _merging-binaries:

Merge binaries: ``merge-bin``
-----------------------------

.. code-block:: bash

  idf.py merge-bin [-o output-file] [-f format] [<format-specific-options>]

There are some situations, e.g. transferring the file to another machine and flashing it without ESP-IDF, where it is convenient to have only one file for flashing instead the several file output of ``idf.py build``.

The command ``idf.py merge-bin`` will merge the bootloader, partition table, the application itself, and other partitions (if there are any) according to the project configuration and create a single binary file ``merged-binary.[bin|hex]`` in the build folder, which can then be flashed later.

It is possible to output merged file in binary (raw), IntelHex (hex) and UF2 (uf2) formats.

The uf2 binary can also be generated by :ref:`idf.py uf2 <generate-uf2-binary>`. The ``idf.py uf2`` is functionally equivalent to ``idf.py merge-bin -f uf2``. However, the ``idf.py merge-bin`` command provides more flexibility and options for merging binaries into various formats described above.

Example usage:

.. code-block:: bash

  idf.py merge-bin -o my-merged-binary.bin -f raw

There are also some format specific options, which are listed below:

- Only for raw format:

  - ``--flash-offset``: This option will create a merged binary that should be flashed at the specified offset, instead of at the standard offset of 0x0.
  - ``--pad-to-size``: If set, the final binary file will be padded with FF bytes up to this flash size in order to fill the full flash content with the image and re-write the whole flash chip upon flashing.

- Only for uf2 format:

  - ``--md5-disable``: This option will disable MD5 checksums at the end of each block. This can be useful for integration with e.g. `tinyuf2 <https://github.com/adafruit/tinyuf2>`__.

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

To enable autocompletion for ``idf.py``, use the ``export`` command (:ref:`setting up the environment for Windows <get-started-set-up-env>`, :ref:`Linux or macOS <get-started-set-up-env-linux-macos>`). Autocompletion is initiated by pressing the TAB key. Type ``idf.py -`` and press the TAB key to autocomplete options.

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

- ``--format`` specifies the output format with available options: ``text``, ``csv``, ``json2``, ``tree``, ``raw``, default being ``text``.
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

.. _generate-uf2-binary:

Generate a UF2 Binary: ``uf2``
------------------------------

.. code-block:: bash

  idf.py uf2

This command generates a UF2 (`USB Flashing Format <https://github.com/microsoft/uf2>`_) binary ``uf2.bin`` in the build directory. This file includes all the necessary binaries (bootloader, app, and partition table) for flashing the target.

This UF2 file can be copied to a USB mass storage device exposed by another ESP running the `ESP USB Bridge <https://github.com/espressif/esp-usb-bridge>`_ project. The bridge MCU will use it to flash the target MCU. This is as simple as copying (or "drag-and-dropping") the file to the exposed disk accessed by a file explorer in your machine.

To generate a UF2 binary for the application only (not including the bootloader and partition table), use the ``uf2-app`` command.

The ``idf.py uf2`` command is functionally equivalent to ``idf.py merge-bin -f uf2`` described :ref:`above <merging-binaries>`. However, the ``idf.py merge-bin`` command provides more flexibility and options for merging binaries into various formats, not only uf2.

.. code-block:: bash

  idf.py uf2-app

Read Otadata Partition: ``read-otadata``
----------------------------------------

.. code-block:: bash

  idf.py read-otadata

This command prints the contents of the ``otadata`` partition which stores the information about the currently selected OTA app slot. Refer to :doc:`/api-reference/system/ota` for more about the ``otadata`` partition.

Start MCP Server: ``mcp-server``
---------------------------------

.. code-block:: bash

  idf.py mcp-server

This command starts an MCP (Model Context Protocol) server that enables AI integration with ESP-IDF projects. The MCP server provides tools and resources that allow AI assistants to interact with your ESP-IDF project through a standardized protocol.

The MCP server provides the following tools:

- ``build_project``: Build the ESP-IDF project with specified target
- ``set_target``: Set the ESP-IDF target (esp32, esp32s3, esp32c6, etc.)
- ``flash_project``: Flash the built project to a connected device
- ``monitor_serial``: Start serial monitor (runs in background)
- ``clean_project``: Clean build artifacts
- ``menuconfig``: Open menuconfig interface (terminal-based)

The MCP server also provides these resources:

- ``project://config``: Get current project configuration
- ``project://status``: Get current project build status
- ``project://devices``: Get list of connected ESP devices

.. note::

    The MCP server requires the ``mcp`` Python package to be installed. Install it with: ``./install.sh --enable-mcp``.

Adding ESP-IDF MCP Server to IDEs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Claude Desktop:**

Use the Claude CLI to add the ESP-IDF MCP server:

.. code-block:: bash

  claude mcp add esp-idf python /path/to/esp-idf/tools/idf.py mcp-server --env IDF_PATH=/path/to/esp-idf

Configuration Presets: ``--preset``
====================================

ESP-IDF supports `CMake presets`_ to simplify managing multiple build configurations. This feature allows you to define reusable configuration profiles that specify build directories, cache variables, and other CMake settings.

.. code-block:: bash

  idf.py --preset <preset-name> build

This command builds the project using the specified configuration preset. The preset defines settings such as the build directory location, CMake cache variables (including ``SDKCONFIG`` paths), and generator preferences.

Preset Definition Files
-----------------------

Create a ``CMakePresets.json`` or ``CMakeUserPresets.json`` file in your project root directory to define **configuration presets**. For example:

.. code-block:: json

  {
      "version": 3,
      "configurePresets": [
          {
              "name": "default",
              "binaryDir": "build/default",
              "displayName": "Default Configuration",
              "cacheVariables": {
                  "SDKCONFIG": "./build/default/sdkconfig"
              }
          },
          {
              "name": "production",
              "binaryDir": "build/production",
              "displayName": "Production Build",
              "cacheVariables": {
                  "SDKCONFIG_DEFAULTS": "sdkconfig.defaults.prod_common;sdkconfig.defaults.production",
                  "SDKCONFIG": "./build/production/sdkconfig"
              }
          }
      ]
  }

.. note::

    The ``version`` field represents the JSON schema version for CMake Presets. In this example it is set to ``3`` to match the schema supported by ESP-IDF's minimal supported CMake version. If you are using a newer CMake version, you can increase the ``version`` field accordingly—see `CMake Presets`_.

**Current Limitations**

- The ``inherits`` field for preset inheritance is not currently supported by ESP-IDF. Presets with inheritance will show a warning.

Automatic Preset Selection
---------------------------

If no preset is specified but a ``CMakePresets.json`` file exists, ``idf.py`` will automatically select a preset:

1. If a preset named ``default`` exists, it will be used.
2. Otherwise, the first preset in the file will be selected.

.. note::

    The environment variable ``IDF_PRESET`` can be used to set the default preset name, e.g., ``export IDF_PRESET=production``. Command-line arguments override environment variables.

**SDKCONFIG File Placement**

By default, the ``sdkconfig`` file is created in the project root directory. However, when using CMake presets, you can specify a custom location for the ``sdkconfig`` file using the ``SDKCONFIG`` cache variable.

For a complete example, see the :example_file:`Multiple Build Configurations Example <build_system/cmake/multi_config/README.md>`.

Global Options
==============

To list all available root level options, run ``idf.py --help``. To list options that are specific for a subcommand, run ``idf.py <command> --help``, e.g., ``idf.py monitor --help``. Here is a list of some useful options:

- ``-C <dir>`` allows overriding the project directory from the default current working directory.
- ``-B <dir>`` allows overriding the build directory from the default ``build`` subdirectory of the project directory.
- ``--ccache`` enables CCache_ when compiling source files if the CCache_ tool is installed. This can dramatically reduce the build time.

.. important::

    Note that some older versions of CCache_ may exhibit bugs on some platforms, so if files are not rebuilt as expected, try disabling CCache_ and rebuilding the project. To enable CCache_ by default, set the ``IDF_CCACHE_ENABLE`` environment variable to a non-zero value.

- ``-v`` flag causes both ``idf.py`` and the build system to produce verbose build output. This can be useful for debugging build problems.
- ``--cmake-warn-uninitialized`` (or ``-w``)  causes CMake to print uninitialized variable warnings found in the project directory only. This only controls CMake variable warnings inside CMake itself, not other types of build warnings. This option can also be set permanently by setting the ``IDF_CMAKE_WARN_UNINITIALIZED`` environment variable to a non-zero value.
- ``--no-hints`` flag disables hints on resolving errors and disable capturing output.

Passing arguments via a ``@file``
---------------------------------

It is possible to pass multiple arguments to ``idf.py`` via a file. The file or path to the file must be annotated with ``@`` at the beginning. Arguments in the file can be separated by newlines or spaces and are expanded exactly as if they had appeared in that order on the idf.py command line.

For example, let's have a file `custom_flash.txt`:

.. code-block:: bash

  flash --baud 115200

Then the command can be executed as: ``idf.py "@custom_flash.txt" monitor``

Arguments from a file can be combined with additional command line arguments, and multiple files annotated with ``@`` can be used simultaneously. For instance, if there is a second file ``another_config.txt``, both can be utilized by specifying ``idf.py "@custom_flash.txt" "@another_config.txt" monitor``.

A further example of how this argument file can be used, e.g., creating configuration profile files via @filename, is in the `Create configuration profile files <profile_file_>`_ paragraph in Multiple Build Configurations Example.

Extending ``idf.py``
====================

``idf.py`` can be extended with additional subcommands, global options, and callbacks provided by extension files in your project and components which participate in the build, as well as by external Python packages exposing entry points.

- **From components participating in the build**: Place a file named ``idf_ext.py`` in the project root or in a component's root directory that is registered in the project's ``CMakeLists.txt``. Component extensions are discovered after the project is configured - run ``idf.py build`` or ``idf.py reconfigure`` to make newly added commands available.
- **From Python entry points**: Any installed Python package may contribute extensions by defining an entry point in the ``idf_extension`` group. Package installation is sufficient, no project build is required.

.. important::

   Extensions must not define subcommands or options that have the same names as the core ``idf.py`` commands. Custom actions and options are checked for name collisions, overriding defaults is not possible and a warning is printed. For Python entry points, use unique identifiers as duplicate entry point names will be ignored with a warning.

Extension File Example
----------------------

An extension file defines an ``action_extensions`` function which returns additional actions/options. The same structure is used for component-based extensions (``idf_ext.py``) and for package-based extensions (e.g., ``<package_name>_ext.py``):

.. code-block:: python

  from typing import Any
  import click

  def action_extensions(base_actions: dict, project_path: str) -> dict:
      def hello_test(subcommand_name: str, ctx: click.Context, global_args: dict, **action_args: Any) -> None:
          message = action_args.get('message')
          print(f"Running action: {subcommand_name}. Message: {message}")

      def global_callback_detail(ctx: click.Context, global_args: dict, tasks: list) -> None:
          if getattr(global_args, 'detail', False):
              print(f"About to execute {len(tasks)} task(s): {[t.name for t in tasks]}")

      return {
          "version": "1",
          "global_options": [
              {
                  "names": ["--detail", "-d"],
                  "is_flag": True,
                  "help": "Enable detailed output",
              }
          ],
          "global_action_callbacks": [global_callback_detail],
          "actions": {
              "hello": {
                  "callback": hello_test,
                  "short_help": "Hello from component",
                  "help": "Test command from component extension",
                  "options": [
                      {
                          "names": ["--message", "-m"],
                          "help": "Custom message to display",
                          "default": "Hi there!",
                          "type": str,
                      }
                  ]
              },
          },
      }


Extension API Reference
-----------------------

The ``action_extensions`` function takes arguments ``base_actions`` (all currently registered commands) and ``project_path`` (absolute project directory) and returns a dictionary with up to four keys:

- ``version``: A string representing the interface version of the extension. Currently, the API version is ``1``. **This key is mandatory** and must be provided.
- ``global_options``: A list of options available globally for all commands. Each option is a dictionary with fields such as ``names``, ``help``, ``type``, ``is_flag``, ``scope``, etc.
- ``global_action_callbacks``: A list of functions called once before any task execution. Each global action callback function accepts three arguments:

   - ``ctx`` — The `click context`_
   - ``global_args`` — All available global arguments
   - ``tasks`` — The list of tasks to be executed. Task refer to the action / sub-command used with `idf.py`

- ``actions``: A dictionary of new subcommands. Each action has a ``callback`` function and may also include ``options``, ``arguments``, ``dependencies``, etc. Each action callback function accepts three to four arguments:

   - ``subcommand_name`` — the name of the command (useful if multiple commands share the same callback)
   - ``ctx`` — the `click context`_
   - ``global_args`` — all available global arguments,
   - ``**action_args`` — (optional) arguments passed to the action

Basic Usage Examples
--------------------

1) Provide an extension from a component in your project

  Create ``idf_ext.py`` in the project root or in a registered component (for example ``components/my_component/idf_ext.py``). Use the extension file example above as your ``idf_ext.py`` implementation.

  Run ``idf.py build`` or ``idf.py reconfigure`` to load the new command, then ``idf.py --help`` will show the new extension.

2) Provide an extension via a Python package entry point

  Implement your extension in a module named ``<package_name>_ext.py`` using the extension file example above, and expose the ``action_extensions`` function via the ``idf_extension`` entry-point group. For example, with ``pyproject.toml``:

  .. code-block:: TOML

    [project]
    name = "my_comp"
    version = "0.1.0"

    [project.entry-points.idf_extension]
    my_pkg_ext = "my_component.my_ext:action_extensions"


  Install the package into the same Python environment as ``idf.py`` (for example with ``pip install -e .`` in the package directory). It is recommended to use a unique module name (e.g., ``<package_name>_ext.py``) to avoid name conflicts. After successful installation, ``idf.py --help`` will show the new extension.

.. _cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool: https://github.com/espressif/esptool/#readme
.. _CCache: https://ccache.dev/
.. _click context: https://click.palletsprojects.com/en/stable/api/#context
.. _CMake presets: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
.. _profile_file: https://github.com/espressif/esp-idf/tree/release/v5.5/examples/build_system/cmake/multi_config#create-configuration-profile-files-via-filename
