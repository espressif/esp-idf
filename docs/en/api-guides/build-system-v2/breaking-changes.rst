Breaking Changes
****************

Most components written for :doc:`Build System v1 </api-guides/build-system>` build under v2 without modification. There are, however, design differences between v1 and v2 that may require changes to a v1 component. This page is a catalog of those differences and how to adapt to each. To keep a single component building under both v1 and v2, see :doc:`managing-compatibility`. The concepts behind these changes are described in :doc:`design`.

.. _cmakev2-post-elf-unavailable:

``idf_build_add_post_elf_dependency`` and ``idf_build_get_post_elf_dependencies`` are Unavailable
=================================================================================================

In v1, components that need to run a step after the executable is linked but before the binary image is generated use ``idf_build_add_post_elf_dependency`` to register a dependency and ``idf_build_get_post_elf_dependencies`` to retrieve the list of such dependencies (see the :doc:`Build System v1 </api-guides/build-system>` API). These functions are **not available** in v2.

In v2, use a build event callback instead. Register a ``POST_ELF`` callback with :cmakev2:ref:`idf_component_register_build_event_callback` in the component's ``project_include.cmake``. The callback receives the executable target name; use it to attach a ``POST_BUILD`` command (for example with ``add_custom_command(TARGET ... POST_BUILD ...)``) or to add custom targets that depend on the executable. This achieves the same ordering, run after the ELF and before the binary, without relying on internal build properties. See :doc:`build-event-callbacks`.

.. _cmakev2-build-components:

The ``BUILD_COMPONENTS`` Build Property is Unavailable
======================================================

In v1, the build system collects the components built by the project in the ``BUILD_COMPONENTS`` build property. This is achieved by evaluating components early using CMake script mode and collecting them based on the dependencies provided in :cmakev2:ref:`idf_component_register`. The ``BUILD_COMPONENTS`` are later evaluated again using CMake's ``add_subdirectory``. This approach imposes several restrictions. First, components cannot express dependencies based on Kconfig variables, because Kconfig variables are not known during the early evaluation. Second, CMake script mode does not allow commands that define build targets or actions, which caused confusion about which commands can be used in a component and when they are actually evaluated.

v2 removes this two-stage component evaluation, so the ``BUILD_COMPONENTS`` build property does not exist. Any attempt to obtain it with :cmakev2:ref:`idf_build_get_property` results in an error. ``BUILD_COMPONENTS`` was primarily used in v1 by components to discover which other components were being built and to adjust their behavior, for example by adding source files. In v2, this can be replaced with the ``$<TARGET_EXISTS:tgt>`` generator expression.

For example, in v1 the ``esp_eth`` component is compiled with the additional ``esp_eth_netif_glue.c`` file when the ``esp_netif`` component is also being built:

.. code-block:: cmake

    idf_build_get_property(components_to_build BUILD_COMPONENTS)
    if(esp_netif IN_LIST components_to_build)
        list(APPEND srcs "src/esp_eth_netif_glue.c")
    endif()

To work with v2, the use of ``BUILD_COMPONENTS`` has to be replaced with:

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

    The ``$<TARGET_EXISTS:tgt>`` generator expression works for both v1 and v2. ``IDF_BUILD_V2`` is used here only to show the two different approaches.

.. _cmakev2-early-expansion:

The ``CMAKE_BUILD_EARLY_EXPANSION`` Variable is Never Set
=========================================================

In v1, each component is evaluated twice: an early pass in CMake script mode that collects the component's requirements, followed by the regular pass that evaluates the component with ``add_subdirectory`` and defines its targets. v1 sets the ``CMAKE_BUILD_EARLY_EXPANSION`` variable during the early pass, and components use it to guard work that must run only in the regular pass, such as defining targets or reading build-time state:

.. code-block:: cmake

    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        # In v1, this runs only in the regular pass, not the early one.
        # ...
    endif()

v2 uses :ref:`single-pass evaluation <cmakev2-term-single-pass-evaluation>`: each component is evaluated once, as ordinary CMake code, so there is no early pass and ``CMAKE_BUILD_EARLY_EXPANSION`` is never set. This affects the two ways the variable is used:

* ``if(NOT CMAKE_BUILD_EARLY_EXPANSION)`` (the usual form) is always true, so its body runs once, exactly as in v1's regular pass. This form needs no change and remains valid under both v1 and v2.
* ``if(CMAKE_BUILD_EARLY_EXPANSION)`` runs only during v1's early pass and never runs in v2. Code that relied on it must be reworked, as v2 has no early pass.

