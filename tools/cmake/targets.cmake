#
# Set the target used for the standard project build.
#
macro(__target_init)
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
endmacro()

#
# Check that the set build target and the config target matches.
#
function(__target_check)
    # Should be called after sdkconfig CMake file has been included.
    idf_build_get_property(idf_target IDF_TARGET)
    if(NOT ${idf_target} STREQUAL ${CONFIG_IDF_TARGET})
        message(FATAL_ERROR "CONFIG_IDF_TARGET in sdkconfig does not match "
            "IDF_TARGET environment variable. To change the target, delete "
            "sdkconfig file and build the project again.")
    endif()
endfunction()

#
# Used by the project CMake file to set the toolchain before project() call.
#
macro(__target_set_toolchain)
    idf_build_get_property(idf_path IDF_PATH)
    # First try to load the toolchain file from the tools/cmake/directory of IDF
    set(toolchain_file_global ${idf_path}/tools/cmake/toolchain-${IDF_TARGET}.cmake)
    if(EXISTS ${toolchain_file_global})
        set(CMAKE_TOOLCHAIN_FILE ${toolchain_file_global})
    else()
        __component_get_target(component_target ${IDF_TARGET})
        if(NOT component_target)
            message(FATAL_ERROR "Unable to resolve '${IDF_TARGET}' for setting toolchain file.")
        endif()
        get_property(component_dir TARGET ${component_target} PROPERTY COMPONENT_DIR)
        # Try to load the toolchain file from the directory of IDF_TARGET component
        set(toolchain_file_component ${component_dir}/toolchain-${IDF_TARGET}.cmake)
        if(EXISTS ${toolchain_file_component})
            set(CMAKE_TOOLCHAIN_FILE ${toolchain_file_component})
        else()
            message(FATAL_ERROR "Toolchain file toolchain-${IDF_TARGET}.cmake not found,"
                    "checked ${toolchain_file_global} and ${toolchain_file_component}")
        endif()
    endif()
endmacro()
