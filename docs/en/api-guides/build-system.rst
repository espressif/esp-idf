Build System
************

:link_to_translation:`zh_CN:[中文]`

This document explains the implementation of the ESP-IDF build system and the concept of "components". Read this document if you want to know how to organise and build a new ESP-IDF project or component.

.. note:: This document describes the CMake-based build system, which is the default since ESP-IDF V4.0. ESP-IDF also supports a :doc:`legacy build system based on GNU Make <build-system-legacy>`, which was the default before ESP-IDF V4.0.


Overview
========

An ESP-IDF project can be seen as an amalgamation of a number of components.
For example, for a webserver that shows the current humidity, there could be:

- The ESP32 base libraries (libc, ROM bindings, etc)
- The WiFi drivers
- A TCP/IP stack
- The FreeRTOS operating system
- A webserver
- A driver for the humidity sensor
- Main code tying it all together

ESP-IDF makes these components explicit and configurable. To do that,
when a project is compiled, the build system will look up all the
components in the ESP-IDF directories, the project directories and
(optionally) in additional custom component directories. It then
allows the user to configure the ESP-IDF project using a a text-based
menu system to customize each component. After the components in the
project are configured, the build system will compile the project.

Concepts
--------

- A "project" is a directory that contains all the files and configuration to build a single "app" (executable), as well as additional supporting elements such as a partition table, data/filesystem partitions, and a bootloader.

- "Project configuration" is held in a single file called ``sdkconfig`` in the root directory of the project. This configuration file is modified via ``idf.py menuconfig`` to customise the configuration of the project. A single project contains exactly one project configuration.

- An "app" is an executable which is built by ESP-IDF. A single project will usually build two apps - a "project app" (the main executable, ie your custom firmware) and a "bootloader app" (the initial bootloader program which launches the project app).

- "components" are modular pieces of standalone code which are compiled into static libraries (.a files) and linked into an app. Some are provided by ESP-IDF itself, others may be sourced from other places.

- "Target" is the hardware for which an application is built. At the moment, ESP-IDF supports only one target, ``esp32``.

Some things are not part of the project:

- "ESP-IDF" is not part of the project. Instead it is standalone, and linked to the project via the ``IDF_PATH`` environment variable which holds the path of the ``esp-idf`` directory. This allows the IDF framework to be decoupled from your project.

- The toolchain for compilation is not part of the project. The toolchain should be installed in the system command line PATH.

Using the Build System
======================

.. _idf.py:

idf.py
------

The ``idf.py`` command line tool provides a front-end for easily managing your project builds. It manages the following tools:

- CMake_, which configures the project to be built
- A command line build tool (either Ninja_ build or `GNU Make`)
- `esptool.py`_ for flashing ESP32.

The :ref:`getting started guide <get-started-configure>` contains a brief introduction to how to set up ``idf.py`` to configure, build, and flash projects.

``idf.py`` should be run in an ESP-IDF "project" directory, ie one containing a ``CMakeLists.txt`` file. Older style projects with a Makefile will not work with ``idf.py``.

Type ``idf.py --help`` for a full list of commands. Here are a summary of the most useful ones:

- ``idf.py menuconfig`` runs the "menuconfig" tool to configure the project.
- ``idf.py build`` will build the project found in the current directory. This can involve multiple steps:

  - Create the build directory if needed. The sub-directory ``build`` is used to hold build output, although this can be changed with the ``-B`` option.
  - Run CMake_ as necessary to configure the project and generate build files for the main build tool.
  - Run the main build tool (Ninja_ or `GNU Make`). By default, the build tool is automatically detected but it can be explicitly set by passing the ``-G`` option to ``idf.py``.

  Building is incremental so if no source files or configuration has changed since the last build, nothing will be done.
- ``idf.py clean`` will "clean" the project by deleting build output files from the build directory, forcing a "full rebuild" the next time the project is built. Cleaning doesn't delete CMake configuration output and some other files.
- ``idf.py fullclean`` will delete the entire "build" directory contents. This includes all CMake configuration output. The next time the project is built, CMake will configure it from scratch. Note that this option recursively deletes *all* files in the build directory, so use with care. Project configuration is not deleted.
- ``idf.py flash`` will automatically build the project if necessary, and then flash it to an ESP32. The ``-p`` and ``-b`` options can be used to set serial port name and flasher baud rate, respectively.
- ``idf.py monitor`` will display serial output from the ESP32. The ``-p`` option can be used to set the serial port name. Type ``Ctrl-]`` to exit the monitor. See :doc:`tools/idf-monitor` for more details about using the monitor.

Multiple ``idf.py`` commands can be combined into one. For example, ``idf.py -p COM4 clean flash monitor`` will clean the source tree, then build the project and flash it to the ESP32 before running the serial monitor.

.. note:: The environment variables ``ESPPORT`` and ``ESPBAUD`` can be used to set default values for the ``-p`` and ``-b`` options, respectively. Providing these options on the command line overrides the default.

.. _idf.py-size:

Advanced Commands
^^^^^^^^^^^^^^^^^

- ``idf.py app``, ``idf.py bootloader``, ``idf.py partition_table`` can be used to build only the app, bootloader, or partition table from the project as applicable.
- There are matching commands ``idf.py app-flash``, etc. to flash only that single part of the project to the ESP32.
- ``idf.py -p PORT erase_flash`` will use esptool.py to erase the ESP32's entire flash chip.
- ``idf.py size`` prints some size information about the app. ``size-components`` and ``size-files`` are similar commands which print more detailed per-component or per-source-file information, respectively. If you define variable ``-DOUTPUT_JSON=1`` when running CMake (or ``idf.py``), the output will be formatted as JSON not as human readable text.
- ``idf.py reconfigure`` re-runs CMake_ even if it doesn't seem to need re-running. This isn't necessary during normal usage, but can be useful after adding/removing files from the source tree, or when modifying CMake cache variables. For example, ``idf.py -DNAME='VALUE' reconfigure`` can be used to set variable ``NAME`` in CMake cache to value ``VALUE``.

The order of multiple ``idf.py`` commands on the same invocation is not important, they will automatically be executed in the correct order for everything to take effect (ie building before flashing, erasing before flashing, etc.).

Using CMake Directly
--------------------

:ref:`idf.py` is a wrapper around CMake_ for convenience. However, you can also invoke CMake directly if you prefer.

.. highlight:: bash

When ``idf.py`` does something, it prints each command that it runs for easy reference. For example, the ``idf.py build`` command is the same as running these commands in a bash shell (or similar commands for Windows Command Prompt)::

  mkdir -p build
  cd build
  cmake .. -G Ninja   # or 'Unix Makefiles'
  ninja

In the above list, the ``cmake`` command configures the project and generates build files for use with the final build tool. In this case the final build tool is Ninja_: running ``ninja`` actually builds the project.

It's not necessary to run ``cmake`` more than once. After the first build, you only need to run ``ninja`` each time. ``ninja`` will automatically re-invoke ``cmake`` if the project needs reconfiguration.

If using CMake with ``ninja`` or ``make``, there are also targets for more of the ``idf.py`` sub-commands - for example running ``make menuconfig`` or ``ninja menuconfig`` in the build directory will work the same as ``idf.py menuconfig``.

.. note::
   If you're already familiar with CMake_, you may find the ESP-IDF CMake-based build system unusual because it wraps a lot of CMake's functionality to reduce boilerplate. See `writing pure CMake components`_ for some information about writing more "CMake style" components.

.. _flash-with-ninja-or-make:

Flashing with ninja or make
^^^^^^^^^^^^^^^^^^^^^^^^^^^

It's possible to build and flash directly from ninja or make by running a target like::

  ninja flash

Or::

  make app-flash

Available targets are: ``flash``, ``app-flash`` (app only), ``bootloader-flash`` (bootloader only).

When flashing this way, optionally set the ``ESPPORT`` and ``ESPBAUD`` environment variables to specify the serial port and baud rate. You can set environment variables in your operating system or IDE project. Alternatively, set them directly on the command line::

  ESPPORT=/dev/ttyUSB0 ninja flash

.. note:: Providing environment variables at the start of the command like this is Bash shell Syntax. It will work on Linux and macOS. It won't work when using Windows Command Prompt, but it will work when using Bash-like shells on Windows.

Or::

  make -j3 app-flash ESPPORT=COM4 ESPBAUD=2000000

.. note:: Providing variables at the end of the command line is ``make`` syntax, and works for ``make`` on all platforms.


Using CMake in an IDE
---------------------

You can also use an IDE with CMake integration. The IDE will want to know the path to the project's ``CMakeLists.txt`` file. IDEs with CMake integration often provide their own build tools (CMake calls these "generators") to build the source files as part of the IDE.

When adding custom non-build steps like "flash" to the IDE, it is recommended to execute ``idf.py`` for these "special" commands.

For more detailed information about integrating ESP-IDF with CMake into an IDE, see `Build System Metadata`_.

.. _setting-python-interpreter:

Setting the Python Interpreter
------------------------------

Currently, ESP-IDF only works with Python 2.7. If you have a system where the default ``python`` interpreter is Python 3.x, this can lead to problems.

