# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gpio_extension_test)

idf_build_get_property(elf EXECUTABLE)
if(CONFIG_COMPILER_DUMP_RTL_FILES)
    set(GPIO_EXTENSIONS_RTL_DIRS
        ${CMAKE_BINARY_DIR}/esp-idf/esp_driver_gpio
        ${CMAKE_BINARY_DIR}/esp-idf/esp_hal_gpio
        ${CMAKE_BINARY_DIR}/esp-idf/hal
    )
    string(JOIN "," RTL_DIRS_STRING ${GPIO_EXTENSIONS_RTL_DIRS})
    add_custom_target(
        check_test_app_sections ALL
        COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
        --rtl-dirs ${RTL_DIRS_STRING}
        --elf-file  ${CMAKE_BINARY_DIR}/gpio_extension_test.elf
        find-refs
        --from-sections=.iram0.text
        --to-sections=.flash.text,.flash.rodata
        --exit-code
        DEPENDS ${elf}
    )
endif()
