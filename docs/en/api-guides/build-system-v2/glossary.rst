.. _cmakev2-glossary:

Glossary
********

This page defines the terms used throughout the Build System v2 documentation. The terms are local to Build System v2; where a term also has a more general meaning in ESP-IDF, the definition here is the one that applies in this documentation.

.. _cmakev2-term-application:

application (app)
   The executable the build system produces from a set of components, together with the binary image flashed to the :ref:`target <cmakev2-term-target>`. A project usually builds two applications: the project application (the firmware) and the bootloader. ``app`` is a synonym.

.. _cmakev2-term-backward-compatible-component:

backward-compatible component
   A component declared with :cmakev2:ref:`idf_component_register`. It builds under both Build System v1 and v2, as long as it stays within the features that v1 also supports. Contrast :ref:`native component <cmakev2-term-native-component>`.

.. _cmakev2-term-build-event-callback:

build event callback
   A function a component registers with :cmakev2:ref:`idf_component_register_build_event_callback` to run at a defined point in the build. The only event currently supported is ``POST_ELF``, fired after the executable is linked and before the binary image is generated. See :doc:`build-event-callbacks`.

.. _cmakev2-term-build-property:

build property
   A setting that is global to the project, stored on the ``idf_build_properties`` target and accessed with :cmakev2:ref:`idf_build_get_property` and :cmakev2:ref:`idf_build_set_property`. Contrast :ref:`component property <cmakev2-term-component-property>` and :ref:`library property <cmakev2-term-library-property>`.

.. _cmakev2-term-common-components:

common components
   The core components, such as ``freertos`` and ``log``, that the build system adds as dependencies of every component declared with :cmakev2:ref:`idf_component_register`, without an explicit ``REQUIRES``. A :ref:`native component <cmakev2-term-native-component>` receives nothing automatically and must include every component it uses, including these.

.. _cmakev2-term-component:

component
   A reusable, separately compiled unit of code: a directory that contains a ``CMakeLists.txt`` file. The build system compiles each component into a library and links the ones the application needs. The component's name is the name of its directory.

.. _cmakev2-term-component-library-target:

component library target
   The library target a component creates, typically with ``add_library``, and whose name the build system passes to the component in the :cmakev2:ref:`COMPONENT_TARGET` variable (also available as ``COMPONENT_LIB``). It holds the component's compiled code. Distinct from the :ref:`interface target <cmakev2-term-interface-target>`.

.. _cmakev2-term-component-property:

component property
   A setting attached to a single component, stored on its :ref:`interface target <cmakev2-term-interface-target>` and accessed with :cmakev2:ref:`idf_component_get_property` and :cmakev2:ref:`idf_component_set_property`. Examples are :cmakev2:ref:`WHOLE_ARCHIVE`, :cmakev2:ref:`LDFRAGMENTS`, and :cmakev2:ref:`LINKER_SCRIPTS`.

.. _cmakev2-term-component-source:

