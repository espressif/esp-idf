idf_component_register(SRC_DIRS . param_test dac_dma_test
                       PRIV_INCLUDE_DIRS include param_test/include
                       PRIV_REQUIRES cmock test_utils driver nvs_flash esp_serial_slave_link
                                     esp_timer esp_adc esp_event esp_wifi)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
