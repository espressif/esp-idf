cmake_minimum_required(VERSION 3.5)
project(esp-idf C CXX ASM)

if(NOT IDF_PATH)
    set(IDF_PATH ${CMAKE_CURRENT_LIST_DIR})
endif()

include(tools/cmake/idf_functions.cmake)

#
# Set variables that control the build configuration and the build itself
#
idf_set_variables()

kconfig_set_variables()

#
# Generate a component dependencies file, enumerating components to be included in the build
# as well as their dependencies.
#
execute_process(COMMAND "${CMAKE_COMMAND}"
    -D "COMPONENTS=${IDF_COMPONENTS}"
    -D "COMPONENT_REQUIRES_COMMON=${IDF_COMPONENT_REQUIRES_COMMON}"
    -D "EXCLUDE_COMPONENTS=${IDF_EXCLUDE_COMPONENTS}"
    -D "TEST_COMPONENTS=${IDF_TEST_COMPONENTS}"
    -D "TEST_EXCLUDE_COMPONENTS=${IDF_TEST_EXCLUDE_COMPONENTS}"
    -D "BUILD_TESTS=${IDF_BUILD_TESTS}"
    -D "DEPENDENCIES_FILE=${CMAKE_BINARY_DIR}/component_depends.cmake"
    -D "COMPONENT_DIRS=${IDF_COMPONENT_DIRS}"
    -D "BOOTLOADER_BUILD=${BOOTLOADER_BUILD}"
    -D "IDF_TARGET=${IDF_TARGET}"
    -D "IDF_PATH=${IDF_PATH}"
    -D "DEBUG=${DEBUG}"
    -P "${IDF_PATH}/tools/cmake/scripts/expand_requirements.cmake"
    WORKING_DIRECTORY "${PROJECT_PATH}"
    RESULT_VARIABLE expand_requirements_result)

if(expand_requirements_result)
    message(FATAL_ERROR "Failed to expand component requirements")
endif()

include("${CMAKE_BINARY_DIR}/component_depends.cmake")

#
# We now have the following component-related variables:
#
# IDF_COMPONENTS is the list of initial components set by the user
# (or empty to include all components in the build).
# BUILD_COMPONENTS is the list of components to include in the build.
# BUILD_COMPONENT_PATHS is the paths to all of these components, obtained from the component dependencies file.
#
# Print the list of found components and test components
#
string(REPLACE ";" " " BUILD_COMPONENTS_SPACES "${BUILD_COMPONENTS}")
message(STATUS "Component names: ${BUILD_COMPONENTS_SPACES}")
unset(BUILD_COMPONENTS_SPACES)
message(STATUS "Component paths: ${BUILD_COMPONENT_PATHS}")

# Print list of test components
if(TESTS_ALL EQUAL 1 OR TEST_COMPONENTS)
    string(REPLACE ";" " " BUILD_TEST_COMPONENTS_SPACES "${BUILD_TEST_COMPONENTS}")
    message(STATUS "Test component names: ${BUILD_TEST_COMPONENTS_SPACES}")
    unset(BUILD_TEST_COMPONENTS_SPACES)
    message(STATUS "Test component paths: ${BUILD_TEST_COMPONENT_PATHS}")
endif()

# Generate project configuration
kconfig_process_config()

# Include sdkconfig.cmake so rest of the build knows the configuration
include(${SDKCONFIG_CMAKE})

# Verify the environment is configured correctly
idf_verify_environment()

# Check git revision (may trigger reruns of cmake)
##  sets IDF_VER to IDF git revision
idf_get_git_revision()

# Check that the targets set in cache, sdkconfig, and in environment all match
idf_check_config_target()

## get PROJECT_VER
if(NOT BOOTLOADER_BUILD)
    app_get_revision("${CMAKE_SOURCE_DIR}")
endif()

# Add some idf-wide definitions
idf_set_global_compile_options()

# generate compile_commands.json (needs to come after project)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#
# Setup variables for linker script generation
#
ldgen_set_variables()

# Include any top-level project_include.cmake files from components
foreach(component ${BUILD_COMPONENT_PATHS})
    set(COMPONENT_PATH "${component}")
    include_if_exists("${component}/project_include.cmake")
    unset(COMPONENT_PATH)
endforeach()

#
# Add each component to the build as a library
#
foreach(COMPONENT_PATH ${BUILD_COMPONENT_PATHS})
    get_filename_component(COMPONENT_NAME ${COMPONENT_PATH} NAME)

    list(FIND BUILD_TEST_COMPONENT_PATHS ${COMPONENT_PATH} idx)

    if(NOT idx EQUAL -1)
        list(GET BUILD_TEST_COMPONENTS ${idx} test_component)
        set(COMPONENT_NAME ${test_component})
    endif()

    component_get_target(COMPONENT_TARGET ${COMPONENT_NAME})

    add_subdirectory(${COMPONENT_PATH} ${COMPONENT_NAME})
endforeach()
unset(COMPONENT_NAME)
unset(COMPONENT_PATH)

# each component should see the include directories of its requirements
#
# (we can't do this until all components are registered and targets exist in cmake, as we have
# a circular requirements graph...)
foreach(component ${BUILD_COMPONENTS})
    component_get_target(component_target ${component})
    if(TARGET ${component_target})
        get_component_requirements(${component} deps priv_deps)

        list(APPEND priv_deps ${IDF_COMPONENT_REQUIRES_COMMON})

        foreach(dep ${deps})
            component_get_target(dep_target ${dep})
            add_component_dependencies(${component_target} ${dep_target} PUBLIC)
        endforeach()

        foreach(dep ${priv_deps})
            component_get_target(dep_target ${dep})
            add_component_dependencies(${component_target} ${dep_target} PRIVATE)
        endforeach()
    endif()
endforeach()

if(IDF_BUILD_ARTIFACTS)
    # Write project description JSON file
    make_json_list("${BUILD_COMPONENTS}" build_components_json)
    make_json_list("${BUILD_COMPONENT_PATHS}" build_component_paths_json)
    configure_file("${IDF_PATH}/tools/cmake/project_description.json.in"
        "${IDF_BUILD_ARTIFACTS_DIR}/project_description.json")
    unset(build_components_json)
    unset(build_component_paths_json)
endif()

set(BUILD_COMPONENTS ${BUILD_COMPONENTS} PARENT_SCOPE)

ldgen_add_dependencies()
