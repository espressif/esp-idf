.. _cmakev2-design:

Design and Architecture
***********************

This page explains how Build System v2 works internally: the stages it runs through, how it models components, how it evaluates and links them, and how configuration is handled. It is a conceptual overview. For task oriented instructions, see the guides on the :doc:`Build System v2 <index>` page; for the exact function and variable reference, see :doc:`api`; for the differences from v1 that may require changes to a component, see :doc:`breaking-changes`.

Overview
========

v2 builds a project from components. A component is a reusable, separately compiled unit of code: a directory that contains a ``CMakeLists.txt`` file. The build system discovers the available components, evaluates the ones the project needs, builds each into a library, and links them into an application executable, from which the final binary image is generated.

Three design choices distinguish v2 from v1 and shape the rest of this document:

- **Single-pass component evaluation.** v2 evaluates each component once, as ordinary CMake code. v1 evaluated components twice: an early pass in CMake script mode to collect dependencies, followed by the real pass. Removing the early pass makes a component's behavior easier to predict and enables the other two changes below.
- **Global configuration visibility.** The project configuration (``sdkconfig``) is generated from the Kconfig of every discovered component, not only the components linked into the build. Configuration can therefore be queried before the dependency graph is known, which is what allows dependencies to be expressed in terms of configuration.
- **Native CMake components.** Because there is no early script-mode pass, a component can be a plain CMake target created with ``add_library``, with full access to native CMake features. The :cmakev2:ref:`idf_component_register` function is still provided for components that must also build under v1.

At a high level, a build proceeds through the following stages:

.. code-block:: text

    components  ->  discovery  ->  configuration  ->  evaluation  ->  libraries  ->  executable  ->  image

The Build Process
=================

A v2 build runs in three stages. The first two are set up by the project's top level ``CMakeLists.txt``:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.22)
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)    # stage 1
    project(my_project C CXX ASM)                      # CMake project setup
    idf_project_default()                              # stages 2 and 3

Stage 1: Infrastructure Initialization
--------------------------------------

Including ``idf.cmake`` prepares the build system before CMake's ``project()`` command runs. This stage does not look at the project's components yet. It:

- loads the v2 CMake modules (component, build, kconfig, project, manager, compat, ldgen, and the image helpers);
- creates the ``idf_build_properties`` interface target that holds global build properties (see `Properties`_), and sets ``IDF_PATH``, ``PREFIX``, ``PROJECT_DIR``, and ``BUILD_DIR``;
- sets the build system version properties ``IDF_BUILD_V2``, ``IDF_BUILD_VER`` (``2``), and ``IDF_BUILD_VER_TAG``;
- determines and validates ``IDF_TARGET``, selects the matching toolchain file, and checks the Python environment and Git submodules.

``idf.cmake`` must be included before ``project()`` because it configures the toolchain that ``project()`` then uses.

Stage 2: Project Initialization
-------------------------------

:cmakev2:ref:`idf_project_init` runs after ``project()`` (it is called for you by :cmakev2:ref:`idf_project_default`). This is where the project's components come into play. In order, it:

1. discovers components and initializes each one (see `The Component Model`_), without evaluating them;
2. generates the initial ``sdkconfig`` from the Kconfig of all discovered components (see `Configuration and Visibility`_);
3. runs the component manager if enabled, which may add managed components and regenerate ``sdkconfig`` until the component set converges (see `Component Manager Integration`_);
4. includes the generated ``sdkconfig.cmake`` so that ``CONFIG_*`` values become CMake variables;
5. derives the global compile options, compile definitions, and link options from the configuration;
6. includes the ``project_include.cmake`` file of every discovered component, in discovery order, in the global scope.

Because step 6 evaluates files in the global scope, :cmakev2:ref:`idf_project_init` is a macro and must be called from the project's top level ``CMakeLists.txt``. Note that ``project_include.cmake`` files are included for all discovered components in discovery order, not in dependency order as in v1; see :ref:`cmakev2-project-include-order`.

