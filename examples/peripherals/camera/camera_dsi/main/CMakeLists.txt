idf_component_register(SRCS "camera_dsi_main.c" "example_dsi_init.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_isp esp_driver_cam esp_driver_i2c esp_lcd
                       )
