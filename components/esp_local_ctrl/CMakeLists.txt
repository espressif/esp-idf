idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(include_dirs include)
set(priv_include_dirs proto-c src)
set(srcs  "src/esp_local_ctrl.c"
          "src/esp_local_ctrl_handler.c"
          "proto-c/esp_local_ctrl.pb-c.c")

if(CONFIG_BT_ENABLED)
    if(CONFIG_BT_BLUEDROID_ENABLED OR CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND srcs
             "src/esp_local_ctrl_transport_ble.c")
    endif()
endif()

list(APPEND srcs
        "src/esp_local_ctrl_transport_httpd.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    REQUIRES protocomm esp_https_server
                    PRIV_REQUIRES protobuf-c)

idf_component_optional_requires(PRIVATE espressif__mdns mdns)
