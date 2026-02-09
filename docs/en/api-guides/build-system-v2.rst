Build System v2
***************

.. attention::

    Build System v2 is currently available as a **Technical Preview** intended for **testing and evaluation**. Features, functionality, and performance are **subject to change without notice**, and **production use is not recommended** at this stage.

ESP-IDF CMake-based build system v2, referred to in this documentation simply as v2 or build system, is a successor to the original CMake-based :doc:`/api-guides/build-system`, referred to as v1. The v2 addresses limitations introduced in the previous version while trying to maintain backward compatibility for components written for v1. The most significant changes include the ability to use Kconfig variables to specify component dependencies, the removal of early component evaluation using CMake script mode, and support for writing components using the native CMake approach. While v2 aims to be as backward compatible with v1 as possible, meaning most components written for v1 should work without modification with v2, there are design differences between v1 and v2 that may require changes in v1 components to work with v2. The incompatibilities are described in :ref:`cmakev2-breaking-changes`.

Creating a New Project
======================

Below is the minimal project's ``CMakeLists.txt`` file, which should suffice for most projects:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
    project(myProject C CXX ASM)
    idf_project_default()

The order of commands in the project's ``CMakeLists.txt`` is important. First, the ``cmake_minimum_required`` command must be called to specify the minimum required version of CMake.

Next, include ``idf.cmake``, which provides the build system functionality. It performs the necessary build system initialization tasks and toolchain configuration, which must be completed before invoking CMake’s ``project()`` command.

Once the build system is initialized, the ``project()`` command should be executed. It sets up project related variables, initializes the toolchain for the required languages, and performs other CMake specific initialization steps.

Finally, the :cmakev2:ref:`idf_project_default` function creates the default project executable based on the ``main`` component and its transitive dependencies. It also generates binary images and adds related targets such as ``flash`` and ``menuconfig``.

Creating a New Component
========================

This section explains how to create a new component. The preferred and generally sufficient method is described in :ref:`cmakev2-creating-compatible-component`. However, if you need more control or prefer a native CMake approach without concern for v1 compatibility, see :ref:`cmakev2-creating-v2-component`.

.. important::

    Always ensure that all variables used within the component's ``CMakeLists.txt`` are initialized before use. The component might be evaluated in the context of other component where some variables may already be set. A common example is appending to CMake's list without explicitly initializing the list variable first.

.. _cmakev2-creating-compatible-component:

Creating a Component Compatible with v1 and v2
----------------------------------------------

The recommended method for creating a new component is to use the :cmakev2:ref:`idf_component_register` function. This function is provided in v2 to maintain backward compatibility with v1 and remains the preferred approach for creating components that work with both versions. The only difference between the v1 and v2 versions of this function is that the v2 version does not support the ``KCONFIG`` and ``KCONFIG_PROJBUILD`` options when using Kconfig filenames that do not follow the :ref:`cmakev2-standardized-kconfigs`. For components that require functionality beyond what :cmakev2:ref:`idf_component_register` provides, please refer to :ref:`cmakev2-breaking-changes`.

.. code-block:: cmake
    :caption: Minimal Component CMakeLists.txt

    idf_component_register(SRCS "foo.c" "bar.c"
                           INCLUDE_DIRS "include"
                           REQUIRES mbedtls)

.. _cmakev2-creating-v2-component:

Creating a Component for v2 Only
--------------------------------

.. important::

    This section describes how to create a component for v2, which is not compatible or usable with v1. To create a component that works with both versions, it is recommended to follow :ref:`cmakev2-creating-compatible-component`.

This section demonstrates how to create a new component using a simple component named ``esp_target_info`` as an example. This component provides the ``print_esp_target_info`` function, which, when invoked, displays basic information about the target.

The layout of the component's directory is organized as follows:

.. code-block:: bash

    esp_target_info
    ├── include
    │   └── esp_target_info.h
    ├── srcs
    │   └── esp_target_info.c
    ├── CMakeLists.txt
    ├── esp_target_info.ld
    └── linker.lf

The ``esp_target_info.h`` header file provides the ``print_esp_target_info`` function declaration as its sole public interface, intended for use by other components.

.. code-block:: c
    :caption: include/esp_target_info.h

    #ifndef _ESP_TARGET_INFO_
    #define _ESP_TARGET_INFO_
    void print_esp_target_info(void);
    #endif

