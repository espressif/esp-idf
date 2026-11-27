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

To emit the ``project_description.json`` and gdbinit files that tooling such as ``idf.py gdb`` consumes, call :cmakev2:ref:`idf_build_generate_metadata` for each executable. These outputs are written to project-wide default paths, so every executable past the first must be given a distinct ``OUTPUT_FILE`` and ``GDBINIT_DIR`` to keep its metadata and gdbinit files from overwriting the others':

.. code-block:: cmake

    idf_build_generate_metadata(BINARY app1_binary)
    idf_build_generate_metadata(BINARY app2_binary
                                OUTPUT_FILE "${CMAKE_BINARY_DIR}/project_description_app2.json"
                                GDBINIT_DIR "${CMAKE_BINARY_DIR}/gdbinit/app2")

Each executable's ``project_description.json`` then references its own ELF and points ``gdbinit_files`` at its own directory, so a debugger loads the symbols of the executable it was launched for.

Tools read the images to flash from ``flasher_args.json``. To generate this file, call :cmakev2:ref:`idf_build_generate_flasher_args`. Without arguments, the file describes the global ``flash`` target. In this example, only ``app1`` is added to the global ``flash`` target with ``FLASH``. Both applications use the same flash offset, and the global ``flash`` target can contain only one image at an offset. For ``app2``, pass its binary with ``BINARY`` and a distinct ``OUTPUT_FILE`` in the build directory:

.. code-block:: cmake

    idf_build_generate_flasher_args()
    idf_build_generate_flasher_args(BINARY app2_binary
                                    OUTPUT_FILE "${CMAKE_BINARY_DIR}/flasher_args_app2.json")

``flasher_args_app2.json`` then contains the images of ``flasher_args.json``, with ``app2.bin`` instead of ``app1.bin``.

The build system also writes ``project_index.json`` to the build directory. For each executable target, this file lists the project description and flasher args files generated above. Tools can use it to find the files of each application:

.. code-block:: json

    {
        "version": 1,
        "apps": {
            "app1.elf": {
                "project_description": ["project_description.json"],
                "flasher_args": ["flasher_args.json"]
            },
            "app2.elf": {
                "project_description": ["project_description_app2.json"],
                "flasher_args": ["flasher_args_app2.json"]
            }
        }
    }

The format is described in :cmakev2:ref:`idf_project_init`.

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
