cmake_minimum_required(VERSION 3.22)

list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/test_apps/components")

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_csi)

idf_build_get_property(elf EXECUTABLE)
if(CONFIG_COMPILER_DUMP_RTL_FILES)
    # Check that the cache-safe ISR code paths don't call into flash
    set(CAM_RTL_DIRS
        ${CMAKE_BINARY_DIR}/esp-idf/esp_driver_cam
        ${CMAKE_BINARY_DIR}/esp-idf/esp_driver_dma
    )
    string(JOIN "," CAM_RTL_DIRS_JOINED ${CAM_RTL_DIRS})

    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${CAM_RTL_DIRS_JOINED}
                      --elf-file ${CMAKE_BINARY_DIR}/test_csi.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()

# TODO IDF-15784: remove
# esp_cam_sensor upstream sets locals without reading them; silence -Wunused-but-set-variable
idf_component_get_property(esp_cam_sensor_lib espressif__esp_cam_sensor COMPONENT_LIB)
target_compile_options(${esp_cam_sensor_lib} PRIVATE -Wno-unused-but-set-variable)

message(STATUS "Checking camera registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "lcd_cam" "hp_sys_clkrst" "*csi*"
                            HAL_MODULES "cam" "*csi*")
