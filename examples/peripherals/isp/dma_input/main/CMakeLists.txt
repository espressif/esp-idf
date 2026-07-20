idf_component_register(SRCS "isp_dma_example_main.c"
                       PRIV_REQUIRES esp_driver_isp esp_mm esp_psram mbedtls
                       INCLUDE_DIRS ".")

target_add_binary_data(${COMPONENT_LIB}
                       "${CMAKE_CURRENT_LIST_DIR}/assets/sensor_240x280_bggr.raw"
                       BINARY RENAME_TO "sensor_raw" ALIGN 16)
