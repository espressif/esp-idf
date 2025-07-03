set(srcs "cert_test.c"
         "cmd_phy.c")
if(CONFIG_COMMANDS_ENABLE_BLE_DTM_TEST)
list(APPEND srcs "cmd_ble_dtm.c")
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES bt
                    INCLUDE_DIRS ".")
