# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main partition_table custom)

project(custom_partition_subtypes)

idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(python PYTHON)
set(blank_file ${build_dir}/blank_file.bin)
idf_component_get_property(partition_table_dir partition_table COMPONENT_DIR)

partition_table_get_partition_info(partition "--partition-type app --partition-subtype my_app1" "name")
partition_table_get_partition_info(partition_size "--partition-type app --partition-subtype my_app1" "size")

add_custom_command(OUTPUT ${blank_file}
    COMMAND ${python} ${partition_table_dir}/gen_empty_partition.py
    ${partition_size} ${blank_file})

add_custom_target(blank_bin ALL DEPENDS ${blank_file})

idf_build_get_property(target IDF_TARGET)

if(NOT ${target} STREQUAL "linux")
    add_dependencies(flash blank_bin)
    esptool_py_flash_to_partition(flash "${partition}" "${blank_file}")
endif()