The ``esp_target_info.c`` file contains the definition for the ``print_esp_target_info`` function. This function outputs the target name based on the configuration ``CONFIG_IDF_TARGET`` variable, the number of CPU cores as obtained from the ``esp_hw_support`` component, the available free heap size from the ``esp_system`` component, and the flash size from the ``spi_flash`` component.

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

The ``esp_target_info.ld`` linker script defines an additional symbol, ``esp_target_chip_info``, which serves as an alias for the ``esp_chip_info`` function. This alias is used in ``esp_target_info.c`` to obtain chip information instead of directly calling the ``esp_chip_info`` function. This serves no real purpose and is merely an example of a working linker script used in the demonstration.

.. code-block:: bash
    :caption: esp_target_info.ld

    esp_target_chip_info = esp_chip_info;

The ``linker.lf`` is a linker fragment that determines the placement of ``print_esp_target_info`` in IRAM instead of flash. This is merely an example for demonstration purposes. For more details on linker fragments, please refer to :doc:`/api-guides/linker-script-generation`.

.. code-block:: bash
    :caption: linker.lf

    [mapping:esp_target_info]
    archive: libesp_target_info.a
    entries:
        print_esp_target_info (noflash)

Now, let's describe how the aforementioned sources can be integrated as a component for the build system by defining the component's ``CMakeLists.txt`` file.

Create a static library target with the target name stored in the ``COMPONENT_TARGET`` variable for the ``srcs/esp_target_info.c`` source file, and add the ``include`` directory as the component's directory with public header files.

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


When a component is evaluated by the build system, the ``COMPONENT_TARGET`` variable is assigned a target name that the component is responsible for creating. This target represents the component within the build and is linked to the component interface target managed by the build system. The component interface target is what other components use to declare dependencies on this component. Other components can obtain a component’s interface target using the :cmakev2:ref:`idf_component_include` function and link to it as needed.

Include all required components and link them to the component's target.

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

    The v1 automatically adds component dependencies in certain situations. For instance, if the ``main`` component does not specify its dependencies, all components identified by the build system are added as dependencies to the ``main`` component. Additionally, component dependencies specified in ``idf_component.yaml`` for the component manager are also added as dependencies to the component. In contrast, the v2 does not automatically add any dependencies, all component dependencies must be explicitly stated.

The ``esp_target_info`` component uses functionality from other components, namely ``esp_hw_support``, ``esp_system``, and ``spi_flash``, to collect basic information about the target. Therefore, it must declare its dependencies on these components. The component acquires the required component interface targets using the :cmakev2:ref:`idf_component_include` function.

Each component is evaluated exactly once by the :cmakev2:ref:`idf_component_include` function. This function essentially invokes CMake’s ``add_subdirectory`` for the specified component directory and links the ``COMPONENT_TARGET`` created by that component with its interface target.

The interface target can then be used by other components to declare dependencies. It can be referenced directly via a predefined alias named ``idf::<component name>``, which provides a convenient and literal way to reference a component’s interface target, as shown in the example above.

The :cmakev2:ref:`idf_component_include` function also supports an ``INTERFACE`` option that allows storing the component’s interface target name in a variable. This approach is useful in more complex cases where the target needs to be passed to other CMake functions, stored in lists, or referenced programmatically.

.. code-block:: cmake
    :caption: Declaring component dependencies using interface target variable

    idf_component_include(spi_flash INTERFACE spi_flash_iface)

    target_link_libraries(${COMPONENT_TARGET} PRIVATE
        ${spi_flash_iface}
    )

Component dependencies can be conditionally expressed based on the configuration. When a component's ``CMakeLists.txt`` is evaluated by the build system, the configuration for all discovered components is available. If a component needs to declare a conditional dependency based on a configuration variable, it must ensure that the component is included in the build system first before using it by including the component with the :cmakev2:ref:`idf_component_include` function. For example, if a component has a conditional dependency on the ``vfs`` component based on the ``CONFIG_VFS_SUPPORT_IO`` configuration option being set, it needs to include the ``vfs`` component before linking it to the component target.

