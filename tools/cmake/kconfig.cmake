include(ExternalProject)

macro(kconfig_set_variables)
    set_default(IDF_SDKCONFIG_DEFAULTS "")

    set_default(CONFIG_DIR ${IDF_BUILD_ARTIFACTS_DIR}/config)
    set_default(SDKCONFIG ${IDF_PROJECT_PATH}/sdkconfig)
    set(SDKCONFIG_HEADER ${CONFIG_DIR}/sdkconfig.h)
    set(SDKCONFIG_CMAKE ${CONFIG_DIR}/sdkconfig.cmake)
    set(SDKCONFIG_JSON ${CONFIG_DIR}/sdkconfig.json)
    set(KCONFIG_JSON_MENUS ${CONFIG_DIR}/kconfig_menus.json)

    set(ROOT_KCONFIG ${IDF_PATH}/Kconfig)
endmacro()

if(CMAKE_HOST_WIN32)
    # Prefer a prebuilt mconf-idf on Windows
    if(DEFINED ENV{MSYSTEM})
        find_program(WINPTY winpty)
    else()
        unset(WINPTY CACHE)  # in case previous CMake run was in a tty and this one is not
    endif()
    find_program(MCONF mconf-idf)

    # Fall back to the old binary which was called 'mconf' not 'mconf-idf'
    if(NOT MCONF)
        find_program(MCONF mconf)
        if(MCONF)
            message(WARNING "Falling back to mconf binary '${MCONF}' not mconf-idf. "
                "This is probably because an old version of IDF mconf is installed and this is fine. "
                "However if there are config problems please check the Getting Started guide for your platform.")
        endif()
    endif()

    if(NOT MCONF)
        find_program(NATIVE_GCC gcc)
        if(NOT NATIVE_GCC)
            message(FATAL_ERROR
                "Windows requires a prebuilt mconf-idf for your platform "
                "on the PATH, or an MSYS2 version of gcc on the PATH to build mconf-idf. "
                "Consult the setup docs for ESP-IDF on Windows.")
        endif()
    elseif(WINPTY)
        set(MCONF "${WINPTY}" "${MCONF}")
    endif()
endif()

