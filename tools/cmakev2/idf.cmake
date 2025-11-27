# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

cmake_minimum_required(VERSION 3.22)

# Update the CMAKE_MODULE_PATH to ensure that additional cmakev2 build system
# modules can be included. The third_party directory from cmakev1 is also
# included for third-party modules shared with cmakev1.
set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../cmake/third_party"
    ${CMAKE_MODULE_PATH})

# The version.cmake file contains the IDF_VERSION variables, which are the same
# for both cmakev1 and cmakev2.
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/version.cmake)

# The gdbinit.cmake file from cmakev1 contains a single function,
# __generate_gdbinit, which is used in the generation of
# project_description.json.
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/gdbinit.cmake)

# The openocd.cmake file from cmakev1 contains a single function,
# __get_openocd_options, which is used in the generation of
# project_description.json.
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/openocd.cmake)

# The depgraph.cmake file from cmakev1 contains helper functions for generating
# a component dependency graph. Let's reuse these functions in the
# idf_build_generate_depgraph function.
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/depgraph.cmake)

include(component)
include(build)
include(kconfig)
include(project)
include(manager)
include(compat)
include(ldgen)
include(dfu)
include(uf2)
include(size)
include(GetGitRevisionDescription)
# For backward compatibility, since externalproject_add is used by
# project_include.cmake in the bootloader component. The ExternalProject
# should probably be included there instead.
include(ExternalProject)

#[[
    __init_build_version()

    Set the global variables IDF_BUILD_V2, IDF_BUILD_VER and IDF_BUILD_VER_TAG,
    as well as the build properties and environmental variables.
#]]
function(__init_build_version)
    set(IDF_BUILD_V2 y PARENT_SCOPE)
    set(IDF_BUILD_VER 2 PARENT_SCOPE)
    set(IDF_BUILD_VER_TAG "v2" PARENT_SCOPE)
    idf_build_set_property(IDF_BUILD_V2 y)
    idf_build_set_property(IDF_BUILD_VER 2)
    idf_build_set_property(IDF_BUILD_VER_TAG "v2")
    set(ENV{IDF_BUILD_V2} y)
    set(ENV{IDF_BUILD_VER} 2)
    set(ENV{IDF_BUILD_VER_TAG} "v2")
endfunction()

#[[
    __init_idf_path()

    Determine the IDF_PATH value, either from the IDF_PATH environmental
    variable or based on the location of this file. Also check there is no
    inconsistency between the two.

    Set the IDF_PATH global variable, environment variable and build property.
#]]
function(__init_idf_path)
    get_filename_component(idf_path_infer "${CMAKE_CURRENT_LIST_DIR}/../.." REALPATH)

    if(NOT DEFINED ENV{IDF_PATH})
        idf_warn("IDF_PATH environment variable not found. "
                 "Setting IDF_PATH to '${idf_path_infer}'.")
        set(idf_path "${idf_path_infer}")
    else()
        get_filename_component(idf_path_env "$ENV{IDF_PATH}" REALPATH)
        if(NOT "${idf_path_env}" STREQUAL "${idf_path_infer}")
            idf_warn("IDF_PATH environment variable is different from inferred IDF_PATH. "
                     "Check if your project's top-level CMakeLists.txt includes the right "
                     "CMake files. Environment IDF_PATH will be used for the build: "
                     "'${idf_path_env}'")
        endif()
        set(idf_path "${idf_path_env}")
    endif()

    idf_build_set_property(IDF_PATH "${idf_path}")
    set(IDF_PATH ${idf_path} PARENT_SCOPE)
    set(ENV{IDF_PATH} ${idf_path})
endfunction()

#[[
    __init_git()

    Determine the executable.

    Set the GIT build property.
#]]
function(__init_git)
    find_package(Git)
    if(NOT GIT_FOUND)
        idf_build_set_property(GIT NOTFOUND)
        idf_warn("Git executable not found.")
        return()
    endif()

    idf_build_set_property(GIT "${GIT_EXECUTABLE}")
endfunction()

#[[
    __init_idf_version()

    Determine the IDF version from the version.txt file. If it is not present,
    use git-describe. If both previous attempts fail, use the IDF_VERSION from
    the environment variables as a fallback.

    Set IDF_VER build property.