.. _cmakev2-kconfig-file-names:

Standardized ``Kconfig`` and ``Kconfig.projbuild`` File Names
=============================================================

v1 allows the use of ``Kconfig`` and ``Kconfig.projbuild`` with custom file names in :cmakev2:ref:`idf_component_register`. This is possible because, in v1, the components participating in the build are collected during early component evaluation, which allows custom names for the Kconfig files to be specified. v2 does not perform early evaluation, and the Kconfig files are collected based on the fixed ``Kconfig`` and ``Kconfig.projbuild`` file names, which must be present in the component's root directory. This can be resolved by renaming the Kconfig files that do not conform to this convention, or by including them from Kconfig files that do.

.. _cmakev2-config-visibility:

Component Configuration Visibility
==================================

In v1, the ``sdkconfig`` is generated from the Kconfig files of the components that are part of the build. This means that if a component is not included in the build, for instance because it is not required as a dependency of another component, its configuration is not visible or available when components are evaluated. In v2, the ``sdkconfig`` is generated from the Kconfig files of all available components. This means that the presence of a configuration option does not guarantee that the component providing it is participating in the build, unlike in v1.

For example, a component might link ``vfs`` only when ``CONFIG_VFS_SUPPORT_IO`` is set. This works in v1 because the option is visible only when ``vfs`` is part of the build:

.. code-block:: cmake

    if(CONFIG_VFS_SUPPORT_IO)
        target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
    endif()

In v2 the option is visible even when ``vfs`` is not in the build, so the condition can be true while the ``idf::vfs`` target does not exist. Because a configuration option no longer implies that its component is present, a component that needs ``vfs`` when the option is set must bring it into the build explicitly with :cmakev2:ref:`idf_component_include` (see :doc:`component-dependencies`):

.. code-block:: cmake

    if(CONFIG_VFS_SUPPORT_IO)
        idf_component_include(vfs)
        target_link_libraries(${COMPONENT_LIB} PUBLIC idf::vfs)
    endif()

.. important::

    This change applies not only to a component's ``CMakeLists.txt`` but also to its source files. The component's code can no longer assume that another component's functionality is available simply because its Kconfig variables are set. For example, if ``CONFIG_VFS_SUPPORT_IO`` is set and the component's code depends on the functionality of the ``vfs`` component, it cannot merely check ``CONFIG_VFS_SUPPORT_IO`` in the source code. It must ensure that the ``vfs`` component is included in the build and that the component declares a dependency on ``vfs`` in its ``CMakeLists.txt``.

.. _cmakev2-recursive-evaluation:

Recursive Evaluation of Components
==================================

In v1, the components participating in the build are collected in the ``BUILD_COMPONENTS`` build property during the early evaluation phase, prior to their evaluation with ``add_subdirectory``. This means v1 is aware of all components participating in the build and their dependencies before CMake evaluates them, which allows v1 to evaluate components non-recursively and within a relatively predictable environment with minimal variables set. In contrast, v2 does not perform early component evaluation, and components are added based on the requirements observed during their evaluation. This means a component can be evaluated recursively within the scope of another component's variables if it is a dependency of that component. In other words, if component A requires component B, then when B is evaluated it can access the variables of A. It is therefore important to ensure that all variables used by a component are properly initialized before use. A typical problem involves CMake lists and the ``APPEND`` operation.

.. code-block:: cmake

    # Wrong
    list(APPEND srcs main.c)

    # Correct
    set(srcs)
    list(APPEND srcs main.c)

.. _cmakev2-project-include-order:

The ``project_include.cmake`` Files are Included in a Non-Specific Order
========================================================================

In v1, the ``project_include.cmake`` files from components are included in the order specified by the ``BUILD_COMPONENTS`` build property. The components in ``BUILD_COMPONENTS`` are sorted based on their requirements, as determined during early evaluation. This means a component's ``project_include.cmake`` is included only after the ``project_include.cmake`` files of its dependencies, as long as there is no cyclic dependency between components. In contrast, v2 does not perform early evaluation of components, and ``BUILD_COMPONENTS`` does not exist. Therefore, in v2 the ``project_include.cmake`` files are included for all discovered components, not just those participating in the build, and in the order in which the components are discovered. As a result, cross-file functionality at the global scope between ``project_include.cmake`` files can no longer be relied upon in v2.