if(NOT MCONF)
    # Use the existing Makefile to build mconf (out of tree) when needed
    #
    set(MCONF ${CMAKE_BINARY_DIR}/kconfig_bin/mconf-idf)

    externalproject_add(mconf-idf
        SOURCE_DIR ${IDF_PATH}/tools/kconfig
        CONFIGURE_COMMAND ""
        BINARY_DIR "kconfig_bin"
        BUILD_COMMAND make -f ${IDF_PATH}/tools/kconfig/Makefile mconf-idf
        BUILD_BYPRODUCTS ${MCONF}
        INSTALL_COMMAND ""
        EXCLUDE_FROM_ALL 1
        )

    file(GLOB mconf_srcfiles ${IDF_PATH}/tools/kconfig/*.c)
    externalproject_add_stepdependencies(mconf-idf build
        ${mconf_srcfiles}
        ${IDF_PATH}/tools/kconfig/Makefile
        ${CMAKE_CURRENT_LIST_FILE})
    unset(mconf_srcfiles)

    set(menuconfig_depends DEPENDS mconf-idf)

endif()

# Find all Kconfig files for all components
function(kconfig_process_config)
    file(MAKE_DIRECTORY "${CONFIG_DIR}")
    set(kconfigs)
    set(kconfigs_projbuild)

    # Components are usually sorted (somewhat) topologically via their dependencies. This extends to the component
    # paths list. Obtain an alphabetical list in order to present menus also in the same order.
    set(components ${BUILD_COMPONENTS})
    list(SORT components)

    foreach(component ${components})
        list(FIND BUILD_COMPONENTS ${component} idx)
        list(GET BUILD_COMPONENT_PATHS ${idx} component_path)
        list(APPEND component_paths ${component_path})
    endforeach()

    # Find Kconfig and Kconfig.projbuild for each component as applicable
    # if any of these change, cmake should rerun
    foreach(dir ${component_paths})
        file(GLOB kconfig "${dir}/Kconfig")
        if(kconfig)
            set(kconfigs "${kconfigs} ${kconfig}")
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${kconfig})
        endif()
        file(GLOB kconfig ${dir}/Kconfig.projbuild)
        if(kconfig)
            set(kconfigs_projbuild "${kconfigs_projbuild} ${kconfig}")
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${kconfig})
        endif()
    endforeach()

    if(IDF_SDKCONFIG_DEFAULTS)
        set(defaults_arg --defaults "${IDF_SDKCONFIG_DEFAULTS}")
    endif()

    if(EXISTS "${IDF_SDKCONFIG_DEFAULTS}.${IDF_TARGET}")
        list(APPEND defaults_arg --defaults "${IDF_SDKCONFIG_DEFAULTS}.${IDF_TARGET}")
    endif()

    # Set these in the parent scope, so that they can be written to project_description.json
    set(kconfigs "${kconfigs}")
    set(COMPONENT_KCONFIGS "${kconfigs}" PARENT_SCOPE)
    set(COMPONENT_KCONFIGS_PROJBUILD "${kconfigs_projbuild}" PARENT_SCOPE)

    set(confgen_basecommand
        ${PYTHON} ${IDF_PATH}/tools/kconfig_new/confgen.py
        --kconfig ${ROOT_KCONFIG}
        --config ${SDKCONFIG}
        ${defaults_arg}
        --env "COMPONENT_KCONFIGS=${kconfigs}"
        --env "COMPONENT_KCONFIGS_PROJBUILD=${kconfigs_projbuild}"
        --env "IDF_CMAKE=y")

    # Generate the menuconfig target (uses C-based mconf-idf tool, either prebuilt or via mconf-idf target above)
    add_custom_target(menuconfig
        ${menuconfig_depends}
        # create any missing config file, with defaults if necessary
        COMMAND ${confgen_basecommand} --env "IDF_TARGET=${IDF_TARGET}" --output config ${SDKCONFIG}
        COMMAND ${CMAKE_COMMAND} -E env
        "COMPONENT_KCONFIGS=${kconfigs}"
        "COMPONENT_KCONFIGS_PROJBUILD=${kconfigs_projbuild}"
        "IDF_CMAKE=y"
        "KCONFIG_CONFIG=${SDKCONFIG}"
        "IDF_TARGET=${IDF_TARGET}"
        ${MCONF} ${ROOT_KCONFIG}
        VERBATIM
        USES_TERMINAL)

    # Custom target to run confserver.py from the build tool
    add_custom_target(confserver
        COMMAND ${CMAKE_COMMAND} -E env
        "COMPONENT_KCONFIGS=${kconfigs}"
        "COMPONENT_KCONFIGS_PROJBUILD=${kconfigs_projbuild}"
        ${PYTHON} ${IDF_PATH}/tools/kconfig_new/confserver.py --kconfig ${IDF_PATH}/Kconfig --config ${SDKCONFIG}
        VERBATIM
        USES_TERMINAL)

    # Generate configuration output via confgen.py
    # makes sdkconfig.h and skdconfig.cmake
    #
    # This happens during the cmake run not during the build
    if(NOT BOOTLOADER_BUILD)
        execute_process(
            COMMAND ${confgen_basecommand}
            --output header ${SDKCONFIG_HEADER}
            --output cmake ${SDKCONFIG_CMAKE}
            --output json ${SDKCONFIG_JSON}
            --output json_menus ${KCONFIG_JSON_MENUS}
            --output config ${SDKCONFIG} # only generate config at the top-level project
            RESULT_VARIABLE config_result)
    else()
        execute_process(
            COMMAND ${confgen_basecommand}
            --output header ${SDKCONFIG_HEADER}
            --output cmake ${SDKCONFIG_CMAKE}
            --output json ${SDKCONFIG_JSON}
            --output json_menus ${KCONFIG_JSON_MENUS}
            RESULT_VARIABLE config_result)
    endif()
    if(config_result)
        message(FATAL_ERROR "Failed to run confgen.py (${confgen_basecommand}). Error ${config_result}")
    endif()

    # When sdkconfig file changes in the future, trigger a cmake run
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${SDKCONFIG}")

    # Ditto if either of the generated files are missing/modified (this is a bit irritating as it means
    # you can't edit these manually without them being regenerated, but I don't know of a better way...)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${SDKCONFIG_HEADER}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${SDKCONFIG_CMAKE}")

    # Or if the config generation tool changes
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${IDF_PATH}/tools/kconfig_new/confgen.py")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${IDF_PATH}/tools/kconfig_new/kconfiglib.py")

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES
        "${SDKCONFIG_HEADER}" "${SDKCONFIG_CMAKE}")

endfunction()