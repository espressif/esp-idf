set(src "test_app_main.c"
        "test_vfs_usb_serial_jtag.c"
        )

idf_component_register(SRCS ${src}
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES esp_driver_usb_serial_jtag unity test_utils esp_psram
                       WHOLE_ARCHIVE
                       )
