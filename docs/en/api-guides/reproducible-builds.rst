Reproducible Builds
===================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

ESP-IDF build system has support for `reproducible builds <https://reproducible-builds.org/docs/definition/>`_.

When reproducible builds are enabled, the application built with ESP-IDF does not depend on the build environment. Both the ``.elf`` file and ``.bin`` files of the application remains exactly the same, even if the following variables change:

- Directory where the project is located
- Directory where ESP-IDF is located (``IDF_PATH``)
- Build time
- Toolchain installation path

Reasons for Non-Reproducible Builds
-----------------------------------

There are several reasons why an application may depend on the build environment, even when the same source code and tools versions are used.

- In C code, ``__FILE__`` preprocessor macro is expanded to the full path of the source file.
- ``__DATE__`` and ``__TIME__`` preprocessor macros are expanded to compilation date and time.
- When the compiler generates object files, it adds sections with debug information. These sections help debuggers, like GDB, to locate the source code which corresponds to a particular location in the machine code. These sections typically contain paths of relevant source files. These paths may be absolute, and will include the path to ESP-IDF or to the project.

There are also other possible reasons, such as unstable order of inputs and non-determinism in the build system.

Enabling Reproducible Builds in ESP-IDF
---------------------------------------

Reproducible builds can be enabled in ESP-IDF using :ref:`CONFIG_APP_REPRODUCIBLE_BUILD` option.

This option is disabled by default. It can be enabled in ``menuconfig``.

The option may also be added into ``sdkconfig.defaults``. If adding the option into ``sdkconfig.defaults``, delete the ``sdkconfig`` file and run the build again. See :ref:`custom-sdkconfig-defaults` for more information.

How Reproducible Builds Are Achieved
------------------------------------

ESP-IDF achieves reproducible builds using the following measures:

- In ESP-IDF source code, ``__DATE__`` and ``__TIME__`` macros are not used when reproducible builds are enabled. Note, if the application source code uses these macros, the build will not be reproducible.
- ESP-IDF build system passes a set of ``-fmacro-prefix-map`` and ``-fdebug-prefix-map`` flags to replace base paths with placeholders:

    - Path to ESP-IDF is replaced with ``/IDF``
    - Path to the project is replaced with ``/IDF_PROJECT``
    - Path to the build directory is replaced with ``/IDF_BUILD``
    - Paths to components are replaced with ``/COMPONENT_NAME_DIR`` (where ``NAME`` is the name of the component)
    - Path to the toolchain is replaced with ``/TOOLCHAIN``

- Build date and time are not included into the :ref:`application  metadata structure <app-image-format-application-description>` and :ref:`bootloader metadata structure <image-format-bootloader-description>` if :ref:`CONFIG_APP_REPRODUCIBLE_BUILD` is enabled.
- ESP-IDF build system ensures that source file lists, component lists and other sequences are sorted before passing them to CMake. Various other parts of the build system, such as the linker script generator also perform sorting to ensure that same output is produced regardless of the environment.

Reproducible Builds and Debugging
---------------------------------

When reproducible builds are enabled, file names included in debug information sections are altered as shown in the previous section. Due to this fact, the debugger (GDB) is not able to locate the source files for the given code location.

This issue can be solved using GDB ``set substitute-path`` command. For example, by adding the following command to GDB init script, the altered paths can be reverted to the original ones.

.. code-block:: none

    set substitute-path /COMPONENT_FREERTOS_DIR /home/user/esp/esp-idf/components/freertos

ESP-IDF build system generates a file with the list of such ``set substitute-path`` commands automatically during the build process. The file is called ``prefix_map_gdbinit`` and is located in the project ``build`` directory.

When :ref:`idf.py gdb <jtag-debugging-with-idf-py>` is used to start debugging, this additional ``gdbinit`` file is automatically passed to GDB. When launching GDB manually or from IDE, please pass this additional ``gdbinit`` script to GDB using ``-x build/prefix_map_gdbinit`` argument.

Factors Which Still Affect Reproducible Builds
----------------------------------------------

Note that the built application still depends on:

- ESP-IDF version
- Versions of the build tools (CMake, Ninja) and the cross-compiler

:doc:`tools/idf-docker-image` can be used to ensure that these factors do not affect the build.