.. note::

    It is still possible to call functions or macros defined in another ``project_include.cmake`` file, provided they are invoked within a CMake function or another non-global scope. Only global-scope interactions are unreliable in v2.

Strict Component Precedence
===========================

v2 strictly adheres to the component precedence for components with the same name, as described in :ref:`cmake-components-same-name`. While v1 allows components discovered in directories specified with the ``EXTRA_COMPONENT_DIRS`` variable to be overridden by `Local Directory Dependencies`_ specified in the ``idf_component.yml`` manifest file, this is no longer possible in v2.

.. _cmakev2-optional-requires:

The Behavior of ``idf_component_optional_requires`` has Changed
===============================================================

In v1, the ``idf_component_optional_requires`` function adds a dependency on a specified component only if that component is already included in the build, for instance because it is already required by another component. To achieve this, v1 examines the ``BUILD_COMPONENTS`` build property, which is generated during the early evaluation phase and lists all components involved in the build.

In v2, there is no early collection phase and ``BUILD_COMPONENTS`` does not exist. The build system discovers components as it evaluates dependencies, so v2 cannot use the same "only if already in the build" check and has to choose a different rule.

The build system supports two behaviors, controlled by the ``IDF_COMPONENT_OPTIONAL_REQUIRES_MODE`` build property:

* **IMMEDIATE (default)**: When a component calls ``idf_component_optional_requires(type req_component)``, the build system includes ``req_component`` and links it to the caller if it is recognized (discovered). No check is made whether the rest of the project actually needs that component. This is safe for multi-binary projects (multiple executables or binaries), but it can pull in more components than necessary and increase build time.

* **DEFERRED**: The build system does not include or link immediately. It records the request and resolves it later in :cmakev2:ref:`idf_build_library`: the optional component is linked only if it ends up in that library's dependency graph. This matches v1 semantics and keeps the number of linked components minimal. It **must not** be used when building more than one library (see below).

A multi-binary project is one that creates more than one executable or binary, for example several application executables built from the same tree (see :doc:`multiple-binaries`). Such a project calls :cmakev2:ref:`idf_build_library` or :cmakev2:ref:`idf_build_executable` more than once. In v2, component targets are shared globally across all libraries. If ``IDF_COMPONENT_OPTIONAL_REQUIRES_MODE`` is set to **DEFERRED**, the build system resolves optional requirements when it processes each library. When it processes the second or a later library, it may add new links to component targets that are already used by the first library. The first library's metadata (such as the list of linker fragments or linked components) was already computed when that library was processed and is not updated. As a result, linker script generation and section placement for the first library can be incorrect or stale. For this reason, DEFERRED mode is not allowed when more than one library is built; the build fails with an error in that case. **IMMEDIATE** mode does not have this problem, because optional requirements are applied during component evaluation, before any per-library metadata is computed. Its side effect is that it can pull in more components than necessary and increase build time.

:cmakev2:ref:`idf_project_default` (the usual entry point for a single-executable project) sets ``IDF_COMPONENT_OPTIONAL_REQUIRES_MODE`` to **DEFERRED** before building the default executable when no libraries have been created yet. So if your project uses ``idf_project_default()`` and builds only one executable, you get DEFERRED behavior automatically and do not need to do anything.

If you do not use :cmakev2:ref:`idf_project_default` and instead call :cmakev2:ref:`idf_project_init` and then the lower-level API (:cmakev2:ref:`idf_build_executable`, :cmakev2:ref:`idf_build_library`) yourself, the default mode is **IMMEDIATE**. If you build only one library or executable and want the same efficient, v1-like behavior as ``idf_project_default``, set the mode to DEFERRED yourself after project init:

.. code-block:: cmake

    idf_project_init()
    idf_build_set_property(IDF_COMPONENT_OPTIONAL_REQUIRES_MODE DEFERRED)
    idf_build_executable(my_app COMPONENTS main ...)
    # ... rest of your project ...

Do **not** set ``IDF_COMPONENT_OPTIONAL_REQUIRES_MODE`` to ``DEFERRED`` if you build multiple libraries; the build will error. Keep the default IMMEDIATE in that case.

.. _Local Directory Dependencies: https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/manifest_file.html#local-directory-dependencies
