# This is the project CMakeLists.txt file for the wifi_ota size metrics test app
cmake_minimum_required(VERSION 3.22)

list(PREPEND SDKCONFIG_DEFAULTS
     "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.size_metrics"
     "sdkconfig.defaults")

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wifi_ota)
