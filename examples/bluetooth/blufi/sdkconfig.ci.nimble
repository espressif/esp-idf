CONFIG_BT_ENABLED=y
CONFIG_BT_NIMBLE_ENABLED=y
CONFIG_BT_NIMBLE_BLUFI_ENABLE=y

# Wi-Fi + NimBLE on ESP32 is IRAM-tight after recent wifi lib updates.
# Move Wi-Fi IRAM-optimized paths back to flash to keep the CI build linking.
CONFIG_ESP_WIFI_IRAM_OPT=n
CONFIG_ESP_WIFI_RX_IRAM_OPT=n
