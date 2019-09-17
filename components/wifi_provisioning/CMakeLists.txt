set(srcs "src/wifi_config.c"
        "src/wifi_scan.c"
        "src/manager.c"
        "src/handlers.c"
        "src/scheme_softap.c"
        "src/scheme_console.c"
        "proto-c/wifi_config.pb-c.c"
        "proto-c/wifi_scan.pb-c.c"
        "proto-c/wifi_constants.pb-c.c")

if(CONFIG_BT_ENABLED)
    if(CONFIG_BT_BLUEDROID_ENABLED OR CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND srcs
            "src/scheme_ble.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS src proto-c ../protocomm/proto-c
                    REQUIRES lwip protocomm
                    PRIV_REQUIRES protobuf-c bt mdns json)

# To avoid warning for strncpy
set_source_files_properties(src/handlers.c src/scheme_softap.c
    PROPERTIES COMPILE_FLAGS
    -Wno-stringop-truncation
)
