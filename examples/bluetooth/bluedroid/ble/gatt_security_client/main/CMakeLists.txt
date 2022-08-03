idf_component_register(SRCS "example_ble_sec_gattc_demo.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
