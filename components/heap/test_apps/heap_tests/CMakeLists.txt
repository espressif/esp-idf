# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#"Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

project(test_heap)

if(CONFIG_COMPILER_DUMP_RTL_FILES)
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${CMAKE_BINARY_DIR}/esp-idf/heap/
                      --elf-file ${CMAKE_BINARY_DIR}/test_heap.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --ignore-symbols=__func__/__assert_func,__func__/heap_caps_alloc_failed
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()
