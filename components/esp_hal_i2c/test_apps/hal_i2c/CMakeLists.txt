# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# Check G1 component dependencies using tools/cmake template
idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

project(hal_i2c)

set(comp_deps_dot "${CMAKE_BINARY_DIR}/component_deps.dot")
idf_build_get_property(target IDF_TARGET)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Checking esp_hal_i2c dependency violations"
    COMMAND python "${IDF_PATH}/tools/test_apps/system/g1_components/check_dependencies.py"
            --component_deps_file ${comp_deps_dot}
            --target ${target}
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(WARNING "Found esp_hal_i2c dependency violations. Please check G1 component dependencies.")
endif()
