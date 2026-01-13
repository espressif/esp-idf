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

    Initialize the Kconfig system infrastructure for the build.  This function
    performs the Kconfig initialization and setup by:
    1. Setting up SDKCONFIG and SDKCONFIG_DEFAULTS build properties
    2. Setting up ESP-IDF root Kconfig files and config directory
    3. Creating the default config directory

    This should be called after component discovery but before component
    manager.

    Note: Regular component Kconfig files are collected during component
    discovery.
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
.. cmakev2:function:: __should_generate_sdkconfig

    .. code-block:: cmake

        __should_generate_sdkconfig(OUTPUT <variable>)

    *OUTPUT[out]*

        The output variable will contain ``YES`` or ``NO`` depending on whether
        the sdkconfig should be generated.

    Check if any new ``Kconfig``, ``Kconfig.projbuild``, or
    ``sdkconfig.rename`` files have been added. If so, store ``YES`` in the
    ``OUTPUT`` variable; otherwise, store ``NO``. The
    ``__generate_sdkconfig()`` function can be called multiple times, such as
    when the initial sdkconfig is generated at the start of the build process
    and later after additional components are fetched by the component manager.
    There might be no components fetched by the component manager, for example,
    in the hello_world example, or the downloaded components may not contain
    any configuration files. In such cases, there is no need to regenerate the
    sdkconfig. This helper function stores the list of configuration files in
    the ``__PREV_KCONFIGS``, ``__PREV_KCONFIG_PROJBUILDS``, and
    ``__PREV_SDKCONFIG_RENAMES`` build properties at its end, and at the
    beginning, it compares them with the current lists of configuration files.
