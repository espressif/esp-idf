# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

# Include the Bluetooth-optimized mbedTLS preset configuration
# This provides optimized settings for Bluetooth applications
# You can customize these settings using 'idf.py menuconfig' or additional config files
cmake_path(CONVERT "$ENV{IDF_PATH}" TO_CMAKE_PATH_LIST ESP_IDF_PATH)
list(APPEND sdkconfig_defaults "${ESP_IDF_PATH}/components/mbedtls/config/mbedtls_preset_bt.conf")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(bleprph)
