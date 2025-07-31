# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Simple project for basic cmakev2 testing
# Run: cmake -S . -B build
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(cmakev2 C)

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

# Test that IDF_VERSION and IDF_VER build property is set
function(test_idf_version)
    if(NOT DEFINED IDF_VERSION_MAJOR OR
            NOT DEFINED IDF_VERSION_MINOR OR
            NOT DEFINED IDF_VERSION_PATCH OR
            NOT DEFINED ENV{IDF_VERSION})
        idf_die("IDF_VERSION not set")
    endif()
    idf_build_get_property(idf_ver IDF_VER)
    if(NOT idf_ver)
        idf_die("IDF_VER build property not set")
    endif()
endfunction()

# Test that Python interpreter is set
function(test_python)
    if(NOT DEFINED PYTHON OR "${PYTHON}" STREQUAL "")
        idf_die("PYTHON variable not defined or empty")
    endif()
endfunction()

# Test that toolchain is properly set
function(test_toolchain)
    if(NOT IDF_TOOLCHAIN)
        idf_die("IDF_TOOLCHAIN variable not defined or empty")
    endif()
    if(NOT CMAKE_TOOLCHAIN_FILE)
        idf_die("CMAKE_TOOLCHAIN_FILE variable not defined or empty")
    endif()
    idf_build_get_property(toolchain IDF_TOOLCHAIN)
    if(NOT toolchain)
        idf_die("IDF_TOOLCHAIN build property not set")
    endif()
    idf_build_get_property(toolchain_file IDF_TOOLCHAIN_FILE)
    if(NOT toolchain_file)
        idf_die("IDF_TOOLCHAIN_FILE build property not set")
    endif()
endfunction()

# Test idf_build_library
function(test_idf_build_library)
    # Create idflibtest with specific set of components and test that it
    # contains them.
    set(components app_trace app_update bootloader bootloader_support bt cmock)

    set(idflib idflibtest)
    idf_build_library(INTERFACE "${idflib}"
                      COMPONENTS "${components}")

    if(NOT TARGET "${idflib}")
        idf_die("'${idflib}' not created")
    endif()

    idf_library_get_property(lib_components "${idflib}" LIBRARY_COMPONENTS)
    if(NOT "${lib_components}" STREQUAL "${components}")
        idf_die("Library '${idflib}' components '${lib_components}' do not match "
                "expected components '${components}'")
    endif()

    # Create idflibtest2 without specifying COMPONENTS and test that it
    # contains all discovered components.
    set(idflib idflibtest2)
    idf_build_library(INTERFACE "${idflib}")

    if(NOT TARGET "${idflib}")
        idf_die("'${idflib}' not created")
    endif()

    idf_library_get_property(lib_components "${idflib}" LIBRARY_COMPONENTS)
    idf_build_get_property(component_names COMPONENTS_DISCOVERED)
    if(NOT "${lib_components}" STREQUAL "${component_names}")
        idf_die("Library '${idflib}' components '${lib_components}' do not match "
                "COMPONENTS_DISCOVERED")
    endif()
endfunction()

function(test_kconfig)
    # Check that kconfig output files were generated
    idf_build_get_property(config_dir CONFIG_DIR)
    set(output_files sdkconfig.h sdkconfig.cmake sdkconfig.json kconfig_menus.json)

    foreach(file ${output_files})
        set(file_path "${config_dir}/${file}")
        if(NOT EXISTS "${file_path}")
            idf_die("Missing kconfig output: ${file_path}")
        endif()
        file(SIZE "${file_path}" file_size)
        if(file_size EQUAL 0)
            idf_die("Empty kconfig output: ${file_path}")
        endif()
    endforeach()

    # Check that kconfig targets were created
    set(targets menuconfig confserver save-defconfig)
    foreach(target ${targets})
        if(NOT TARGET ${target})
            idf_die("Missing kconfig target: ${target}")
        endif()
    endforeach()
endfunction()

# Run tests
test_component_priority()
test_idf_version()
test_python()
test_toolchain()
test_idf_build_library()
test_kconfig()
__dump_all_properties()

message("ALL TESTS PASSED")
