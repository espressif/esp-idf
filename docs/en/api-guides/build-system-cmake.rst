Build System (CMake)
********************

.. include:: ../cmake-warning.rst

.. include:: ../cmake-pending-features.rst

This document explains the implementation of the CMake-based ESP-IDF build system and the concept of "components". :doc:`Documentation for the GNU Make based build system <build-system>` is also available

Read this document if you want to know how to organise and build a new ESP-IDF project or component using the CMake-based build system.


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

The :ref:`getting started guide <get-started-configure-cmake>` contains a brief introduction to how to set up ``idf.py`` to configure, build, and flash projects.

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
- ``idf.py reconfigure`` re-runs CMake_ even if it doesn't seem to need re-running. This isn't necessary during normal usage, but can be useful after adding/removing files from the source tree.
- ``idf.py flash`` will automatically build the project if necessary, and then flash it to an ESP32. The ``-p`` and ``-b`` options can be used to set serial port name and flasher baud rate, respectively.
- ``idf.py monitor`` will display serial output from the ESP32. The ``-p`` option can be used to set the serial port name. Type ``Ctrl-]`` to exit the monitor. See :doc:`/get-started/idf-monitor` for more details about using the monitor.

Multiple ``idf.py`` commands can be combined into one. For example, ``idf.py -p COM4 clean flash monitor`` will clean the source tree, then build the project and flash it to the ESP32 before running the serial monitor.

.. note:: The environment variables ``ESPPORT`` and ``ESPBAUD`` can be used to set default values for the ``-p`` and ``-b`` options, respectively. Providing these options on the command line overrides the default.

Advanced Commands
^^^^^^^^^^^^^^^^^

- ``idf.py app``, ``idf.py bootloader``, ``idf.py partition_table`` can be used to build only the app, bootloader, or partition table from the project as applicable.
- There are matching commands ``idf.py app-flash``, etc. to flash only that single part of the project to the ESP32.
- ``idf.py -p PORT erase_flash`` will use esptool.py to erase the ESP32's entire flash chip.
- ``idf.py size`` prints some size information about the app. ``size-components`` and ``size-files`` are similar commands which print more detailed per-component or per-source-file information, respectively.

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

Using CMake in an IDE
---------------------

You can also use an IDE with CMake integration. The IDE will want to know the path to the project's ``CMakeLists.txt`` file. IDEs with CMake integration often provide their own build tools (CMake calls these "generators") to build the source files as part of the IDE.

When adding custom non-build steps like "flash" to the IDE, it is recommended to execute ``idf.py`` for these "special" commands.

For more detailed information about integrating ESP-IDF with CMake into an IDE, see `Build System Metadata`_.

.. setting-python-interpreter:

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

