idf_component_register(SRCS "cmd_nvs.c"
                    INCLUDE_DIRS .
                    REQUIRES console nvs_flash)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
