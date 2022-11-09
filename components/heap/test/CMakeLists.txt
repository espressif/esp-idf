idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES cmock test_utils heap spi_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

if(CONFIG_COMPILER_DUMP_RTL_FILES)
    idf_build_get_property(elf_file_name EXECUTABLE GENERATOR_EXPRESSION)
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dir ${CMAKE_BINARY_DIR}/esp-idf/heap/
                      --elf-file ${CMAKE_BINARY_DIR}/${elf_file_name}
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --ignore-symbols=__func__/__assert_func,__func__/heap_caps_alloc_failed
                      --exit-code
                      DEPENDS ${elf_file_name}
                      )
endif()
