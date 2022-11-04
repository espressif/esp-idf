idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES cmock test_utils esp_ringbuf driver spi_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
