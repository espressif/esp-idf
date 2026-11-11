Subprojects
***********

Some parts of an ESP-IDF application are separate programs, for example the bootloader, ESP-TEE and the program the ULP coprocessor runs. Each of them is built as a subproject, a CMake project that the application's build adds with ``externalproject_add``. A subproject has its own CMake cache and build directory, and the application uses what it builds, for example by embedding its binary.

A subproject is built together with the application because the two depend on each other. The ULP coprocessor program, for example, compiles some of the application's sources, shares data with the application in RTC memory, and its symbols are linked into the application.

Adding a subproject
===================

:cmakev2:ref:`idf_subproject_add` adds a subproject. It calls ``externalproject_add`` for you and sets up the subproject's configuration. Pass it the name of the subproject, then the keyword ``EXTERNALPROJECT_OPTIONS`` followed by the options for ``externalproject_add``, such as ``SOURCE_DIR``.

.. code-block:: cmake

    idf_subproject_add(sensor
        EXTERNALPROJECT_OPTIONS
            SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sensor")

:cmakev2:ref:`idf_subproject_add` sets these ``externalproject_add`` options unless they are set in ``EXTERNALPROJECT_OPTIONS``:

- ``BINARY_DIR``, the subproject's build directory, is ``build/subprojects/<name>``, where ``<name>`` is the name of the subproject.
- ``BUILD_ALWAYS`` is on, so the subproject's build tool runs on every build of the main project and rebuilds what has changed, for example after a change to the subproject's sources or to the main project's configuration. If it is off, the subproject is built only after its configure step runs again, and a change to its sources alone is not built.
- ``INSTALL_COMMAND`` is empty, so the subproject has no install step. The install step runs after the build and usually copies the subproject's outputs to another directory, for example with ``cmake --install``. The main project usually takes the outputs from the subproject's build directory, so the step is not needed. Set ``INSTALL_COMMAND`` if the outputs must be copied somewhere else.

The subproject builds no bootloader. The main project builds the bootloader that the application uses.

Every subproject is built with a configuration, the values of the config options that its components use. The configuration comes from one of two places.

- The subproject's own configuration, which is the default. The subproject resolves the config options of the components it discovers and keeps the values in an ``sdkconfig`` of its own. Use it when the subproject's components add config options of their own, as the components of the ULP coprocessor program do. See `Subproject with a configuration of its own`_.
- The main project's configuration. The subproject is built with the main project's values and keeps no ``sdkconfig`` of its own. Use it when the subproject's components add no config options of their own, as with the bootloader. Pass ``SHARED_CONFIG`` to :cmakev2:ref:`idf_subproject_add` to choose it. See `Subproject that shares the main project's configuration`_.

The options of :cmakev2:ref:`idf_subproject_add` set up the subproject's configuration. ``SDKCONFIG``, ``SDKCONFIG_DEFAULTS`` and ``GENERATE_SDKCONFIG`` in the environment do not apply to the subproject.

Subproject with a configuration of its own
==========================================

The subproject resolves its configuration from the Kconfig files of the components it discovers and keeps it in an ``sdkconfig`` of its own. It is configured the same way as an application.

- Its components can add config options.
- The ``sdkconfig.defaults`` in its directory sets its default values.
- ``idf.py set-target`` in the main project changes the target of the subproject too.

Each project resolves its own values, so a config option that both projects declare can have a different value in each. Sometimes the two values must be the same, for example when a source file is compiled into both projects, or when the subproject uses memory that the main project reserves. In that case, import the config option from the main project, see `Importing a config option of the main project`_.

Where the configuration is kept
-------------------------------

The subproject keeps its configuration in the file ``sdkconfig`` in its ``SOURCE_DIR``. To keep it in another file, set the path with the ``SDKCONFIG`` option. A relative path is relative to the subproject's source directory.

.. code-block:: cmake

    idf_subproject_add(sensor
        SDKCONFIG sdkconfig.sensor
        EXTERNALPROJECT_OPTIONS
            SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sensor")

Set the path with ``SDKCONFIG`` of :cmakev2:ref:`idf_subproject_add` only. The build stops with an error if the subproject's ``CMakeLists.txt`` sets ``SDKCONFIG`` to another path, if ``CMAKE_ARGS`` sets ``-DSDKCONFIG``, or if the path is the main project's ``sdkconfig``.

Subprojects built from the same directory use the same ``sdkconfig``, as the executables of a multi-binary application do. The build configures and builds them one at a time, so that only one of them writes the file at a time. Each of them writes the config options of the components it discovers, so they should discover the same components. To keep their configurations apart, give each of them its own path with ``SDKCONFIG``.

Importing a config option of the main project
---------------------------------------------

List the config options that must have the main project's value in ``CONFIG_OPTIONS``.

.. code-block:: cmake

    idf_subproject_add(sensor
        CONFIG_OPTIONS CONFIG_FREERTOS_UNICORE CONFIG_SENSOR_FILTER_WINDOW
        EXTERNALPROJECT_OPTIONS
            SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sensor")

The list can also come from a variable, for example ``CONFIG_OPTIONS ${shared_options}``.

Each time the subproject is configured, it gets the main project's value of each listed config option.

- The value is in the subproject's ``sdkconfig.h``, ``sdkconfig.cmake``, ``sdkconfig.json`` and ``sdkconfig`` under its usual ``CONFIG_`` name, so C sources, CMake code, Kconfig conditions, linker scripts and linker fragments use it like any other value.
- When the main project's value changes, the subproject is configured again on the next build.
- A value set in the subproject's configuration, for example with menuconfig, is replaced the next time the subproject is configured, and a warning names the config option.
- All other config options keep the subproject's own values, including the values from its ``sdkconfig.defaults``.

Which config options to list:

- A config option that a component of the subproject declares. The config options of ESP-IDF components are declared in every subproject. A config option of a component of the main project is declared if the subproject discovers that component too, for example through ``EXTRA_COMPONENT_DIRS``.
- A config option that is set, rather than one that is derived from others. A config option without a prompt, such as ``CONFIG_FREERTOS_NUMBER_OF_CORES``, takes its value from the config options it depends on, so list those, here ``CONFIG_FREERTOS_UNICORE``. A config option without a prompt can be listed as well, and is then checked rather than imported.
- For a choice, all of its config options.

After the subproject's configuration is resolved, the build checks that each listed config option has the same value in both projects. A bool option that is off counts as the same as a config option without a value. If a value differs, the build stops with an error that gives both values. This happens when:

- The subproject derives another value for a config option without a prompt.
- The dependencies of the config option are met in one project only.
- No component of the subproject declares the config option.

Importing the config options that the listed one depends on usually fixes the error. A listed name that has no value in either project gives a warning, because it may be misspelled.

Subproject that shares the main project's configuration
=======================================================

Add ``SHARED_CONFIG`` to build the subproject with the main project's configuration.

.. code-block:: cmake

    idf_subproject_add(sensor SHARED_CONFIG
        EXTERNALPROJECT_OPTIONS
            SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sensor")

- The subproject reads the main project's ``sdkconfig`` and the files generated from it, and does not write them.
- Its components are built with the main project's values, including those from the main project's ``sdkconfig.defaults``.
- It suits a subproject whose components add no config options. A config option that only a component of the subproject declares is not in the main project's ``sdkconfig``, so it gets its default value.

``SDKCONFIG`` and ``CONFIG_OPTIONS`` apply to a subproject with a configuration of its own only.
