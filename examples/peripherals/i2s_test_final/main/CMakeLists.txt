idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES spi_flash esp_driver_gpio esp_driver_i2s driver
                       INCLUDE_DIRS ".")

# # 数学関数（sin関数など）を使うためにリンク
# target_link_libraries(${COMPONENT_LIB} INTERFACE m)