#]]
function(__init_idf_version)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(git GIT)
    if(EXISTS "${idf_path}/version.txt")
        file(STRINGS "${idf_path}/version.txt" idf_ver)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${idf_path}/version.txt")
    else()
        # Try to get the version using git-describe.
        git_describe(idf_ver "${idf_path}" "--match=v*.*")
        if(NOT idf_ver)
            # The Git describe command failed unexpectedly, so the version is
            # set to IDF_VERSION as specified in version.cmake.
            set(idf_ver "$ENV{IDF_VERSION}")
        endif()
    endif()
    # Trim IDF_VER to the necessary 32 characters.
    string(SUBSTRING "${idf_ver}" 0 31 idf_ver)
    idf_build_set_property(IDF_VER ${idf_ver})
endfunction()

#[[
    __init_python()

    Determine Python interpreter, either from the PYTHON CMake cache variable
    or environmental variable or default it to "python".

    If the PYTHON_DEPS_CHECKED CMake cache variable is not set, check if all
    Python packages dependencies are satisfied. For instance, if a tool calling
    CMake has already performed this check, it doesn't need to be repeated.

    Set the global PYTHON variable, environment variable and build property.
#]]
function(__init_python)
    __get_default_value(VARIABLE PYTHON
                        DEFAULT "python"
                        OUTPUT python)
    file(TO_CMAKE_PATH ${python} python)

    idf_build_set_property(PYTHON "${python}")
    set(PYTHON "${python}" PARENT_SCOPE)

    if(PYTHON_DEPS_CHECKED)
        idf_dbg("Python dependencies have already been verified.")
        return()
    endif()

    idf_build_get_property(idf_path IDF_PATH)

    idf_msg("Checking Python dependencies...")
    execute_process(
        COMMAND "${python}" "${idf_path}/tools/idf_tools.py" "check-python-dependencies"
        RESULT_VARIABLE result
    )
    if(result EQUAL 1)
        # The function check_python_dependencies returns an error code of 1 if
        # it fails.
        idf_die("Some Python dependencies must be installed. Check above message for details.")
    elseif(NOT result EQUAL 0)
        # This means that check_python_dependencies.py failed to run entirely,
        # and the result should be an error message.
        idf_die("Failed to run Python dependency check. Python: ${python}, Error: ${result}")
    endif()
endfunction()

#[[
    __init_idf_target()

    Determine the IDF_TARGET value from the IDF_TARGET environment variable,
    the CMake cache variable, or the sdkconfig files. If none of these are set,
    use the default esp32 target. Ensure there are no inconsistencies in the
    IDF_TARGET values set in different locations.

    Set the IDF_TARGET as a global variable, in the CMake cache, as an
    environment variable, and as a build property.
#]]
function(__init_idf_target)
    set(sdkconfig_target "")
    set(target "")
    set(sdkconfig_file "")

    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(sdkconfig_defaults SDKCONFIG_DEFAULTS)

    foreach(config ${sdkconfig} ${sdkconfig_defaults})
        idf_dbg("Searching for target in '${config}'")
        __get_sdkconfig_option(OPTION CONFIG_IDF_TARGET
                               SDKCONFIG "${config}"
                               OUTPUT sdkconfig_target)
        if(sdkconfig_target)
            set(sdkconfig_file "${config}")
            break()
        endif()
    endforeach()

    __get_default_value(VARIABLE IDF_TARGET
                        DEFAULT NOTFOUND
                        OUTPUT target)

    if(NOT target)
        if(sdkconfig_target)
            idf_msg("IDF_TARGET is not set, guessed '${sdkconfig_target}' "
                    "from sdkconfig '${sdkconfig_file}'")
            set(target "${sdkconfig_target}")
        else()
            idf_msg("IDF_TARGET not set, using default target: esp32")
            set(target "esp32")
        endif()
    endif()

    # Verify that the chosen target aligns with the CMake cache.
    set(cache_target $CACHE{IDF_TARGET})
    if(cache_target)
        if(NOT "${cache_target}" STREQUAL "${target}")
            idf_die("IDF_TARGET '${cache_target}' in CMake cache does not match "
                    "currently selected IDF_TARGET '${target}'. "
                    "To change the target, clear the build directory and sdkconfig file, "
                    "and build the project again.")
        endif()
    endif()

    # Verify that the chosen target aligns with the sdkconfig.
    if(sdkconfig_target AND "${sdkconfig_file}" STREQUAL "${sdkconfig}")
        if("$ENV{_IDF_PY_SET_TARGET_ACTION}" STREQUAL "1")
            idf_dbg("The target consistency check for the target specified in ${sdkconfig} "
                    "was skipped because the set-target action is being executed.")
        elseif(NOT "${sdkconfig_target}" STREQUAL "${target}")
            idf_die("Target '${sdkconfig_target}' in sdkconfig '${sdkconfig}' "
                    "does not match currently selected IDF_TARGET '${target}'. "
                    "To change the target, clear the build directory and sdkconfig file, "
                    "and build the project again.")
        endif()
    endif()

    idf_build_set_property(IDF_TARGET "${target}")
    set(ENV{IDF_TARGET} ${target})
    set(IDF_TARGET ${target} CACHE STRING "IDF Build Target")
