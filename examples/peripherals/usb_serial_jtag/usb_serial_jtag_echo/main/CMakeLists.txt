idf_component_register(SRCS "usb_serial_echo_main.c"
                    PRIV_REQUIRES esp_driver_usb_serial_jtag
                    INCLUDE_DIRS ".")
