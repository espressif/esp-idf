Creating a New Component
************************

A component is a reusable unit that the build system compiles into a library and links into the application that uses it. There are two ways to write one:

- **Backward-compatible**, using the :cmakev2:ref:`idf_component_register` function. This is the recommended approach and the one most components use. A component written this way builds under both v1 and v2 as long as it stays within the features v1 also supports. Relying on v2-only behavior, such as configuration-conditional dependencies, can break it under v1; see :doc:`managing-compatibility`.
- **Native CMake**, written as a plain CMake static library. This gives full access to native CMake, but on its own the component builds only under v2. It can still be made to work under v1; see :doc:`managing-compatibility`.

Prefer :cmakev2:ref:`idf_component_register`. Choose a native CMake component only when it needs CMake features that :cmakev2:ref:`idf_component_register` does not expose.

Component Layout
================

A component is a directory that contains a ``CMakeLists.txt`` file, its source files, and a directory with its public header files. For example:

.. code-block:: text

    my_component
    ├── CMakeLists.txt
    ├── my_component.c
    └── include
        └── my_component.h

The component's name is the name of its directory. The build system finds components in the project's ``components`` directory, in the directories listed in ``EXTRA_COMPONENT_DIRS``, and among the components bundled with ESP-IDF. How components are discovered is described in :doc:`design`. The layout is the same for both kinds of component; only the content of ``CMakeLists.txt`` differs, as described in the sections below.

.. _cmakev2-component-compatible:

Backward-Compatible Component
=============================

The :cmakev2:ref:`idf_component_register` function declares a component's sources, include directories, and dependencies. It is the recommended way to write a component and behaves the same under both v1 and v2, so a component written this way builds under either build system. For a complete, runnable example, see the ``main`` component of the hello_world project: :example_file:`main/CMakeLists.txt <build_system/cmakev2/get-started/hello_world/main/CMakeLists.txt>`.

Registering the Component
-------------------------

The component's ``CMakeLists.txt`` calls :cmakev2:ref:`idf_component_register` to declare its source files, include directories, and dependencies:

.. code-block:: cmake
    :caption: my_component/CMakeLists.txt

    idf_component_register(SRCS "my_component.c"
                           INCLUDE_DIRS "include"
                           REQUIRES mbedtls)

The most commonly used arguments are:

``SRCS``
    The component's source files. They are compiled into the component's library.

``INCLUDE_DIRS``
    Public include directories. They are added to the include path of this component and of every component that depends on it.

``PRIV_INCLUDE_DIRS``
    Private include directories. They are added only to this component's own include path.

``REQUIRES``
    Public component dependencies. Their headers and libraries are available to this component and propagate to components that depend on it.

``PRIV_REQUIRES``
    Private component dependencies. They are available to this component only and do not propagate to its dependents.

A component that has no source files of its own, for example a configuration-only or header-only component, can call :cmakev2:ref:`idf_component_register` with only ``INCLUDE_DIRS``, or with no arguments at all.

:cmakev2:ref:`idf_component_register` accepts further arguments, including ``SRC_DIRS`` and ``EXCLUDE_SRCS`` for selecting sources by directory, ``LDFRAGMENTS`` for linker fragments, ``EMBED_FILES`` and ``EMBED_TXTFILES`` for embedding binary data, ``REQUIRED_IDF_TARGETS`` to restrict the component to specific chips, and ``WHOLE_ARCHIVE``. These behave as in v1. For the full list of arguments, see the :cmakev2:ref:`API Reference <idf_component_register>`.

Declaring Dependencies
----------------------

A component declares the other components it uses with ``REQUIRES`` (public) and ``PRIV_REQUIRES`` (private). For example, an application's ``main`` component that uses two other components privately:

.. code-block:: cmake
    :caption: main/CMakeLists.txt

    idf_component_register(SRCS "app_main.c"
                           PRIV_REQUIRES component1 component2)

Like v1, the common components (``freertos``, ``log``, and so on) are added automatically; every other component a component uses must be listed in ``REQUIRES`` or ``PRIV_REQUIRES``.