.. code-block:: cmake
    :caption: Declaring optional component dependency

    if(CONFIG_VFS_SUPPORT_IO)
        idf_component_include(vfs)
        target_link_libraries(${COMPONENT_TARGET} PRIVATE
            idf::vfs
        )
    endif()

.. important::

    Always ensure that the required component is included before using it. The presence of certain component configuration options does not guarantee that the component will be included in the project build.

Set miscellaneous component properties. This primarily serves as an example to demonstrate other commonly used features that the component may configure.

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

WHOLE_ARCHIVE:
    Include all object files from the component's static library when linked.

LDFRAGMENTS:
    Include files from this property to be processed with the :doc:`/api-guides/linker-script-generation`.

LINKER_SCRIPTS:
    Include files from this property in the link command line as linker scripts.

Also ensure that the ``esp_chip_info`` function is retained in the final binary even when section garbage collection, ``--gc-sections``, is enabled. This is required because ``esp_target_info.ld`` defines ``esp_target_chip_info`` as an alias for ``esp_chip_info``, and without forcing the linker to include it, the underlying ``esp_chip_info`` function could be discarded as unused.

.. _cmakev2-breaking-changes:

Breaking Changes for v1 Components
==================================

While most of the v1 components should be usable without any modifications in v2, there are some differences between v1 and v2 that might require changes to v1 components. This section outlines the breaking changes that may occur if a v1 component is used with v2 and explains how the component can be modified to work with both v1 and v2.

Updating Component for v2 Compatibility
----------------------------------------------

If a component needs to be evaluated with both v1 and v2, and requires modifications to function with v2, the ``IDF_BUILD_V2`` variable can be used by the component to determine whether it is being evaluated under v1 or v2, and adjust its behavior accordingly. The ``IDF_BUILD_V2`` is set when the component is evaluated with v2. This variable can be used to conditionally adjust only small parts of the existing v1 component, or a completely new ``CMakeLists.txt`` can be created for v2 and conditionally included in the v1 component's ``CMakeLists.txt``. For more information on how to write a v2 component, please see `Creating a New Component`_.

Below is the ``hello_world`` example component with its ``CMakeLists.txt`` adjusted to include and evaluate the separate v2 component ``CMakeLists_v2.txt`` if evaluated under v2.

.. note::

    The existing v1 ``hello_world`` component should function without modification in v2. This is just a simple example of how ``IDF_BUILD_V2`` can be used to evaluate different component's ``CMakeLists.txt`` files for v1 and v2.

Adjusted original v1 hello_world example ``CMakeLists.txt``.

.. code-block:: cmake

    if(IDF_BUILD_V2)
        # Include component CMake code for v2 and return.
        include(CMakeLists_v2.txt)
        return()
    endif()

    # Here follows the original component CMake code for v1.
    idf_component_register(SRCS "hello_world_main.c"
                           PRIV_REQUIRES spi_flash
                           INCLUDE_DIRS "")


The hello_world example ``CMakeLists_v2.txt`` for v2.

.. code-block:: cmake

    idf_component_include(spi_flash)

    add_library(${COMPONENT_TARGET} STATIC
        "hello_world_main.c"
    )

    target_include_directories(${COMPONENT_TARGET} PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
    )

    target_link_libraries(${COMPONENT_TARGET} PRIVATE
        idf::spi_flash
    )

The ``BUILD_COMPONENTS`` Build Property is Unavailable
------------------------------------------------------

The v1, by design, collects components built by the project in the ``BUILD_COMPONENTS`` build property. This is achieved by evaluating components early using the CMake script mode and collecting components based on their dependencies as provided in :cmakev2:ref:`idf_component_register`. Later, the ``BUILD_COMPONENTS`` are evaluated again using CMake's ``add_subdirectory``. This approach imposes several restrictions. First, components cannot express their dependencies based on Kconfig variables because Kconfig variables are not known during the early evaluation. Second, the CMake script mode does not allow CMake commands that define build targets or actions, which caused confusion about which commands can be used in a component and when they are actually evaluated. The v2 removes this two-stage component evaluation, and hence the ``BUILD_COMPONENTS`` build property doesn't exist in v2. Any attempt to obtain the ``BUILD_COMPONENTS`` build property with :cmakev2:ref:`idf_build_get_property` will result in an error. The ``BUILD_COMPONENTS`` build property was primarily used in v1 by components to discover which other components were being built by the project and to adjust their behavior, for example, by adding additional source files. In v2, the functionality of the ``BUILD_COMPONENTS`` build property can be replaced with the ``$<TARGET_EXISTS:tgt>`` generator expression.

