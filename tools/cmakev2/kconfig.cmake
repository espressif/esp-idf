# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Kconfig utilities for ESP-IDF CMake build system v2
# This module provides functions for Kconfig processing and sdkconfig generation

include_guard(GLOBAL)

include(utilities)
include(build)
include(component)

#[[
   __init_kconfig()

   Initialize the Kconfig system infrastructure for the build.
   This function performs the Kconfig initialization and setup by:
   1. Setting up SDKCONFIG and SDKCONFIG_DEFAULTS build properties
   2. Setting up ESP-IDF root Kconfig files and config directory
   3. Creating the default config directory

   This should be called after component discovery but before component manager.
   Note: Regular component Kconfig files are collected during component discovery.
#]]
function(__init_kconfig)
    idf_build_get_property(idf_path IDF_PATH)

    # Initialize SDKCONFIG and SDKCONFIG_DEFAULTS build properties using environment
    # variables, CMake cache variables, or default values.
    if(EXISTS "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
        set(sdkconfig_defaults "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
    else()
        set(sdkconfig_defaults "")
    endif()

    __get_default_value(VARIABLE SDKCONFIG
                        DEFAULT "${CMAKE_SOURCE_DIR}/sdkconfig"
                        OUTPUT sdkconfig)
    __get_default_value(VARIABLE SDKCONFIG_DEFAULTS
                        DEFAULT "${sdkconfig_defaults}"
                        OUTPUT sdkconfig_defaults)

    __get_absolute_paths(PATHS "${sdkconfig}" OUTPUT sdkconfig)
    __get_absolute_paths(PATHS "${sdkconfig_defaults}" OUTPUT sdkconfig_defaults)

    set(sdkconfig_defaults_checked "")
    foreach(sdkconfig_default ${sdkconfig_defaults})
        if(NOT EXISTS "${sdkconfig_default}")
            idf_die("SDKCONFIG_DEFAULTS '${sdkconfig_default}' does not exist.")
        endif()
        list(APPEND sdkconfig_defaults_checked ${sdkconfig_default})
    endforeach()

    idf_build_set_property(SDKCONFIG "${sdkconfig}")
    idf_build_set_property(SDKCONFIG_DEFAULTS "${sdkconfig_defaults_checked}")

    # Setup ESP-IDF root Kconfig and sdkconfig.rename files.
    idf_build_set_property(__ROOT_KCONFIG "${idf_path}/Kconfig")
    idf_build_set_property(__ROOT_SDKCONFIG_RENAME "${idf_path}/sdkconfig.rename")

    # Setup and create the default config directory.
    idf_build_get_property(build_dir BUILD_DIR)
    set(config_dir "${build_dir}/config")
    file(MAKE_DIRECTORY "${config_dir}")
    idf_build_set_property(CONFIG_DIR "${config_dir}")
endfunction()

#[[
   __generate_sdkconfig()

   This function performs the complete Kconfig generation process:
   1. Collect Kconfig files from discovered components
   2. Collect Kconfig files from bootloader components
   3. Set up the Kconfig environment with all collected files
   4. Generate all output files (sdkconfig.h, sdkconfig.cmake, etc.)
#]]
function(__generate_sdkconfig)
    idf_msg("Generating sdkconfig configuration...")

    # Collect Kconfig files from discovered components
    __consolidate_component_kconfig_files()

    # Collect Kconfig files from bootloader components
    __collect_kconfig_files_from_bootloader_components()

    # Prepare Kconfig environment using collected files
    __setup_kconfig_environment()

    # Generate Kconfig outputs
    __generate_kconfig_outputs()

    idf_msg("Generated sdkconfig configuration")
endfunction()
