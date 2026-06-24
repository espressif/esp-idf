idf_component_register(SRCS "i2c_u8g2_main.c" "u8g2_demo.c"
                       PRIV_REQUIRES esp_driver_i2c esp_driver_gpio
                       INCLUDE_DIRS ".")
