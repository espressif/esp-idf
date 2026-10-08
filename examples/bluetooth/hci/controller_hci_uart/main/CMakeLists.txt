# Console and extra command groups are optional (see Example Configuration in menuconfig).
# esp_driver_uart is always needed: esp_bt.h pulls in driver/uart.h when HCI runs over UART.
set(srcs "main.c")
set(priv_requires bt nvs_flash esp_driver_uart)
if(CONFIG_ENABLE_HCI_CONSOLE)
    list(APPEND srcs "hci_common.c")
    list(APPEND priv_requires console)
endif()
if(CONFIG_ENABLE_BLE_DTM_CONFIGURATION_COMMAND)
    list(APPEND srcs "ble_dtm_config.c")
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES ${priv_requires}
                    INCLUDE_DIRS ".")