endfunction()

#[[
    __init_toolchain()

    Determine the IDF_TOOLCHAIN value from the IDF_TOOLCHAIN environment
    variable or the CMake cache variable. If none of these are set, use the
    default gcc toolchain. Ensure there are no inconsistencies in the
    IDF_TOOLCHAIN values set in different locations. Also ensure that the
    CMAKE_TOOLCHAIN_FILE is set to the correct file according to the current
    IDF_TARGET.

    Set the IDF_TOOLCHAIN and IDF_TOOLCHAIN_FILE build properties. Also,
    configure the IDF_TOOLCHAIN CMake cache variable and set the
    CMAKE_TOOLCHAIN_FILE global variable.
#]]
function(__init_toolchain)
    set(cache_toolchain $CACHE{IDF_TOOLCHAIN})
    set(cache_toolchain_file $CACHE{CMAKE_TOOLCHAIN_FILE})

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(idf_target IDF_TARGET)

    __get_default_value(VARIABLE IDF_TOOLCHAIN
                        DEFAULT gcc
                        OUTPUT toolchain)
    if(cache_toolchain)
        if(NOT "${cache_toolchain}" STREQUAL "${toolchain}")
            idf_die("IDF_TOOLCHAIN '${cache_toolchain}' in CMake cache does not match "
                    "currently selected IDF_TOOLCHAIN '${toolchain}'. To change "
                    "the toolchain, clear the build directory and sdkconfig file, "
                    "and build the project again.")
        endif()
    endif()

    if("${toolchain}" STREQUAL "clang")
        set(toolchain_type "clang-")
    endif()

    # Check that the selected target is consistent with the toolchain file in
    # the CMake cache.
    if(cache_toolchain_file)
        string(FIND "${cache_toolchain_file}" "-${toolchain_type}${idf_target}.cmake" found)
        if(${found} EQUAL -1)
            get_filename_component(cache_toolchain_file_stem "${cache_toolchain_file}" NAME_WE)
            idf_die("CMAKE_TOOLCHAIN_FILE '${cache_toolchain_file_stem}' "
                    "does not match currently selected IDF_TARGET '${idf_target}'. "
                    "To change the target, clear the build directory and sdkconfig file, "
                    "and build the project again.")
        endif()
    endif()

    set(toolchain_file "${idf_path}/tools/cmake/toolchain-${toolchain_type}${idf_target}.cmake")
    if(NOT EXISTS ${toolchain_file})
        idf_die("Toolchain file ${toolchain_file} not found")
    endif()

    set(IDF_TOOLCHAIN ${toolchain} CACHE STRING "IDF Build Toolchain Type")
    set(CMAKE_TOOLCHAIN_FILE "${toolchain_file}" PARENT_SCOPE)
    idf_build_set_property(IDF_TOOLCHAIN "${toolchain}")
    idf_build_set_property(IDF_TOOLCHAIN_FILE "${toolchain_file}")
endfunction()

#[[
    __init_ccache()

    Enable ccache if requested through CCACHE_ENABLE.
#]]
function(__init_ccache)
    if(NOT CCACHE_ENABLE)
        return()
    endif()

    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        idf_msg("ccache will be used for faster recompilation")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    else()
        idf_warn("enabled ccache in build but ccache program not found")
    endif()
endfunction()

