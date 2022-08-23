idf_component_register(SRC_DIRS .
                       PRIV_REQUIRES cmock test_utils spi_flash esp_psram
                    )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