If using ``idf.py``, running ``idf.py`` as ``python2 $IDF_PATH/tools/idf.py ...`` will work around this issue (``idf.py`` will tell other Python processes to use the same Python interpreter). You can set up a shell alias or another script to simplify the command.

If using CMake directly, running ``cmake -D PYTHON=python2 ...`` will cause CMake to override the default Python interpreter.

If using an IDE with CMake, setting the ``PYTHON`` value as a CMake cache override in the IDE UI will override the default Python interpreter.

To manage the Python version more generally via the command line, check out the tools pyenv_ or virtualenv_. These let you change the default python version.

.. _example-project-structure:

Example Project
===============

.. highlight:: none

An example project directory tree might look like this::

    - myProject/
                 - CMakeLists.txt
                 - sdkconfig
                 - components/ - component1/ - CMakeLists.txt
                                             - Kconfig
                                             - src1.c
                               - component2/ - CMakeLists.txt
                                             - Kconfig
                                             - src1.c
                                             - include/ - component2.h
                 - main/       - src1.c
                               - src2.c

                 - build/

This example "myProject" contains the following elements:

- A top-level project CMakeLists.txt file. This is the primary file which CMake uses to learn how to build the project; and may set project-wide CMake variables. It includes the file :idf_file:`/tools/cmake/project.cmake` which
  implements the rest of the build system. Finally, it sets the project name and defines the project.

- "sdkconfig" project configuration file. This file is created/updated when ``idf.py menuconfig`` runs, and holds configuration for all of the components in the project (including ESP-IDF itself). The "sdkconfig" file may or may not be added to the source control system of the project.

- Optional "components" directory contains components that are part of the project. A project does not have to contain custom components of this kind, but it can be useful for structuring reusable code or including third party components that aren't part of ESP-IDF.

- "main" directory is a special "pseudo-component" that contains source code for the project itself. "main" is a default name, the CMake variable ``COMPONENT_DIRS`` includes this component but you can modify this variable. Alternatively, ``EXTRA_COMPONENT_DIRS`` can be set in the top-level CMakeLists.txt to look for components in other places. See the :ref:`renaming main <rename-main>` section for more info. If you have a lot of source files in your project, we recommend grouping most into components instead of putting them all in "main".

- "build" directory is where build output is created. This directory is created by ``idf.py`` if it doesn't already exist. CMake configures the project and generates interim build files in this directory. Then, after the main build process is run, this directory will also contain interim object files and libraries as well as final binary output files. This directory is usually not added to source control or distributed with the project source code.

Component directories each contain a component ``CMakeLists.txt`` file. This file contains variable definitions
to control the build process of the component, and its integration into the overall project. See `Component CMakeLists Files`_ for more details.

Each component may also include a ``Kconfig`` file defining the `component configuration`_ options that can be set via ``menuconfig``. Some components may also include ``Kconfig.projbuild`` and ``project_include.cmake`` files, which are special files for `overriding parts of the project`_.

Project CMakeLists File
=======================

Each project has a single top-level ``CMakeLists.txt`` file that contains build settings for the entire project. By default, the project CMakeLists can be quite minimal.

Minimal Example CMakeLists
--------------------------

.. highlight:: cmake

Minimal project::

      cmake_minimum_required(VERSION 3.5)
      include($ENV{IDF_PATH}/tools/cmake/project.cmake)
      project(myProject)


.. _project-mandatory-parts:

Mandatory Parts
---------------

The inclusion of these three lines, in the order shown above, is necessary for every project:

- ``cmake_minimum_required(VERSION 3.5)`` tells CMake the minimum version that is required to build the project. ESP-IDF is designed to work with CMake 3.5 or newer. This line must be the first line in the CMakeLists.txt file.
- ``include($ENV{IDF_PATH}/tools/cmake/project.cmake)`` pulls in the rest of the CMake functionality to configure the project, discover all the components, etc.
- ``project(myProject)`` creates the project itself, and specifies the project name. The project name is used for the final binary output files of the app - ie ``myProject.elf``, ``myProject.bin``. Only one project can be defined per CMakeLists file.


Optional Project Variables
--------------------------

These variables all have default values that can be overridden for custom behaviour. Look in :idf_file:`/tools/cmake/project.cmake` for all of the implementation details.

- ``COMPONENT_DIRS``,``COMPONENTS_DIRS``: Directories to search for components. Defaults to `IDF_PATH/components`, `PROJECT_DIR/components`, and ``EXTRA_COMPONENT_DIRS``. Override this variable if you don't want to search for components in these places.
- ``EXTRA_COMPONENT_DIRS``, ``EXTRA_COMPONENTS_DIRS``: Optional list of additional directories to search for components. Paths can be relative to the project directory, or absolute.
- ``COMPONENTS``: A list of component names to build into the project. Defaults to all components found in the ``COMPONENT_DIRS`` directories. Use this variable to "trim down" the project for faster build times. Note that any component which "requires" another component via the REQUIRES or PRIV_REQUIRES arguments on component registration will automatically have it added to this list, so the ``COMPONENTS`` list can be very short.

Any paths in these variables can be absolute paths, or set relative to the project directory.

To set these variables, use the `cmake set command <cmake set_>`_ ie ``set(VARIABLE "VALUE")``. The ``set()`` commands should be placed after the ``cmake_minimum(...)`` line but before the ``include(...)`` line.

.. _rename-main:

Renaming ``main`` component
----------------------------

The build system provides special treatment to the ``main`` component. It is a component that gets automatically added to the build provided
that it is in the expected location, PROJECT_DIR/main. All other components in the build are also added as its dependencies,
saving the user from hunting down dependencies and providing a build that works right out of the box. Renaming the ``main`` component
causes the loss of these behind-the-scences heavy lifting, requiring the user to specify the location of the newly renamed component
and manually specifying its dependencies. Specifically, the steps to renaming ``main`` are as follows:

1. Rename ``main`` directory.
2. Set ``EXTRA_COMPONENT_DIRS`` in the project CMakeLists.txt to include the renamed ``main`` directory.
3. Specify the dependencies in the renamed component's CMakeLists.txt file via REQUIRES or PRIV_REQUIRES arguments :ref:`on component registration<cmake_minimal_component_cmakelists>`.

.. _component-directories:

Component CMakeLists Files
==========================

Each project contains one or more components. Components can be part of ESP-IDF, part of the project's own components directory, or added from custom component directories (:ref:`see above <component-directories>`).

A component is any directory in the ``COMPONENT_DIRS`` list which contains a ``CMakeLists.txt`` file.

Searching for Components
------------------------

The list of directories in ``COMPONENT_DIRS`` is searched for the project's components. Directories in this list can either be components themselves (ie they contain a `CMakeLists.txt` file), or they can be top-level directories whose sub-directories are components.

When CMake runs to configure the project, it logs the components included in the build. This list can be useful for debugging the inclusion/exclusion of certain components.

Multiple components with the same name
--------------------------------------

When ESP-IDF is collecting all the components to compile, it will do this in the order specified by ``COMPONENT_DIRS``; by default, this means ESP-IDF's internal components first, then the project's components, and finally any components set in ``EXTRA_COMPONENT_DIRS``. If two or more of these directories
contain component sub-directories with the same name, the component in the last place searched is used. This allows, for example, overriding ESP-IDF components
with a modified version by copying that component from the ESP-IDF components directory to the project components directory and then modifying it there.
If used in this way, the ESP-IDF directory itself can remain untouched.

.. _cmake_minimal_component_cmakelists:

Minimal Component CMakeLists
----------------------------

.. highlight:: cmake

The minimal component ``CMakeLists.txt`` file simply registers the component to the build system using ``idf_component_register``::

  idf_component_register(SRCS "foo.c" "bar.c"
                         INCLUDE_DIRS "include")

- ``SRCS`` is a list of source files (``*.c``, ``*.cpp``, ``*.cc``, ``*.S``). These source files will be compiled into the component library.
- ``INCLUDE_DIRS`` is a list of directories to add to the global include search path for any component which requires this component, and also the main source files.

A library with the name of the component will be built and linked into the final app. 
Directories are usually specified relative to the ``CMakeLists.txt`` file itself, although they can be absolute.

There are other arguments that can be passed to ``idf_component_register``. These arguments 
are discussed :ref:`here<cmake-component-register>`.

See `example component CMakeLists`_ for more complete component ``CMakeLists.txt`` examples.

.. _component variables:

Preset Component Variables
--------------------------

The following component-specific variables are available for use inside component CMakeLists, but should not be modified:

- ``COMPONENT_DIR``: The component directory. Evaluates to the absolute path of the directory containing ``CMakeLists.txt``. The component path cannot contain spaces. This is the same as the ``CMAKE_CURRENT_SOURCE_DIR`` variable.
- ``COMPONENT_NAME``: Name of the component. Same as the name of the component directory.
- ``COMPONENT_ALIAS``: Alias of the library created internally by the build system for the component.
- ``COMPONENT_LIB``: Name of the library created internally by the build system for the component. 

The following variables are set at the project level, but available for use in component CMakeLists:

- ``CONFIG_*``: Each value in the project configuration has a corresponding variable available in cmake. All names begin with ``CONFIG_``. :doc:`More information here </api-reference/kconfig>`.
- ``ESP_PLATFORM``: Set to 1 when the CMake file is processed within ESP-IDF build system.

Build/Project Variables
------------------------

