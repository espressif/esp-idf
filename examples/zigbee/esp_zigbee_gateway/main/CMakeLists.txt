idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "."
                       PRIV_REQUIRES ieee802154 esp_driver_usb_serial_jtag esp_coex esp_netif
                                     vfs esp_wifi spiffs nvs_flash esp_driver_uart)
