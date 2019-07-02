set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS src proto-c ../protocomm/proto-c)
set(COMPONENT_SRCS  "src/wifi_config.c"
                    "src/wifi_scan.c"
                    "src/manager.c"
                    "src/handlers.c"
                    "src/scheme_softap.c"
                    "src/scheme_console.c"
                    "proto-c/wifi_config.pb-c.c"
                    "proto-c/wifi_scan.pb-c.c"
                    "proto-c/wifi_constants.pb-c.c")

set(COMPONENT_REQUIRES lwip protocomm)
set(COMPONENT_PRIV_REQUIRES protobuf-c bt mdns json)

if(CONFIG_BT_ENABLED)
    if(CONFIG_BLUEDROID_ENABLED)
        list(APPEND COMPONENT_SRCS
            "src/scheme_ble.c")
    endif()
endif()

register_component()
