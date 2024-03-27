# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/unit-test-app/components"
)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if($ENV{CI_PIPELINE_ID})
    idf_build_set_property(COMPILE_DEFINITIONS TEST_SUFFIX_STR="_$ENV{CI_PIPELINE_ID}" APPEND)
endif()

project(test_temperature_sensor)