#]]
function(__should_generate_sdkconfig)
    set(options)
    set(one_value OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    idf_build_get_property(kconfigs __KCONFIGS)
    idf_build_get_property(projbuilds __KCONFIG_PROJBUILDS)
    idf_build_get_property(renames __SDKCONFIG_RENAMES)
    list(SORT kconfigs)
    list(SORT projbuilds)
    list(SORT renames)

    idf_build_get_property(prev_kconfigs __PREV_KCONFIGS)
    idf_build_get_property(prev_projbuilds __PREV_KCONFIG_PROJBUILDS)
    idf_build_get_property(prev_renames __PREV_SDKCONFIG_RENAMES)
    list(SORT prev_kconfigs)
    list(SORT prev_projbuilds)
    list(SORT prev_renames)

    idf_build_set_property(__PREV_KCONFIGS "${kconfigs}")
    idf_build_set_property(__PREV_KCONFIG_PROJBUILDS "${projbuilds}")
    idf_build_set_property(__PREV_SDKCONFIG_RENAMES "${renames}")

    if("${kconfigs}" STREQUAL "${prev_kconfigs}"
            AND "${projbuilds}" STREQUAL "${prev_projbuilds}"
            AND "${renames}" STREQUAL "${prev_renames}")
        set(${ARG_OUTPUT}  NO PARENT_SCOPE)
    else()
        set(${ARG_OUTPUT}  YES PARENT_SCOPE)
    endif()
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
    # Collect Kconfig files from discovered components
    __consolidate_component_kconfig_files()

    # Collect Kconfig files from bootloader components
    __collect_kconfig_files_from_bootloader_components()

    # Check if sdkconfig needs to be generated
    __should_generate_sdkconfig(OUTPUT generate)
    if(NOT generate)
        idf_dbg("Configuration files have not changed, skipping sdkconfig generation.")
        return()
    endif()

    idf_msg("Generating sdkconfig configuration...")

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

    Consolidate Kconfig files from discovered components into global build
    properties.  This scans the COMPONENT_INTERFACES build property and for
    each component, retrieves its Kconfig files from component properties and
    adds them to the global __KCONFIGS, __KCONFIG_PROJBUILDS, and
    __SDKCONFIG_RENAMES build properties.
#]]
function(__consolidate_component_kconfig_files)
    idf_build_get_property(component_interfaces COMPONENT_INTERFACES)
    if(NOT component_interfaces)
        idf_die("No components discovered. This must be run after component discovery.")
    endif()

    # Clean the build properties before adding new ones. This ensures that
    # we don't have duplicate Kconfig files in the build properties.
    idf_build_set_property(__KCONFIGS "")
    idf_build_set_property(__KCONFIG_PROJBUILDS "")
    idf_build_set_property(__SDKCONFIG_RENAMES "")

    # Iterate through all interfaces of discovered components and consolidate
    # their Kconfig files
    foreach(component_interface IN LISTS component_interfaces)
        # Get Kconfig files from component properties
        __idf_component_get_property_unchecked(component_kconfig "${component_interface}" __KCONFIG)
        __idf_component_get_property_unchecked(component_projbuild "${component_interface}" __KCONFIG_PROJBUILD)
        __idf_component_get_property_unchecked(component_rename "${component_interface}" __SDKCONFIG_RENAME)

        if(component_kconfig)
            idf_build_set_property(__KCONFIGS "${component_kconfig}" APPEND)
        endif()

        if(component_projbuild)
            idf_build_set_property(__KCONFIG_PROJBUILDS "${component_projbuild}" APPEND)
        endif()

        if(component_rename)
            idf_build_set_property(__SDKCONFIG_RENAMES "${component_rename}" APPEND)
        endif()
    endforeach()
endfunction()

#[[
    __collect_kconfig_files_from_bootloader_components()

    Collect Kconfig files from bootloader components and store them in build
    properties.  Bootloader components are located in the bootloader_components
    directory of the project.  This function only runs if the bootloader
    component is discovered.
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

    *directory[in]*

        Path to the directory to collect Kconfig files from.

    *target[in]*

        Target name for target-specific files.

    *out_kconfigs[out]*

        List of Kconfig files.

    *out_projbuilds[out]*

        List of projbuild files.

    *out_renames[out]*

        List of rename files.
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
                idf_warn("${filename} file should be named 'Kconfig' (uppercase K, rest lowercase).
                 Full path to the file: ${file}")
            endif()

        # Check for Kconfig.projbuild file
        elseif(filename_lower STREQUAL "kconfig.projbuild")
            list(APPEND projbuild_files "${file}")
            if(NOT filename STREQUAL "Kconfig.projbuild")
                idf_warn("${filename} file should be named 'Kconfig.projbuild' (uppercase K, rest lowercase).
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

    Setup the Kconfig environment for kconfgen.  This function creates the
    environment and prepares Kconfig source files.
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
    __create_executable_config_env_file(executable)

    *executable[in]*

        Executable target for which to generate ``config.env`` file.

    Generate the ``config.env`` file for the specified ``executable``. The
    configuration file will be stored in the build directory, within a
    directory named after the ``executable`` target name. The ``kconfigs*.in``
    files, which are generated by ``prepare_kconfig_files.py``, will also be
    stored in this directory.

    This function primarily prepares the arguments for the
    ``__create_config_env_file`` function based on the components linked to the
    ``executable``, ensuring that component Kconfig files are stored in the
    appropriate ``kconfigs*.in`` files, depending on whether the component is
    linked to the executable.

    The directory where the generated files are stored is added to the
    ``executable`` ``CONFIG_ENV_DIR`` property.
#]]
function(__create_executable_config_env_file executable)

    get_target_property(config_env_dir "${executable}" CONFIG_ENV_DIR)
    if(config_env_dir)
        return()
    endif()

    __get_executable_library_or_die(TARGET "${executable}" OUTPUT library)

    idf_library_get_property(component_interfaces_linked "${library}" LIBRARY_COMPONENT_INTERFACES_LINKED)

    set(kconfigs "")
    set(kconfigs_projbuild "")
    set(kconfigs_excluded "")
    set(kconfigs_projbuild_excluded "")

    idf_build_get_property(component_interfaces COMPONENT_INTERFACES)
    foreach(component_interface IN LISTS component_interfaces)
        __idf_component_get_property_unchecked(component_kconfig "${component_interface}" __KCONFIG)
        __idf_component_get_property_unchecked(component_projbuild "${component_interface}" __KCONFIG_PROJBUILD)

        if(component_kconfig)
            if("${component_interface}" IN_LIST component_interfaces_linked)
                list(APPEND kconfigs "${component_kconfig}")
            else()
                list(APPEND kconfigs_excluded "${component_kconfig}")
            endif()
        endif()

        if(component_projbuild)
            if("${component_interface}" IN_LIST component_interfaces_linked)
                list(APPEND kconfigs_projbuild "${component_projbuild}")
            else()
                list(APPEND kconfigs_projbuild_excluded "${component_projbuild}")
            endif()
        endif()

    endforeach()

    string(REGEX REPLACE "[^A-Za-z0-9_]" "_" executable_sanitized "${executable}")
    idf_build_get_property(build_dir BUILD_DIR)
    set(kconfigs_dir "${build_dir}/${executable_sanitized}")

    set(kconfigs_path "${kconfigs_dir}/kconfigs.in")
    set(kconfigs_projbuild_path "${kconfigs_dir}/kconfigs_projbuild.in")
    set(kconfigs_excluded_path "${kconfigs_dir}/kconfigs_excluded.in")
    set(kconfigs_projbuild_excluded_path "${kconfigs_dir}/kconfigs_projbuild_excluded.in")
    set(config_env_path "${kconfigs_dir}/config.env")

    __create_config_env_file("${config_env_path}"
                             KCONFIGS "${kconfigs}"
                             KCONFIGS_PROJBUILD "${kconfigs_projbuild}"
                             KCONFIGS_EXCLUDED "${kconfigs_excluded}"
                             KCONFIGS_PROJBUILD_EXCLUDED "${kconfigs_projbuild_excluded}")

    set_target_properties("${executable}" PROPERTIES CONFIG_ENV_DIR "${kconfigs_dir}")
endfunction()

#[[
    __create_config_env_file(config_env_path
                             [KCONFIGS <kconfig>...]
                             [KCONFIGS_PROJBUILD <projbuild>...]
                             [KCONFIGS_EXCLUDED <kconfig>...]
                             [KCONFIGS_PROJBUILD_EXCLUDED <projbuild>...])

    *config_env_path[in]*

        Path for the generated configuration environment file.

    *KCONFIGS[in,opt]*

        Kconfig file or list of Kconfig files to be sourced in the
        ``kconfigs.in`` file.  Can be used multiple times. If not provided, the
        Kconfig files stored in the ``__KCONFIGS`` build property are used.

    *KCONFIGS_PROJBUILD[in,opt]*

        Kconfig file or list of Kconfig files to be sourced in the
        ``kconfigs_projbuild.in`` file. Can be used multiple times. If not
        provided, the Kconfig files stored in the ``__KCONFIGS`` build property
        are used.

    *KCONFIGS_EXCLUDED[in,opt]*

        Kconfig file or list of Kconfig files to be sourced in the
        ``kconfigs_excluded.in`` file. Can be used multiple times.

    *KCONFIGS_PROJBUILD_EXCLUDED[in,opt]*

        Kconfig file or list of Kconfig files to be sourced in the
        ``kconfigs_projbuild_excluded.in`` file. Can be used multiple times.

    Generate a configuration file for the ``prepare_kconfig_files.py`` script
    at ``config_env_path``. This file includes, among other details, the paths
    where ``prepare_kconfig_files.py`` should generate the ``kconfigs*.in``
    files. These are Kconfig files, sourced in the main Kconfig file through
    environmental variables, and contain a list of component configurations
    that should be sourced. The ``kconfigs*.in`` file paths in the generated
    configuration file are located in the same directory as the configuration
    file itself. This means that ``prepare_kconfig_files.py`` will produce the
    ``kconfigs*.in`` files in the same directory where the configuration file
    created by this function is generated.
#]]
function(__create_config_env_file config_env_path)
    set(options)
    set(one_value)
    set(multi_value KCONFIGS KCONFIGS_PROJBUILD KCONFIGS_EXCLUDED KCONFIGS_PROJBUILD_EXCLUDED)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    # Get all necessary build properties
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(kconfigs __KCONFIGS)
    idf_build_get_property(projbuilds __KCONFIG_PROJBUILDS)
    idf_build_get_property(renames __SDKCONFIG_RENAMES)
    idf_build_get_property(target IDF_TARGET)
    idf_build_get_property(toolchain IDF_TOOLCHAIN)
    idf_build_get_property(sdkconfig SDKCONFIG)

    if(ARG_KCONFIGS)
        set(kconfigs "${ARG_KCONFIGS}")
    endif()

    if(ARG_KCONFIGS_PROJBUILD)
        set(projbuilds "${ARG_KCONFIGS_PROJBUILD}")
    endif()

    set(kconfigs_excluded "")
    if(ARG_KCONFIGS_EXCLUDED)
        set(kconfigs_excluded "${ARG_KCONFIGS_EXCLUDED}")
    endif()

    set(kconfigs_projbuild_excluded "")
    if(ARG_KCONFIGS_PROJBUILD_EXCLUDED)
        set(kconfigs_projbuild_excluded "${ARG_KCONFIGS_PROJBUILD_EXCLUDED}")
    endif()

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
    set(template_path "${idf_path}/tools/kconfig_new/config_buildv2.env.in")
    if(NOT EXISTS "${template_path}")
        idf_die("Kconfig environment template file not found at ${template_path}")
    endif()

    get_filename_component(config_env_dir "${config_env_path}" DIRECTORY)

    # Set up variables for the config.env.in template
    set(kconfigs "${kconfigs}")
    set(kconfig_projbuilds "${projbuilds}")
    set(kconfigs_excluded "${kconfigs_excluded}")
    set(kconfigs_projbuild_excluded "${kconfigs_projbuild_excluded}")
    set(sdkconfig_renames "${renames}")
    set(idf_target "${target}")
    set(idf_toolchain "${toolchain}")
    set(idf_path "${idf_path}")
    set(kconfigs_path "${config_env_dir}/kconfigs.in")
    set(kconfigs_projbuild_path "${config_env_dir}/kconfigs_projbuild.in")
    set(kconfigs_excluded_path "${config_env_dir}/kconfigs_excluded.in")
    set(kconfigs_projbuild_excluded_path "${config_env_dir}/kconfigs_projbuild_excluded.in")

    # Generate the config.env file from the config.env.in template
    configure_file("${template_path}" "${config_env_path}")

    idf_dbg("Created config environment file: ${config_env_path}")
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

    # Create and store base kconfgen command for this generation and target reuse
    __create_base_kconfgen_command("${sdkconfig}" "${sdkconfig_defaults}")

    # Create and store common kconfgen outputs command
    set(kconfgen_outputs_cmd
        --output header "${sdkconfig_header}"
        --output cmake "${sdkconfig_cmake}"
        --output json "${sdkconfig_json}"
        --output config "${sdkconfig}"
    )

    idf_build_set_property(__KCONFGEN_OUTPUTS_CMD "${kconfgen_outputs_cmd}")

    # Generate Kconfig outputs using kconfgen
    __run_kconfgen()

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

    *sdkconfig[in]*

        Path to sdkconfig file.

    *sdkconfig_defaults[in]*

        List of sdkconfig defaults files.

    Create the base kconfgen command and store it as a build property for
    reuse.
#]]
function(__create_base_kconfgen_command sdkconfig sdkconfig_defaults)
    # Get all necessary properties for base command
    idf_build_get_property(python PYTHON)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(root_sdkconfig_rename __ROOT_SDKCONFIG_RENAME)
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
        --env "IDF_BUILD_V2=y")

    # Store base command as a build property
    idf_build_set_property(__BASE_KCONFGEN_CMD "${base_kconfgen_cmd}")
endfunction()

#[[
    __run_kconfgen()

    Run kconfgen and generate all output files using the stored base command.
    Assumes that the base command is already created by
    __create_base_kconfgen_command().
#]]
function(__run_kconfgen)
    idf_build_get_property(base_kconfgen_cmd __BASE_KCONFGEN_CMD)
    idf_build_get_property(kconfgen_outputs_cmd __KCONFGEN_OUTPUTS_CMD)
    idf_build_get_property(config_env_path __CONFIG_ENV_PATH)

    # Create full command with output file paths
    set(kconfgen_cmd ${base_kconfgen_cmd} ${kconfgen_outputs_cmd})

    idf_dbg("Running kconfgen: ${kconfgen_cmd}")
    execute_process(
        COMMAND ${kconfgen_cmd}
        --env-file "${config_env_path}"
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
.. cmakev2:function:: idf_create_menuconfig

    .. code-block:: cmake

        idf_create_menuconfig(<executable>
                              TARGET <target>)

    *executable[in]*

    Executable target for which to create the menuconfig target.

    *TARGET[in]*

        Name of the menuconfig target to be created.

    Create a menuconfig target with the name specified by the ``TARGET``
    option for an ``executable``.
#]]
function(idf_create_menuconfig executable)
    set(options)
    set(one_value TARGET)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    if(TARGET "${ARG_TARGET}")
        idf_die("TARGET '${ARG_TARGET}' for menuconfig already exists")
    endif()

    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(kconfgen_cmd __BASE_KCONFGEN_CMD)
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(target IDF_TARGET)
    idf_build_get_property(toolchain IDF_TOOLCHAIN)
    idf_build_get_property(idf_init_version __IDF_INIT_VERSION)
    idf_build_get_property(idf_env_fpga __IDF_ENV_FPGA)
    idf_build_get_property(kconfgen_outputs_cmd __KCONFGEN_OUTPUTS_CMD)

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

    __create_executable_config_env_file("${executable}")
    get_target_property(config_env_dir "${executable}" CONFIG_ENV_DIR)

    add_custom_target("${ARG_TARGET}"
        # Prepare Kconfig source files
        COMMAND ${python} "${idf_path}/tools/kconfig_new/prepare_kconfig_files.py"
        --list-separator=semicolon
        --env-file "${config_env_dir}/config.env"
        # Generate config with current settings
        COMMAND ${kconfgen_cmd}
        --env "IDF_TARGET=${target}"
        --env "IDF_TOOLCHAIN=${toolchain}"
        --env "IDF_ENV_FPGA=${idf_env_fpga}"
        --env "IDF_INIT_VERSION=${idf_init_version}"
        --dont-write-deprecated
        ${kconfgen_outputs_cmd}
        --env-file "${config_env_dir}/config.env"
        # Check terminal capabilities
        COMMAND ${python} "${idf_path}/tools/check_term.py"
        # Run menuconfig
        COMMAND ${CMAKE_COMMAND} -E env
        "COMPONENT_KCONFIGS_SOURCE_FILE=${config_env_dir}/kconfigs.in"
        "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE=${config_env_dir}/kconfigs_projbuild.in"
        "COMPONENT_KCONFIGS_EXCLUDED_SOURCE_FILE=${config_env_dir}/kconfigs_excluded.in"
        "COMPONENT_KCONFIGS_PROJBUILD_EXCLUDED_SOURCE_FILE=${config_env_dir}/kconfigs_projbuild_excluded.in"
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
        ${kconfgen_outputs_cmd}
        --env-file "${config_env_dir}/config.env"
        USES_TERMINAL
        COMMENT "Running menuconfig..."
    )
endfunction()

#[[
.. cmakev2:function:: idf_create_confserver

    .. code-block:: cmake

        idf_create_confserver(<executable>
                              TARGET <target>)

    *executable[in]*

    Executable target for which to create the confserver target.

    *TARGET[in]*

        Name of the confserver target to be created.

    Create a confserver target with the name specified by the ``TARGET``
    option for an ``executable``.
#]]
function(idf_create_confserver executable)
    set(options)
    set(one_value TARGET)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_TARGET)
        idf_die("TARGET option is required")
    endif()

    idf_build_get_property(python PYTHON)
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(root_sdkconfig_rename __ROOT_SDKCONFIG_RENAME)
    idf_build_get_property(base_kconfgen_cmd __BASE_KCONFGEN_CMD)
    idf_build_get_property(config_dir CONFIG_DIR)

    __create_executable_config_env_file("${executable}")
    get_target_property(config_env_dir "${executable}" CONFIG_ENV_DIR)

    add_custom_target("${ARG_TARGET}"
        # Prepare Kconfig source files
        COMMAND ${python} "${idf_path}/tools/kconfig_new/prepare_kconfig_files.py"
        --list-separator=semicolon
        --env-file "${config_env_dir}/config.env"
        # Generate kconfig_menus.json
        COMMAND ${base_kconfgen_cmd}
        --env-file "${config_env_dir}/config.env"
        --output json_menus "${config_dir}/kconfig_menus.json"
        # Run confserver
        COMMAND ${python} -m kconfserver
        --env-file "${config_env_dir}/config.env"
        --kconfig "${root_kconfig}"
        --sdkconfig-rename "${root_sdkconfig_rename}"
        --config "${sdkconfig}"
        VERBATIM
        USES_TERMINAL
        COMMENT "Running confserver..."
    )
endfunction()

#[[
.. cmakev2:function:: idf_create_save_defconfig

    .. code-block:: cmake

        idf_create_save_defconfig()

    Create the save-defconfig target.
#]]
function(idf_create_save_defconfig)
    idf_build_get_property(prepare_cmd __PREPARE_KCONFIG_CMD)
    idf_build_get_property(config_env_path __CONFIG_ENV_PATH)
    idf_build_get_property(kconfgen_cmd __BASE_KCONFGEN_CMD)

    add_custom_target(save-defconfig
        # Prepare Kconfig source files
        COMMAND ${prepare_cmd}
        # Generate save-defconfig
        COMMAND ${kconfgen_cmd}
        --dont-write-deprecated
        --output savedefconfig "${CMAKE_SOURCE_DIR}/sdkconfig.defaults"
        --env-file "${config_env_path}"
        USES_TERMINAL
        COMMENT "Saving defconfig..."
        VERBATIM
    )
endfunction()
