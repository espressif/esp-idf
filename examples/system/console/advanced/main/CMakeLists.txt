idf_component_register(SRCS "console_example_main.c" "console_settings.c"
                       PRIV_REQUIRES console esp_driver_uart fatfs esp_driver_usb_serial_jtag
                                     esp_usb_cdc_rom_console nvs_flash cmd_system cmd_wifi cmd_nvs
                       INCLUDE_DIRS ".")
