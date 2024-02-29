idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "src/wifi_config.c"
        "src/wifi_scan.c"
        "src/wifi_ctrl.c"
        "src/manager.c"
        "src/handlers.c"
        "src/scheme_console.c"
        "proto-c/wifi_config.pb-c.c"
        "proto-c/wifi_scan.pb-c.c"
        "proto-c/wifi_ctrl.pb-c.c"
        "proto-c/wifi_constants.pb-c.c")

if(CONFIG_ESP_WIFI_SOFTAP_SUPPORT)
    list(APPEND srcs "src/scheme_softap.c")
endif()

if(CONFIG_BT_ENABLED)
    if(CONFIG_BT_BLUEDROID_ENABLED OR CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND srcs
            "src/scheme_ble.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS src proto-c
                    REQUIRES lwip protocomm
                    PRIV_REQUIRES protobuf-c bt json esp_timer esp_wifi)