#[[
    __init_components()

    Search for possible component directories categorized by their source,
    which could be ``idf_components``, ``project_extra_components``, or
    ``project_components``. Components added by the component manager are
    initialized later as ``project_managed_components`` after the component
    manager is called.

    The search respects the variables set by the user e.g. in the project's
    CMakeLists.txt file. These are maintained for backward compatibility.

    COMPONENT_DIRS
        If set, component directories are searched exclusively in the paths
        provided in ``COMPONENT_DIRS``.

    EXTRA_COMPONENT_DIRS
        Includes extra paths to search if ``COMPONENT_DIRS`` is not specified.

    EXTRA_COMPONENT_EXCLUDE_DIRS
        List of paths to exclude from searching the component directories.

    Each component is initialized for every component directory found.
#]]
function(__init_components)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(prefix PREFIX)

    __get_component_paths(PATHS "${idf_path}/components"
                          OUTPUT idf_components)
    if(COMPONENT_DIRS)
        # The user explicitly stated the locations to search for components.
        # For backward compatibility, check that the paths in
        # COMPONENT_DIRS exist.
        __get_component_paths(PATHS ${COMPONENT_DIRS}
                              EXCLUDE_PATHS ${EXTRA_COMPONENT_EXCLUDE_DIRS}
                              SOURCE "COMPONENT_DIRS"
                              CHECK
                              OUTPUT project_components)
    else()
        __get_component_paths(PATHS "${CMAKE_CURRENT_SOURCE_DIR}/main"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/components"
                              EXCLUDE_PATHS ${EXTRA_COMPONENT_EXCLUDE_DIRS}
                              OUTPUT project_components)
        if(EXTRA_COMPONENT_DIRS)
            # For backward compatibility, check that the paths in
            # EXTRA_COMPONENT_DIRS exist.
            __get_component_paths(PATHS ${EXTRA_COMPONENT_DIRS}
                                  EXCLUDE_PATHS ${EXTRA_COMPONENT_EXCLUDE_DIRS}
                                  SOURCE "EXTRA_COMPONENT_DIRS"
                                  CHECK
                                  OUTPUT project_extra_components)
        endif()
    endif()

    foreach(path IN LISTS idf_components)
        __init_component(DIRECTORY "${path}"
                         PREFIX "${prefix}"
                         SOURCE "idf_components")
    endforeach()

    foreach(path IN LISTS project_components)
        __init_component(DIRECTORY "${path}"
                         PREFIX "${prefix}"
                         SOURCE "project_components")
    endforeach()

    foreach(path IN LISTS project_extra_components)
        __init_component(DIRECTORY "${path}"
                         PREFIX "${prefix}"
                         SOURCE "project_extra_components")
    endforeach()
endfunction()

#[[
    __init_submodules()

    Initialize submodules that are not yet initialized, and issue a warning for
    submodules that do not match the recorded hash in the git tree.
#]]
function(__init_submodules)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(git GIT)

    # For internal use: Skip the submodule check if running on GitLab CI
    # and the job is configured to not clone submodules.
    if("$ENV{IDF_SKIP_CHECK_SUBMODULES}" STREQUAL "1")
        idf_msg("skip submodule check on internal CI")
        return()
    endif()

    if(NOT git)
        idf_warn("Git executable was not found. Git submodule checks will not be executed. "
                 "If you have any build issues at all, start by adding git executable to "
                 "the PATH and rerun cmake to not see this warning again.")
        return()
    endif()

    execute_process(
        COMMAND ${git} submodule status
        WORKING_DIRECTORY ${idf_path}
        OUTPUT_VARIABLE status
        ERROR_VARIABLE stderr
        RESULT_VARIABLE rv
    )

    if(rv)
        idf_warn("Git submodule status command failed(${rv}): ${stderr}"
                 "Git submodule checks will not be performed. ")
        return()
    endif()

    __split(STRING "${status}"
            OUTPUT lines
            REMOVE_EMPTY)

    # The output of the git submodule status command is not guaranteed to be
    # stable. It may be necessary to check the GIT_VERSION_STRING and make
    # adjustments in the future.
    foreach(line IN LISTS lines)
        string(REGEX MATCH "(.)[0-9a-f]+ ([^\( ]+) ?" _ignored "${line}")
        set(status "${CMAKE_MATCH_1}")
        set(submodule_path "${CMAKE_MATCH_2}")

        if("${status}" STREQUAL "-")  # missing submodule
            idf_msg("Initialising new submodule ${submodule_path}...")
            execute_process(
                COMMAND ${git} submodule update --init --recursive ${submodule_path}
                WORKING_DIRECTORY ${idf_path}
                ERROR_VARIABLE stderr
                RESULT_VARIABLE rv
                )
            if(rv)
                idf_die("Git submodule '${submodule_path}' init failed(${rv}): ${stderr}")
            endif()

        elseif(NOT "${status}" STREQUAL " ")
            idf_warn("Git submodule ${submodule_path} is out of date. "
                     "Run the following command to fix: "
                     "git submodule update --init --recursive")
        endif()

        # Ensure CMake is rerun if the submodule's .git file is modified or
        # altered, such as in the case of an accidental deinitialization.
        get_filename_component(submodule_abs_path ${submodule_path} ABSOLUTE BASE_DIR ${idf_path})
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${submodule_abs_path}/.git)
        # If the HEAD file in the submodule's directory changes (i.e., if there
        # are commit changes), it will at least display the 'out of date'
        # warning.
        set(submodule_head "${idf_path}/.git/modules/${submodule_path}/HEAD")
        if(EXISTS "${submodule_head}")
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${submodule_head})
        endif()

    endforeach()
