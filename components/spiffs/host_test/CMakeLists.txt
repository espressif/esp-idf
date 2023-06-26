cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
# Freertos is included via common components, however, currently only the mock component is compatible with linux
# target.
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")

project(host_test_spiffs)

# Custom procedure to build/clean image.bin
add_custom_target(image.bin)

# Expand image.bin to the same size as "spiffs" partition in partition_table.csv - 2*1024*1024 = 2097152 = 2M
add_custom_command(
    TARGET image.bin
    POST_BUILD
    COMMAND python ../../spiffsgen.py 2097152 ../../spiffs ${build_dir}/image.bin
)

set_property(
    DIRECTORY
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${build_dir}/image.bin")


add_dependencies(host_test_spiffs.elf image.bin)
