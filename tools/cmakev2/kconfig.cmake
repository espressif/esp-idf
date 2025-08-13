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

# =============================================================================
# KCONFIG COLLECTION FUNCTIONS
# =============================================================================

#[[
   __consolidate_component_kconfig_files()

   Consolidate Kconfig files from discovered components into global build properties.
   This scans the COMPONENTS_DISCOVERED build property and for each component,
   retrieves its Kconfig files from component properties and adds them to the
   global __KCONFIGS, __KCONFIG_PROJBUILDS, and __SDKCONFIG_RENAMES build properties.
#]]
function(__consolidate_component_kconfig_files)
    idf_build_get_property(components_discovered COMPONENTS_DISCOVERED)
    if(NOT components_discovered)
        idf_die("No components discovered. This must be run after component discovery.")
    endif()

    # Clean the build properties before adding new ones. This ensures that
    # we don't have duplicate Kconfig files in the build properties.
    idf_build_set_property(__KCONFIGS "")
    idf_build_set_property(__KCONFIG_PROJBUILDS "")
    idf_build_set_property(__SDKCONFIG_RENAMES "")
    idf_build_set_property(__KCONFIGS_EXCLUDED "")
    idf_build_set_property(__KCONFIGS_PROJBUILD_EXCLUDED "")

    # Iterate through all discovered components and consolidate their Kconfig files
    foreach(component_name IN LISTS components_discovered)
        # Get Kconfig files from component properties
        idf_component_get_property(component_kconfig "${component_name}" __KCONFIG)
        idf_component_get_property(component_projbuild "${component_name}" __KCONFIG_PROJBUILD)
        idf_component_get_property(component_rename "${component_name}" __SDKCONFIG_RENAME)

        # Check if component is included in build
        # TODO: Review this integration hook: Component system sets COMPONENT_INCLUDED property
        idf_component_get_property(component_included "${component_name}" COMPONENT_INCLUDED)

        # Add to appropriate build properties based on component inclusion status
        if(component_included)
            if(component_kconfig)
                idf_build_set_property(__KCONFIGS "${component_kconfig}" APPEND)
            endif()
            if(component_projbuild)
                idf_build_set_property(__KCONFIG_PROJBUILDS "${component_projbuild}" APPEND)
            endif()
        else()
            if(component_kconfig)
                idf_build_set_property(__KCONFIGS_EXCLUDED "${component_kconfig}" APPEND)
            endif()
            if(component_projbuild)
                idf_build_set_property(__KCONFIGS_PROJBUILD_EXCLUDED "${component_projbuild}" APPEND)
            endif()
        endif()

        if(component_rename)
            idf_build_set_property(__SDKCONFIG_RENAMES "${component_rename}" APPEND)
        endif()
    endforeach()
endfunction()

#[[
   __collect_kconfig_files_from_bootloader_components()

   Collect Kconfig files from bootloader components and store them in build properties.
   Bootloader components are located in the bootloader_components directory of the project.
   This function only runs if the bootloader component is discovered.
#]]
function(__collect_kconfig_files_from_bootloader_components)
    # Check if bootloader component is discovered - only then collect bootloader components
    idf_build_get_property(components_discovered COMPONENTS_DISCOVERED)
    if(NOT "bootloader" IN_LIST components_discovered)
        return()
    endif()

    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(project_dir PROJECT_DIR)

    # Find bootloader component directories
    __get_component_paths(PATHS "${project_dir}/bootloader_components"
                          OUTPUT bootloader_component_dirs)

    foreach(bootloader_component_dir ${bootloader_component_dirs})
        __collect_kconfig_files_from_directory("${bootloader_component_dir}" "${idf_target}"
            bootloader_kconfig bootloader_projbuild bootloader_rename)

        # Set build properties. Bootloader components are only evaluated for Kconfig files at this stage.
        if(bootloader_kconfig)
            idf_build_set_property(__KCONFIGS "${bootloader_kconfig}" APPEND)
        endif()
        if(bootloader_projbuild)
            idf_build_set_property(__KCONFIG_PROJBUILDS "${bootloader_projbuild}" APPEND)
        endif()
        if(bootloader_rename)
            idf_build_set_property(__SDKCONFIG_RENAMES "${bootloader_rename}" APPEND)
        endif()
    endforeach()
endfunction()

