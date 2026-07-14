idf_component_register(SRCS "example_sensor_init.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_driver_i2c esp_cam_sensor
                       )

# TODO IDF-15784: remove
# esp_cam_sensor upstream sets locals without reading them; silence -Wunused-but-set-variable
idf_component_get_property(esp_cam_sensor_lib espressif__esp_cam_sensor COMPONENT_LIB)
target_compile_options(${esp_cam_sensor_lib} PRIVATE -Wno-unused-but-set-variable)
