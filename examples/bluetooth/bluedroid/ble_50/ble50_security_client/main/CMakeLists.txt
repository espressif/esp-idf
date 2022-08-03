idf_component_register(SRCS "ble50_sec_gattc_demo.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
