Integrating Third-Party Libraries
*********************************

A project can use libraries that are not ESP-IDF components in three ways: by importing a precompiled static library, by building a library from source and wrapping it as a component, or by linking an external CMake library directly. Each approach is shown below and has a complete example.

Importing a Prebuilt Library
============================

If you already have a compiled static library (``.a``) and its headers, import it with :cmakev2:ref:`add_prebuilt_library` and link it to a component. The :example:`import_prebuilt example <build_system/cmakev2/features/import_prebuilt>` imports ``libprebuilt.a``:

.. code-block:: cmake
    :caption: main/CMakeLists.txt

    idf_component_register(SRCS "main.c"
                           INCLUDE_DIRS ".")

    # Import the prebuilt library and declare the components it depends on.
    add_prebuilt_library(prebuilt "libprebuilt.a"
                         PRIV_REQUIRES spi_flash app_update log)

    # main calls a function from the library, so link it.
    target_link_libraries(${COMPONENT_LIB} PRIVATE prebuilt)

:cmakev2:ref:`add_prebuilt_library` creates an imported library target from the ``.a`` file. ``REQUIRES`` and ``PRIV_REQUIRES`` declare the ESP-IDF components that the library itself depends on. Add the library's include directory with ``target_include_directories`` so that code using it can find its headers.

Building a Library from Source as a Component
=============================================

If the third-party library is built with its own CMake build, build it with CMake's ``ExternalProject_Add`` and wrap it in a component. This is the recommended approach for C++ libraries, because the wrapping component can declare a dependency on the ESP-IDF C++ runtime through ``PRIV_REQUIRES cxx``. The :example:`import_lib example <build_system/cmakev2/features/import_lib>` downloads and builds ``tinyxml2``:

.. code-block:: cmake
    :caption: components/tinyxml2/CMakeLists.txt

    idf_component_register()

    include(ExternalProject)
    externalproject_add(tinyxml2_proj
        URL https://github.com/leethomason/tinyxml2/archive/refs/tags/9.0.0.zip
        # Use the same toolchain as the project.
        CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
                   -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/tinyxml2_install
        BUILD_BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/tinyxml2_install/lib/libtinyxml2.a")

    # Consume the built library: import it, add its headers, and link it.
    add_prebuilt_library(tinyxml2_lib "${CMAKE_CURRENT_BINARY_DIR}/tinyxml2_install/lib/libtinyxml2.a"
                         PRIV_REQUIRES cxx)
    target_include_directories(tinyxml2_lib INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/tinyxml2_install/include")
    add_dependencies(tinyxml2_lib tinyxml2_proj)

    target_link_libraries(${COMPONENT_LIB} INTERFACE tinyxml2_lib)

``ExternalProject_Add`` downloads, configures, builds, and installs the library at build time, using the same toolchain as the project. The resulting ``.a`` is then consumed with :cmakev2:ref:`add_prebuilt_library`, exactly as for a prebuilt library. See the example for the complete, commented version.

Linking an External CMake Library Directly
===========================================

For a self-contained library, typically a pure C library, you can fetch and link it directly without a separate wrapper component. Because v2 components are native CMake code, a component can use CMake's ``FetchContent`` and link the fetched target. The :example:`import_lib_direct example <build_system/cmakev2/features/import_lib_direct>` fetches ``lwjson`` and links it to ``main``:

.. code-block:: cmake
    :caption: main/CMakeLists.txt

    include(FetchContent)
    fetchcontent_declare(lwjson
        GIT_REPOSITORY https://github.com/MaJerle/lwjson.git
        GIT_TAG        v1.8.1)
    fetchcontent_makeavailable(lwjson)

    idf_component_register(SRCS "main.c"
                           INCLUDE_DIRS ".")

    target_link_libraries(${COMPONENT_LIB} PUBLIC lwjson)

``FetchContent`` makes the library's CMake target available, which is then linked to the component with ``target_link_libraries``. This is the simplest approach when the library is a plain CMake project with no special runtime requirements.
