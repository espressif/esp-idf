idf_component_register(SRCS "ble_ams_demo.c" "ble_ams.c"
                    PRIV_REQUIRES bt nvs_flash
                    INCLUDE_DIRS  ".")
