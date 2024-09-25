# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(partitions_ota)

# Copy storage.bin from test folder to build directory
add_custom_target(copy_storage_bin ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/test/storage.bin
        ${CMAKE_BINARY_DIR}/storage.bin
    COMMENT "Copying test/storage.bin to build directory"
    DEPENDS ${CMAKE_SOURCE_DIR}/test/storage.bin
)
