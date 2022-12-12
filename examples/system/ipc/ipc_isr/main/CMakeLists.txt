idf_component_register(SRCS "main.c"
                            "asm_funcs.S"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