component source
   A location the build system searches for components, each with a precedence. From highest to lowest: ``project_components`` (the project's ``main`` and ``components`` directories), ``project_extra_components`` (directories in ``EXTRA_COMPONENT_DIRS``), ``project_managed_components`` (fetched by the component manager), and ``idf_components`` (bundled with ESP-IDF). A component from a higher-precedence source shadows a same-named component from a lower one. See :doc:`design`.

.. _cmakev2-term-configuration:

configuration (sdkconfig)
   The project's Kconfig-based settings, persisted in the ``sdkconfig`` file and exposed to source code and ``CMakeLists.txt`` files as ``CONFIG_*`` options. In v2 the configuration is generated from the Kconfig of every :ref:`discovered <cmakev2-term-discovery>` component, not only the ones in the build. See :doc:`/api-guides/kconfig/index`.

.. _cmakev2-term-discovery:

discovery
   Registering a component with the build system: recording its directory, Kconfig files, and ``project_include.cmake``, and creating its :ref:`interface target <cmakev2-term-interface-target>`. A discovered component is known and configurable, but is not compiled. Contrast :ref:`inclusion <cmakev2-term-inclusion>`.

.. _cmakev2-term-inclusion:

inclusion
   Bringing a discovered component into the build by evaluating it: the build system calls ``add_subdirectory`` on it, which runs its ``CMakeLists.txt`` and creates its :ref:`component library target <cmakev2-term-component-library-target>`. Only included components are compiled and linked. :cmakev2:ref:`idf_component_include` performs the inclusion. Contrast :ref:`discovery <cmakev2-term-discovery>`.

.. _cmakev2-term-interface-alias:

interface alias
   The ``idf::<name>`` alias of a component's :ref:`interface target <cmakev2-term-interface-target>`. It is the canonical, readable way for other components to refer to a component, for example ``idf::spi_flash``.

.. _cmakev2-term-interface-target:

interface target
   The ``idf_<name>`` target created for a component at :ref:`discovery <cmakev2-term-discovery>`. It carries the component's properties and is what other components link against, usually through its :ref:`interface alias <cmakev2-term-interface-alias>` ``idf::<name>``. Distinct from the :ref:`component library target <cmakev2-term-component-library-target>`.

.. _cmakev2-term-library:

library
   An interface library produced by :cmakev2:ref:`idf_build_library` that bundles a set of components and their transitive dependencies. Linking it into an executable brings in those components' code, include paths, and link options. Not to be confused with a component's own :ref:`library target <cmakev2-term-component-library-target>` or the final binary image.

.. _cmakev2-term-library-property:

library property
   A setting recorded for a single :ref:`library <cmakev2-term-library>` produced by :cmakev2:ref:`idf_build_library`, such as the list of components it linked. Contrast :ref:`build property <cmakev2-term-build-property>` and :ref:`component property <cmakev2-term-component-property>`.

.. _cmakev2-term-linker-fragment:

linker fragment
   A file, contributed through a component's :cmakev2:ref:`LDFRAGMENTS` property, that the linker script generator uses to place a component's code and data into memory regions. Distinct from a linker script, which is added with :cmakev2:ref:`LINKER_SCRIPTS`. See :doc:`/api-guides/linker-script-generation`.

.. _cmakev2-term-main-component:

main component
   The component named ``main`` from which :cmakev2:ref:`idf_project_default` builds the default application. Building from ``main`` is a convention of :cmakev2:ref:`idf_project_default`, not a requirement of the build system; a project that uses the lower-level API can build its application from any component.

.. _cmakev2-term-native-component:

native component
   A component written as a plain CMake static library, created directly with ``add_library`` instead of :cmakev2:ref:`idf_component_register`. It has full access to native CMake, but on its own builds only under v2. Contrast :ref:`backward-compatible component <cmakev2-term-backward-compatible-component>`.

.. _cmakev2-term-optional-dependency:

optional dependency
   A dependency declared with :cmakev2:ref:`idf_component_optional_requires` that links another component only when it is part of the build. v2 resolves it in one of two modes, ``IMMEDIATE`` or ``DEFERRED``; see :ref:`cmakev2-optional-requires`.

.. _cmakev2-term-project:

project
   A directory that contains a top-level ``CMakeLists.txt``, usually a :ref:`main component <cmakev2-term-main-component>`, and optionally a ``components`` directory with additional components. It builds into one or more :ref:`applications <cmakev2-term-application>`. See :doc:`creating-project`.

.. _cmakev2-term-single-pass-evaluation:

single-pass evaluation
   The v2 design property that each component is :ref:`evaluated <cmakev2-term-inclusion>` exactly once, as ordinary CMake code. v1 instead evaluated components twice, with an early pass in CMake script mode. See :doc:`design`.

.. _cmakev2-term-target:

target
   The chip the project is built for, for example ``esp32``, selected with ``idf.py set-target`` and available as the :cmakev2:ref:`IDF_TARGET` build property and the ``CONFIG_IDF_TARGET`` option. The word "target" also refers to a CMake build target, as in :ref:`interface target <cmakev2-term-interface-target>`; this documentation uses "target" for the chip unless qualified.
