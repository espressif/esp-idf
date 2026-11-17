Building Multiple Configurations
********************************

A project can build the same application repeatedly with different configurations, each into its own build directory, going beyond the single-configuration default of :cmakev2:ref:`idf_project_default`. To build several different applications from one source tree instead, see :doc:`multiple-binaries`.

This is done with CMake presets. Each preset selects its own ``sdkconfig.defaults`` files and its own build directory, so the configurations are built side by side without affecting each other. The :example:`multi_config example <build_system/cmakev2/features/multi_config>` defines a default (development) preset and two production presets:

.. code-block:: json
    :caption: CMakePresets.json

    {
        "version": 3,
        "configurePresets": [
            {
                "name": "default",
                "binaryDir": "build/default",
                "cacheVariables": {
                    "SDKCONFIG": "./build/default/sdkconfig"
                }
            },
            {
                "name": "prod1",
                "binaryDir": "build/prod1",
                "cacheVariables": {
                    "SDKCONFIG_DEFAULTS": "sdkconfig.defaults.prod_common;sdkconfig.defaults.prod1",
                    "SDKCONFIG": "./build/prod1/sdkconfig"
                }
            },
            {
                "name": "prod2",
                "binaryDir": "build/prod2",
                "cacheVariables": {
                    "SDKCONFIG_DEFAULTS": "sdkconfig.defaults.prod_common;sdkconfig.defaults.prod2",
                    "SDKCONFIG": "./build/prod2/sdkconfig"
                }
            }
        ]
    }

So that the configurations do not interfere, the project keeps each ``sdkconfig`` in its build directory:

.. code-block:: cmake
    :caption: CMakeLists.txt

    set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")
    include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
    project(multi_config C CXX ASM)
    idf_project_default()

Build a specific configuration by selecting its preset:

.. code-block:: bash

    idf.py --preset prod1 build

Each preset builds into its own directory with its own configuration, from the same application sources.
