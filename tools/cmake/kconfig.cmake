include(ExternalProject)

macro(kconfig_set_variables)
    set_default(SDKCONFIG ${PROJECT_PATH}/sdkconfig)
    set(SDKCONFIG_HEADER ${CMAKE_BINARY_DIR}/sdkconfig.h)
    set(SDKCONFIG_CMAKE ${CMAKE_BINARY_DIR}/sdkconfig.cmake)
    set(SDKCONFIG_JSON ${CMAKE_BINARY_DIR}/sdkconfig.json)

    set(ROOT_KCONFIG ${IDF_PATH}/Kconfig)

    set_default(SDKCONFIG_DEFAULTS "${SDKCONFIG}.defaults")
endmacro()

if(CMAKE_HOST_WIN32)
    # Prefer a prebuilt mconf on Windows
    find_program(WINPTY winpty)
    find_program(MCONF mconf)

    if(NOT MCONF)
        find_program(NATIVE_GCC gcc)
        if(NOT NATIVE_GCC)
            message(FATAL_ERROR
                "Windows requires a prebuilt ESP-IDF-specific mconf for your platform "
                "on the PATH, or an MSYS2 version of gcc on the PATH to build mconf. "
                "Consult the setup docs for ESP-IDF on Windows.")
        endif()
    elseif(WINPTY)
        set(MCONF "${WINPTY}" "${MCONF}")
    endif()
endif()

if(NOT MCONF)
    # Use the existing Makefile to build mconf (out of tree) when needed
    #
    set(MCONF kconfig_bin/mconf)

    externalproject_add(mconf
        SOURCE_DIR ${IDF_PATH}/tools/kconfig
        CONFIGURE_COMMAND ""
        BINARY_DIR "kconfig_bin"
        BUILD_COMMAND make -f ${IDF_PATH}/tools/kconfig/Makefile mconf
        BUILD_BYPRODUCTS ${MCONF}
        INSTALL_COMMAND ""
        EXCLUDE_FROM_ALL 1
        )
    set(menuconfig_depends DEPENDS mconf)
endif()

# Find all Kconfig files for all components
function(kconfig_process_config)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/config")
    set(kconfigs)
    set(kconfigs_projbuild)

    # Find Kconfig and Kconfig.projbuild for each component as applicable
    # if any of these change, cmake should rerun
    foreach(dir ${BUILD_COMPONENT_PATHS} "${CMAKE_SOURCE_DIR}/main")
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

    if(EXISTS ${SDKCONFIG_DEFAULTS})
        set(defaults_arg --defaults "${SDKCONFIG_DEFAULTS}")
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
        --create-config-if-missing
        --env "COMPONENT_KCONFIGS=${kconfigs}"
        --env "COMPONENT_KCONFIGS_PROJBUILD=${kconfigs_projbuild}")

    # Generate the menuconfig target (uses C-based mconf tool, either prebuilt or via mconf target above)
    add_custom_target(menuconfig
        ${menuconfig_depends}
        # create any missing config file, with defaults if necessary
        COMMAND ${confgen_basecommand} --output config ${SDKCONFIG}
        COMMAND ${CMAKE_COMMAND} -E env
        "COMPONENT_KCONFIGS=${kconfigs}"
        "COMPONENT_KCONFIGS_PROJBUILD=${kconfigs_projbuild}"
        "KCONFIG_CONFIG=${SDKCONFIG}"
        ${MCONF} ${ROOT_KCONFIG}
        VERBATIM
        USES_TERMINAL)

    # Generate configuration output via confgen.py
    # makes sdkconfig.h and skdconfig.cmake
    #
    # This happens during the cmake run not during the build
    execute_process(COMMAND ${confgen_basecommand}
        --output header ${SDKCONFIG_HEADER}
        --output cmake ${SDKCONFIG_CMAKE}
        --output json ${SDKCONFIG_JSON}
        RESULT_VARIABLE config_result)
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