The following are some project/build variables that are available as build properties and whose values can be queried using ``idf_build_get_property``
from the component CMakeLists.txt:

- ``PROJECT_NAME``: Name of the project, as set in project CMakeLists.txt file.
- ``PROJECT_DIR``: Absolute path of the project directory containing the project CMakeLists. Same as the ``CMAKE_SOURCE_DIR`` variable.
- ``COMPONENTS``: Names of all components that are included in this build, formatted as a semicolon-delimited CMake list.
- ``IDF_VER``: Git version of ESP-IDF (produced by ``git describe``)
- ``IDF_VERSION_MAJOR``, ``IDF_VERSION_MINOR``, ``IDF_VERSION_PATCH``: Components of ESP-IDF version, to be used in conditional expressions. Note that this information is less precise than that provided by ``IDF_VER`` variable. ``v4.0-dev-*``, ``v4.0-beta1``, ``v4.0-rc1`` and ``v4.0`` will all have the same values of ``IDF_VERSION_*`` variables, but different ``IDF_VER`` values.
- ``IDF_TARGET``: Name of the target for which the project is being built.
- ``PROJECT_VER``: Project version.

  * If ``PROJECT_VER`` variable is set in project CMakeLists.txt file, its value will be used.
  * Else, if the ``PROJECT_DIR/version.txt`` exists, its contents will be used as ``PROJECT_VER``.
  * Else, if the project is located inside a Git repository, the output of git describe will be used.
  * Otherwise, ``PROJECT_VER`` will be "1".

Other build properties are listed :ref:`here<cmake-build-properties>`.

Controlling Component Compilation
---------------------------------

.. highlight:: cmake

To pass compiler options when compiling source files belonging to a particular component, use the ``target_compile_options`` function::

  target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-variable)

To apply the compilation flags to a single source file, use the CMake `set_source_files_properties`_ command::

    set_source_files_properties(mysrc.c
        PROPERTIES COMPILE_FLAGS
        -Wno-unused-variable
    )

This can be useful if there is upstream code that emits warnings.

When using these commands, place them after the call to ``idf_component_register`` in the component CMakeLists file.

.. _component-configuration:

Component Configuration
=======================

Each component can also have a ``Kconfig`` file, alongside ``CMakeLists.txt``. This contains
configuration settings to add to the configuration menu for this component.

These settings are found under the "Component Settings" menu when menuconfig is run.

To create a component Kconfig file, it is easiest to start with one of the Kconfig files distributed with ESP-IDF.

For an example, see `Adding conditional configuration`_.

Preprocessor Definitions
========================

The ESP-IDF build system adds the following C preprocessor definitions on the command line:

- ``ESP_PLATFORM`` : Can be used to detect that build happens within ESP-IDF.
- ``IDF_VER`` : Defined to a git version string.  E.g. ``v2.0`` for a tagged release or ``v1.0-275-g0efaa4f`` for an arbitrary commit.

Component Requirements
======================

When compiling each component, the ESP-IDF build system recursively evaluates its components.

Each component's source file is compiled with these include path directories, as specified in the passed arguments
to ``idf_component_register``:

