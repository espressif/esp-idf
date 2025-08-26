set(src "test_app_main.c")

idf_component_register(SRCS ${src}
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES esp_system esp_vfs_console unity esp_usb_cdc_rom_console
                       WHOLE_ARCHIVE
                       )
