<<<<<<< HEAD:examples/peripherals/isp/auto_focus/main/CMakeLists.txt
idf_component_register(SRCS "isp_af_dsi_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_isp esp_driver_cam esp_driver_i2c esp_lcd dsi_init
=======
idf_component_register(SRCS "isp_dsi_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mm esp_driver_isp esp_driver_cam esp_driver_i2c esp_lcd dsi_init sensor_init
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade:examples/peripherals/isp/multi_pipelines/main/CMakeLists.txt
                       )
