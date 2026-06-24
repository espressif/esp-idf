# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(partitions_ota)

# Copy storage.bin from test folder to build directory
if(CONFIG_SECURE_SIGNED_DATA_PARTITION)
    set(storage_file signed_storage.bin)
else()
    set(storage_file storage.bin)
endif()
add_custom_target(copy_storage_bin ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/test/${storage_file}
        ${CMAKE_BINARY_DIR}/${storage_file}
    COMMENT "Copying test/${storage_file} to build directory"
    DEPENDS ${CMAKE_SOURCE_DIR}/test/${storage_file}
)