endfunction()

#[[
    __init_idf_target_arch()

    Set the IDF_TARGET_ARCH value based on the sdkconfig. This means it must be
    initialized after the sdkconfig is generated and its CMake version is
    included.
#]]
function(__init_idf_target_arch)
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        idf_build_set_property(IDF_TARGET_ARCH "xtensa")
    elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
        idf_build_set_property(IDF_TARGET_ARCH "riscv")
    else()
        # Currently, no architecture is specified for Linux host builds.
        idf_build_set_property(IDF_TARGET_ARCH "")
    endif()
endfunction()

#[[
    The idf_build_properties interface target is exclusively used to store
    information about global build properties and is not linked or used in any
    other way. This is created very early so that all the initialization
    functions can use it.

    List of build properties

    IDF_PATH
        Path to esp-idf directory.

    PREFIX
        Prefix used for component target names.

    COMPONENTS_DISCOVERED
        List of component names identified by the build system. These
        components are initialized and can have properties attached to them.
        However, they are not necessarily included in the build through
        add_subdirectory.

    COMPONENT_INTERFACES
        This is a list of component interface targets for the components in
        ``COMPONENTS_DISCOVERED``. It is used when searching for a component,
        such as by its name, to set or retrieve the component's properties.

    COMPONENTS_INCLUDED
        This is a list of component names that were included in the build,
        meaning their CMakeLists.txt files were processed with an
        add_subdirectory call. Each component is evaluated exactly once, and
        this list serves as a record of which components have already been
        evaluated.  Although each component can only be evaluated once, it can
        be used in multiple idf_component_include calls. If a component is
        requested to be included a second time, this list is checked. If the
        component is already included, the idf_component_include function
        simply returns, as there is nothing further to do except add a new
        alias target if requested.
#]]
add_library(idf_build_properties INTERFACE)

# The __idf_component_interface_cache target is used to maintain internal
# mappings between component identifiers, such as component name or alias, and
# the component interface target, which is the primary target for the
# component.
add_library(__idf_component_interface_cache INTERFACE)

# Set build system prefix for component targets.
idf_build_set_property(PREFIX "idf")

# Set project directory property.
idf_build_set_property(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Set build directory property.
idf_build_set_property(BUILD_DIR "${CMAKE_BINARY_DIR}")

# Enable the generation of compile_commands.json.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize build system version.
__init_build_version()

# Initialize IDF_PATH and set it as a global and environmental variable, as
# well as a build property.
__init_idf_path()

# Determine git executable and set GIT build property.
__init_git()

# Initialize git submodules.
__init_submodules()

# Initialize the IDF_VER build property.
__init_idf_version()

# Determine the Python interpreter and check package dependencies if necessary.
__init_python()

# Initialize Kconfig system infrastructure.
__init_kconfig()

# Set IDF_TARGET.
__init_idf_target()

# Set IDF_TOOLCHAIN, IDF_TOOLCHAIN_FILE and CMAKE_TOOLCHAIN_FILE.
__init_toolchain()

# Enable ccache if requested.
__init_ccache()

#[[

At this point, the build system infrastructure is ready.
Project-specific operations (component discovery, Kconfig generation,
component manager, etc.) are handled in idf_project_init() after the
project() call.

#]]