Stage 3: Build Definition
-------------------------

The final stage defines what is actually built. :cmakev2:ref:`idf_project_default` builds the default application from the ``main`` component and its dependencies and adds the binary, flash, and utility targets. Internally it calls :cmakev2:ref:`idf_build_executable` (see `Libraries, Executables, and Linking`_). Projects that need more than one binary, or that drive the build from an external CMake project, call the lower level functions directly; see :doc:`multiple-binaries` and :doc:`idf-as-library`.

The Component Model
===================

A component is a directory that contains a ``CMakeLists.txt`` file. The component's name is the name of its directory. Each component is built into its own library (typically a static library, or an interface library when it has no source files) and can declare dependencies on other components.

Component Sources and Precedence
--------------------------------

The build system looks for components in several locations, each associated with a *source* that carries a priority. From highest to lowest priority:

.. list-table::
   :header-rows: 1
   :widths: 30 14 56

   * - Source
     - Priority
     - Where the components come from
   * - ``project_components``
     - 3 (highest)
     - The project's ``main`` and ``components`` directories (or ``COMPONENT_DIRS``)
   * - ``project_extra_components``
     - 2
     - Directories listed in ``EXTRA_COMPONENT_DIRS``
   * - ``project_managed_components``
     - 1
     - Components fetched by the component manager
   * - ``idf_components``
     - 0 (lowest)
     - Components bundled with ESP-IDF (``$IDF_PATH/components``)

When two directories provide a component with the same name, the higher priority one wins and shadows the other; two components of the same name at the same priority are an error. This lets a project override a bundled ESP-IDF component by placing a component of the same name in its own ``components`` directory.

Components published under a namespace (for example ``espressif__led_strip``) are also reachable under their short name (``led_strip``) when that short name is unambiguous.

.. _cmakev2-design-discovery-inclusion:

Discovery vs. Inclusion
-----------------------

Discovery and inclusion are distinct, and the distinction is central to v2.

- **Discovery** registers a component: the build system records its directory, Kconfig files, and ``project_include.cmake``, creates its interface target, and makes its configuration available. A discovered component is *known* but is not built.
- **Inclusion** evaluates a component: the build system calls ``add_subdirectory`` on it, which runs its ``CMakeLists.txt`` and creates its library target. Only included components are compiled and linked.

All available components are discovered, but only the components actually required by the application are included. For example, a default build of the ``hello_world`` example discovers around 150 components yet includes only about 55, the ones that ``main`` transitively needs. The rest stay configurable (their options still appear in ``menuconfig``) but contribute no code.

Interface Targets and the Component Library
-------------------------------------------

Each component is represented by two CMake targets:

- An **interface target**, named ``idf_<name>``, created at discovery. It carries the component's properties and is what other components link against. It has a convenience alias ``idf::<name>``.
- A **component library target**, whose name is given to the component in the ``COMPONENT_TARGET`` variable. The component is responsible for creating this target (for example with ``add_library``); it holds the component's compiled code.

When a component is included, the build system links its library target into its interface target. A component that depends on another therefore links the dependency's ``idf::<name>`` interface, and CMake propagates the dependency's include directories and library transitively. Other components reference a component only through its interface target, never its library target directly.

Single-Pass Component Evaluation
================================

A component is evaluated exactly once. :cmakev2:ref:`idf_component_include` performs the evaluation: it calls ``add_subdirectory`` the first time the component is requested, and on any later request it returns immediately because the component is already evaluated. A per-component flag records that the component has been included, so repeated requests from different dependents are cheap and never re-run the component's ``CMakeLists.txt``.

Evaluation is recursive and depth first. While a component is being evaluated, its own calls to :cmakev2:ref:`idf_component_include` (directly, or through the ``REQUIRES`` of :cmakev2:ref:`idf_component_register`) evaluate its dependencies before it finishes. The application's dependency graph is thus realized by walking it outward from ``main``, rather than by collecting it in advance as v1 did.