- The current component's ``INCLUDE_DIRS`` and ``PRIV_INCLUDE_DIRS``.
- The ``INCLUDE_DIRS`` set by all components in the current component's ``REQUIRES`` and ``PRIV_REQUIRES`` variables (ie all the current component's public and private dependencies).
- All of the ``REQUIRES`` of those components, evaluated recursively (ie all public dependencies of this component's dependencies, recursively expanded).

When writing a component
------------------------

- ``REQUIRES`` should be set to all components whose header files are #included from the *public* header files of this component.
- ``PRIV_REQUIRES`` should be set to all components whose header files are #included from *any source files* of this component, unless already listed in ``COMPONENT_REQUIRES``. Or any component which is required to be linked in order for this component to function correctly.
- ``REQUIRES`` and/or ``PRIV_REQUIRES`` should be set before calling ``idf_component_register()``.
- The values of ``REQUIRES`` and ``PRIV_REQUIRES`` should not depend on any configuration choices (``CONFIG_xxx`` macros). This is because requirements are expanded before configuration is loaded. Other component variables (like include paths or source files) can depend on configuration choices.
- Not setting either or both ``REQUIRES`` variables is fine. If the component has no requirements except for the "common" components needed for RTOS, libc, etc (``COMPONENT_REQUIRES_COMMON``) then both variables can be empty or unset.

Components which support only some targets (values of ``IDF_TARGET``) may specify ``REQUIRED_IDF_TARGETS`` in the ``idf_component_register`` call to express these requirements. In this case the build system will generate an error if the component is included into the build, but does not support selected target.

When creating a project
-----------------------

- By default, every component is included in the build.
- If you set the ``COMPONENTS`` variable to a minimal list of components used directly by your project, then the build will include:

  - Components mentioned explicitly in ``COMPONENTS``.
  - Those components' requirements (evaluated recursively).
  - The "common" components that every component depends on.
- Setting ``COMPONENTS`` to the minimal list of required components can significantly reduce compile times.

.. _component-requirements-implementation:

Requirements in the build system implementation
-----------------------------------------------

- Very early in the CMake configuration process, the script ``expand_requirements.cmake`` is run. This script does a partial evaluation of all component CMakeLists.txt files and builds a graph of component requirements (this graph may have cycles). The graph is used to generate a file ``component_depends.cmake`` in the build directory.
- The main CMake process then includes this file and uses it to determine the list of components to include in the build (internal ``BUILD_COMPONENTS`` variable). The ``BUILD_COMPONENTS`` variable is sorted so dependencies are listed first, however as the component dependency graph has cycles this cannot be guaranteed for all components. The order should be deterministic given the same set of components and component dependencies.
- The value of ``BUILD_COMPONENTS`` is logged by CMake as "Component names: "
- Configuration is then evaluated for the components included in the build.
- Each component is included in the build normally and the CMakeLists.txt file is evaluated again to add the component libraries to the build.

Component Dependency Order
^^^^^^^^^^^^^^^^^^^^^^^^^^

The order of components in the ``BUILD_COMPONENTS`` variable determines other orderings during the build:

- Order that :ref:`project_include.cmake` files are included into the project.
- Order that the list of header paths is generated for compilation (via ``-I`` argument). (Note that for a given component's source files, only that component's dependency's header paths are passed to the compiler.)


Overriding Parts of the Project
===============================

.. _project_include.cmake:

project_include.cmake
---------------------

For components that have build requirements which must be evaluated before any component CMakeLists
files are evaluated, you can create a file called ``project_include.cmake`` in the
component directory. This CMake file is included when ``project.cmake`` is evaluating the entire project.

``project_include.cmake`` files are used inside ESP-IDF, for defining project-wide build features such as ``esptool.py`` command line arguments and the ``bootloader`` "special app".

Unlike component ``CMakeLists.txt`` files, when including a ``project_include.cmake`` file the current source directory (``CMAKE_CURRENT_SOURCE_DIR`` and working directory) is the project directory. Use the variable ``COMPONENT_DIR`` for the absolute directory of the component.

Note that ``project_include.cmake`` isn't necessary for the most common component uses - such as adding include directories to the project, or ``LDFLAGS`` to the final linking step. These values can be customised via the ``CMakeLists.txt`` file itself. See `Optional Project Variables`_ for details.

``project_include.cmake`` files are included in the order given in ``BUILD_COMPONENTS`` variable (as logged by CMake). This means that a component's ``project_include.cmake`` file will be included after it's all dependencies' ``project_include.cmake`` files, unless both components are part of a dependency cycle. This is important if a ``project_include.cmake`` file relies on variables set by another component. See also :ref:`above<component-requirements-implementation>`.

Take great care when setting variables or targets in a ``project_include.cmake`` file. As the values are included into the top-level project CMake pass, they can influence or break functionality across all components!

KConfig.projbuild
-----------------

This is an equivalent to ``project_include.cmake`` for :ref:`component-configuration` KConfig files. If you want to include
configuration options at the top-level of menuconfig, rather than inside the "Component Configuration" sub-menu, then these can be defined in the KConfig.projbuild file alongside the ``CMakeLists.txt`` file.

Take care when adding configuration values in this file, as they will be included across the entire project configuration. Where possible, it's generally better to create a KConfig file for :ref:`component-configuration`.

``project_include.cmake`` files are used inside ESP-IDF, for defining project-wide build features such as ``esptool.py`` command line arguments and the ``bootloader`` "special app".

Configuration-Only Components
=============================

Special components which contain no source files, only ``Kconfig.projbuild`` and ``KConfig``, can have a one-line ``CMakeLists.txt`` file which calls the function ``idf_component_register()`` with no 
arguments specified. This function will include the component in the project build, but no library will be built *and* no header files will be added to any include paths.


Debugging CMake
===============

For full details about CMake_ and CMake commands, see the `CMake v3.5 documentation`_.

Some tips for debugging the ESP-IDF CMake-based build system:

- When CMake runs, it prints quite a lot of diagnostic information including lists of components and component paths.
- Running ``cmake -DDEBUG=1`` will produce more verbose diagnostic output from the IDF build system.
- Running ``cmake`` with the ``--trace`` or ``--trace-expand`` options will give a lot of information about control flow. See the `cmake command line documentation`_.

When included from a project CMakeLists file, the ``project.cmake`` file defines some utility modules and global variables and then sets ``IDF_PATH`` if it was not set in the system environment.

It also defines an overridden custom version of the built-in CMake_ ``project`` function. This function is overridden to add all of the ESP-IDF specific project functionality.

.. _warn-undefined-variables:

Warning On Undefined Variables
------------------------------

By default, ``idf.py`` passes the ``--warn-uninitialized`` flag to CMake_ so it will print a warning if an undefined variable is referenced in the build. This can be very useful to find buggy CMake files.

If you don't want this behaviour, it can be disabled by passing ``--no-warnings`` to ``idf.py``.

Browse the :idf_file:`/tools/cmake/project.cmake` file and supporting functions in :idf_file:`/tools/cmake/idf_functions.cmake` for more details.

.. _gnu-make-to-cmake:


Example Component CMakeLists
============================

Because the build environment tries to set reasonable defaults that will work most
of the time, component ``CMakeLists.txt`` can be very small or even empty (see `Minimal Component CMakeLists`_). However, overriding `component variables`_ is usually required for some functionality.

Here are some more advanced examples of component CMakeLists files.

Adding conditional configuration
--------------------------------

The configuration system can be used to conditionally compile some files
depending on the options selected in the project configuration.

.. highlight:: none

``Kconfig``::

    config FOO_ENABLE_BAR
        bool "Enable the BAR feature."
        help
            This enables the BAR feature of the FOO component.

``CMakeLists.txt``::

    set(COMPONENT_SRCS "foo.c" "more_foo.c")

    if(CONFIG_FOO_ENABLE_BAR)
        list(APPEND COMPONENT_SRCS "bar.c")
    endif()

This example makes use of the CMake `if <cmake if_>`_ function and `list APPEND <cmake list_>`_ function.

This can also be used to select or stub out an implementation, as such:

``Kconfig``::

    config ENABLE_LCD_OUTPUT
        bool "Enable LCD output."
        help
            Select this if your board has a LCD.

    config ENABLE_LCD_CONSOLE
        bool "Output console text to LCD"
        depends on ENABLE_LCD_OUTPUT
        help
            Select this to output debugging output to the lcd

    config ENABLE_LCD_PLOT
        bool "Output temperature plots to LCD"
        depends on ENABLE_LCD_OUTPUT
        help
            Select this to output temperature plots

.. highlight:: cmake

``CMakeLists.txt``::

    if(CONFIG_ENABLE_LCD_OUTPUT)
       set(COMPONENT_SRCS lcd-real.c lcd-spi.c)
    else()
       set(COMPONENT_SRCS lcd-dummy.c)
    endif()

    # We need font if either console or plot is enabled
    if(CONFIG_ENABLE_LCD_CONSOLE OR CONFIG_ENABLE_LCD_PLOT)
      list(APPEND COMPONENT_SRCS "font.c")
    endif()


Conditions which depend on the target
-------------------------------------

The current target is available to CMake files via ``IDF_TARGET`` variable.

In addition to that, if target ``xyz`` is used (``IDF_TARGET=xyz``), then Kconfig variable ``CONFIG_IDF_TARGET_XYZ`` will be set.

Note that component dependencies may depend on ``IDF_TARGET`` variable, but not on Kconfig variables. Also one can not use Kconfig variables in ``include`` statements in CMake files, but ``IDF_TARGET`` can be used in such context.


Source Code Generation
----------------------

Some components will have a situation where a source file isn't
supplied with the component itself but has to be generated from
another file. Say our component has a header file that consists of the
converted binary data of a BMP file, converted using a hypothetical
tool called bmp2h. The header file is then included in as C source
file called graphics_lib.c::

    add_custom_command(OUTPUT logo.h
         COMMAND bmp2h -i ${COMPONENT_DIR}/logo.bmp -o log.h
         DEPENDS ${COMPONENT_DIR}/logo.bmp
         VERBATIM)

    add_custom_target(logo DEPENDS logo.h)
    add_dependencies(${COMPONENT_LIB} logo)

    set_property(DIRECTORY "${COMPONENT_DIR}" APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES logo.h)

This answer is adapted from the `CMake FAQ entry <cmake faq generated files_>`_, which contains some other examples that will also work with ESP-IDF builds.

In this example, logo.h will be generated in the
current directory (the build directory) while logo.bmp comes with the
component and resides under the component path. Because logo.h is a
generated file, it should be cleaned when the project is cleaned. For this reason
it is added to the `ADDITIONAL_MAKE_CLEAN_FILES`_ property.

.. note::

   If generating files as part of the project CMakeLists.txt file, not a component CMakeLists.txt, then use build property ``PROJECT_DIR`` instead of ``${COMPONENT_DIR}`` and ``${PROJECT_NAME}.elf`` instead of ``${COMPONENT_LIB}``.)

If a a source file from another component included ``logo.h``, then ``add_dependencies`` would need to be called to add a dependency between
the two components, to ensure that the component source files were always compiled in the correct order.

.. _cmake_embed_data:

Embedding Binary Data
---------------------

Sometimes you have a file with some binary or text data that you'd like to make available to your component - but you don't want to reformat the file as C source.

You can specify argument ``COMPONENT_EMBED_FILES`` in the component registration, giving space-delimited names of the files to embed::

  idf_component_register(...
                         EMBED_FILES server_root_cert.der)


Or if the file is a string, you can use the variable ``COMPONENT_EMBED_TXTFILES``. This will embed the contents of the text file as a null-terminated string::

  idf_component_register(...
                         EMBED_TXTFILES server_root_cert.pem)

.. highlight:: c

The file's contents will be added to the .rodata section in flash, and are available via symbol names as follows::

  extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
  extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

The names are generated from the full name of the file, as given in ``COMPONENT_EMBED_FILES``. Characters /, ., etc. are replaced with underscores. The _binary prefix in the symbol name is added by objcopy and is the same for both text and binary files.

.. highlight:: cmake

To embed a file into a project, rather than a component, you can call the function ``target_add_binary_data`` like this::

  target_add_binary_data(myproject.elf "main/data.bin" TEXT)

Place this line after the ``project()`` line in your project CMakeLists.txt file. Replace ``myproject.elf`` with your project name. The final argument can be ``TEXT`` to embed a null-terminated string, or ``BINARY`` to embed the content as-is.

For an example of using this technique, see :example:`protocols/https_request` - the certificate file contents are loaded from the text .pem file at compile time.

Code and Data Placements
------------------------

ESP-IDF has a feature called linker script generation that enables components to define where its code and data will be placed in memory through
linker fragment files. These files are processed by the build system, and is used to augment the linker script used for linking
app binary. See :doc:`Linker Script Generation <linker-script-generation>` for a quick start guide as well as a detailed discussion
of the mechanism.

.. _component-build-full-override:

Fully Overriding The Component Build Process
--------------------------------------------

.. highlight:: cmake

Obviously, there are cases where all these recipes are insufficient for a
certain component, for example when the component is basically a wrapper
around another third-party component not originally intended to be
compiled under this build system. In that case, it's possible to forego
the ESP-IDF build system entirely by using a CMake feature called ExternalProject_. Example component CMakeLists::

  # External build process for quirc, runs in source dir and
  # produces libquirc.a
  externalproject_add(quirc_build
      PREFIX ${COMPONENT_DIR}
      SOURCE_DIR ${COMPONENT_DIR}/quirc
      CONFIGURE_COMMAND ""
      BUILD_IN_SOURCE 1
      BUILD_COMMAND make CC=${CMAKE_C_COMPILER} libquirc.a
      INSTALL_COMMAND ""
      )

   # Add libquirc.a to the build process
   #
   add_library(quirc STATIC IMPORTED GLOBAL)
   add_dependencies(quirc quirc_build)

   set_target_properties(quirc PROPERTIES IMPORTED_LOCATION
        ${COMPONENT_DIR}/quirc/libquirc.a)
   set_target_properties(quirc PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        ${COMPONENT_DIR}/quirc/lib)

   set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
        "${COMPONENT_DIR}/quirc/libquirc.a")

(The above CMakeLists.txt can be used to create a component named ``quirc`` that builds the quirc_ project using its own Makefile.)

- ``externalproject_add`` defines an external build system.

  - ``SOURCE_DIR``, ``CONFIGURE_COMMAND``, ``BUILD_COMMAND`` and ``INSTALL_COMMAND`` should always be set. ``CONFIGURE_COMMAND`` can be set to an empty string if the build system has no "configure" step. ``INSTALL_COMMAND`` will generally be empty for ESP-IDF builds.
  - Setting ``BUILD_IN_SOURCE`` means the build directory is the same as the source directory. Otherwise you can set ``BUILD_DIR``.
  - Consult the ExternalProject_ documentation for more details about ``externalproject_add()``

