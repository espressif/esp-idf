
set(srcs "isp_dsi_main.c"
        "example_dpc.c")


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_isp esp_driver_cam esp_driver_i2c esp_lcd dsi_init sensor_init
                       )
