#This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

list(APPEND sdkconfig_defaults
    ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_preset_phy_tsens.conf
)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

project(phy_tsens)
