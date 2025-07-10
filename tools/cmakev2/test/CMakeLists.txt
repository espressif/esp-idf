# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Simple project for basic cmakev2 testing
# Run: cmake -S . -B build
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(cmakev2 C)

# Dump build and component properties
function(test_dump_properties)
    __dump_build_properties()
    idf_build_get_property(component_names COMPONENTS_DISCOVERED)
    __dump_component_properties("${component_names}")
endfunction()

# Test component priority
function(test_component_priority)
    # Set the idf component to be replaced with a testing component of higher
    # priority.
    set(component_name "esp_system")

    # Check that idf component is between discovered components.
    __get_component_interface(COMPONENT "${component_name}"
                              OUTPUT component_interface)
    if("${component_interface}" STREQUAL "NOTFOUND")
        idf_die("Component '${component_name}' not found")
    endif()

    # Check that idf component has "idf_components" as source.
    idf_component_get_property(component_source
                               ${component_interface}
                               COMPONENT_SOURCE)
    if(NOT "${component_source}" STREQUAL "idf_components")
        idf_die("Unexpected idf component '${component_name}' source '${component_source}'")
    endif()

    # Create fake component with same name as idf component.
    set(component_dir "${CMAKE_CURRENT_BINARY_DIR}/${component_name}")
    file(MAKE_DIRECTORY "${component_dir}")
    file(TOUCH "${component_dir}/CMakeLists.txt")

    # Initialize fake component with higher "project_components" priority.
    idf_build_get_property(component_prefix PREFIX)
    __init_component(DIRECTORY "${component_dir}"
                     PREFIX "${component_prefix}"
                     SOURCE "project_components")

    # Check that the idf component was replaced with fake component.
    idf_component_get_property(component_source
                               ${component_interface}
                               COMPONENT_SOURCE)
    if(NOT "${component_source}" STREQUAL "project_components")
        idf_die("Unexpected fake component '${component_name}' source '${component_source}'")
    endif()

    idf_component_get_property(component_dir
                               ${component_interface}
                               COMPONENT_DIR)
    if(NOT "${component_dir}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}/${component_name}")
        idf_die("Unexpected fake component '${component_name}' directory '${component_dir}'")
    endif()
    __dump_component_properties("${component_name}")
endfunction()

# Test that IDF_VERSION is set
function(test_idf_version)
    if(NOT DEFINED IDF_VERSION_MAJOR OR
       NOT DEFINED IDF_VERSION_MINOR OR
       NOT DEFINED IDF_VERSION_PATCH OR
       NOT DEFINED ENV{IDF_VERSION})
        idf_die("IDF_VERSION not set")
   endif()
endfunction()

# Run tests
test_dump_properties()
test_component_priority()
test_idf_version()

message("ALL TESTS PASSED")
