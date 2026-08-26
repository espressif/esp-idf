idf_component_register(SRCS "ppa_transform_example_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_psram esp_driver_ppa mbedtls)

target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_LIST_DIR}/assets/image.rgb" BINARY RENAME_TO "image_rgb")
