# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers" "sdkconfig.defaults")

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(usb_cdc_vfs_test)

idf_build_get_property(elf EXECUTABLE)
if(CONFIG_COMPILER_DUMP_RTL_FILES)
    add_custom_target(check_test_app_sections ALL
                        COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                        --rtl-dirs ${CMAKE_BINARY_DIR}/esp-idf/driver/,${CMAKE_BINARY_DIR}/esp-idf/hal/
                        --elf-file ${CMAKE_BINARY_DIR}/*.elf
                        find-refs
                        --from-sections=.iram0.text
                        --to-sections=.flash.text,.flash.rodata
                        --exit-code
                        DEPENDS ${elf}
                        )
endif()
