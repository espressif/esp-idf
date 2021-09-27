cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(lcd_lvgl)

# As the upstream LVGL library has build warnings in esp-idf build system, this is only for temporarily workaround
# Will remove this workaround when upstream LVGL fixes the warnings in the next release
idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
target_compile_options(${lvgl_lib} PRIVATE "-Wno-empty-body" "-Wno-strict-prototypes")
