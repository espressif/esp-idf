include(component_utils)

macro(idf_set_target)
    # Input is IDF_TARGET environement variable
    set(env_idf_target $ENV{IDF_TARGET})

    if(NOT env_idf_target)
        # IDF_TARGET not set in environment, see if it is set in cache
        if(IDF_TARGET)
            set(env_idf_target ${IDF_TARGET})
        else()
            set(env_idf_target esp32)
            message(STATUS "IDF_TARGET not set, using default target: ${env_idf_target}")
        endif()
    else()
        # IDF_TARGET set both in environment and in cache, must be the same
        if(NOT ${IDF_TARGET} STREQUAL ${env_idf_target})
            message(FATAL_ERROR "IDF_TARGET in CMake cache does not match "
                            "IDF_TARGET environment variable. To change the target, clear "
                            "the build directory and sdkconfig file, and build the project again")
        endif()
    endif()

    # IDF_TARGET will be used by Kconfig, make sure it is set
    set(ENV{IDF_TARGET} ${env_idf_target})

    # Finally, set IDF_TARGET in cache
    set(IDF_TARGET ${env_idf_target} CACHE STRING "IDF Build Target")

    message(STATUS "Building for target ${IDF_TARGET}")
endmacro()

macro(idf_check_config_target)
    if(NOT ${IDF_TARGET} STREQUAL ${CONFIG_IDF_TARGET})
        message(FATAL_ERROR "CONFIG_IDF_TARGET in sdkconfig does not match "
            "IDF_TARGET environement variable. To change the target, delete "
            "sdkconfig file and build the project again.")
    endif()
endmacro()

macro(idf_set_toolchain)
    # First try to load the toolchain file from the tools/cmake/ directory of IDF
    set(toolchain_file_global $ENV{IDF_PATH}/tools/cmake/toolchain-${IDF_TARGET}.cmake)
    if(EXISTS ${toolchain_file_global})
        set(CMAKE_TOOLCHAIN_FILE ${toolchain_file_global})
    else()
        # Try to load the toolchain file from the directory of ${IDF_TARGET} component
        components_find_all("${IDF_COMPONENT_DIRS}" ALL_COMPONENT_PATHS ALL_COMPONENTS ALL_TEST_COMPONENTS)
        find_component_path(${IDF_TARGET} "${ALL_COMPONENTS}" "${ALL_COMPONENT_PATHS}" target_component_path)
        set(toolchain_file_component ${target_component_path}/toolchain-${IDF_TARGET}.cmake)
        if(EXISTS ${toolchain_file_component})
            set(CMAKE_TOOLCHAIN_FILE ${toolchain_file_component})
        else()
            message(FATAL_ERROR "Toolchain file toolchain-${IDF_TARGET}.cmake not found,"
                    "checked ${toolchain_file_global} and ${toolchain_file_component}")
        endif()
    endif()
endmacro()
