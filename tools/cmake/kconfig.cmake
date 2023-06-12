include(ExternalProject)

function(__kconfig_init)
    idf_build_get_property(idf_path IDF_PATH)

    idf_build_set_property(__ROOT_KCONFIG ${idf_path}/Kconfig)
    idf_build_set_property(__ROOT_SDKCONFIG_RENAME ${idf_path}/sdkconfig.rename)
    idf_build_set_property(__OUTPUT_SDKCONFIG 1)
endfunction()

#
# Initialize Kconfig-related properties for components.
# This function assumes that all basic properties of the components have been
# set prior to calling it.
#
function(__kconfig_component_init component_target)
    __component_get_property(component_dir ${component_target} COMPONENT_DIR)
    file(GLOB kconfig "${component_dir}/Kconfig")
    list(SORT kconfig)
    __component_set_property(${component_target} KCONFIG "${kconfig}")
    file(GLOB kconfig "${component_dir}/Kconfig.projbuild")
    list(SORT kconfig)
    __component_set_property(${component_target} KCONFIG_PROJBUILD "${kconfig}")
    file(GLOB sdkconfig_rename "${component_dir}/sdkconfig.rename")
    file(GLOB sdkconfig_rename_target "${component_dir}/sdkconfig.rename.${IDF_TARGET}")

    list(APPEND sdkconfig_rename ${sdkconfig_rename_target})
    list(SORT sdkconfig_rename)
    __component_set_property(${component_target} SDKCONFIG_RENAME "${sdkconfig_rename}")
endfunction()

#
# Add bootloader components Kconfig and Kconfig.projbuild files to BOOTLOADER_KCONFIG
# and BOOTLOADER_KCONFIGS_PROJ properties respectively.
#
function(__kconfig_bootloader_component_add component_dir)
    idf_build_get_property(bootloader_kconfigs BOOTLOADER_KCONFIGS)
    idf_build_get_property(bootloader_kconfigs_proj BOOTLOADER_KCONFIGS_PROJ)

    file(GLOB kconfig "${component_dir}/Kconfig")
    list(SORT kconfig)
    if(EXISTS "${kconfig}" AND NOT IS_DIRECTORY "${kconfig}")
        list(APPEND bootloader_kconfigs "${kconfig}")
    endif()

    file(GLOB kconfig "${component_dir}/Kconfig.projbuild")
    list(SORT kconfig)
    if(EXISTS "${kconfig}" AND NOT IS_DIRECTORY "${kconfig}")
        list(APPEND bootloader_kconfigs_proj "${kconfig}")
    endif()

    idf_build_set_property(BOOTLOADER_KCONFIGS "${bootloader_kconfigs}")
    idf_build_set_property(BOOTLOADER_KCONFIGS_PROJ "${bootloader_kconfigs_proj}")
endfunction()


#
# Find the initial IDF version used to generate the config.
# This is needed to pass this variable back to confgen, so that the
# value of CONFIG_IDF_INIT_VERSION option stays the same.
#
function(__get_init_config_version config version_out)
    set(${version_out} NOTFOUND PARENT_SCOPE)

    if(NOT EXISTS "${config}")
        return()
    endif()

    file(STRINGS "${config}" lines)
    foreach(line ${lines})
        string(STRIP "${line}" line)
        if(NOT "${line}" MATCHES "CONFIG_IDF_INIT_VERSION=\"([0-9]+\.[0-9]+\.[0-9]+)\"$")
            continue()
        endif()

        string(REGEX REPLACE "CONFIG_IDF_INIT_VERSION=\"([0-9]+\.[0-9]+\.[0-9]+)\"$" "\\1" version "${line}")
        set(${version_out} ${version} PARENT_SCOPE)
        return()
    endforeach()
endfunction()


