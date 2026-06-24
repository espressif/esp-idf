idf_component_register(SRCS "jpeg_encode_example_main.c"
                       PRIV_REQUIRES esp_driver_jpeg mbedtls
                       INCLUDE_DIRS ".")

target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_LIST_DIR}/assets/esp720p.rgb" BINARY RENAME_TO "esp720p_rgb")
