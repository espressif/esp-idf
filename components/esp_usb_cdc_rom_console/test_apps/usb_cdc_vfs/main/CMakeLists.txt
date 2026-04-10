set(src "test_app_main.c")

idf_component_register(SRCS ${src}
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES esp_system esp_stdio unity esp_usb_cdc_rom_console vfs
                       WHOLE_ARCHIVE
                       )