To keep a component backward compatible with v1, its dependencies cannot be made conditional on configuration options, which v1 does not support. The same applies to other v2-only features. How to handle the differences between v1 and v2 in a single component is described in :doc:`managing-compatibility`.

Differences from v1
-------------------

:cmakev2:ref:`idf_component_register` behaves the same in v1 and v2, with one exception: the v2 version does not support the ``KCONFIG`` and ``KCONFIG_PROJBUILD`` arguments for Kconfig files whose names do not follow the standard ``Kconfig`` and ``Kconfig.projbuild`` convention. See :ref:`cmakev2-kconfig-file-names`.

.. important::

    Make sure every variable used in the component's ``CMakeLists.txt`` is initialized before use. In v2 a component may be evaluated within the variable scope of another component, so a variable can already hold a value set by that component. A common mistake is appending to a CMake list without first initializing it. This is explained in :ref:`cmakev2-recursive-evaluation`.

For the complete list of differences that may require changes to a v1 component, see :doc:`breaking-changes`. To keep a single component building under both v1 and v2, see :doc:`managing-compatibility`.

.. _cmakev2-component-native:

Native CMake Component
======================

A native CMake component creates its library target directly with ``add_library``, instead of calling :cmakev2:ref:`idf_component_register`. This gives full access to native CMake, which is useful when :cmakev2:ref:`idf_component_register` does not provide enough control. On its own such a component builds only under v2; see :doc:`managing-compatibility` to keep it working under v1 as well.

This section builds a complete example component, ``esp_target_info``, step by step, and exercises the main features available to a native component: creating the component target, depending on other components, expressing a dependency conditionally, and setting component properties such as linker fragments and linker scripts. A configuration-driven native component is also available as the :example:`conditional_component example <build_system/cmakev2/features/conditional_component>`.

The Example Component
---------------------

The ``esp_target_info`` component provides a single function, ``print_esp_target_info``, which prints basic information about the target: its name, the number of CPU cores, the free heap size, and the flash size. Its directory is laid out as follows:

.. code-block:: text

    esp_target_info
    ├── include
    │   └── esp_target_info.h
    ├── srcs
    │   └── esp_target_info.c
    ├── CMakeLists.txt
    ├── esp_target_info.ld
    └── linker.lf

The ``esp_target_info.h`` header declares the single public function:

.. code-block:: c
    :caption: include/esp_target_info.h

    #ifndef _ESP_TARGET_INFO_
    #define _ESP_TARGET_INFO_
    void print_esp_target_info(void);
    #endif

The ``esp_target_info.c`` source defines it. It reads the target name from the ``CONFIG_IDF_TARGET`` configuration option, the CPU core count from the ``esp_hw_support`` component, the free heap size from ``esp_system``, and the flash size from ``spi_flash``:

.. code-block:: c
    :caption: srcs/esp_target_info.c

    #include <stdio.h>
    #include <inttypes.h>
    #include "esp_chip_info.h"
    #include "esp_flash.h"
    #include "esp_system.h"

    void esp_target_chip_info(esp_chip_info_t*);

    void print_esp_target_info(void)
    {
        esp_chip_info_t chip_info;
        uint32_t flash_size = 0;

        esp_target_chip_info(&chip_info);
        esp_flash_get_size(NULL, &flash_size);

        printf("target: %s\n", CONFIG_IDF_TARGET);
        printf("cpu cores: %d\n", chip_info.cores);
        printf("free heap size: %" PRIu32 "\n", esp_get_minimum_free_heap_size());
        printf("flash size: %" PRIu32 "B\n", flash_size);
    }

The ``esp_target_info.ld`` linker script defines the symbol ``esp_target_chip_info`` as an alias for ``esp_chip_info``. The source calls the alias instead of ``esp_chip_info`` directly. This serves no real purpose other than to demonstrate a working linker script:

.. code-block:: text
    :caption: esp_target_info.ld

    esp_target_chip_info = esp_chip_info;

The ``linker.lf`` linker fragment places ``print_esp_target_info`` in IRAM instead of flash, again only for demonstration. For the linker fragment format, see :doc:`/api-guides/linker-script-generation`:

.. code-block:: text
    :caption: linker.lf

    [mapping:esp_target_info]
    archive: libesp_target_info.a
    entries:
        print_esp_target_info (noflash)

The rest of this section builds the component's ``CMakeLists.txt`` that ties these files together, one step at a time.

The Component Target
--------------------

When the build system evaluates a component, it sets the :cmakev2:ref:`COMPONENT_TARGET` variable to the name of the library target that the component must create. The component creates this target, typically with ``add_library``, and the build system links it into the component's interface target so that other components can depend on it. Other components refer to this component through its interface alias ``idf::<name>``, never through ``COMPONENT_TARGET`` directly.

The first step creates the static library from the component's source and exposes its public headers (highlighted lines):

.. code-block:: cmake
    :caption: CMakeLists.txt
    :linenos:
    :emphasize-lines: 1-7

    add_library(${COMPONENT_TARGET} STATIC
        "srcs/esp_target_info.c"
    )

    target_include_directories(${COMPONENT_TARGET} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    )

    idf_component_include(esp_hw_support)
    idf_component_include(spi_flash)
    idf_component_include(esp_system)

    target_link_libraries(${COMPONENT_TARGET} PRIVATE
        idf::esp_hw_support
        idf::spi_flash
        idf::esp_system
    )

    idf_component_set_property(${COMPONENT_TARGET} WHOLE_ARCHIVE TRUE)
    idf_component_set_property(${COMPONENT_TARGET} LDFRAGMENTS linker.lf APPEND)
    idf_component_set_property(${COMPONENT_TARGET} LINKER_SCRIPTS esp_target_info.ld APPEND)

    target_link_options(${COMPONENT_TARGET} INTERFACE "SHELL:-u esp_chip_info")

``target_include_directories(... PUBLIC ...)`` makes the ``include`` directory part of the include path of this component and of every component that links it, which is the native equivalent of the ``INCLUDE_DIRS`` argument of :cmakev2:ref:`idf_component_register`. Use ``PRIVATE`` for include directories that should not be visible to dependents.

Depending on Other Components
-----------------------------

``esp_target_info`` uses functionality from ``esp_hw_support``, ``esp_system``, and ``spi_flash``, so it must declare its dependencies on them. A native component does this in two steps: it includes each component with :cmakev2:ref:`idf_component_include`, then links the component's ``idf::<name>`` interface target. The highlighted lines add these dependencies:

.. code-block:: cmake
    :caption: CMakeLists.txt
    :linenos:
    :emphasize-lines: 9-17

    add_library(${COMPONENT_TARGET} STATIC
        "srcs/esp_target_info.c"
    )

    target_include_directories(${COMPONENT_TARGET} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    )

    idf_component_include(esp_hw_support)
    idf_component_include(spi_flash)
    idf_component_include(esp_system)

    target_link_libraries(${COMPONENT_TARGET} PRIVATE
        idf::esp_hw_support
        idf::spi_flash
        idf::esp_system
    )

    idf_component_set_property(${COMPONENT_TARGET} WHOLE_ARCHIVE TRUE)
    idf_component_set_property(${COMPONENT_TARGET} LDFRAGMENTS linker.lf APPEND)
    idf_component_set_property(${COMPONENT_TARGET} LINKER_SCRIPTS esp_target_info.ld APPEND)

    target_link_options(${COMPONENT_TARGET} INTERFACE "SHELL:-u esp_chip_info")

.. note::

    Unlike v1, v2 adds no dependency automatically. In v1, a ``main`` component with no declared dependencies receives every component in the build as a dependency, and dependencies declared in ``idf_component.yml`` for the component manager are added automatically. In v2, every dependency a component uses must be declared explicitly.

:cmakev2:ref:`idf_component_include` evaluates the named component exactly once: it calls CMake's ``add_subdirectory`` for the component and links the ``COMPONENT_TARGET`` it creates into its interface target. Other components reference that interface target through the ``idf::<name>`` alias, as in the ``target_link_libraries`` call above. Link the interface ``PRIVATE`` for a private dependency, or ``PUBLIC`` for one that should propagate to dependents.

