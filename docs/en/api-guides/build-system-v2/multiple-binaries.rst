Building Multiple Binaries
**************************

A project does not have to produce a single application; it can build several independent binaries from one source tree, going beyond the single-application default of :cmakev2:ref:`idf_project_default`. This fits a set of related applications that share a common component base and the same configuration but differ in which components they include, for example several feature or product variants of the same firmware; building them from one project compiles the shared components once and keeps them consistent across binaries. Applications that each need their own configuration should be separate projects. To build one application in several configurations instead, see :doc:`multiple-configurations`.

To build more than one executable from a project, call :cmakev2:ref:`idf_project_init` once and then :cmakev2:ref:`idf_build_executable` for each binary, giving each its own set of components. Component targets are shared across the whole project, so a component used by several executables is built only once. The :example:`multi_binary example <build_system/cmakev2/features/multi_binary>` builds two applications, ``app1`` and ``app2``, from shared and distinct components:

.. code-block:: cmake
    :caption: CMakeLists.txt

    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
    project(multi_binary C CXX ASM)
    idf_project_init()

    idf_build_executable(app1.elf COMPONENTS app1_main component1 component2)
    idf_build_executable(app2.elf COMPONENTS app2_main component1 component2 component3)

Each executable then gets its own binary, flash, and configuration targets, named so that they do not collide:

.. code-block:: cmake

    idf_build_binary(app1.elf OUTPUT_FILE "${CMAKE_BINARY_DIR}/app1.bin" TARGET app1_binary)
    idf_flash_binary(app1_binary TARGET app1-flash NAME "app1" FLASH)
    idf_create_menuconfig(app1.elf TARGET app1-menuconfig)

    idf_build_binary(app2.elf OUTPUT_FILE "${CMAKE_BINARY_DIR}/app2.bin" TARGET app2_binary)
    idf_flash_binary(app2_binary TARGET app2-flash NAME "app2")

    add_custom_target(app ALL DEPENDS app1.bin app2.bin)

A single build now produces both ``app1.bin`` and ``app2.bin``, each with its own ``appN-flash`` and ``appN-menuconfig`` targets.

.. note::

    A multi-binary project has a single project-wide ``sdkconfig``, and a
    component shared between executables is one instance with one
    configuration. All ``appN-menuconfig`` targets therefore edit the same
    ``sdkconfig``: changing a shared component's option through any of them
    affects every executable that uses that component. To build the same
    application in genuinely independent configurations, use
    :doc:`multiple-configurations` instead.

.. important::

    A multi-binary project builds more than one library, so it must use the **IMMEDIATE** optional-requires mode, which is the default. Do not set ``IDF_COMPONENT_OPTIONAL_REQUIRES_MODE`` to ``DEFERRED`` when building multiple libraries; the build fails with an error. See :ref:`cmakev2-optional-requires`.
