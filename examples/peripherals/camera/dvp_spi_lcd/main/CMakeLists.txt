idf_component_register(SRCS "dvp_spi_lcd_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_cam esp_driver_i2c esp_lcd sensor_init
                       )
