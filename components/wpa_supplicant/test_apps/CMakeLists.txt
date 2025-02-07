cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
list(PREPEND SDKCONFIG_DEFAULTS
    "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers"
    "sdkconfig.defaults")

if($ENV{CI_PIPELINE_ID})
    idf_build_set_property(COMPILE_DEFINITIONS TEST_SUFFIX_STR="_$ENV{CI_PIPELINE_ID}" APPEND)
endif()

project(wpa_supplicant_test)
