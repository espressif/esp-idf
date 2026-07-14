# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(isp_multi_pipelines)

# TODO IDF-15784: remove
# esp_cam_sensor upstream sets locals without reading them; silence -Wunused-but-set-variable
idf_component_get_property(esp_cam_sensor_lib espressif__esp_cam_sensor COMPONENT_LIB)
target_compile_options(${esp_cam_sensor_lib} PRIVATE -Wno-unused-but-set-variable)
