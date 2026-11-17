Creating a New Project
**********************

This page shows how to create a new ESP-IDF project that uses Build System v2. A v2 project has the same layout as a v1 project; only the top-level ``CMakeLists.txt`` differs. To convert an existing v1 project to v2, see :doc:`updating-project`.

Project Structure
=================

A project is a directory that contains a top-level ``CMakeLists.txt``, a ``main`` component, and optionally a ``components`` directory with additional components. The minimal ``hello_world`` project looks like this:

.. code-block:: text

    hello_world
    ├── CMakeLists.txt
    └── main
        ├── CMakeLists.txt
        └── hello_world_main.c

The top-level ``CMakeLists.txt`` configures the build system and defines the application. The ``main`` component holds the application's entry point and is built and linked automatically. The complete project is available as :example:`get-started/hello_world <build_system/cmakev2/get-started/hello_world>`.

The Project CMakeLists.txt
==========================

The following minimal top-level ``CMakeLists.txt`` is sufficient for most projects:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
    project(hello_world C CXX ASM)
    idf_project_default()

The order of these commands is important:

#. ``cmake_minimum_required`` sets the minimum required CMake version. It must come first.
#. ``include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)`` loads the build system. It performs the build system initialization and toolchain configuration that must be completed before CMake's ``project()`` command. This line is what selects v2; a v1 project includes ``tools/cmake/project.cmake`` instead.
#. ``project(<name> C CXX ASM)`` runs CMake's project setup: it sets project-related variables and initializes the toolchain for the listed languages. ESP-IDF sources use C, C++, and assembly, so all three must be listed; omitting one leaves CMake with no toolchain for that language and the build fails. The project name becomes the application and binary image name.
#. :cmakev2:ref:`idf_project_default` builds the default application from the ``main`` component and its transitive dependencies, generates the binary image, and adds the usual targets such as ``flash`` and ``menuconfig``.

For finer control over what is built, for example to produce more than one binary, call the lower-level functions instead of :cmakev2:ref:`idf_project_default`; see :doc:`multiple-binaries` and :doc:`idf-as-library`. The full build flow is described in :doc:`design`.

The main Component
==================

The application's entry point lives in the ``main`` component. :cmakev2:ref:`idf_project_default` builds the application from ``main`` and its dependencies, which is why a project that uses ``idf_project_default`` has a ``main`` component. The build system itself does not require a component named ``main``; this is a convention of ``idf_project_default``, and a project that drives the build with the lower-level API can build its application from any component (see :doc:`multiple-binaries` and :doc:`idf-as-library`). In ``hello_world``, ``main`` registers a single source file and a private dependency:

.. code-block:: cmake
    :caption: main/CMakeLists.txt

    idf_component_register(SRCS "hello_world_main.c"
                           PRIV_REQUIRES spi_flash
                           INCLUDE_DIRS "")

This is the recommended way to declare a component, and it works under both v1 and v2. For details on writing components, see :doc:`creating-component`.

Building and Flashing
=====================

A v2 project is built with ``idf.py``, exactly as a v1 project:

.. code-block:: bash

    idf.py set-target <target>
    idf.py build
    idf.py flash monitor

The available actions (``build``, ``flash``, ``monitor``, ``menuconfig``, ``size``, and others) are the same as for v1 and are described in :doc:`/api-guides/tools/idf-py`.