Two consequences follow from evaluating components as ordinary, nested CMake code:

- **Variable hygiene.** A component may be evaluated within the variable scope of the component that pulled it in. A component must initialize every variable it uses rather than relying on it being unset. See :ref:`cmakev2-recursive-evaluation`.
- **No precomputed component list.** There is no point at which the full set of components is known before evaluation, so the v1 ``BUILD_COMPONENTS`` property does not exist in v2. See :ref:`cmakev2-build-components`.

Circular dependencies are tolerated because the interface target of every component exists from discovery, before any component is evaluated. If component A requires B and B requires A, then while B is being evaluated it can already link A's interface target, and A finishes evaluating after B. The build system tracks the chain of in-progress evaluations to avoid infinite recursion.

Dependency Resolution
=====================

A component declares the components it needs and links their interface targets. With :cmakev2:ref:`idf_component_register`, this is done through ``REQUIRES`` (public dependencies, propagated to dependents) and ``PRIV_REQUIRES`` (private dependencies). The function includes each required component and links its ``idf::<name>`` interface, public or private. A native CMake component does the same explicitly, by calling :cmakev2:ref:`idf_component_include` and ``target_link_libraries(... idf::<name>)``.

With ``idf_component_register``, the :ref:`common components <cmakev2-term-common-components>` (``freertos``, ``log``, ``esp_system``, and so on) are still added automatically, as in v1. A native CMake component receives nothing automatically: it must declare every component it uses with ``idf_component_include``, including those common ones.

Because the full configuration is available during evaluation (see `Configuration and Visibility`_), a component can decide its dependencies based on ``CONFIG_*`` options. This is the main new capability in v2 and is covered in :doc:`component-dependencies`. Optional dependencies, linked only when the other component is already part of the build, are available through ``idf_component_optional_requires``; its two resolution modes are described in :ref:`cmakev2-optional-requires`.

Configuration and Visibility
============================

Project configuration uses Kconfig, as in v1. Each component may provide a ``Kconfig`` file, and a ``Kconfig.projbuild`` for project wide options. These files must use exactly those names in the component's root directory; see :ref:`cmakev2-kconfig-file-names`. v2 collects the Kconfig files from **every discovered component** and generates the project configuration from all of them, regardless of which components end up in the build.

The configuration is generated into several files under ``build/config``:

- ``sdkconfig``: the human-readable, persisted configuration, kept in the project directory;
- ``sdkconfig.h``: C and C++ preprocessor defines, included by component source code;
- ``sdkconfig.cmake``: ``set(CONFIG_* ...)`` statements, included during stage 2 so that component ``CMakeLists.txt`` files can read ``CONFIG_*`` variables;
- ``sdkconfig.json``: a machine-readable form for tools.

Generating configuration from all discovered components is what makes configuration available before the dependency graph is resolved, and therefore what makes configuration-driven dependencies possible. It also has an important implication: the presence of a ``CONFIG_*`` option does not mean that the component defining it is part of the build. For example, in a default ``hello_world`` build, ``CONFIG_LWIP_MAX_SOCKETS`` is defined even though the ``lwip`` component is only discovered, not linked. Component code and ``CMakeLists.txt`` files must therefore not assume that another component is present merely because its configuration is set. This difference from v1 is detailed in :ref:`cmakev2-config-visibility`.

Component Manager Integration
=============================

The :doc:`component manager </api-guides/tools/idf-component-manager>` lets a component declare dependencies on components from the ESP Component Registry, from Git, or from local paths, in an ``idf_component.yml`` manifest. In v2, the components the manager resolves are added as a component source (``project_managed_components``) and are then discovered and included like any other component.