For example, the ``esp_eth`` component is compiled with the additional ``esp_eth_netif_glue.c`` file if the ``esp_netif`` component is also being built in v1 with:

.. code-block:: cmake

    idf_build_get_property(components_to_build BUILD_COMPONENTS)
    if(esp_netif IN_LIST components_to_build)
        list(APPEND srcs "src/esp_eth_netif_glue.c")
    endif()

To use the ``esp_eth`` component with v2, the usage of ``BUILD_COMPONENTS`` has to be replaced with:

.. code-block:: cmake

    if(IDF_BUILD_V2)
        target_sources(${COMPONENT_TARGET} PRIVATE "$<$<TARGET_EXISTS:idf::esp_netif>:src/esp_eth_netif_glue.c>")
    else()
        idf_build_get_property(components_to_build BUILD_COMPONENTS)
        if(esp_netif IN_LIST components_to_build)
            list(APPEND srcs "src/esp_eth_netif_glue.c")
        endif()
    endif()

.. note::

    The ``$<TARGET_EXISTS:tgt>`` generator expression should work for both v1 and v2. The ``IDF_BUILD_V2`` is used to demonstrate the different approaches in v1 and v2.

.. _cmakev2-standardized-kconfigs:

Standardized ``Kconfig`` and ``Kconfig.projbuild`` File Names
-------------------------------------------------------------

The v1 allows the use of ``Kconfig`` and ``Kconfig.projbuild`` with custom file names in :cmakev2:ref:`idf_component_register`. This is possible because, in v1, the components participating in the project build are collected during early component evaluation, allowing for the specification of custom names for the Kconfig files. The v2 does not perform early evaluation, and the Kconfig files are collected based on the fixed ``Kconfig`` and ``Kconfig.projbuild`` file names, which must be present in the component's root directory. This can be resolved by renaming the Kconfig files that do not conform to this naming convention or by including them in Kconfig files that comply with this convention.


Component Configuration Visibility
----------------------------------

In v1, the ``sdkconfig`` is generated based on Kconfig files provided by components that are part of the project build. This means that if a component is not included in the project build, for instance, if it is not required as a dependency of another component, its configuration will not be visible or available when components are evaluated. In v2, the ``sdkconfig`` is generated based on Kconfig files for all available components. This means the presence of some configuration options does not guarantee that the component providing this configuration option is participating in the build, unlike in v1.

For example, in v1, the following condition in the ``esp_vfs_console`` component links the ``vfs`` component to the ``esp_vfs_console`` component based on the ``CONFIG_VFS_SUPPORT_IO`` configuration option being set. This works in v1 because the ``CONFIG_VFS_SUPPORT_IO`` configuration option is not available if the ``vfs`` component is not participating in the project build.

.. code-block:: cmake

    if(CONFIG_VFS_SUPPORT_IO)
        target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
        # Make sure esp_vfs_console_register gets called at startup stage
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_include_console_register")
    endif()

In contrast, in v2, configuration for all discovered components is available. This means that even if a component does not participate in the project build, its configuration will still be accessible. If the ``CONFIG_VFS_SUPPORT_IO`` configuration option is set by default, the condition will evaluate as true in v2, even if the ``vfs`` component is not participating in the project build and the ``idf::vfs`` target does not exist. The ``$<TARGET_EXISTS:tgt>`` generator expression can be used to express such dependencies in v2.

.. code-block:: cmake

    if(IDF_BUILD_V2)
        target_link_libraries(${COMPONENT_LIB} PUBLIC "$<$<TARGET_EXISTS:idf::vfs>:idf::vfs>")
        # Make sure esp_vfs_console_register gets called at startup stage
        target_link_libraries(${COMPONENT_LIB} INTERFACE "$<$<TARGET_EXISTS:idf::vfs>:-u esp_vfs_include_console_register>")
    else()
        if(CONFIG_VFS_SUPPORT_IO)
            target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
            # Make sure esp_vfs_console_register gets called at startup stage
            target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_vfs_include_console_register")
        endif()
    endif()

