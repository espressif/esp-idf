# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#"Trim" the build. Include the minimal set of components, main, and anything it depends on.
set(COMPONENTS main)

project(test_heap)

string(JOIN "," ignore_refs
    heap_caps_*/__func__*
    tlsf_*/__func__*
    multi_heap_*/__func__*
    dram_alloc_to_iram_addr/__func__*
    list_*/__func__*
)

if(CONFIG_COMPILER_DUMP_RTL_FILES)
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${CMAKE_BINARY_DIR}/esp-idf/heap/
                      --elf-file ${CMAKE_BINARY_DIR}/test_heap.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --ignore-refs=${ignore_refs}
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()
