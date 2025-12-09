idf_component_register(SRCS "usb_host_lib_main.c" "class_driver.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_gpio
                    )