- The second set of commands adds a library target, which points to the "imported" library file built by the external system. Some properties need to be set in order to add include directories and tell CMake where this file is.
- Finally, the generated library is added to `ADDITIONAL_MAKE_CLEAN_FILES`_. This means ``make clean`` will delete this library. (Note that the other object files from the build won't be deleted.)

.. note:: When using an external build process with PSRAM, remember to add ``-mfix-esp32-psram-cache-issue`` to the C compiler arguments. See :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND` for details of this flag.

.. _ADDITIONAL_MAKE_CLEAN_FILES_note:

ExternalProject dependencies, clean builds
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CMake has some unusual behaviour around external project builds:

- `ADDITIONAL_MAKE_CLEAN_FILES`_ only works when "make" is used as the build system. If Ninja_ or an IDE build system is used, it won't delete these files when cleaning.
- However, the ExternalProject_ configure & build commands will *always* be re-run after a clean is run.
- Therefore, there are two alternative recommended ways to configure the external build command:

    1. Have the external ``BUILD_COMMAND`` run a full clean compile of all sources. The build command will be run if any of the dependencies passed to ``externalproject_add`` with ``DEPENDS`` have changed, or if this is a clean build (ie any of ``idf.py clean``, ``ninja clean``, or ``make clean`` was run.)
    2. Have the external ``BUILD_COMMAND`` be an incremental build command. Pass the parameter ``BUILD_ALWAYS 1`` to ``externalproject_add``. This means the external project will be built each time a build is run, regardless of dependencies. This is only recommended if the external project has correct incremental build behaviour, and doesn't take too long to run.

The best of these approaches for building an external project will depend on the project itself, its build system, and whether you anticipate needing to frequently recompile the project.

.. _custom-sdkconfig-defaults:

Custom sdkconfig defaults
=========================

For example projects or other projects where you don't want to specify a full sdkconfig configuration, but you do want to override some key values from the ESP-IDF defaults, it is possible to create a file ``sdkconfig.defaults`` in the project directory. This file will be used when creating a new config from scratch, or when any new config value hasn't yet been set in the ``sdkconfig`` file.

To override the name of this file, set the ``SDKCONFIG_DEFAULTS`` environment variable.

Target-dependent sdkconfig defaults
-----------------------------------

In addition to ``sdkconfig.defaults`` file, build system will also load defaults from ``sdkconfig.defaults.TARGET_NAME`` file, where ``TARGET_NAME`` is the value of ``IDF_TARGET``. For example, for ``esp32`` target, default settings will be taken from ``sdkconfig.defaults`` first, and then from ``sdkconfig.defaults.esp32``.

If ``SDKCONFIG_DEFAULTS`` is used to override the name of defaults file, the name of target-specific defaults file will be derived from ``SDKCONFIG_DEFAULTS`` value.


Flash arguments
===============

There are some scenarios that we want to flash the target board without IDF. For this case we want to save the built binaries, esptool.py and esptool write_flash arguments. It's simple to write a script to save binaries and esptool.py.

After running a project build, the build directory contains binary output files (``.bin`` files) for the project and also the following flashing data files:

- ``flash_project_args`` contains arguments to flash the entire project (app, bootloader, partition table, PHY data if this is configured).
- ``flash_app_args`` contains arguments to flash only the app.
- ``flash_bootloader_args`` contains arguments to flash only the bootloader.

.. highlight:: bash

You can pass any of these flasher argument files to ``esptool.py`` as follows::

  python esptool.py --chip esp32 write_flash @build/flash_project_args

Alternatively, it is possible to manually copy the parameters from the argument file and pass them on the command line.

The build directory also contains a generated file ``flasher_args.json`` which contains project flash information, in JSON format. This file is used by ``idf.py`` and can also be used by other tools which need information about the project build.

Building the Bootloader
=======================

The bootloader is built by default as part of ``idf.py build``, or can be built standalone via ``idf.py bootloader``.

The bootloader is a special "subproject" inside :idf:`/components/bootloader/subproject`. It has its own project CMakeLists.txt file and builds separate .ELF and .BIN files to the main project. However it shares its configuration and build directory with the main project.

The subproject is inserted as an external project from the top-level project, by the file :idf_file:`/components/bootloader/project_include.cmake`. The main build process runs CMake for the subproject, which includes discovering components (a subset of the main components) and generating a bootloader-specific config (derived from the main ``sdkconfig``).

Selecting the Target
====================

Currently ESP-IDF supports one target, ``esp32``. It is used by default by the build system. Developers working on adding multiple target support can change the target as follows::

  rm sdkconfig
  idf.py -DIDF_TARGET=new_target reconfigure


Writing Pure CMake Components
=============================

The ESP-IDF build system "wraps" CMake with the concept of "components", and helper functions to automatically integrate these components into a project build.

However, underneath the concept of "components" is a full CMake build system. It is also possible to make a component which is pure CMake.

.. highlight:: cmake

Here is an example minimal "pure CMake" component CMakeLists file for a component named ``json``::

  add_library(json STATIC
  cJSON/cJSON.c
  cJSON/cJSON_Utils.c)

  target_include_directories(json PUBLIC cJSON)

- This is actually an equivalent declaration to the IDF ``json`` component :idf_file:`/components/json/CMakeLists.txt`.
- This file is quite simple as there are not a lot of source files. For components with a large number of files, the globbing behaviour of ESP-IDF's component logic can make the component CMakeLists style simpler.)
- Any time a component adds a library target with the component name, the ESP-IDF build system will automatically add this to the build, expose public include directories, etc. If a component wants to add a library target with a different name, dependencies will need to be added manually via CMake commands.


Using Third-Party CMake Projects with Components
================================================

CMake is used for a lot of open-source C and C++ projects — code that users can tap into for their applications. One of the benefits of having a CMake build system
is the ability to import these third-party projects, sometimes even without modification! This allows for users to be able to get functionality that may
not yet be provided by a component, or use another library for the same functionality.

.. highlight:: cmake

Importing a library might look like this for a hypothetical library ``foo`` to be used in the ``main`` component::

  # Register the component
  idf_component_register()

  # Set values of hypothetical variables that control the build of `foo`
  set(FOO_BUILD_STATIC OFF)
  set(FOO_BUILD_TESTS OFF)

  # Create and import the library targets
  add_subdirectory(foo)

  # Link `foo` to `main` component
  target_link_libraries(main foo)

For an actual example, take a look at :example:`build_system/cmake/import_lib`. Take note that what needs to be done in order to import
the library may vary. It is recommended to read up on the library's documentation for instructions on how to
import it from other projects. Studying the library's CMakeLists.txt and build structure can also be helpful.

It is also possible to wrap a third-party library to be used as a component in this manner. For example, the :component:`mbedtls` component is a wrapper for
Espressif's fork of `mbedtls <https://github.com/ARMmbed/mbedtls>`_. See its :component_file:`component CMakeLists.txt <mbedtls/CMakeLists.txt>`.

The CMake variable ``ESP_PLATFORM`` is set to 1 whenever the ESP-IDF build system is being used. Tests such as ``if (ESP_PLATFORM)`` can be used in generic CMake code if special IDF-specific logic is required.


Using ESP-IDF in Custom CMake Projects
======================================

ESP-IDF provides a template CMake project for easily creating an application. However, in some instances the user might already
have an existing CMake project or may want to create a custom one. In these cases it is desirable to be able to consume IDF components
as libraries to be linked to the user's targets (libraries/ executables).

It is possible to do so by using the :ref:`build system APIs provided<cmake_buildsystem_api>` by :idf_file:`tools/cmake/idf.cmake`. For example:

.. code-block:: cmake

  cmake_minimum_required(VERSION 3.5)
  project(my_custom_app C)

  # Include CMake file that provides ESP-IDF CMake build system APIs.
  include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

  # Include ESP-IDF components in the build, may be thought as an equivalent of 
  # add_subdirectory() but with some additional procesing and magic for ESP-IDF build
  # specific build processes.
  idf_build_process(esp32)
  
  # Create the project executable and plainly link the newlib component to it using 
  # its alias, idf::newlib.
  add_executable(${CMAKE_PROJECT_NAME}.elf main.c)
  target_link_libraries(${CMAKE_PROJECT_NAME}.elf idf::newlib)

  # Let the build system know what the project executable is to attach more targets, dependencies, etc.
  idf_build_executable(${CMAKE_PROJECT_NAME}.elf)

The example in :example:`build_system/cmake/idf_as_lib` demonstrates the creation of an application equivalent to :example:`hello world application <get-started/hello_world>`
using a custom CMake project.

.. note:: The IDF build system can only set compiler flags for source files that it builds. When an external CMakeLists.txt file is used and PSRAM is enabled, remember to add ``-mfix-esp32-psram-cache-issue`` to the C compiler arguments. See :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND` for details of this flag.
.. _cmake_buildsystem_api:

ESP-IDF CMake Build System API
==============================

idf-build-commands
------------------

.. code-block:: none

  idf_build_get_property(var property [GENERATOR_EXPRESSION])

Retrieve a :ref:`build property<cmake-build-properties>` *property* and store it in *var* accessible from the current scope. Specifying
*GENERATOR_EXPRESSION* will retrieve the generator expression string for that property, instead of the actual value, which
can be used with CMake commands that support generator expressions.

.. code-block:: none

  idf_build_set_property(property val [APPEND])

Set a :ref:`build property<cmake-build-properties>` *property* with value *val*. Specifying *APPEND* will append the specified value to the current
value of the property. If the property does not previously exist or it is currently empty, the specified value becomes 
the first element/member instead. 

.. code-block:: none

  idf_build_component(component_dir)

Present a directory *component_dir* that contains a component to the build system. Relative paths are converted to absolute paths with respect to current directory.
All calls to this command must be performed before `idf_build_process`. 

This command does not guarantee that the component will be processed during build (see the `COMPONENTS` argument description for `idf_build_process`)

.. code-block:: none

  idf_build_process(target 
                    [PROJECT_DIR project_dir]
                    [PROJECT_VER project_ver]
                    [PROJECT_NAME project_name]
                    [SDKCONFIG sdkconfig]
                    [SDKCONFIG_DEFAULTS sdkconfig_defaults]
                    [BUILD_DIR build_dir]
                    [COMPONENTS component1 component2 ...])

Performs the bulk of the behind-the-scenes magic for including ESP-IDF components such as component configuration, libraries creation, 
dependency expansion and resolution. Among these functions, perhaps the most important 
from a user's perspective is the libraries creation by calling each component's ``idf_component_register``. This command creates the libraries for each component, which are accessible using aliases in the form
idf::*component_name*. These aliases can be used to link the components to the user's own targets, either libraries
or executables. 

The call requires the target chip to be specified with *target* argument. Optional arguments for the call include:

- PROJECT_DIR - directory of the project; defaults to CMAKE_SOURCE_DIR
- PROJECT_NAME - name of the project; defaults to CMAKE_PROJECT_NAME
- PROJECT_VER - version/revision of the project; defaults to "0.0.0"
- SDKCONFIG - output path of generated sdkconfig file; defaults to PROJECT_DIR/sdkconfig or CMAKE_SOURCE_DIR/sdkconfig depending if PROJECT_DIR is set
- SDKCONFIG_DEFAULTS - defaults file to use for the build; defaults to empty
- BUILD_DIR - directory to place ESP-IDF build-related artifacts, such as generated binaries, text files, components; defaults to CMAKE_BINARY_DIR
- COMPONENTS - select components to process among the components known by the build system (added via `idf_build_component`). This argument is used to trim the build. 
  Other components are automatically added if they are required in the dependency chain, i.e. 
  the public and private requirements of the components in this list are automatically added, and in turn the public and private requirements of those requirements, 
  so on and so forth. If not specified, all components known to the build system are processed.

.. code-block:: none

  idf_build_executable(executable)

Specify the executable *executable* for ESP-IDF build. This attaches additional targets such as dependencies related to
flashing, generating additional binary files, etc. Should be called after ``idf_build_process``.

.. code-block:: none

  idf_build_get_config(var config [GENERATOR_EXPRESSION])

Get the value of the specified config. Much like build properties, specifying
*GENERATOR_EXPRESSION* will retrieve the generator expression string for that config, instead of the actual value, which
can be used with CMake commands that support generator expressions. Actual config values are only known after call to `idf_build_process`, however.

.. _cmake-build-properties:

idf-build-properties
--------------------

These are properties that describe the build. Values of build properties can be retrieved by using the build command ``idf_build_get_property``.
For example, to get the Python interpreter used for the build:

.. code-block: cmake

  idf_build_get_property(python PYTHON)
  message(STATUS "The Python intepreter is: ${python}")

  - BUILD_DIR - build directory; set from ``idf_build_process`` BUILD_DIR argument
  - BUILD_COMPONENTS - list of components (more specifically, component aliases) included in the build; set by ``idf_build_process``
  - C_COMPILE_OPTIONS - compile options applied to all components' C source files
  - COMPILE_OPTIONS - compile options applied to all components' source files, regardless of it being C or C++
  - COMPILE_DEFINITIONS - compile definitions applied to all component source files
  - CXX_COMPILE_OPTIONS - compile options applied to all components' C++ source files
  - EXECUTABLE - project executable; set by call to ``idf_build_executable``
  - EXECUTABLE_NAME - name of project executable without extension; set by call to ``idf_build_executable``
  - IDF_PATH - ESP-IDF path; set from IDF_PATH environment variable, if not, inferred from the location of ``idf.cmake``
  - IDF_TARGET - target chip for the build; set from the required target argument for ``idf_build_process``
  - IDF_VER - ESP-IDF version; set from either a version file or the Git revision of the IDF_PATH repository
  - INCLUDE_DIRECTORIES - include directories for all component source files
  - KCONFIGS - list of Kconfig files found in components in build; set by ``idf_build_process``
  - KCONFIG_PROJBUILDS - list of Kconfig.projbuild diles found in components in build; set by ``idf_build_process``
  - PROJECT_NAME - name of the project; set from ``idf_build_process`` PROJECT_NAME argument
  - PROJECT_DIR - directory of the project; set from ``idf_build_process`` PROJECT_DIR argument
  - PROJECT_VER - version of the project; set from ``idf_build_process`` PROJECT_VER argument
  - PYTHON - Python interpreter used for the build; set from PYTHON environment variable if available, if not "python" is used
  - SDKCONFIG - full path to output config file; set from ``idf_build_process`` SDKCONFIG argument
  - SDKCONFIG_DEFAULTS - full path to config defaults file; set from ``idf_build_process`` SDKCONFIG_DEFAULTS argument
  - SDKCONFIG_HEADER - full path to C/C++ header file containing component configuration; set by ``idf_build_process``
  - SDKCONFIG_CMAKE - full path to CMake file containing component configuration; set by ``idf_build_process``
  - SDKCONFIG_JSON - full path to JSON file containing component configuration; set by ``idf_build_process``
  - SDKCONFIG_JSON_MENUS - full path to JSON file containing config menus; set by ``idf_build_process``

idf-component-commands
----------------------

.. code-block:: none

  idf_component_get_property(var component property [GENERATOR_EXPRESSION])

Retrieve a specified *component*'s :ref:`component property<cmake-component-properties>`, *property* and store it in *var* accessible from the current scope. Specifying
*GENERATOR_EXPRESSION* will retrieve the generator expression string for that property, instead of the actual value, which
can be used with CMake commands that support generator expressions.

.. code-block:: none

  idf_component_set_property(property val [APPEND])

Set a specified *component*'s :ref:`component property<cmake-component-properties>`, *property* with value *val*. Specifying *APPEND* will append the specified value to the current
value of the property. If the property does not previously exist or it is currently empty, the specified value becomes 
the first element/member instead. 

.. _cmake-component-register:

.. code-block:: none

  idf_component_register([[SRCS src1 src2 ...] | [[SRC_DIRS dir1 dir2 ...] [EXCLUDE_SRCS src1 src2 ...]]
                         [INCLUDE_DIRS dir1 dir2 ...]
                         [PRIV_INCLUDE_DIRS dir1 dir2 ...]
                         [REQUIRES component1 component2 ...]
                         [PRIV_REQUIRES component1 component2 ...]
                         [LDFRAGMENTS ldfragment1 ldfragment2 ...]
                         [REQUIRED_IDF_TARGETS target1 target2 ...]
                         [EMBED_FILES file1 file2 ...]
                         [EMBED_TXTFILES file1 file2 ...])

Register a component to the build system. Much like the ``project()`` CMake command, this should be called from the component's 
CMakeLists.txt directly (not through a function or macro) and is recommended to be called before any other command. Here are some
guidelines on what commands can **not** be called before ``idf_component_register``:
  
  - commands that are not valid in CMake script mode
  - custom commands defined in project_include.cmake
  - build system API commands except ``idf_build_get_property``; although consider whether the property may not have been set yet

Commands that set and operate on variables are generally okay to call before ``idf_component_register``.

The arguments for ``idf_component_register`` include:

  - SRCS - component source files used for creating a static library for the component; if not specified, component is a treated as a 
    config-only component and an interface library is created instead.
  - SRC_DIRS, EXCLUDE_SRCS - used to glob source files (.c, .cpp, .S) by specifying directories, instead of specifying source files manually via SRCS. Note that this is subject to the :ref:`limitations of globbing in CMake<cmake-file-globbing>`. Source files specified in EXCLUDE_SRCS are removed from the globbed files.
  - INCLUDE_DIRS - paths, relative to the component directory, which will be added to the include search path for all other components which require the current component
  - PRIV_INCLUDE_DIRS - directory paths, must be relative to the component directory, which will be added to the include search path for this component's source files only
  - REQUIRES - public component requirements for the component
  - PRIV_REQUIRES - private component requirements for the component; ignored on config-only components
  - LDFRAGMENTS - component linker fragment files
  - REQUIRED_IDF_TARGETS - specify the only target the component supports

The following are used for :ref:`embedding data into the component<cmake_embed_data>`, and is considered as source files
when determining if a component is config-only. This means that even if the component does not specify source files, a static library is still
created internally for the component if it specifies either:

  - EMBED_FILES - binary files to be embedded in the component
  - EMBED_TXTFILES - text files to be embedded in the component
  
.. _cmake-component-properties:

idf-component-properties
------------------------

These are properties that describe a component. Values of component properties can be retrieved by using the build command ``idf_component_get_property``.
For example, to get the directory of the ``freertos`` component:

.. code-block: cmake

  idf_component_get_property(dir freertos COMPONENT_DIR)
  message(STATUS "The 'freertos' component directory is: ${dir}")

- COMPONENT_ALIAS - alias for COMPONENT_LIB used for linking the component to external targets; set by ``idf_build_component`` and alias library itself
  is created by ``idf_component_register``
- COMPONENT_DIR - component directory; set by ``idf_build_component``
- COMPONENT_LIB - name for created component static/interface library; set by ``idf_build_component`` and library itself
  is created by ``idf_component_register``
- COMPONENT_NAME - name of the component; set by ``idf_build_component`` based on the component directory name
- COMPONENT_TYPE - type of the component, whether LIBRARY or CONFIG_ONLY. A component is of type LIBRARY if it specifies
  source files or embeds a file
- EMBED_FILES - list of files to embed in component; set from ``idf_component_register`` EMBED_FILES argument
- EMBED_TXTFILES - list of text files to embed in component; set from ``idf_component_register`` EMBED_TXTFILES argument
- INCLUDE_DIRS - list of component include directories; set from ``idf_component_register`` INCLUDE_DIRS argument
- KCONFIG - component Kconfig file; set by ``idf_build_component``
- KCONFIG_PROJBUILD - component Kconfig.projbuild; set by ``idf_build_component``
- LDFRAGMENTS - list of component linker fragment files; set from ``idf_component_register`` LDFRAGMENTS argument
- PRIV_INCLUDE_DIRS - list of component private include directories; set from ``idf_component_register`` PRIV_INCLUDE_DIRS on components of type LIBRARY
- PRIV_REQUIRES - list of private component dependentices; set from ``idf_component_register`` PRIV_REQUIRES argument
- REQUIRED_IDF_TARGETS - list of targets the component supports; set from ``idf_component_register`` EMBED_TXTFILES argument
- REQUIRES - list of public component dependencies; set from ``idf_component_register`` REQUIRES argument
- SRCS - list of component source files; set from SRCS or SRC_DIRS/EXCLUDE_SRCS argument of ``idf_component_register``

.. _cmake-file-globbing:

File Globbing & Incremental Builds
==================================

.. highlight:: cmake

The preferred way to include source files in an ESP-IDF component is to list them manually via SRCS argument to ``idf_component_register``::

  idf_component_register(SRCS library/a.c library/b.c platform/platform.c
                         ...)

This preference reflects the `CMake best practice <https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1/>`_ of manually listing source files. This could, however, be inconvenient when there are lots of source files to add to the build. The ESP-IDF build system provides an alternative way for specifying source files using ``SRC_DIRS``::

  idf_component_register(SRC_DIRS library platform
                         ...)

This uses globbing behind the scenes to find source files in the specified directories. Be aware, however, that if a new source file is added and this method is used, then CMake won't know to automatically re-run and this file won't be added to the build.

The trade-off is acceptable when you're adding the file yourself, because you can trigger a clean build or run ``idf.py reconfigure`` to manually re-run CMake_. However, the problem gets harder when you share your project with others who may check out a new version using a source control tool like Git...

For components which are part of ESP-IDF, we use a third party Git CMake integration module (:idf_file:`/tools/cmake/third_party/GetGitRevisionDescription.cmake`) which automatically re-runs CMake any time the repository commit changes. This means if you check out a new ESP-IDF version, CMake will automatically rerun.

For project components (not part of ESP-IDF), there are a few different options:

- If keeping your project file in Git, ESP-IDF will automatically track the Git revision and re-run CMake if the revision changes.
- If some components are kept in a third git repository (not the project repository or ESP-IDF repository), you can add a call to the ``git_describe`` function in a component CMakeLists file in order to automatically trigger re-runs of CMake when the Git revision changes.
- If not using Git, remember to manually run ``idf.py reconfigure`` whenever a source file may change.
- To avoid this problem entirely, use ``COMPONENT_SRCS`` to list all source files in project components.

The best option will depend on your particular project and its users.

Build System Metadata
=====================

For integration into IDEs and other build systems, when CMake runs the build process generates a number of metadata files in the ``build/`` directory. To regenerate these files, run ``cmake`` or ``idf.py reconfigure`` (or any other ``idf.py`` build command).

- ``compile_commands.json`` is a standard format JSON file which describes every source file which is compiled in the project. A CMake feature generates this file, and many IDEs know how to parse it.
- ``project_description.json`` contains some general information about the ESP-IDF project, configured paths, etc.
- ``flasher_args.json`` contains esptool.py arguments to flash the project's binary files. There are also ``flash_*_args`` files which can be used directly with esptool.py. See `Flash arguments`_.
- ``CMakeCache.txt`` is the CMake cache file which contains other information about the CMake process, toolchain, etc.
- ``config/sdkconfig.json`` is a JSON-formatted version of the project configuration values.
- ``config/kconfig_menus.json`` is a JSON-formatted version of the menus shown in menuconfig, for use in external IDE UIs.

JSON Configuration Server
-------------------------

.. highlight :: json

A tool called ``confserver.py`` is provided to allow IDEs to easily integrate with the configuration system logic. ``confserver.py`` is designed to run in the background and interact with a calling process by reading and writing JSON over process stdin & stdout.

You can run ``confserver.py`` from a project via ``idf.py confserver`` or ``ninja confserver``, or a similar target triggered from a different build generator.

The config server outputs human-readable errors and warnings on stderr and JSON on stdout. On startup, it will output the full values of each configuration item in the system as a JSON dictionary, and the available ranges for values which are range constrained. The same information is contained in ``sdkconfig.json``::

  {"version": 1, "values": { "ITEM": "value", "ITEM_2": 1024, "ITEM_3": false }, "ranges" : { "ITEM_2" : [ 0, 32768 ] } }

Only visible configuration items are sent. Invisible/disabled items can be parsed from the static ``kconfig_menus.json`` file which also contains the menu structure and other metadata (descriptions, types, ranges, etc.)

The Configuration Server will then wait for input from the client. The client passes a request to change one or more values, as a JSON object followed by a newline::

   {"version": "1", "set": {"SOME_NAME": false, "OTHER_NAME": true } }

The Configuration Server will parse this request, update the project ``sdkconfig`` file, and return a full list of changes::

   {"version": 1, "values": {"SOME_NAME": false, "OTHER_NAME": true , "DEPENDS_ON_SOME_NAME": null}}

Items which are now invisible/disabled will return value ``null``. Any item which is newly visible will return its newly visible current value.

If the range of a config item changes, due to conditional range depending on another value, then this is also sent::

   {"version": 1, "values": {"OTHER_NAME": true }, "ranges" : { "HAS_RANGE" : [ 3, 4 ] } }

If invalid data is passed, an "error" field is present on the object::

    {"version": 1, "values": {}, "error": ["The following config symbol(s) were not visible so were not updated: NOT_VISIBLE_ITEM"]}

By default, no config changes are written to the sdkconfig file. Changes are held in memory until a "save" command is sent::

    {"version": 1, "save": null }

To reload the config values from a saved file, discarding any changes in memory, a "load" command can be sent::

    {"version": 1, "load": null }

The value for both "load" and "save" can be a new pathname, or "null" to load/save the previous pathname.

The response to a "load" command is always the full set of config values and ranges, the same as when the server is initially started.

Any combination of "load", "set", and "save" can be sent in a single command and commands are executed in that order. Therefore it's possible to load config from a file, set some config item values and then save to a file in a single command.

.. note:: The configuration server does not automatically load any changes which are applied externally to the ``sdkconfig`` file. Send a "load" command or restart the server if the file is externally edited.

.. note:: The configuration server does not re-run CMake to regenerate other build files or metadata files after ``sdkconfig`` is updated. This will happen automatically the next time ``CMake`` or ``idf.py`` is run.

Build System Internals
=======================

Build Scripts
-------------

The listfiles for the ESP-IDF build system reside in :idf:`/tools/cmake`. The modules which implement core build system functionality are as follows:

    - build.cmake - Build related commands i.e. build initialization, retrieving/setting build properties, build processing.
    - component.cmake - Component related commands i.e. adding components, retrieving/setting component properties, registering components.
    - kconfig.cmake - Generation of configuration files (sdkconfig, sdkconfig.h, sdkconfig.cmake, etc.) from Kconfig files.
    - ldgen.cmake - Generation  of  final linker script from linker fragment files.
    - target.cmake - Setting build target and toolchain file.
    - utilities.cmake - Miscellaneous helper commands.

 Aside from these files, there are two other important CMake scripts in :idf:`/tools/cmake`:

    - idf.cmake - Sets up the build and includes the core modules listed above. Included in CMake projects in order to access ESP-IDF build system functionality.
    - project.cmake - Includes ``idf.cmake`` and provides a custom ``project()`` command that takes care of all the heavy lifting of building an executable. Included in the top-level CMakeLists.txt of standard ESP-IDF projects.

The rest of the files in :idf:`/tools/cmake` are support or third-party scripts used in the build process. 

Build Process
-------------

This section describes the standard ESP-IDF application build process. The build process can be broken down roughly into four phases:

.. blockdiag::
    :scale: 100%
    :caption: ESP-IDF Build System Process
    :align: center
    
    blockdiag idf-build-system-process {
        Initialization -> Enumeration
        Enumeration -> Processing
        Processing -> Finalization
    }

Initialization 
^^^^^^^^^^^^^^
  This phase sets up necessary parameters for the build.

    - Upon inclusion of ``idf.cmake`` in ``project.cmake``, the following steps are performed:
        - Set ``IDF_PATH`` from environment variable or inferred from path to ``project.cmake`` included in the top-level CMakeLists.txt.
        - Add :idf:`/tools/cmake` to ``CMAKE_MODULE_PATH`` and include core modules plus the various helper/third-party scripts.
        - Set build tools/executables such as default Python interpreter, mconf, etc.
        - Get ESP-IDF git revision and store as ``IDF_VER``.
        - Set global build specifications i.e. compile options, compile definitions, include directories for all components in the build.
        - Add components in :idf:`components` to the build.
    - The initial part of the custom ``project()`` command performs the following steps:
        - Set ``IDF_TARGET`` from environment variable or CMake cache and the corresponding ``CMAKE_TOOLCHAIN_FILE`` to be used. 
        - Add components in ``EXTRA_COMPONENTS_DIRS`` to the build.
        - Prepare arguments for calling command ``idf_build_process()`` from variables such as ``COMPONENTS``/``EXCLUDE_COMPONENTS``, ``SDKCONFIG``, ``SDKCONFIG_DEFAULTS``.

  The call to ``idf_build_process()`` command marks the end of this phase.

Enumeration
^^^^^^^^^^^
  This phase builds a final list of components to be processed in the build, and is performed in the first half of ``idf_build_process()``.

    - Retrieve each component's public and private requirements. A child process is created which executes each component's CMakeLists.txt in script mode. The values of ``idf_component_register`` REQUIRES and PRIV_REQUIRES argument is returned to the parent build process. This is called early expansion. The variable ``CMAKE_BUILD_EARLY_EXPANSION`` is defined during this step.
    - Recursively include components based on public and private requirements.

Processing
^^^^^^^^^^
  This phase processes the components in the build, and is the second half of ``idf_build_process()``.

  - Load project configuration from sdkconfig file and generate an sdkconfig.cmake and sdkconfig.h header. These define configuration variables/macros that are accessible from the build scripts and C/C++ source/header files, respectively.
  - Include each component's ``project_include.cmake``.
  - Add each component as a subdirectory, processing its CMakeLists.txt. The component CMakeLists.txt calls the registration command, ``idf_component_register`` which adds source files, include directories, creates component library, links dependencies, etc.

Finalization
^^^^^^^^^^^^
  This phase is everything after ``idf_build_process()``. 
  
  - Create executable and link the component libraries to it.
  - Generate project metadata files such as project_description.json and display relevant information about the project built.


Browse :idf_file:`/tools/cmake/project.cmake` for more details.

Migrating from ESP-IDF GNU Make System
======================================

Some aspects of the CMake-based ESP-IDF build system are very similar to the older GNU Make-based system. For example, to adapt a ``component.mk`` file to ``CMakeLists.txt`` variables like ``COMPONENT_ADD_INCLUDEDIRS`` and ``COMPONENT_SRCDIRS`` can stay the same and the syntax only needs changing to CMake syntax.

Automatic Conversion Tool
-------------------------

.. highlight:: bash

An automatic project conversion tool is available in :idf_file:`/tools/cmake/convert_to_cmake.py`. Run this command line tool with the path to a project like this::

    $IDF_PATH/tools/cmake/convert_to_cmake.py /path/to/project_dir

The project directory must contain a Makefile, and GNU Make (``make``) must be installed and available on the PATH.

The tool will convert the project Makefile and any component ``component.mk`` files to their equivalent ``CMakeLists.txt`` files.

It does so by running ``make`` to expand the ESP-IDF build system variables which are set by the build, and then producing equivalent CMakelists files to set the same variables.

The conversion tool is not capable of dealing with complex Makefile logic or unusual targets. These will need to be converted by hand.

No Longer Available in CMake
----------------------------

Some features are significantly different or removed in the CMake-based system. The following variables no longer exist in the CMake-based build system:

- ``COMPONENT_BUILD_DIR``: Use ``CMAKE_CURRENT_BINARY_DIR`` instead.
- ``COMPONENT_LIBRARY``: Defaulted to ``$(COMPONENT_NAME).a``, but the library name could be overriden by the component. The name of the component library can no longer be overriden by the component.
- ``CC``, ``LD``, ``AR``, ``OBJCOPY``: Full paths to each tool from the gcc xtensa cross-toolchain. Use ``CMAKE_C_COMPILER``, ``CMAKE_C_LINK_EXECUTABLE``, ``CMAKE_OBJCOPY``, etc instead. `Full list here <cmake language variables_>`_.
- ``HOSTCC``, ``HOSTLD``, ``HOSTAR``: Full names of each tool from the host native toolchain. These are no longer provided, external projects should detect any required host toolchain manually.
- ``COMPONENT_ADD_LDFLAGS``: Used to override linker flags. Use the CMake `target_link_libraries`_ command instead.
- ``COMPONENT_ADD_LINKER_DEPS``: List of files that linking should depend on. `target_link_libraries`_ will usually infer these dependencies automatically. For linker scripts, use the provided custom CMake function ``target_linker_scripts``.
- ``COMPONENT_SUBMODULES``: No longer used, the build system will automatically enumerate all submodules in the ESP-IDF repository.
- ``COMPONENT_EXTRA_INCLUDES``: Used to be an alternative to ``COMPONENT_PRIV_INCLUDEDIRS`` for absolute paths. Use ``COMPONENT_PRIV_INCLUDEDIRS`` for all cases now (can be relative or absolute).
- ``COMPONENT_OBJS``: Previously, component sources could be specified as a list of object files. Now they can be specified as an list of source files via ``COMPONENT_SRCS``.
- ``COMPONENT_OBJEXCLUDE``: Has been replaced with ``COMPONENT_SRCEXCLUDE``. Specify source files (as absolute paths or relative to component directory), instead.
- ``COMPONENT_EXTRA_CLEAN``: Set property ``ADDITIONAL_MAKE_CLEAN_FILES`` instead but note :ref:`CMake has some restrictions around this functionality <ADDITIONAL_MAKE_CLEAN_FILES_note>`.
- ``COMPONENT_OWNBUILDTARGET`` & ``COMPONENT_OWNCLEANTARGET``: Use CMake `ExternalProject`_ instead. See :ref:`component-build-full-override` for full details.
- ``COMPONENT_CONFIG_ONLY``: Call ``register_config_only_component()`` instead. See `Configuration-Only Components`_.
- ``CFLAGS``, ``CPPFLAGS``, ``CXXFLAGS``: Use equivalent CMake commands instead. See `Controlling Component Compilation`_.

No Default Values
-----------------

The following variables no longer have default values:

- ``COMPONENT_SRCDIRS``
- ``COMPONENT_ADD_INCLUDEDIRS``

No Longer Necessary
-------------------

It is no longer necessary to set ``COMPONENT_SRCDIRS`` if setting ``COMPONENT_SRCS`` (in fact, in the CMake-based system ``COMPONENT_SRCS`` is ignored if ``COMPONENT_SRCDIRS`` is set).

Flashing from make
------------------

``make flash`` and similar targets still work to build and flash. However, project ``sdkconfig`` no longer specifies serial port and baud rate. Environment variables can be used to override these. See :ref:`flash-with-ninja-or-make` for more details.

.. _esp-idf-template: https://github.com/espressif/esp-idf-template
.. _cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool.py: https://github.com/espressif/esptool/#readme
.. _CMake v3.5 documentation: https://cmake.org/cmake/help/v3.5/index.html
.. _cmake command line documentation: https://cmake.org/cmake/help/v3.5/manual/cmake.1.html#options
.. _cmake add_library: https://cmake.org/cmake/help/v3.5/command/add_library.html
.. _cmake if: https://cmake.org/cmake/help/v3.5/command/if.html
.. _cmake list: https://cmake.org/cmake/help/v3.5/command/list.html
.. _cmake project: https://cmake.org/cmake/help/v3.5/command/project.html
.. _cmake set: https://cmake.org/cmake/help/v3.5/command/set.html
.. _cmake string: https://cmake.org/cmake/help/v3.5/command/string.html
.. _cmake faq generated files: https://cmake.org/Wiki/CMake_FAQ#How_can_I_generate_a_source_file_during_the_build.3F
.. _ADDITIONAL_MAKE_CLEAN_FILES: https://cmake.org/cmake/help/v3.5/prop_dir/ADDITIONAL_MAKE_CLEAN_FILES.html
.. _ExternalProject: https://cmake.org/cmake/help/v3.5/module/ExternalProject.html
.. _cmake language variables: https://cmake.org/cmake/help/v3.5/manual/cmake-variables.7.html#variables-for-languages
.. _set_source_files_properties: https://cmake.org/cmake/help/v3.5/command/set_source_files_properties.html
.. _target_compile_options: https://cmake.org/cmake/help/v3.5/command/target_compile_options.html
.. _target_link_libraries: https://cmake.org/cmake/help/v3.5/command/target_link_libraries.html#command:target_link_libraries
.. _cmake_toolchain_file: https://cmake.org/cmake/help/v3.5/variable/CMAKE_TOOLCHAIN_FILE.html
.. _quirc: https://github.com/dlbeer/quirc
.. _pyenv: https://github.com/pyenv/pyenv#README
.. _virtualenv: https://virtualenv.pypa.io/en/stable/
