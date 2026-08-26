Using ESP-IDF as a Library
**************************

In the usual project, :cmakev2:ref:`idf_project_default` configures everything and builds the application from the ``main`` component. A standard CMake project can instead drive the ESP-IDF build itself with the lower-level API: it initializes the build system, bundles the required ESP-IDF components into a single library with :cmakev2:ref:`idf_build_library`, and links that library into its own executable. This is useful for integrating ESP-IDF into an existing CMake project, or for building host and target binaries from the same sources. The :example:`idf_as_lib example <build_system/cmakev2/features/idf_as_lib>` shows the complete pattern.

Driving the Build
=================

The project's ``CMakeLists.txt`` includes ``idf.cmake``, runs ``project()``, and creates its own executable, then uses the lower-level API to attach ESP-IDF to it:

.. code-block:: cmake
    :caption: CMakeLists.txt

    cmake_minimum_required(VERSION 3.22)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
    project(my_app C CXX ASM)

    add_executable(my_app.elf main.c)

    # Initialize the build system and bundle the needed components.
    idf_project_init()
    idf_build_library(idf_components COMPONENTS spi_flash esp_system)

    # Apply the include paths, definitions, and options that ESP-IDF
    # normally adds, then link the bundled library.
    idf_build_get_property(include_directories INCLUDE_DIRECTORIES GENERATOR_EXPRESSION)
    target_include_directories(my_app.elf PRIVATE "${include_directories}")
    idf_build_get_property(compile_definitions COMPILE_DEFINITIONS GENERATOR_EXPRESSION)
    target_compile_definitions(my_app.elf PRIVATE "${compile_definitions}")
    idf_build_get_compile_options(compile_options)
    target_compile_options(my_app.elf PRIVATE "${compile_options}")

    target_link_libraries(my_app.elf PRIVATE idf_components)

The key functions are:

:cmakev2:ref:`idf_project_init`
    Initialize the build system after ``project()``: discover components, generate the configuration, and set up the compile and link options. It must be called before the functions below.

:cmakev2:ref:`idf_build_library`
    Bundle the listed components and their transitive dependencies into a single interface library, here named ``idf_components``. Linking this library into an executable brings in the components' code, include paths, and linker options.

:cmakev2:ref:`idf_build_get_property` and :cmakev2:ref:`idf_build_get_compile_options`
    Read the global include directories, compile definitions, and compile options that ESP-IDF applies to its components, so that the same settings can be applied to the project's own executable.

Generating the Binary and Flash Targets
========================================

To produce a flashable image and the usual targets, add them explicitly after linking:

.. code-block:: cmake

    idf_build_binary(my_app.elf
                     OUTPUT_FILE "${CMAKE_BINARY_DIR}/my_app.bin"
                     TARGET my_app_binary)
    idf_flash_binary(my_app_binary TARGET app-flash NAME "app" FLASH)
    idf_check_binary_size(my_app_binary)
    idf_build_generate_metadata(BINARY my_app_binary)
    idf_build_generate_flasher_args()
    add_custom_target(app ALL DEPENDS my_app_binary)

:cmakev2:ref:`idf_build_binary` generates the binary image from the ELF, :cmakev2:ref:`idf_flash_binary` adds the flash target, and :cmakev2:ref:`idf_build_generate_metadata` writes ``project_description.json`` for tooling. These are the steps that :cmakev2:ref:`idf_project_default` performs automatically.

Building for Host and Target
============================

Because the ESP-IDF parts are explicit, the same project can also build a plain host executable. ``idf.py`` sets the :cmakev2:ref:`ESP_PLATFORM` variable, so the ESP-IDF specific code can be guarded with it:

.. code-block:: cmake

    if(ESP_PLATFORM)
        include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
    endif()

    project(my_app C CXX ASM)
    add_executable(my_app.elf main.c)

    if(ESP_PLATFORM)
        # idf_project_init(), idf_build_library(), and the rest go here.
    endif()

Without ``ESP_PLATFORM``, the same ``CMakeLists.txt`` builds an ordinary host executable with no ESP-IDF dependency. Guard the ESP-IDF API calls in the source with ``#ifdef ESP_PLATFORM`` as well. The :example:`idf_as_lib example <build_system/cmakev2/features/idf_as_lib>` builds both ways from a single ``main.c``.

.. note::

    A project that builds a single library this way uses the IMMEDIATE optional-requires mode by default. To get the same minimal, v1-like component set as :cmakev2:ref:`idf_project_default`, set the mode to DEFERRED after :cmakev2:ref:`idf_project_init`. See :ref:`cmakev2-optional-requires`.
