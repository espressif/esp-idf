set(srcs "ble_spp_server_demo.c")

if(CONFIG_EXAMPLE_ENABLE_RF_TESTING_CONFIGURATION_COMMAND)
list(APPEND srcs
     "rf_tesing_configuration_cmd.c")
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES bt nvs_flash esp_driver_uart console
                    INCLUDE_DIRS ".")
