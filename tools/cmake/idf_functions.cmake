# Some IDF-specific functions and functions

include(crosstool_version_check)

#
# Set some variables used by rest of the build
#
# Note at the time this macro is expanded, the config is not yet
# loaded and the toolchain and project are not yet set
#
macro(idf_set_global_variables)
    # Note that CONFIG_xxx is not available when this function is called

    set_default(EXTRA_COMPONENT_DIRS "")

    # Commmon components, required by every component in the build
    #
    set_default(COMPONENT_REQUIRES_COMMON "cxx esp32 newlib freertos heap log soc")

    # PROJECT_PATH has the path to the IDF project (top-level cmake directory)
    #
    # (cmake calls this CMAKE_SOURCE_DIR, keeping old name for compatibility.)
    set(PROJECT_PATH "${CMAKE_SOURCE_DIR}")

    if(MAIN_SRCS)
        message(WARNING "main is now a component, use of MAIN_SRCS is deprecated")
        set_default(COMPONENT_DIRS "${PROJECT_PATH}/components ${EXTRA_COMPONENT_DIRS} \
                                    ${IDF_PATH}/components")
    else()
        set_default(COMPONENT_DIRS "${PROJECT_PATH}/components ${EXTRA_COMPONENT_DIRS} \
                                    ${IDF_PATH}/components ${PROJECT_PATH}/main")
    endif()

    spaces2list(COMPONENT_DIRS)

    spaces2list(COMPONENTS)

    # Tell cmake to drop executables in the top-level build dir
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

    # path to idf.py tool
    set(IDFTOOL ${PYTHON} "${IDF_PATH}/tools/idf.py")
endmacro()

# Add all the IDF global compiler & preprocessor options
# (applied to all components). Some are config-dependent
#
# If you only want to set options for a particular component,
# don't call or edit this function. TODO DESCRIBE WHAT TO DO INSTEAD
#
function(idf_set_global_compiler_options)
    add_definitions(-DESP_PLATFORM)
    add_definitions(-DHAVE_CONFIG_H)

    if(CONFIG_OPTIMIZATION_LEVEL_RELEASE)
        add_compile_options(-Os)
    else()
        add_compile_options(-Og)
    endif()

    add_c_compile_options(-std=gnu99)

    add_cxx_compile_options(-std=gnu++11 -fno-rtti)

    if(CONFIG_CXX_EXCEPTIONS)
        add_cxx_compile_options(-fexceptions)
    else()
        add_cxx_compile_options(-fno-exceptions)
    endif()

    # Default compiler configuration
    add_compile_options(-ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib)

    # Default warnings configuration
    add_compile_options(
        -Wall
        -Werror=all
        -Wno-error=unused-function
        -Wno-error=unused-but-set-variable
        -Wno-error=unused-variable
        -Wno-error=deprecated-declarations
        -Wextra
        -Wno-unused-parameter
        -Wno-sign-compare)
    add_c_compile_options(
        -Wno-old-style-declaration
        )

    # Stack protection
    if(NOT BOOTLOADER_BUILD)
        if(CONFIG_STACK_CHECK_NORM)
            add_compile_options(-fstack-protector)
        elseif(CONFIG_STACK_CHECK_STRONG)
            add_compile_options(-fstack-protector-strong)
        elseif(CONFIG_STACK_CHECK_ALL)
            add_compile_options(-fstack-protector-all)
        endif()
    endif()

    if(CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED)
        add_definitions(-DNDEBUG)
    endif()

    # Always generate debug symbols (even in Release mode, these don't
    # go into the final binary so have no impact on size)
    add_compile_options(-ggdb)

    # Enable ccache if it's on the path
    if(NOT CCACHE_DISABLE)
        find_program(CCACHE_FOUND ccache)
        if(CCACHE_FOUND)
            message(STATUS "ccache will be used for faster builds")
            set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        endif()
    endif()

endfunction()


# Verify the IDF environment is configured correctly (environment, toolchain, etc)
function(idf_verify_environment)

    if(NOT CMAKE_PROJECT_NAME)
        message(FATAL_ERROR "Internal error, IDF project.cmake should have set this variable already")
    endif()

    # Check toolchain is configured properly in cmake
    if(NOT ( ${CMAKE_SYSTEM_NAME} STREQUAL "Generic" AND ${CMAKE_C_COMPILER} MATCHES xtensa))
        message(FATAL_ERROR "Internal error, toolchain has not been set correctly by project "
            "(or an invalid CMakeCache.txt file has been generated somehow)")
    endif()

    #
    # Warn if the toolchain version doesn't match
    #
    # TODO: make these platform-specific for diff toolchains
    gcc_version_check("5.2.0")
    crosstool_version_check("1.22.0-80-g6c4433a")

endfunction()

# idf_add_executable
#
# Calls add_executable to add the final project executable
# Adds .map & .bin file targets
# Sets up flash-related targets
function(idf_add_executable)
    set(exe_target ${PROJECT_NAME}.elf)

    if(MAIN_SRCS)
        spaces2list(MAIN_SRCS)
        add_executable(${exe_target} ${MAIN_SRCS})
    else()
        # Create a dummy file to work around CMake requirement of having a source
        # file while adding an executable
        add_executable(${exe_target} "${CMAKE_CURRENT_BINARY_DIR}/dummy_main_src.c")
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy_main_src.c)

        add_custom_target(dummy_main_src DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dummy_main_src.c)

        add_dependencies(${exe_target} dummy_main_src)
    endif()

    add_map_file(${exe_target})
endfunction()


# add_map_file
#
# Set linker args for 'exe_target' to generate a linker Map file
function(add_map_file exe_target)
    get_filename_component(basename ${exe_target} NAME_WE)
    set(mapfile "${basename}.map")
    target_link_libraries(${exe_target} "-Wl,--gc-sections -Wl,--cref -Wl,--Map=${mapfile} -Wl,--start-group")
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${mapfile}")

    # add size targets, depend on map file, run idf_size.py
    add_custom_target(size
        DEPENDS ${exe_target}
        COMMAND ${PYTHON} ${IDF_PATH}/tools/idf_size.py ${mapfile}
        )
    add_custom_target(size-files
        DEPENDS ${exe_target}
        COMMAND ${PYTHON} ${IDF_PATH}/tools/idf_size.py --files ${mapfile}
        )
    add_custom_target(size-components
        DEPENDS ${exe_target}
        COMMAND ${PYTHON} ${IDF_PATH}/tools/idf_size.py --archives ${mapfile}
        )

endfunction()

# component_compile_options
#
# Wrapper around target_compile_options that passes the component name
function(component_compile_options)
    target_compile_options(${COMPONENT_NAME} PRIVATE ${ARGV})
endfunction()

# component_compile_definitions
#
# Wrapper around target_compile_definitions that passes the component name
function(component_compile_definitions)
    target_compile_definitions(${COMPONENT_NAME} PRIVATE ${ARGV})
endfunction()

# idf_get_git_revision
#
# Set global IDF_VER to the git revision of ESP-IDF.
#
# Running git_describe() here automatically triggers rebuilds
# if the ESP-IDF git version changes
function(idf_get_git_revision)
    if(EXISTS "${IDF_PATH}/version.txt")
        file(STRINGS "${IDF_PATH}/version.txt" IDF_VER)
    else()
        git_describe(IDF_VER "${IDF_PATH}")
    endif()
    add_definitions(-DIDF_VER=\"${IDF_VER}\")
    git_submodule_check("${IDF_PATH}")
    set(IDF_VER ${IDF_VER} PARENT_SCOPE)
endfunction()
