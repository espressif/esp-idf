set(srcs "isp_dsi_main.c"
        "example_buffer.c"
        "example_af.c"
        "example_awb.c"
        "example_pipelines.c")

if(CONFIG_EXAMPLE_ISP_CROP_ENABLE)
    list(APPEND srcs "example_crop.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_isp esp_driver_cam esp_driver_i2c esp_lcd dsi_init sensor_init
                       )
