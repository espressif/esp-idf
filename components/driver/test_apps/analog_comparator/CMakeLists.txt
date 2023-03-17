# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_ana_cmpr)

if(CONFIG_COMPILER_DUMP_RTL_FILES)
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dir ${CMAKE_BINARY_DIR}/esp-idf/driver/
                      --elf-file ${CMAKE_BINARY_DIR}/test_ana_cmpr.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()
