idf_component_register(SRCS "esp_eddystone_api.c"
                            "esp_eddystone_demo.c"
                        INCLUDE_DIRS "")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
