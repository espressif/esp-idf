idf_component_register(SRCS "jpeg_decode_example_main.c"
                       PRIV_REQUIRES esp_driver_jpeg mbedtls
                       INCLUDE_DIRS ".")

target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_LIST_DIR}/assets/image.jpg" BINARY RENAME_TO "example_jpeg")
