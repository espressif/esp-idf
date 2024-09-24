idf_component_register(SRCS "ble_ancs_demo.c" "ble_ancs.c"
                    PRIV_REQUIRES bt nvs_flash
                    INCLUDE_DIRS  ".")
