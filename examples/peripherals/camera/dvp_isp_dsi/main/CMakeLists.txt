idf_component_register(SRCS "dvp_isp_dsi_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_isp esp_driver_cam esp_driver_i2c dsi_init sensor_init
                       )
