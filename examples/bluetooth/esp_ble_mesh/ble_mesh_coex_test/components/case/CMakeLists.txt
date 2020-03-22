set(srcs "ble_unit.c"
    "run_tc.c"
    "sync.c"
    "test_env.c"
    "wifi_connect.c"
    "wifi_unit.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  "."
                    REQUIRES console nvs_flash bt
                    REQUIRED_IDF_TARGETS esp32)