#
# Generate the config files and create config related targets and configure
# dependencies.
#
function(__kconfig_generate_config sdkconfig sdkconfig_defaults)
    # List all Kconfig and Kconfig.projbuild in known components
    idf_build_get_property(component_targets __COMPONENT_TARGETS)
    idf_build_get_property(build_component_targets __BUILD_COMPONENT_TARGETS)
    foreach(component_target ${component_targets})
        if(component_target IN_LIST build_component_targets)
            __component_get_property(kconfig ${component_target} KCONFIG)
            __component_get_property(kconfig_projbuild ${component_target} KCONFIG_PROJBUILD)
            __component_get_property(sdkconfig_rename ${component_target} SDKCONFIG_RENAME)
            if(kconfig)
                list(APPEND kconfigs ${kconfig})
            endif()
            if(kconfig_projbuild)
                list(APPEND kconfig_projbuilds ${kconfig_projbuild})
            endif()
            if(sdkconfig_rename)
                list(APPEND sdkconfig_renames ${sdkconfig_rename})
            endif()
        endif()
    endforeach()

    __get_init_config_version(${sdkconfig} idf_init_version)
    if(NOT idf_init_version)
        set(idf_init_version $ENV{IDF_VERSION})
    endif()
    set(ENV{IDF_INIT_VERSION} ${idf_init_version})

    # Take into account bootloader components configuration files
    idf_build_get_property(bootloader_kconfigs BOOTLOADER_KCONFIGS)
    idf_build_get_property(bootloader_kconfigs_proj BOOTLOADER_KCONFIGS_PROJ)
    if(bootloader_kconfigs)
        list(APPEND kconfigs "${bootloader_kconfigs}")
    endif()
    if(bootloader_kconfigs_proj)
        list(APPEND kconfig_projbuilds "${bootloader_kconfigs_proj}")
    endif()

    # Store the list version of kconfigs and kconfig_projbuilds
    idf_build_set_property(KCONFIGS "${kconfigs}")
    idf_build_set_property(KCONFIG_PROJBUILDS "${kconfig_projbuilds}")

    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(idf_toolchain IDF_TOOLCHAIN)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(idf_env_fpga __IDF_ENV_FPGA)

    # These are the paths for files which will contain the generated "source" lines for COMPONENT_KCONFIGS and
    # COMPONENT_KCONFIGS_PROJBUILD
    set(kconfigs_projbuild_path "${CMAKE_CURRENT_BINARY_DIR}/kconfigs_projbuild.in")
    set(kconfigs_path "${CMAKE_CURRENT_BINARY_DIR}/kconfigs.in")

    # Place config-related environment arguments into config.env file
    # to work around command line length limits for execute_process
    # on Windows & CMake < 3.11
    set(config_env_path "${CMAKE_CURRENT_BINARY_DIR}/config.env")
    configure_file("${idf_path}/tools/kconfig_new/config.env.in" ${config_env_path})
    idf_build_set_property(CONFIG_ENV_PATH ${config_env_path})

    if(sdkconfig_defaults)
        foreach(sdkconfig_default ${sdkconfig_defaults})
            list(APPEND defaults_arg --defaults "${sdkconfig_default}")
            if(EXISTS "${sdkconfig_default}.${idf_target}")
                list(APPEND defaults_arg --defaults "${sdkconfig_default}.${idf_target}")
            endif()
        endforeach()
    endif()

    idf_build_get_property(root_kconfig __ROOT_KCONFIG)
    idf_build_get_property(root_sdkconfig_rename __ROOT_SDKCONFIG_RENAME)
    idf_build_get_property(python PYTHON)

    set(prepare_kconfig_files_command
        ${python} ${idf_path}/tools/kconfig_new/prepare_kconfig_files.py
        --list-separator=semicolon
        --env-file ${config_env_path})

    set(kconfgen_basecommand
        ${python} -m kconfgen
        --list-separator=semicolon
        --kconfig ${root_kconfig}
        --sdkconfig-rename ${root_sdkconfig_rename}
        --config ${sdkconfig}
        ${defaults_arg}
        --env-file ${config_env_path})

    idf_build_get_property(build_dir BUILD_DIR)
    set(config_dir ${build_dir}/config)
    file(MAKE_DIRECTORY "${config_dir}")

    message(STATUS "Project sdkconfig file ${sdkconfig}")

    # Generate the config outputs
    set(sdkconfig_cmake ${config_dir}/sdkconfig.cmake)
    set(sdkconfig_header ${config_dir}/sdkconfig.h)
    set(sdkconfig_json ${config_dir}/sdkconfig.json)
    set(sdkconfig_json_menus ${config_dir}/kconfig_menus.json)

    idf_build_get_property(output_sdkconfig __OUTPUT_SDKCONFIG)
    if(output_sdkconfig)
        execute_process(
            COMMAND ${prepare_kconfig_files_command})
        execute_process(
            COMMAND ${kconfgen_basecommand}
            --output header ${sdkconfig_header}
            --output cmake ${sdkconfig_cmake}
            --output json ${sdkconfig_json}
            --output json_menus ${sdkconfig_json_menus}
            --output config ${sdkconfig}
            RESULT_VARIABLE config_result)
    else()
        execute_process(
            COMMAND ${prepare_kconfig_files_command})
        execute_process(
            COMMAND ${kconfgen_basecommand}
            --output header ${sdkconfig_header}
            --output cmake ${sdkconfig_cmake}
            --output json ${sdkconfig_json}
            --output json_menus ${sdkconfig_json_menus}
            RESULT_VARIABLE config_result)
    endif()

    if(config_result)
        message(FATAL_ERROR "Failed to run kconfgen (${kconfgen_basecommand}). Error ${config_result}")
    endif()

    # Add the generated config header to build specifications.
    idf_build_set_property(INCLUDE_DIRECTORIES ${config_dir} APPEND)

    # When sdkconfig file changes in the future, trigger a cmake run
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${sdkconfig}")

    # Ditto if either of the generated files are missing/modified (this is a bit irritating as it means
    # you can't edit these manually without them being regenerated, but I don't know of a better way...)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${sdkconfig_header}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${sdkconfig_cmake}")

    # Or if the config generation tool changes
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${idf_path}/tools/kconfig_new/confgen.py")

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
                ADDITIONAL_CLEAN_FILES "${sdkconfig_header}" "${sdkconfig_cmake}")

    idf_build_set_property(SDKCONFIG_HEADER ${sdkconfig_header})
    idf_build_set_property(SDKCONFIG_JSON ${sdkconfig_json})
    idf_build_set_property(SDKCONFIG_CMAKE ${sdkconfig_cmake})
    idf_build_set_property(SDKCONFIG_JSON_MENUS ${sdkconfig_json_menus})
    idf_build_set_property(CONFIG_DIR ${config_dir})

    set(MENUCONFIG_CMD ${python} -m menuconfig)
    set(TERM_CHECK_CMD ${python} ${idf_path}/tools/check_term.py)

    # Generate the menuconfig target
    add_custom_target(menuconfig
        ${menuconfig_depends}
        # create any missing config file, with defaults if necessary
        COMMAND ${prepare_kconfig_files_command}
        COMMAND ${kconfgen_basecommand}
        --env "IDF_TARGET=${idf_target}"
        --env "IDF_TOOLCHAIN=${idf_toolchain}"
        --env "IDF_ENV_FPGA=${idf_env_fpga}"
        --env "IDF_INIT_VERSION=${idf_init_version}"
        --dont-write-deprecated
        --output config ${sdkconfig}
        COMMAND ${TERM_CHECK_CMD}
        COMMAND ${CMAKE_COMMAND} -E env
        "COMPONENT_KCONFIGS_SOURCE_FILE=${kconfigs_path}"
        "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE=${kconfigs_projbuild_path}"
        "KCONFIG_CONFIG=${sdkconfig}"
        "IDF_TARGET=${idf_target}"
        "IDF_TOOLCHAIN=${idf_toolchain}"
        "IDF_ENV_FPGA=${idf_env_fpga}"
        "IDF_INIT_VERSION=${idf_init_version}"
        ${MENUCONFIG_CMD} ${root_kconfig}
        USES_TERMINAL
        # additional run of kconfgen esures that the deprecated options will be inserted into sdkconfig (for backward
        # compatibility)
        COMMAND ${kconfgen_basecommand}
        --env "IDF_TARGET=${idf_target}"
        --env "IDF_TOOLCHAIN=${idf_toolchain}"
        --env "IDF_ENV_FPGA=${idf_env_fpga}"
        --env "IDF_INIT_VERSION=${idf_init_version}"
        --output config ${sdkconfig}
        )

    # Custom target to run kconfserver from the build tool
    add_custom_target(confserver
        COMMAND ${prepare_kconfig_files_command}
        COMMAND ${python} -m kconfserver
        --env-file ${config_env_path}
        --kconfig ${IDF_PATH}/Kconfig
        --sdkconfig-rename ${root_sdkconfig_rename}
        --config ${sdkconfig}
        VERBATIM
        USES_TERMINAL)

    add_custom_target(save-defconfig
        COMMAND ${prepare_kconfig_files_command}
        COMMAND ${kconfgen_basecommand}
        --dont-write-deprecated
        --output savedefconfig ${CMAKE_SOURCE_DIR}/sdkconfig.defaults
        USES_TERMINAL
        )
endfunction()
