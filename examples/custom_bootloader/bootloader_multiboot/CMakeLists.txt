# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(main)

# For this example, we need two binaries to embed in the flash, for the `ota` partitions defined
# in the CSV partition table.
# It is possible to provide binaries files directly to `esptool_py_flash_target_image`, but let's
# be clean and build two subproject that will generate the binaries to embed in the two new
# `ota` sections.
idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(idf_target IDF_TARGET)

# Use two examples that are available in the `examples` directory.
file(REMOVE $ENV{IDF_PATH}/examples/get-started/hello_world/sdkconfig)
set(hello_world_build_dir "${CMAKE_CURRENT_BINARY_DIR}/build_hello_world")
externalproject_add(hello_world
    SOURCE_DIR $ENV{IDF_PATH}/examples/get-started/hello_world
    CMAKE_ARGS -DIDF_PATH=${idf_path} -DIDF_TARGET=${idf_target} -DSDKCONFIG=${hello_world_build_dir}/sdkconfig
    BINARY_DIR "${hello_world_build_dir}"
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${hello_world_build_dir}/hello_world.bin"
)
# Because the CI wants all the binaries to be in the `build/` directory, we need to create a
# custom command to move the generated file
add_custom_target(move_hello_world ALL
    COMMAND ${CMAKE_COMMAND} -E copy
            "${hello_world_build_dir}/hello_world.bin"
            "${CMAKE_CURRENT_BINARY_DIR}/hello_world.bin"
    DEPENDS hello_world
)


# Do the same thing for the console example
file(REMOVE $ENV{IDF_PATH}/examples/system/console/basic/sdkconfig)
set(console_build_dir "${CMAKE_CURRENT_BINARY_DIR}/build_console")
externalproject_add(console
    SOURCE_DIR $ENV{IDF_PATH}/examples/system/console/basic
    CMAKE_ARGS -DIDF_PATH=${idf_path} -DIDF_TARGET=${idf_target} -DSDKCONFIG=${console_build_dir}/sdkconfig
    BINARY_DIR "${console_build_dir}"
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${console_build_dir}/console.bin"
)
add_custom_target(move_console ALL
    COMMAND ${CMAKE_COMMAND} -E copy
            "${console_build_dir}/console.bin"
            "${CMAKE_CURRENT_BINARY_DIR}/console.bin"
    DEPENDS console
)

# Tell esptool about the generated binaries, it will flash them when using `idf.py flash`
partition_table_get_partition_info(offset "--partition-name hello_world" "offset")
esptool_py_flash_target_image(flash "hello_world" "${offset}" "${CMAKE_CURRENT_BINARY_DIR}/hello_world.bin")

partition_table_get_partition_info(offset "--partition-name console" "offset")
esptool_py_flash_target_image(flash "console" "${offset}" "${CMAKE_CURRENT_BINARY_DIR}/console.bin")
