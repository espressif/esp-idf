set(COMPONENT_SRCS  "ble_unit.c"
    "run_tc.c"
    "sync.c"
    "test_env.c"
    "wifi_connect.c"
    "wifi_unit.c")

set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_REQUIRES  console nvs_flash bt)
set(SUPPORTED_TARGETS esp32)
register_component()
