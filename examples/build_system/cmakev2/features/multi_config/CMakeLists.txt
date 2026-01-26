cmake_minimum_required(VERSION 3.22)

# In this example, sdkconfig file is placed into the build directory.
# This allows building development and production configs side by side,
# without having them influence each other.
set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
project(multi_config C CXX ASM)
idf_project_default()
