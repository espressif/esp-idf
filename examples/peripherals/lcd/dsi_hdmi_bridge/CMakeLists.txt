cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)
project(dsi_hdmi_bridge)

# TODO IDF-15784: remove
# lvgl upstream sets locals without reading them; silence -Wunused-but-set-variable
idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
target_compile_options(${lvgl_lib} PRIVATE -Wno-unused-but-set-variable)
