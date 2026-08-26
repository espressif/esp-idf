# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/test_apps/components"
)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if($ENV{CI_PIPELINE_ID})
    idf_build_set_property(COMPILE_DEFINITIONS TEST_SUFFIX_STR="_$ENV{CI_PIPELINE_ID}" APPEND)
endif()

project(test_temperature_sensor)

idf_build_get_property(elf EXECUTABLE)
if(CONFIG_COMPILER_DUMP_RTL_FILES)
    set(TSENS_RTL_DIRS
        ${CMAKE_BINARY_DIR}/esp-idf/esp_driver_tsens
        ${CMAKE_BINARY_DIR}/esp-idf/esp_hal_ana_conv
        ${CMAKE_BINARY_DIR}/esp-idf/hal
    )
    string(JOIN "," TSENS_RTL_DIRS_JOINED ${TSENS_RTL_DIRS})
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${TSENS_RTL_DIRS_JOINED}
                      --elf-file ${CMAKE_BINARY_DIR}/test_temperature_sensor.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()