- A top-level project CMakeLists.txt file. This is the primary file which CMake uses to learn how to build the project. The project CMakeLists.txt file sets the ``MAIN_SRCS`` variable which lists all of the source files in the "main" directory (part of this project's executable). It may set other project-wide CMake variables, as well. Then it includes the file :idf_file:`/tools/cmake/project.cmake` which
  implements the rest of the build system. Finally, it sets the project name and defines the project.

- "sdkconfig" project configuration file. This file is created/updated when ``idf.py menuconfig`` runs, and holds configuration for all of the components in the project (including ESP-IDF itself). The "sdkconfig" file may or may not be added to the source control system of the project.

- Optional "components" directory contains components that are part of the project. A project does not have to contain custom components of this kind, but it can be useful for structuring reusable code or including third party components that aren't part of ESP-IDF.

- "main" directory is a special "by convention" directory that contains source code for the project executable itself. These source files are listed in the project's CMakeLists file. You don't need to name this directory "main", but we recommend you follow this convention. If you have a lot of source files in your project, we recommend grouping most into components instead of putting them all in "main".

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

      set(MAIN_SRCS main/src1.c main/src2.c)

      include($ENV{IDF_PATH}/tools/cmake/project.cmake)
      project(myProject)


.. _project-mandatory-parts:

Mandatory Parts
---------------

The inclusion of these four lines, in the order shown above, is necessary for every project:

- ``cmake_minimum_required(VERSION 3.5)`` tells CMake what version is required to build the project. ESP-IDF is designed to work with CMake 3.5 or newer. This line must be the first line in the CMakeLists.txt file.
- ``set(MAIN_SRCS xxx)`` sets a variable - ``MAIN_SRCS`` to be a list of the "main" source files in the project. Paths are relative to the CMakeLists. They don't specifically need to be under the "main" sub-directory, but this structure is encouraged.

  It is *strongly recommended not to add a lot of files to the MAIN_SRCS list*. If you have a lot of source files then it recommended to organise them functionally into individual components under the project "components" directory. This will make your project more maintainable, reusable, and easier to configure. Components are further explained below.

  ``MAIN_SRCS`` must name at least one source file (although that file doesn't need to necessarily include an ``app_main()`` function or anything else).
- ``include($ENV{IDF_PATH}/tools/cmake/project.cmake)`` pulls in the rest of the CMake functionality to configure the project, discover all the components, etc.
- ``project(myProject)`` creates the project itself, and specifies the project name. The project name is used for the final binary output files of the app - ie ``myProject.elf``, ``myProject.bin``. Only one project can be defined per CMakeLists file.


Optional Project Variables
--------------------------

These variables all have default values that can be overridden for custom behaviour. Look in :idf_file:`/tools/cmake/project.cmake` for all of the implementation details.

- ``COMPONENT_DIRS``: Directories to search for components. Defaults to `${IDF_PATH}/components`, `${PROJECT_PATH}/components`, and ``EXTRA_COMPONENT_DIRS``. Override this variable if you don't want to search for components in these places.
- ``EXTRA_COMPONENT_DIRS``: Optional list of additional directories to search for components. Paths can be relative to the project directory, or absolute.
- ``COMPONENTS``: A list of component names to build into the project. Defaults to all components found in the ``COMPONENT_DIRS`` directories. Use this variable to "trim down" the project for faster build times. Note that any component which "requires" another component via ``COMPONENT_REQUIRES`` will automatically have it added to this list, so the ``COMPONENTS`` list can be very short.
- ``COMPONENT_REQUIRES_COMMON``: A list of components that every component requires. These components are automatically added to every component's ``COMPONENT_PRIV_REQUIRES`` list and also the project's ``COMPONENTS`` list. By default, this variable is set to the minimal set of core "system" components needed for any ESP-IDF project. Usually, you would not change this variable in your project.

Any paths in these variables can be absolute paths, or set relative to the project directory.

To set these variables, use the `cmake set command <cmake set_>`_ ie ``set(VARIABLE "VALUE")``. The ``set()`` commands should be placed after the ``cmake_minimum(...)`` line but before the ``include(...)`` line.

.. _component-directories-cmake:

Component CMakeLists Files
==========================

Each project contains one or more components. Components can be part of ESP-IDF, part of the project's own components directory, or added from custom component directories (:ref:`see above <component-directories-cmake>`).

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

Minimal Component CMakeLists
----------------------------

.. highlight:: cmake

The minimal component ``CMakeLists.txt`` file is as follows::

  set(COMPONENT_SRCDIRS ".")
  set(COMPONENT_ADD_INCLUDEDIRS "include")
  register_component()

- ``COMPONENT_SRCDIRS`` is a (space-separated) list of directories to search for source files. Source files (``*.c``, ``*.cpp``, ``*.cc``, ``*.S``) in these directories will be compiled into the component library.
- ``COMPONENT_ADD_INCLUDEDIRS`` is a (space-separated) list of directories to add to the global include search path for any component which requires this component, and also the main source files.
- ``register_component()`` is required to add the component (using the variables set above) to the build. A library with the name of the component will be built and linked into the final app. If this step is skipped (perhaps due to use of a CMake `if function <cmake if_>`_ or similar), this component will not be part of the build.

Directories are usually specified relative to the ``CMakeLists.txt`` file itself, although they can be absolute.

See `example component CMakeLists`_ for more complete component ``CMakeLists.txt`` examples.

.. _component variables:

Preset Component Variables
--------------------------

The following component-specific variables are available for use inside component CMakeLists, but should not be modified:

- ``COMPONENT_PATH``: The component directory. Evaluates to the absolute path of the directory containing ``component.mk``. The component path cannot contain spaces. This is the same as the ``CMAKE_CURRENT_SOURCE_DIR`` variable.
- ``COMPONENT_NAME``: Name of the component. Same as the name of the component directory.

The following variables are set at the project level, but available for use in component CMakeLists:

- ``PROJECT_NAME``: Name of the project, as set in project Makefile
- ``PROJECT_PATH``: Absolute path of the project directory containing the project Makefile. Same as the ``CMAKE_SOURCE_DIR`` variable.
- ``COMPONENTS``: Names of all components that are included in this build, formatted as a semicolon-delimited CMake list.
- ``CONFIG_*``: Each value in the project configuration has a corresponding variable available in make. All names begin with ``CONFIG_``. :doc:`More information here </api-reference/kconfig>`.
- ``IDF_VER``: Git version of ESP-IDF (produced by ``git describe``)

If you modify any of these variables inside ``CMakeLists.txt`` then this will not prevent other components from building but it may make your component hard to build and/or debug.

- ``COMPONENT_ADD_INCLUDEDIRS``: Paths, relative to the component directory, which will be added to the include search path for
  all other components which require this one. If an include directory is only needed to compile this specific component,
  add it to ``COMPONENT_PRIV_INCLUDEDIRS`` instead.
- ``COMPONENT_REQUIRES`` is a (space-separated) list of components that are required to include this project's header files into other components. If this component has a header file in a ``COMPONENT_ADD_INCLUDEDIRS`` directory that includes a header from another component, that component should be listed in ``COMPONENT_REQUIRES``. Requirements are recursive.

  The ``COMPONENT_REQUIRES`` list can be empty because some very common components (like newlib for libc, freertos for RTOS functions, etc) are always required by all components. This list is found in the project-level variable ``COMPONENT_REQUIRES_COMMON``.

  If a component only requires another component's headers to compile its source files (not for including this component's headers), then these components should be listed in ``COMPONENT_PRIV_REQUIRES`` instead.

  See `Component Requirements`_ for more details.

Optional Component-Specific Variables
-------------------------------------

The following variables can be set inside ``component.mk`` to control the build of that component:

- ``COMPONENT_PRIV_INCLUDEDIRS``: Directory paths, must be relative to
  the component directory, which will be added to the include search
  path for this component's source files only.
- ``COMPONENT_PRIV_REQUIRES`` is a (space-separated) list of components that are required to either compile or link this component's source files. These components' header paths do not propagate to other components which require it, they are only used to compile this component's sources. See `Component Requirements`_ for more details.
- ``COMPONENT_SRCDIRS``: Directory paths, must be relative to the component directory, which will be searched for source files (``*.cpp``,
  ``*.c``, ``*.S``). Set this to specify a list of directories which contain source files.
- ``COMPONENT_SRCS``: Paths to individual source files to compile. Setting this causes ``COMPONENT_SRCDIRS`` to be ignored. Setting this variable instead gives you finer grained control over which files are compiled.
  If you don't set ``COMPONENT_SRCDIRS`` or ``COMPONENT_SRCS``, your component won't compile a library but it may still add include paths for use when compiling other components or the source files listed in ``MAIN_SRCS``.
- ``COMPONENT_SRCEXCLUDE``: Paths to source files to exclude from component. Can be set in conjunction with ``COMPONENT_SRCDIRS`` if there is a directory with a large number of source files to include in the component but one or more source files which should not be. Paths can be specified relative to the component directory or absolute.

Controlling Component Compilation
---------------------------------

.. highlight:: cmake

To pass compiler options when compiling source files belonging to a particular component, use the ``component_compile_options`` function::

  component_compile_options(-Wno-unused-variable)

This is a wrapper around the CMake `target_compile_options`_ command.

To apply the compilation flags to a single source file, use the CMake `set_source_files_properties`_ command::

    set_source_files_properties(mysrc.c
        PROPERTIES COMPILE_FLAGS
        -Wno-unused-variable
    )

This can be useful if there is upstream code that emits warnings.

When using these commands, place them after the ``register_component()`` line in the component CMakeLists file.

.. _component-configuration-cmake:

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

- ``ESP_PLATFORM`` — Can be used to detect that build happens within ESP-IDF.
- ``IDF_VER`` — Defined to a git version string.  E.g. ``v2.0`` for a tagged release or ``v1.0-275-g0efaa4f`` for an arbitrary commit.

Component Requirements
======================

When compiling each component, the ESP-IDF build system recursively evaluates its components.

Each component's source file is compiled with these include path directories:

- The current component's ``COMPONENT_ADD_INCLUDEDIRS`` and ``COMPONENT_PRIV_INCLUDEDIRS``.
- The ``COMPONENT_ADD_INCLUDEDIRS`` set by all components in the current component's ``COMPONENT_REQUIRES`` and ``COMPONENT_PRIV_REQUIRES`` variables (ie all the current component's public and private dependencies).
- All of the ``COMPONENT_REQUIRES`` of those components, evaluated recursively (ie all public dependencies of this component's dependencies, recursively expanded).

When writing a component
------------------------

- ``COMPONENT_REQUIRES`` should be set to all components whose header files are #included from the *public* header files of this component.
- ``COMPONENT_PRIV_REQUIRES`` should be set to all components whose header files are #included from *any source files* of this component, unless already listed in ``COMPONENT_REQUIRES``. Or any component which is required to be linked in order for this component to function correctly.
- ``COMPONENT_REQUIRES`` and/or ``COMPONENT_PRIV_REQUIRES`` should be set before calling ``register_component()``.
- The values of ``COMPONENT_REQUIRES`` and ``COMPONENT_PRIV_REQUIRES`` should not depend on any configuration choices (``CONFIG_xxx`` macros). This is because requirements are expanded before configuration is loaded. Other component variables (like include paths or source files) can depend on configuration choices.
- Not setting either or both ``REQUIRES`` variables is fine. If the component has no requirements except for the "common" components needed for RTOS, libc, etc (``COMPONENT_REQUIRES_COMMON``) then both variables can be empty or unset.

When creating a project
-----------------------

- By default, every component is included in the build.
- If you set the ``COMPONENTS`` variable to a minimal list of components used directly by your project, then the build will include:
  - Components mentioned explicitly in ``COMPONENTS``.
  - Those components' requirements (evaluated recursively).
  - The "common" components that every component depends on.
- Setting ``COMPONENTS`` to the minimal list of required components can significantly reduce compile times.
- When compiling the project's source files (``MAIN_SRCS``), the public header directories (``COMPONENT_ADD_INCLUDEDIRS`` list) of all components included in the build are available.

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
- Order that component object archives are passed to the linker (note that the build system also passes ``--start-group`` and ``--end-group`` to the linker to allow cycles in linker dependencies, however the basic order is determined by ``BUILD_COMPONENTS``.


Build Process Internals
=======================

For full details about CMake_ and CMake commands, see the `CMake v3.5 documentation`_.

project.cmake contents
----------------------

When included from a project CMakeLists file, the ``project.cmake`` file defines some utility modules and global variables and then sets ``IDF_PATH`` if it was not set in the system environment.

It also defines an overridden custom version of the built-in CMake_ ``project`` function. This function is overridden to add all of the ESP-IDF specific project functionality.

project function
----------------

The custom ``project()`` function performs the following steps:

- Evaluates component dependencies and builds the ``BUILD_COMPONENTS`` list of components to include in the build (see :ref:`above<component-requirements-implementation>`).
- Finds all components in the project (searching ``COMPONENT_DIRS`` and filtering by ``COMPONENTS`` if this is set).
- Loads the project configuration from the ``sdkconfig`` file and generates a ``sdkconfig.cmake`` file and a ``sdkconfig.h`` header. These define configuration values in CMake and C/C++, respectively. If the project configuration changes, cmake will automatically be re-run to re-generate these files and re-configure the project.
- Sets the `CMAKE_TOOLCHAIN_FILE`_ variable to the ESP-IDF toolchain file with the Xtensa ESP32 toolchain.
- Declare the actual cmake-level project by calling the `CMake project function <cmake project_>`_.
- Load the git version. This includes some magic which will automatically re-run CMake if a new revision is checked out in git. See `File Globbing & Incremental Builds`_.
- Include :ref:`project_include.cmake` files from any components which have them.
- Add each component to the build. Each component CMakeLists file calls ``register_component``, calls the CMake `add_library <cmake add_library_>`_ function to add a library and then adds source files, compile options, etc.
- Add the final app executable to the build.
- Go back and add inter-component dependencies between components (ie adding the public header directories of each component to each other component).

Browse the :idf_file:`/tools/cmake/project.cmake` file and supporting functions in :idf_file:`/tools/cmake/idf_functions.cmake` for more details.

Debugging CMake
---------------

Some tips for debugging the ESP-IDF CMake-based build system:

- When CMake runs, it prints quite a lot of diagnostic information including lists of components and component paths.
- Running ``cmake -DDEBUG=1`` will produce more verbose diagnostic output from the IDF build system.
- Running ``cmake`` with the ``--trace`` or ``--trace-expand`` options will give a lot of information about control flow. See the `cmake command line documentation`_.

.. _warn-undefined-variables-cmake:

Warning On Undefined Variables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, ``idf.py`` passes the ``--warn-uninitialized`` flag to CMake_ so it will print a warning if an undefined variable is referenced in the build. This can be very useful to find buggy CMake files.

If you don't want this behaviour, it can be disabled by passing ``--no-warnings`` to ``idf.py``.

Overriding Parts of the Project
-------------------------------

.. _project_include.cmake:

project_include.cmake
^^^^^^^^^^^^^^^^^^^^^

For components that have build requirements which must be evaluated before any component CMakeLists
files are evaluated, you can create a file called ``project_include.cmake`` in the
component directory. This CMake file is included when ``project.cmake`` is evaluating the entire project.

``project_include.cmake`` files are used inside ESP-IDF, for defining project-wide build features such as ``esptool.py`` command line arguments and the ``bootloader`` "special app".

Unlike component ``CMakeLists.txt`` files, when including a ``project_include.cmake`` file the current source directory (``CMAKE_CURRENT_SOURCE_DIR`` and working directory) is the project directory. Use the variable ``COMPONENT_PATH`` for the absolute directory of the component.

Note that ``project_include.cmake`` isn't necessary for the most common component uses - such as adding include directories to the project, or ``LDFLAGS`` to the final linking step. These values can be customised via the ``CMakeLists.txt`` file itself. See `Optional Project Variables`_ for details.

``project_include.cmake`` files are included in the order given in ``BUILD_COMPONENTS`` variable (as logged by CMake). This means that a component's ``project_include.cmake`` file will be included after it's all dependencies' ``project_include.cmake`` files, unless both components are part of a dependency cycle. This is important if a ``project_include.cmake`` file relies on variables set by another component. See also :ref:`above<component-requirements-implementation>`.

Take great care when setting variables or targets in a ``project_include.cmake`` file. As the values are included into the top-level project CMake pass, they can influence or break functionality across all components!

KConfig.projbuild
^^^^^^^^^^^^^^^^^

This is an equivalent to ``project_include.cmake`` for :ref:`component-configuration-cmake` KConfig files. If you want to include
configuration options at the top-level of menuconfig, rather than inside the "Component Configuration" sub-menu, then these can be defined in the KConfig.projbuild file alongside the ``CMakeLists.txt`` file.

Take care when adding configuration values in this file, as they will be included across the entire project configuration. Where possible, it's generally better to create a KConfig file for :ref:`component-configuration-cmake`.


Configuration-Only Components
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Special components which contain no source files, only ``Kconfig.projbuild`` and ``KConfig``, can have a one-line ``CMakeLists.txt`` file which calls the function ``register_config_only_component()``. This function will include the component in the project build, but no library will be built *and* no header files will be added to any include paths.

If a CMakeLists.txt file doesn't call ``register_component()`` or ``register_config_only_component()``, it will be excluded from the project entirely. This may sometimes be desirable, depending on the project configuration.

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
    endif(CONFIG_FOO_ENABLE_BAR)

This example makes use of the CMake `if function <cmake if_>`_ and `list APPEND <cmake list_>`_ function.

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


Source Code Generation
----------------------

Some components will have a situation where a source file isn't
supplied with the component itself but has to be generated from
another file. Say our component has a header file that consists of the
converted binary data of a BMP file, converted using a hypothetical
tool called bmp2h. The header file is then included in as C source
file called graphics_lib.c::

    add_custom_command(OUTPUT logo.h
         COMMAND bmp2h -i ${COMPONENT_PATH}/logo.bmp -o log.h
         DEPENDS ${COMPONENT_PATH}/logo.bmp
         VERBATIM)

    add_custom_target(logo DEPENDS logo.h)
    add_dependencies(${COMPONENT_NAME} logo)

    set_property(DIRECTORY "${COMPONENT_PATH}" APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES logo.h)

This answer is adapted from the `CMake FAQ entry <cmake faq generated files_>`_, which contains some other examples that will also work with ESP-IDF builds.

In this example, logo.h will be generated in the
current directory (the build directory) while logo.bmp comes with the
component and resides under the component path. Because logo.h is a
generated file, it should be cleaned when the project is cleaned. For this reason
it is added to the `ADDITIONAL_MAKE_CLEAN_FILES`_ property.

.. note::

   If generating files as part of the project CMakeLists.txt file, not a component CMakeLists.txt, then use ``${PROJECT_PATH}`` instead of ``${COMPONENT_PATH}`` and ``${PROJECT_NAME}.elf`` instead of ``${COMPONENT_NAME}``.)

If a a source file from another component included ``logo.h``, then ``add_dependencies`` would need to be called to add a dependency between
the two components, to ensure that the component source files were always compiled in the correct order.

Embedding Binary Data
---------------------

Sometimes you have a file with some binary or text data that you'd like to make available to your component - but you don't want to reformat the file as C source.

You can set a variable ``COMPONENT_EMBED_FILES`` in your component's CMakeLists, giving space-delimited names of the files to embed::

  set(COMPONENT_EMBED_FILES server_root_cert.der)

Or if the file is a string, you can use the variable ``COMPONENT_EMBED_TXTFILES``. This will embed the contents of the text file as a null-terminated string::

  set(COMPONENT_EMBED_TXTFILES server_root_cert.pem)

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
      PREFIX ${COMPONENT_PATH}
      SOURCE_DIR ${COMPONENT_PATH}/quirc
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
        ${COMPONENT_PATH}/quirc/libquirc.a)
   set_target_properties(quirc PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        ${COMPONENT_PATH}/quirc/lib)

   set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
        "${COMPONENT_PATH}/quirc/libquirc.a")