#[[
   __collect_kconfig_files_from_directory(directory target out_kconfigs out_projbuilds out_renames)

   Collect Kconfig files from a single directory.

   :directory[in]: Path to the directory to collect Kconfig files from.
   :target[in]: Target name for target-specific files.
   :out_kconfigs[out]: List of Kconfig files.
   :out_projbuilds[out]: List of projbuild files.
   :out_renames[out]: List of rename files.
#]]
function(__collect_kconfig_files_from_directory directory target out_kconfigs out_projbuilds out_renames)
    file(GLOB all_files "${directory}/*")

    set(kconfig_files "")
    set(projbuild_files "")
    set(rename_files "")

    foreach(file IN LISTS all_files)
        get_filename_component(filename "${file}" NAME)
        string(TOLOWER "${filename}" filename_lower)

        # Check for Kconfig file
        if(filename_lower STREQUAL "kconfig")
            list(APPEND kconfig_files "${file}")
            if(NOT filename STREQUAL "Kconfig")
                idf_warn("${filename} should be named 'Kconfig' (uppercase K, rest lowercase).
                 Full path to the file: ${file}")
            endif()

        # Check for Kconfig.projbuild file
        elseif(filename_lower STREQUAL "kconfig.projbuild")
            list(APPEND projbuild_files "${file}")
            if(NOT filename STREQUAL "Kconfig.projbuild")
                idf_warn("${filename} should be named 'Kconfig.projbuild' (uppercase K, rest lowercase).
                 Full path to the file: ${file}")
            endif()
        endif()

        # Check for sdkconfig.rename files
        if(filename_lower STREQUAL "sdkconfig.rename")
            list(APPEND rename_files "${file}")
        endif()

        # Look for target-specific sdkconfig.rename files
        if(target)
            if(filename_lower STREQUAL "sdkconfig.rename.${target}")
                list(APPEND rename_files "${file}")
            endif()
        endif()
    endforeach()

    list(SORT kconfig_files)
    list(SORT projbuild_files)
    list(SORT rename_files)

    set(${out_kconfigs} "${kconfig_files}" PARENT_SCOPE)
    set(${out_projbuilds} "${projbuild_files}" PARENT_SCOPE)
    set(${out_renames} "${rename_files}" PARENT_SCOPE)
endfunction()

# =============================================================================
# KCONFIG ENVIRONMENT FUNCTIONS
# =============================================================================

#[[
   __setup_kconfig_environment()

   Setup the Kconfig environment for kconfgen.
   This function creates the environment and prepares Kconfig source files.
#]]
function(__setup_kconfig_environment)
    # Create the config.env file, which contains all environment variables for the python script
    idf_build_get_property(build_dir BUILD_DIR)
    set(config_env_path "${build_dir}/config.env")
    __create_config_env_file("${config_env_path}")

    # Store environment path in build properties (used by kconfgen)
    idf_build_set_property(__CONFIG_ENV_PATH "${config_env_path}")

    # Now prepare Kconfig source files using the prepare_kconfig_files.py script
    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    set(prepare_cmd ${python} "${idf_path}/tools/kconfig_new/prepare_kconfig_files.py"
        --list-separator=semicolon
        --env-file "${config_env_path}")

    idf_build_set_property(__PREPARE_KCONFIG_CMD "${prepare_cmd}")

    idf_dbg("Preparing Kconfig source files: ${prepare_cmd}")
    execute_process(
        COMMAND ${prepare_cmd}
        RESULT_VARIABLE prepare_result
    )

    if(prepare_result)
        idf_die("Failed to prepare Kconfig source files: ${prepare_result}")
    endif()
endfunction()



#[[
   __create_config_env_file(env_path)

   Create config environment file for kconfgen.
   This function gathers all necessary properties and creates the config.env file
   used by the kconfgen Python script.

   :env_path[in]: Path where to create the config.env file.
#]]
function(__create_config_env_file env_path)
    # Get all necessary build properties
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(kconfigs __KCONFIGS)
    idf_build_get_property(projbuilds __KCONFIG_PROJBUILDS)
    idf_build_get_property(renames __SDKCONFIG_RENAMES)
    idf_build_get_property(target IDF_TARGET)
    idf_build_get_property(toolchain IDF_TOOLCHAIN)
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(kconfigs_excluded __KCONFIGS_EXCLUDED)
    idf_build_get_property(kconfigs_projbuild_excluded __KCONFIGS_PROJBUILD_EXCLUDED)

    # Define Kconfig source file paths
    set(kconfigs_projbuild_path "${build_dir}/kconfigs_projbuild.in")
    set(kconfigs_path "${build_dir}/kconfigs.in")
    set(kconfigs_excluded_path "${build_dir}/kconfigs_excluded.in")
    set(kconfigs_projbuild_excluded_path "${build_dir}/kconfigs_projbuild_excluded.in")

    # Get IDF version info
    __get_sdkconfig_option(OPTION CONFIG_IDF_INIT_VERSION
                           SDKCONFIG "${sdkconfig}"
                           OUTPUT idf_init_version)
    if(NOT idf_init_version)
        set(idf_init_version "$ENV{IDF_VERSION}")
    endif()
    set(ENV{IDF_INIT_VERSION} "${idf_init_version}")
    idf_build_set_property(__IDF_INIT_VERSION "${idf_init_version}")

    # Get IDF_ENV_FPGA from the environment
    set(idf_env_fpga "$ENV{IDF_ENV_FPGA}")
    if(NOT idf_env_fpga)
        set(idf_env_fpga "")
    endif()
    idf_build_set_property(__IDF_ENV_FPGA "${idf_env_fpga}")

    # Get the config.env.in template path
    set(template_path "${idf_path}/tools/kconfig_new/config.env.in")
    if(NOT EXISTS "${template_path}")
        idf_die("Kconfig environment template file not found at ${template_path}")
    endif()

    # Set up variables for the config.env.in template
    set(kconfigs "${kconfigs}")
    set(kconfig_projbuilds "${projbuilds}")
    set(sdkconfig_renames "${renames}")
    set(idf_target "${target}")
    set(idf_toolchain "${toolchain}")
    set(idf_path "${idf_path}")
    set(kconfigs_path "${kconfigs_path}")
    set(kconfigs_projbuild_path "${kconfigs_projbuild_path}")
    set(kconfigs_excluded_path "${kconfigs_excluded_path}")
    set(kconfigs_projbuild_excluded_path "${kconfigs_projbuild_excluded_path}")
    set(kconfigs_excluded "${kconfigs_excluded}")
    set(kconfigs_projbuild_excluded "${kconfigs_projbuild_excluded}")

    # Generate the config.env file from the config.env.in template
    configure_file("${template_path}" "${env_path}")

    idf_dbg("Created config environment file: ${env_path}")
endfunction()

# =============================================================================
# KCONFIG OUTPUT GENERATION FUNCTIONS
# =============================================================================

#[[
   __generate_kconfig_outputs()

   Generate all Kconfig output files.
   Generates: sdkconfig.h, sdkconfig.cmake, sdkconfig.json, kconfig_menus.json

   Must be called after __setup_kconfig_environment().
#]]
function(__generate_kconfig_outputs)
    # Get inputs from build properties
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(sdkconfig_defaults SDKCONFIG_DEFAULTS)
    idf_msg("Project sdkconfig file ${sdkconfig}")

    # Set up output paths
    idf_build_get_property(config_dir CONFIG_DIR)
    if(NOT config_dir)
        idf_die("Kconfig directory not created. Call __init_kconfig() first.")
    endif()

    set(sdkconfig_header "${config_dir}/sdkconfig.h")
    set(sdkconfig_cmake "${config_dir}/sdkconfig.cmake")
    set(sdkconfig_json "${config_dir}/sdkconfig.json")
    set(sdkconfig_json_menus "${config_dir}/kconfig_menus.json")

    # Create and store base kconfgen command for this generation and target reuse
    __create_base_kconfgen_command("${sdkconfig}" "${sdkconfig_defaults}")

    # Generate Kconfig outputs using kconfgen
    __run_kconfgen("${sdkconfig_header}" "${sdkconfig_cmake}" "${sdkconfig_json}" "${sdkconfig_json_menus}")

    # Add the generated config header to build specifications
    idf_build_set_property(INCLUDE_DIRECTORIES "${config_dir}" APPEND)

    # Set up file dependencies for CMake reconfiguration
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${sdkconfig}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${sdkconfig_header}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${sdkconfig_cmake}")

    # Add dependency on kconfgen tool
    idf_build_get_property(idf_path IDF_PATH)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${idf_path}/tools/kconfig_new/confgen.py")

    # Set up clean files
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
                ADDITIONAL_CLEAN_FILES "${sdkconfig_header}" "${sdkconfig_cmake}")

    # Store output paths in build properties
    idf_build_set_property(__SDKCONFIG_HEADER "${sdkconfig_header}")
    idf_build_set_property(__SDKCONFIG_CMAKE "${sdkconfig_cmake}")
    idf_build_set_property(__SDKCONFIG_JSON "${sdkconfig_json}")
    idf_build_set_property(__SDKCONFIG_JSON_MENUS "${sdkconfig_json_menus}")

    idf_msg("Generated Kconfig outputs in ${config_dir}")
endfunction()

#[[
   __create_base_kconfgen_command(sdkconfig sdkconfig_defaults)

   Create the base kconfgen command and store it as a build property for reuse.

   :sdkconfig[in]: Path to sdkconfig file.
   :sdkconfig_defaults[in]: List of sdkconfig defaults files.
#]]
function(__create_base_kconfgen_command sdkconfig sdkconfig_defaults)
    # Get all necessary properties for base command
    idf_build_get_property(python PYTHON)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(root_sdkconfig_rename __ROOT_SDKCONFIG_RENAME)
    idf_build_get_property(config_env_path __CONFIG_ENV_PATH)
    idf_build_get_property(target IDF_TARGET)

    # Set up defaults arguments
    set(defaults_args "")
    if(sdkconfig_defaults)
        foreach(default_file IN LISTS sdkconfig_defaults)
            list(APPEND defaults_args "--defaults" "${default_file}")
            if(EXISTS "${default_file}.${target}")
                list(APPEND defaults_args "--defaults" "${default_file}.${target}")
            endif()
        endforeach()
    endif()

    # Create base kconfgen command
    set(base_kconfgen_cmd ${python} -m kconfgen
        --list-separator=semicolon
        --kconfig "${root_kconfig}"
        --sdkconfig-rename "${root_sdkconfig_rename}"
        --config "${sdkconfig}"
        ${defaults_args}
        --env "IDF_BUILD_V2=y"
        --env-file "${config_env_path}")

    # Store base command as a build property
    idf_build_set_property(__BASE_KCONFGEN_CMD "${base_kconfgen_cmd}")
endfunction()

#[[
   __run_kconfgen(header_path cmake_path json_path menus_path)

   Run kconfgen and generate all output files using the stored base command.
   Assumes that the base command is already created by __create_base_kconfgen_command().

   :header_path[in]: Path for sdkconfig.h output.
   :cmake_path[in]: Path for sdkconfig.cmake output.
   :json_path[in]: Path for sdkconfig.json output.
   :menus_path[in]: Path for kconfig_menus.json output.
#]]
function(__run_kconfgen header_path cmake_path json_path menus_path)
    idf_build_get_property(base_kconfgen_cmd __BASE_KCONFGEN_CMD)
    idf_build_get_property(sdkconfig SDKCONFIG)

    # Create full command with output file paths
    set(kconfgen_cmd ${base_kconfgen_cmd}
        --output header "${header_path}"
        --output cmake "${cmake_path}"
        --output json "${json_path}"
        --output json_menus "${menus_path}"
        --output config "${sdkconfig}"
    )

    idf_dbg("Running kconfgen: ${kconfgen_cmd}")
    execute_process(
        COMMAND ${kconfgen_cmd}
        RESULT_VARIABLE kconfgen_result
    )

    if(kconfgen_result)
        idf_die("Failed to run kconfgen: ${kconfgen_result}")
    endif()
endfunction()

# =============================================================================
# KCONFIG TARGETS FUNCTIONS
# =============================================================================

#[[
   __create_kconfig_targets()

   Create Kconfig related targets.
   This function must be called after all kconfig processing is complete, including
   the component manager. It creates the following targets:

   - menuconfig
   - confserver
   - save-defconfig
#]]
function(__create_kconfig_targets)
    # Create Kconfig targets
    __create_menuconfig_target()
    __create_confserver_target()
    __create_save_defconfig_target()
endfunction()

#[[
   __create_menuconfig_target()

   Create menuconfig target.
#]]
function(__create_menuconfig_target)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(prepare_cmd __PREPARE_KCONFIG_CMD)
    idf_build_get_property(kconfgen_cmd __BASE_KCONFGEN_CMD)
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(target IDF_TARGET)
    idf_build_get_property(toolchain IDF_TOOLCHAIN)
    idf_build_get_property(idf_init_version __IDF_INIT_VERSION)
    idf_build_get_property(idf_env_fpga __IDF_ENV_FPGA)

    # Set up kconfig source file paths
    set(kconfigs_path "${build_dir}/kconfigs.in")
    set(kconfigs_projbuild_path "${build_dir}/kconfigs_projbuild.in")
    set(kconfigs_excluded_path "${CMAKE_CURRENT_BINARY_DIR}/kconfigs_excluded.in")
    set(kconfigs_projbuild_excluded_path "${CMAKE_CURRENT_BINARY_DIR}/kconfigs_projbuild_excluded.in")

    # Newer versions of esp-idf-kconfig renamed menuconfig to esp_menuconfig
    # Order matters here, we want to use esp_menuconfig if it is available
    execute_process(
        COMMAND "${python}" -c "import esp_menuconfig"
        RESULT_VARIABLE ESP_MENUCONFIG_AVAILABLE
        OUTPUT_QUIET ERROR_QUIET
    )
    if(ESP_MENUCONFIG_AVAILABLE EQUAL 0)
        set(MENUCONFIG_CMD "${python}" -m esp_menuconfig)
    else()
        set(MENUCONFIG_CMD "${python}" -m menuconfig)
    endif()

    add_custom_target(menuconfig
        # Prepare Kconfig source files
        COMMAND ${prepare_cmd}
        # Generate config with current settings
        COMMAND ${kconfgen_cmd}
        --env "IDF_TARGET=${target}"
        --env "IDF_TOOLCHAIN=${toolchain}"
        --env "IDF_ENV_FPGA=${idf_env_fpga}"
        --env "IDF_INIT_VERSION=${idf_init_version}"
        --dont-write-deprecated
        --output config ${sdkconfig}
        # Check terminal capabilities
        COMMAND ${python} "${idf_path}/tools/check_term.py"
        # Run menuconfig
        COMMAND ${CMAKE_COMMAND} -E env
        "COMPONENT_KCONFIGS_SOURCE_FILE=${kconfigs_path}"
        "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE=${kconfigs_projbuild_path}"
        "COMPONENT_KCONFIGS_EXCLUDED_SOURCE_FILE=${kconfigs_excluded_path}"
        "COMPONENT_KCONFIGS_PROJBUILD_EXCLUDED_SOURCE_FILE=${kconfigs_projbuild_excluded_path}"
        "KCONFIG_CONFIG=${sdkconfig}"
        "IDF_TARGET=${target}"
        "IDF_TOOLCHAIN=${toolchain}"
        "IDF_ENV_FPGA=${idf_env_fpga}"
        "IDF_INIT_VERSION=${idf_init_version}"
        "IDF_BUILD_V2=y"
        ${MENUCONFIG_CMD} "${root_kconfig}"
        # Post-menuconfig: insert deprecated options for backward compatibility
        COMMAND ${kconfgen_cmd}
        --env "IDF_TARGET=${target}"
        --env "IDF_TOOLCHAIN=${toolchain}"
        --env "IDF_ENV_FPGA=${idf_env_fpga}"
        --env "IDF_INIT_VERSION=${idf_init_version}"
        --output config "${sdkconfig}"
        USES_TERMINAL
        COMMENT "Running menuconfig..."
    )
endfunction()

#[[
   __create_confserver_target()

   Create confserver target.
#]]
function(__create_confserver_target)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(prepare_cmd __PREPARE_KCONFIG_CMD)
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(root_sdkconfig_rename __ROOT_SDKCONFIG_RENAME)
    idf_build_get_property(config_env_path __CONFIG_ENV_PATH)

    add_custom_target(confserver
        # Prepare Kconfig source files
        COMMAND ${prepare_cmd}
        # Run confserver
        COMMAND ${python} -m kconfserver
        --env-file "${config_env_path}"
        --kconfig "${root_kconfig}"
        --sdkconfig-rename "${root_sdkconfig_rename}"
        --config "${sdkconfig}"
        VERBATIM
        USES_TERMINAL
        COMMENT "Running confserver..."
    )
endfunction()

#[[
   __create_save_defconfig_target()

   Create save-defconfig target.
#]]
function(__create_save_defconfig_target)
    idf_build_get_property(prepare_cmd __PREPARE_KCONFIG_CMD)
    idf_build_get_property(kconfgen_cmd __BASE_KCONFGEN_CMD)

    add_custom_target(save-defconfig
        # Prepare Kconfig source files
        COMMAND ${prepare_cmd}
        # Generate save-defconfig
        COMMAND ${kconfgen_cmd}
        --dont-write-deprecated
        --output savedefconfig "${CMAKE_SOURCE_DIR}/sdkconfig.defaults"
        USES_TERMINAL
        COMMENT "Saving defconfig..."
    )
endfunction()
