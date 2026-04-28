# Both backends are listed; each .c file body is wrapped in
# #if CONFIG_BT_NIMBLE_ENABLED / CONFIG_BT_BLUEDROID_ENABLED so only
# the matching backend produces code. This is the standard IDF
# pattern for conditional sources, because sdkconfig isn't loaded
# during the early CMake component-requirement scan.
idf_component_register(SRCS "main.c"
                            "ble_uart_nimble.c"
                            "ble_uart_bluedroid.c"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
