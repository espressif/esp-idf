idf_component_register(SRCS "gattc_gatts_coex.c"
                    INCLUDE_DIRS  ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
