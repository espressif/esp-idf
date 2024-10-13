idf_component_register(SRCS "hid_host_example.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES usb esp_driver_gpio
                       )
