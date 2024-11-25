cmake_minimum_required(VERSION 3.16)

# In this example, sdkconfig file is placed into the build directory.
# This allows building development and production configs side by side,
# without having them influence each other.
set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)

project(multi_config)