(The above CMakeLists.txt can be used to create a component named ``quirc`` that builds the quirc_ project using its own Makefile.)

- ``externalproject_add`` defines an external build system.
  
  - ``SOURCE_DIR``, ``CONFIGURE_COMMAND``, ``BUILD_COMMAND`` and ``INSTALL_COMMAND`` should always be set. ``CONFIGURE_COMMAND`` can be set to an empty string if the build system has no "configure" step. ``INSTALL_COMMAND`` will generally be empty for ESP-IDF builds.
  - Setting ``BUILD_IN_SOURCE`` means the build directory is the same as the source directory. Otherwise you can set ``BUILD_DIR``.
  - Consult the ExternalProject_ documentation for more details about ``externalproject_add()``

- The second set of commands adds a library target, which points to the "imported" library file built by the external system. Some properties need to be set in order to add include directories and tell CMake where this file is.
- Finally, the generated library is added to `ADDITIONAL_MAKE_CLEAN_FILES`_. This means ``make clean`` will delete this library. (Note that the other object files from the build won't be deleted.)

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

.. _custom-sdkconfig-defaults-cmake:

Custom sdkconfig defaults
=========================

For example projects or other projects where you don't want to specify a full sdkconfig configuration, but you do want to override some key values from the ESP-IDF defaults, it is possible to create a file ``sdkconfig.defaults`` in the project directory. This file will be used when creating a new config from scratch, or when any new config value hasn't yet been set in the ``sdkconfig`` file.

To override the name of this file, set the ``SDKCONFIG_DEFAULTS`` environment variable.


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


File Globbing & Incremental Builds
==================================

.. highlight:: cmake

The preferred way to include source files in an ESP-IDF component is to set ``COMPONENT_SRC_DIRS``::

  set(COMPONENT_SRCDIRS library platform)

The build system will automatically find (via "file globbing") all source files in this directory. Alternatively, files can be specified individually::

  set(COMPONENT_SRCS library/a.c library/b.c platform/platform.c)

CMake_ recommends always to name all files individually (ie ``COMPONENT_SRCS``). This is because CMake is automatically re-run whenever a CMakeLists file changes. If a new source file is added and file globbing is used, then CMake won't know to automatically re-run and this file won't be added to the build.

The trade-off is acceptable when you're adding the file yourself, because you can trigger a clean build or run ``idf.py reconfigure`` to manually re-run CMake_. However, the problem gets harder when you share your project with others who may check out a new version using a source control tool like Git...

For components which are part of ESP-IDF, we use a third party Git CMake integration module (:idf_file:`/tools/cmake/third_party/GetGitRevisionDescription.cmake`) which automatically re-runs CMake any time the repository commit changes. This means if you check out a new ESP-IDF version, CMake will automatically rerun.

For project CMakeLists files, ``MAIN_SRCS`` is a list of source files. Therefore if a new file is added, CMakeLists will change and this triggers a re-run of CMake. (This is the approach which CMake_ recommends.)

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

.. _gnu-make-to-cmake:

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

'main' is no longer a component
-------------------------------

In the GNU Make build system ``main`` is a component with a ``component.mk`` file like other components.

Due to CMake requirements for building executables, ``main`` source files are now linked directly into the final binary. The source files in ``main`` must be listed in the ``MAIN_SRCS`` variable (see :ref:`project mandatory variables <project-mandatory-parts>` for more details). At least one source file has to be listed here (although it doesn't need to contain anything in particular).

In general, it's better not to have too many source files in ``MAIN_SRCS``. If you find that you are adding many source files here, see if you can reorganize and group some into project components (see the :ref:`example project structure <example-project-structure>`, above).

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

It is no longer necessary to set ``COMPONENT_SRCDIRS`` if setting ``COMPONENT_SRCS`` (in fact, in the CMake-based system ``COMPONENT_SRCDIRS`` is ignored if ``COMPONENT_SRCS`` is set).


.. _esp-idf-template: https://github.com/espressif/esp-idf-template
.. _cmake: https://cmake.org
.. _ninja: https://ninja-build.org
.. _esptool.py: https://github.com/espressif/esptool/#readme
.. _CMake v3.5 documentation: https://cmake.org/cmake/help/v3.5/index.html
.. _cmake command line documentation: https://cmake.org/cmake/help/v3.5/manual/cmake.1.html#options
.. _cmake add_library: https://cmake.org/cmake/help/v3.5/command/project.html
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