.. note::

    The ``$<TARGET_EXISTS:tgt>`` generator expression should work for both v1 and v2. The ``IDF_BUILD_V2`` is used to demonstrate the different approaches in v1 and v2.

.. important::

    This change applies not only to a component's ``CMakeLists.txt`` but also to its source files. The component's code can no longer assume that the functionality of other components is available simply because their Kconfig variables are set. For example, if the ``CONFIG_VFS_SUPPORT_IO`` configuration variable is set and the component's code depends on the functionality of the ``vfs`` component, it cannot merely check ``CONFIG_VFS_SUPPORT_IO`` in the source code. It must ensure that the ``vfs`` component is included in the project build and that the component has a declared dependency on the ``vfs`` component in its ``CMakeLists.txt``.

Recursive Evaluation of Components
----------------------------------

In v1, the components participating in the project build are collected in the ``BUILD_COMPONENTS`` build property during the early evaluation phase, prior to their evaluation with ``add_subdirectory``. This means that v1 is aware of all components participating in the project build and their dependencies before CMake evaluates them with the ``add_subdirectory`` call. This allows v1 to evaluate components in a non-recursive manner and within a relatively predictable environment with minimal variables set. In contrast, v2 does not perform early component evaluation and components are added based on the requirements observed during their evaluation. This means a component can be evaluated recursively within the scope of another component's variables if it is a dependency of that component. In other words, if component A requires component B, then when B is evaluated, it can access the variables of A. Hence, it's important to ensure that all variables used by the component are properly initialized before usage. A typical problem involves CMake lists and the ``APPEND`` operation.

.. code-block:: cmake

    # Wrong
    list(APPEND srcs main.c)

    # Correct
    set(srcs)
    list(APPEND srcs main.c)

The ``project_include.cmake`` Files are Included in a Non-Specific Order
------------------------------------------------------------------------

In v1, the ``project_include.cmake`` files from components are included in the order specified by the ``BUILD_COMPONENTS`` build property. The components in ``BUILD_COMPONENTS`` are sorted based on their requirements, as determined during early evaluation. This means a given component's ``project_include.cmake`` is included only after all ``project_include.cmake`` files for its dependencies have been included. This holds true as long as there is no cyclic dependency between components. In contrast, v2 does not perform early evaluation of components, and ``BUILD_COMPONENTS`` does not exist. Therefore, in v2, the ``project_include.cmake`` files are included for all discovered components, not just those participating in the project build, and they are included in the order in which the components are discovered by v2. As a result, cross-file functionality at the global scope between ``project_include.cmake`` files can no longer be relied upon in v2.

.. note::

    It is still possible to call functions or macros defined in another ``project_include.cmake`` file, provided they are invoked within a CMake function or another non-global scope. Only global-scope interactions are unreliable in v2.

Strict Component Precedence
---------------------------

The v2 strictly adheres to the component precedence for components with the same name, as described in :ref:`cmake-components-same-name`. While v1 allows components discovered in directories specified with the ``EXTRA_COMPONENT_DIRS`` variable to be overridden by `Local Directory Dependencies`_ specified in the ``idf_component.yml`` manifest file, this is no longer possible in v2.

API Reference
=============

This section contains an automatically generated API reference from the build
system v2 CMake source code.

.. _cmakev2_variables:

Variables
---------


.. _cmakev2_functions:

Functions
---------


.. _cmakev2_macros:

Macros
------


.. cmakev2:include:: ../../../tools/cmakev2/build.cmake
.. cmakev2:include:: ../../../tools/cmakev2/compat.cmake
.. cmakev2:include:: ../../../tools/cmakev2/component.cmake
.. cmakev2:include:: ../../../tools/cmakev2/idf.cmake
.. cmakev2:include:: ../../../tools/cmakev2/kconfig.cmake
.. cmakev2:include:: ../../../tools/cmakev2/ldgen.cmake
.. cmakev2:include:: ../../../tools/cmakev2/manager.cmake
.. cmakev2:include:: ../../../tools/cmakev2/project.cmake
.. cmakev2:include:: ../../../tools/cmakev2/utilities.cmake


.. _Local Directory Dependencies: https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/manifest_file.html#local-directory-dependencies
