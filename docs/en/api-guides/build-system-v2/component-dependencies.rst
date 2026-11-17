Configuring Component Dependencies
**********************************

A component declares the other components it uses, and v2 lets those declarations depend on the project configuration. Expressing dependencies in terms of Kconfig options, so that the set of components built into a project follows its configuration, is the main new capability of v2. This page covers how to declare dependencies and, in particular, how to make them conditional on configuration.

.. important::

    Configuration-conditional dependencies are a v2-only feature. A component that makes its dependencies conditional on ``CONFIG_*`` options will not work as expected under v1. To keep such a component working under both v1 and v2, see :doc:`managing-compatibility`.

How Dependencies Are Declared
=============================

A component brings another component into the build and links it. With :cmakev2:ref:`idf_component_register`, this is the ``REQUIRES`` and ``PRIV_REQUIRES`` arguments (see :ref:`cmakev2-component-compatible`). A native CMake component does it explicitly, by calling :cmakev2:ref:`idf_component_include` and linking the dependency's ``idf::<name>`` interface target (see :ref:`cmakev2-component-native`):

.. code-block:: cmake

    idf_component_include(log)
    target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::log)

With ``idf_component_register``, the :ref:`common components <cmakev2-term-common-components>` (``freertos``, ``log``, and so on) are added automatically, as in v1. A native CMake component receives nothing automatically and must include every component it uses with :cmakev2:ref:`idf_component_include`, including the common ones.

Configuration-Driven Dependencies
==================================

A dependency is declared in one of two ways (see `How Dependencies Are Declared`_): with the ``REQUIRES`` or ``PRIV_REQUIRES`` arguments of :cmakev2:ref:`idf_component_register`, or with :cmakev2:ref:`idf_component_include` in a native CMake component. Because the configuration of all discovered components is available while a component is evaluated, either form can be made conditional on ``CONFIG_*`` options.

The option that drives the dependency is declared in a Kconfig file (see :doc:`/api-guides/kconfig/index`):

.. code-block:: kconfig
    :caption: main/Kconfig.projbuild

    config EXAMPLE_ENABLE_LOGGING
        bool "Enable Logging Utility Component"
        default y

Using ``idf_component_register``
--------------------------------

A component that uses :cmakev2:ref:`idf_component_register` builds its ``PRIV_REQUIRES`` list (or ``REQUIRES`` for a public dependency) conditionally and passes it to the call. This is usually the most natural form for a component coming from v1:

.. code-block:: cmake

    set(reqs)
    if(CONFIG_EXAMPLE_ENABLE_LOGGING)
        list(APPEND reqs logging_util)
    endif()

    idf_component_register(SRCS main.c PRIV_REQUIRES ${reqs})

Initialize the list with ``set(reqs)`` before appending to it, so it does not inherit a value from an enclosing component's scope; see :ref:`cmakev2-recursive-evaluation`.

Using ``idf_component_include``
-------------------------------

A native CMake component expresses the same condition around the :cmakev2:ref:`idf_component_include` call and the ``target_link_libraries``. The :example:`conditional_component example <build_system/cmakev2/features/conditional_component>` is written this way: its ``main`` component depends on the ``logging_util`` and ``math_util`` components only when the corresponding options are enabled.

.. code-block:: cmake
    :caption: main/CMakeLists.txt

    add_library(${COMPONENT_TARGET} STATIC main.c)

    if(CONFIG_EXAMPLE_ENABLE_LOGGING)
        idf_component_include(logging_util)
        target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::logging_util)
    endif()

In both forms, the component's source code uses the same option to compile the dependent code conditionally:

.. code-block:: c

    #ifdef CONFIG_EXAMPLE_ENABLE_LOGGING
    #include "logging_util.h"
    #endif

.. important::

    A dependency must be included with :cmakev2:ref:`idf_component_include` before it is used. In v2 the configuration of every discovered component is visible, so a ``CONFIG_*`` option being set does not mean the component that defines it is part of the build. Do not assume a component is available just because its configuration option is set; include it explicitly. This visibility change from v1 is detailed in :ref:`cmakev2-config-visibility`.

Optional Dependencies
=====================

A component can declare an optional dependency with :cmakev2:ref:`idf_component_optional_requires`, which links another component only if it is already part of the build. This function exists mainly for backward compatibility with v1 and should generally be avoided in v2; prefer making dependencies conditional on configuration options, as described above. When it is used, v2 resolves the dependency in one of two modes, IMMEDIATE or DEFERRED, depending on how the project is built; the modes and their constraints are described in :ref:`cmakev2-optional-requires`.
