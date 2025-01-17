# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gpio_test)

if(CONFIG_COMPILER_DUMP_RTL_FILES)
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${CMAKE_BINARY_DIR}/esp-idf/esp_driver_gpio/,${CMAKE_BINARY_DIR}/esp-idf/hal/
                      --elf-file ${CMAKE_BINARY_DIR}/gpio_test.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()

include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
message(STATUS "Checking gpio registers are not read-write by half-word")
check_register_rw_half_word(SOC_MODULES "gpio" "io_mux" "rtc_cntl" "rtc_io" "pcr" "hp_sys_clkrst" "hp_system"
                                        "lp_aon" "lp_iomux" "pmu"
                            HAL_MODULES "gpio")

message(STATUS "Checking rtcio registers are not read-write by half-word")
check_register_rw_half_word(SOC_MODULES "rtc_io" "sens" "pcr" "lp_aon" "lp_io" "lp_gpio" "lp_iomux" "lpperi" "pmu"
                            HAL_MODULES "rtc_io")
