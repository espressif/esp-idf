cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
# Freertos is included via common components, however, currently only the mock component is compatible with linux
# target.
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")

project(partition_api_test)

#extra step to build 8M partition table on top of (default) 4M partition table built by partition-table dependency
set(flashsize_opt --flash-size 8MB)
set(partition_csv "partition_table_8M.csv")
set(partition_bin "partition-table_8M.bin")

idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(python PYTHON)

set(gen_partition_table "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/../../../partition_table/gen_esp32part.py" "-q"
                        "${flashsize_opt}" "--")

set(partition_table_display
    COMMAND ${CMAKE_COMMAND} -E echo "Partition table binary generated. Contents:"
    COMMAND ${CMAKE_COMMAND} -E echo "*******************************************************************************"
    COMMAND ${gen_partition_table} "${build_dir}/partition_table/${partition_bin}"
    COMMAND ${CMAKE_COMMAND} -E echo "*******************************************************************************"
)

add_custom_command(OUTPUT "${build_dir}/partition_table/${partition_bin}"
    COMMAND ${gen_partition_table}
        "${CMAKE_CURRENT_SOURCE_DIR}/${partition_csv}"
        "${build_dir}/partition_table/${partition_bin}"
        ${partition_table_display}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${partition_csv}
    VERBATIM)

add_custom_target(partition_table_bin_8M DEPENDS "${build_dir}/partition_table/${partition_bin}"
                                              )
add_custom_target(partition-table-8M
                    DEPENDS partition_table_bin_8M
                    ${partition_table_display}
                    VERBATIM)

add_dependencies(partition_api_test.elf partition-table partition-table-8M)
