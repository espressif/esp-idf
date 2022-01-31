idf_component_register(SRCS "ble_compatibility_test.c"
                    INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
