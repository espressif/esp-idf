# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
list(PREPEND SDKCONFIG_DEFAULTS
             "${IDF_PATH}/examples/zigbee/zb_common_components/example_common/sdkconfig.mbedtls_minimal"
             "sdkconfig.defaults")
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(light_switch)
