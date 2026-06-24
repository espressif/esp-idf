idf_component_register(SRCS "esp_zigbee_gateway.c"
                    PRIV_REQUIRES ieee802154 esp_driver_usb_serial_jtag esp_coex esp_netif
                                  vfs esp_wifi nvs_flash esp_driver_uart
                    INCLUDE_DIRS ".")
