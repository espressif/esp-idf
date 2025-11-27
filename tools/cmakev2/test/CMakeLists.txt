# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Simple project for basic cmakev2 testing
# Run: cmake -S . -B build
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(cmakev2
    VERSION 1.2.3
    LANGUAGES C CXX ASM)

add_custom_target(flash)

idf_project_init()

idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

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
    idf_build_library("${idflib}" COMPONENTS "${components}")

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
    idf_build_library("${idflib}")

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
    set(output_files sdkconfig.h sdkconfig.cmake sdkconfig.json)

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

# Test that PROJECT_NAME and PROJECT_VER build property is set and contain
# values provided in project() call.
function(test_project_properties)
    idf_build_get_property(project_name PROJECT_NAME)
    if(NOT project_name)
        idf_die("PROJECT_NAME build property not set")
    endif()
    if(NOT "${project_name}" STREQUAL "${PROJECT_NAME}")
        idf_die("PROJECT_NAME build property '${project_name}' != '${PROJECT_NAME}'")
    endif()
    idf_build_get_property(project_ver PROJECT_VER)
    if(NOT project_ver)
        idf_die("PROJECT_VER build property not set")
    endif()
    if(NOT "${project_ver}" STREQUAL "${PROJECT_VERSION}")
        idf_die("PROJECT_VER build property '${project_ver}' != '${PROJECT_VERSION}'")
    endif()
endfunction()

# Test that the targets for component1 and component2 are created when
# component2 is included.
function(test_include_component)
    idf_build_library(idflibtest3 COMPONENTS component2)
    idf_component_get_property(component_real_target
                               component2
                               COMPONENT_REAL_TARGET)
    if(NOT TARGET ${component_real_target})
        idf_die("Missing component2 target")
    endif()

    # Test that component1 is included as a dependency of component2.
    idf_component_get_property(component_real_target
                               component1
                               COMPONENT_REAL_TARGET)
    if(NOT TARGET ${component_real_target})
        idf_die("Missing component1 target")
    endif()
endfunction()

# Add two executables fatfs_example and hello_world_example, generate
# binary images for them and add flash and menuconfig targets.
# After configuration these can be build with
#    idf.py hello_world_example_bin
#    idf.py fatfs_example_bin
# or simply
#   idf.py hello_world_example-flash monitor
#   idf.py fatfs-flash monitor
# The menuconfig can be invoked with
#   idf.py --no-hints menuconfig-hello_world
#   idf.py --no-hints menuconfig-fatfs
# The confserver can be invoked with
#   idf.py confserver-hello_world
#   idf.py confserver-fatfs
function(test_executable)
    idf_build_executable(fatfs_example
                         COMPONENTS fatfs_example
                         MAPFILE_TARGET fatfs_example_mapfile)
    idf_build_binary(fatfs_example
                     TARGET fatfs_example_bin
                     OUTPUT_FILE fatfs_example.bin)
    idf_check_binary_size(fatfs_example_bin)
    idf_flash_binary(fatfs_example_bin
                     TARGET fatfs_example-flash
                     NAME fatfs_example)
    idf_create_menuconfig(fatfs_example
                          TARGET menuconfig-fatfs)
    idf_build_generate_metadata(fatfs_example_bin
                                OUTPUT_FILE project_description_fatfs.json)
    idf_build_generate_depgraph(fatfs_example
                                OUTPUT_FILE component_deps_fatfs.dot)
    idf_create_confserver(fatfs_example
                          TARGET confserver-fatfs)

    if(TARGET fatfs_example_mapfile)
        idf_create_size_report(fatfs_example_mapfile
                               TARGET fatfs-size)
    endif()


    idf_build_executable(hello_world_example
                         COMPONENTS hello_world_example
                         MAPFILE_TARGET hello_world_example_mapfile)
    idf_build_binary(hello_world_example
                     TARGET hello_world_example_bin
                     OUTPUT_FILE hello_world_example.bin)
    idf_check_binary_size(hello_world_example_bin)
    idf_flash_binary(hello_world_example_bin
                     TARGET hello_world_example-flash
                     NAME hello_world_example)
    idf_create_menuconfig(hello_world_example
                          TARGET menuconfig-hello_world)
    idf_build_generate_metadata(hello_world_example_bin
                                OUTPUT_FILE project_description_hello_world.json)
    idf_build_generate_depgraph(hello_world_example
                                OUTPUT_FILE component_deps_hello_world.dot)
    idf_create_confserver(hello_world_example
                          TARGET confserver-hello_world)

    if(TARGET hello_world_example_mapfile)
        idf_create_size_report(hello_world_example_mapfile
                               TARGET hello-size)
    endif()
endfunction()

# Run tests
test_idf_version()
test_python()
test_toolchain()
test_idf_build_library()
test_project_properties()
test_include_component()
test_executable()

# Create default project
idf_project_default()

# The kconfig test also verifies whether kconfig-related targets, such as
# menuconfig, are created. These targets are now generated within
# idf_project_default rather than globally, so this test should be run only
# after the default project is created.
test_kconfig()

# Call this test last because it replaces the ESP system component.
test_component_priority()

__dump_all_properties()

message("ALL TESTS PASSED")
