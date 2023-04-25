idf_component_register(SRCS "hello_world_main.c"
                    INCLUDE_DIRS ""
                    PRIV_REQUIRES esp_timer spi_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
