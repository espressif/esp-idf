# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(dac_test)

idf_build_get_property(elf EXECUTABLE)
if(CONFIG_COMPILER_DUMP_RTL_FILES)
    # Collect RTL directories in a variable for readability. Join them
    # with commas so they are passed as a single --rtl-dirs argument to the script.
    set(DAC_RTL_DIRS
        ${CMAKE_BINARY_DIR}/esp-idf/esp_driver_dac
        ${CMAKE_BINARY_DIR}/esp-idf/hal
        ${CMAKE_BINARY_DIR}/esp-idf/esp_hal_ana_conv
    )
    string(JOIN "," DAC_RTL_DIRS_JOINED ${DAC_RTL_DIRS})
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${DAC_RTL_DIRS_JOINED}
                      --elf-file ${CMAKE_BINARY_DIR}/dac_test.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()

message(STATUS "Checking dac registers are not read-write by half-word")
include($ENV{IDF_PATH}/tools/ci/check_register_rw_half_word.cmake)
check_register_rw_half_word(SOC_MODULES "rtc_io" "sens" "apb_saradc"
                            HAL_MODULES "dac")
