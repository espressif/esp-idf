idf_component_register(SRCS "example_sensor_init.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_driver_i2c esp_cam_sensor
                       )
