# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/test_apps/components")
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# Test-only: enable DHCP options parser test hook in lwip (set before project() so
# all components pick up the compile definition; not in Kconfig)
idf_build_set_property(COMPILE_DEFINITIONS "LWIP_DHCPS_TEST_PARSE_OPTIONS=1" APPEND)
project(lwip_test)