:cmakev2:ref:`idf_component_include` can also store the interface target name in a variable through its ``INTERFACE`` option, which is convenient when the target is passed to other CMake commands, stored in a list, or referenced programmatically:

.. code-block:: cmake

    idf_component_include(spi_flash INTERFACE spi_flash_iface)
    target_link_libraries(${COMPONENT_TARGET} PRIVATE ${spi_flash_iface})

Because the configuration of all discovered components is available while a component is evaluated, a dependency can be made conditional on a ``CONFIG_*`` option. The component must still be included before it is linked:

.. code-block:: cmake

    if(CONFIG_VFS_SUPPORT_IO)
        idf_component_include(vfs)
        target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::vfs)
    endif()

.. important::

    Always include a component before using it. In v2 the configuration of every discovered component is visible, so a ``CONFIG_*`` option being set does not mean the component that defines it is part of the build. Configuration-driven dependencies are covered in detail in :doc:`component-dependencies`.

Setting Component Properties
----------------------------

A native component configures build behavior through component properties, set with :cmakev2:ref:`idf_component_set_property`. The highlighted lines set the properties for ``esp_target_info`` and force a symbol to be retained:

.. code-block:: cmake
    :caption: CMakeLists.txt
    :linenos:
    :emphasize-lines: 19-23

    add_library(${COMPONENT_TARGET} STATIC
        "srcs/esp_target_info.c"
    )

    target_include_directories(${COMPONENT_TARGET} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    )

    idf_component_include(esp_hw_support)
    idf_component_include(spi_flash)
    idf_component_include(esp_system)

    target_link_libraries(${COMPONENT_TARGET} PRIVATE
        idf::esp_hw_support
        idf::spi_flash
        idf::esp_system
    )

    idf_component_set_property(${COMPONENT_TARGET} WHOLE_ARCHIVE TRUE)
    idf_component_set_property(${COMPONENT_TARGET} LDFRAGMENTS linker.lf APPEND)
    idf_component_set_property(${COMPONENT_TARGET} LINKER_SCRIPTS esp_target_info.ld APPEND)

    target_link_options(${COMPONENT_TARGET} INTERFACE "SHELL:-u esp_chip_info")

:cmakev2:ref:`WHOLE_ARCHIVE`
    Keep every object file of the component's library in the final binary, even if no symbol references it. This is used for link-time registration, where objects register themselves through constructors or linker-section entries. The :example:`plugins example <build_system/cmakev2/features/plugins>` relies on it.

:cmakev2:ref:`LDFRAGMENTS`
    Linker fragment files for the component, processed by the linker script generator. Here it adds ``linker.lf``. See :doc:`/api-guides/linker-script-generation`.

:cmakev2:ref:`LINKER_SCRIPTS`
    Linker script files added to the link command for the component. Here it adds ``esp_target_info.ld``.

The final line forces the linker to keep ``esp_chip_info`` in the binary even when section garbage collection (``--gc-sections``) is enabled. This is needed because ``esp_target_info.ld`` defines ``esp_target_chip_info`` as an alias for ``esp_chip_info``; without forcing the reference, the underlying ``esp_chip_info`` function could be discarded as unused. ``target_link_options(... INTERFACE "SHELL:-u esp_chip_info")`` adds the ``-u esp_chip_info`` undefined-symbol reference to every target that links the component.

The full list of component properties is in the :ref:`API Reference <cmakev2_component_properties>`. To run a custom step at a point in the build, such as acting on the linked executable, a component can register a build event callback; see :doc:`build-event-callbacks`.

How the Component is Built
--------------------------

A component is compiled and linked only if it is required, directly or transitively, by a component that is being built. :cmakev2:ref:`idf_project_default` builds the application from the ``main`` component, so a native component becomes part of the application when ``main`` or one of its dependencies includes it. ``main`` is a convention of :cmakev2:ref:`idf_project_default`, not a requirement of the build system; a project that drives the build with the lower-level API can build the application from any component (see :doc:`multiple-binaries` and :doc:`idf-as-library`). The difference between discovering a component and including it is described in :doc:`design`.
