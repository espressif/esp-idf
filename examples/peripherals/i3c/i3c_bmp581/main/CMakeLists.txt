set(srcs "i3c_bmp581_main.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES esp_driver_i3c esp_driver_gpio
                       INCLUDE_DIRS ".")