Because configuration affects which components a project uses, and managed components bring their own Kconfig, the manager runs as part of stage 2 in an iterative loop: it resolves and downloads dependencies, the configuration is regenerated to include the new components' Kconfig, and the process repeats until the component set is stable. The requirements the manager resolves for each component are injected back as that component's ``REQUIRES`` and ``PRIV_REQUIRES``.

Libraries, Executables, and Linking
===================================

The application is assembled by two functions.

:cmakev2:ref:`idf_build_library` aggregates a set of components into a single interface library. Given a list of components, it includes each one (pulling in transitive dependencies), links their interface targets into the library, and records which components were actually linked. It also collects each linked component's linker fragments and archives for linker script generation, processes the component linker scripts, and runs the component validation checks. The library is itself an interface target: it carries no code of its own, only the aggregated include directories, libraries, and link options.

:cmakev2:ref:`idf_build_executable` builds an application on top of :cmakev2:ref:`idf_build_library`. It creates an internal library from the requested components, creates the executable target, and links the library into it, so the executable inherits every linked component's code, include paths, link options, and linker scripts. It can also produce a linker map file. After the executable target is created, the build system fires the ``POST_ELF`` build event, which lets a component run an action on the linked ELF; see :doc:`build-event-callbacks`.

:cmakev2:ref:`idf_project_default` is the common case: it calls :cmakev2:ref:`idf_build_executable` with the ``main`` component to build a single application. A project can instead call these functions directly to build several binaries, or more than one library. Component targets are created once and shared across all libraries in the project. Multiple binary builds are described in :doc:`multiple-binaries`, and using the build system from an external CMake project in :doc:`idf-as-library`.

Linker Script Generation
------------------------

Placement of code and data into memory regions is controlled by linker fragments, as in v1. Each component can contribute linker fragment files through its ``LDFRAGMENTS`` property. When a library is built, the build system gathers the fragment files and the archives of the linked components and runs the ``ldgen`` tool, which expands a linker script template into the final linker script used for the link. Static linker scripts are added directly, and template scripts are generated per library so that a component linked into more than one library does not collide with itself. For the linker fragment format, see :doc:`/api-guides/linker-script-generation`.

Properties
==========

The build system stores its state on CMake interface targets used as property bags, rather than in global variables. There are three kinds:

- **Build properties**: global to the project, stored on the ``idf_build_properties`` target. Accessed with :cmakev2:ref:`idf_build_set_property` and :cmakev2:ref:`idf_build_get_property` (for example ``IDF_TARGET``, ``IDF_PATH``, ``COMPONENTS_DISCOVERED``).
- **Component properties**: per component, stored on the component's interface target. Accessed with :cmakev2:ref:`idf_component_set_property` and :cmakev2:ref:`idf_component_get_property` (for example ``WHOLE_ARCHIVE``, ``LDFRAGMENTS``, ``LINKER_SCRIPTS``). A property can be read by component name, alias, or target.
- **Library properties**: per library produced by :cmakev2:ref:`idf_build_library`.

Properties can be appended to, and can be returned as generator expressions for use at generate time. The full list of public functions and properties is in :doc:`api`.

Generated Artifacts and Targets
===============================

From the linked executable (the ELF), :cmakev2:ref:`idf_project_default` produces the application binary image and the supporting targets:

- the **binary image** (``.bin``), generated from the ELF and, when secure boot signing is enabled, signed;
- **flash targets** (``flash`` and ``app-flash``) that write the image to the device, and ``app`` that builds it;
- **metadata**: ``project_description.json``, describing the project, its configuration, and its components, for IDEs and other tools;
- **configuration targets**: ``menuconfig``, ``confserver``, ``save-defconfig``, and ``config-report``;
- **analysis and packaging targets**: ``size`` (binary size report) and ``uf2`` (USB flashing image), plus ``dfu`` on targets that support it.

These targets are normally invoked through ``idf.py`` rather than directly. The set of targets follows the configuration and target; for example, the DFU targets are created only on chips that support DFU.
