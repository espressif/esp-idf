# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mipi_dsi_lcd_panel_test)

target_add_binary_data(mipi_dsi_lcd_panel_test.elf "resources/pictures/hello.yuv" BINARY)
target_add_binary_data(mipi_dsi_lcd_panel_test.elf "resources/pictures/world.yuv" BINARY)

if(CONFIG_COMPILER_DUMP_RTL_FILES)
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${CMAKE_BINARY_DIR}/esp-idf/esp_lcd/,${CMAKE_BINARY_DIR}/esp-idf/hal/
                      --elf-file ${CMAKE_BINARY_DIR}/mipi_dsi_lcd_panel_test.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()

message(STATUS "Checking mipi_dsi registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "*dsi*" "hp_sys_clkrst"
                            HAL_MODULES "*dsi*")
