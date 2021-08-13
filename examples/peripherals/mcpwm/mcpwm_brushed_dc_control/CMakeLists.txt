# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/examples/common_components"
                         "$ENV{IDF_PATH}/examples/peripherals/pcnt/rotary_encoder/components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mcpwm_brushed_dc_control)
