idf_component_register(SRC_DIRS . param_test touch_sensor_test dac_dma_test
                       PRIV_INCLUDE_DIRS include param_test/include touch_sensor_test/include
                       PRIV_REQUIRES cmock test_utils driver nvs_flash
                                     esp_timer esp_adc esp_event esp_wifi spi_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

# A local copy of idf-extra-components esp_serial_slave_link, for stabilities of the SDIO test
add_subdirectory(esp_serial_slave_link)
