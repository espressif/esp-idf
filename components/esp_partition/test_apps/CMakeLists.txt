# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp_partition_test)

if(CONFIG_PARTITION_TABLE_MD5_TOLERATE_MISSING)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(python PYTHON)
    add_custom_command(TARGET partition_table_bin POST_BUILD
        COMMAND "${python}" "${CMAKE_CURRENT_SOURCE_DIR}/strip_partition_table_md5.py"
                "${build_dir}/partition_table/partition-table.bin"
        VERBATIM)
endif()
