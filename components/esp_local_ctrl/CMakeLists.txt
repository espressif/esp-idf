set(include_dirs include)
set(priv_include_dirs proto-c src ../protocomm/proto-c)
set(srcs  "src/esp_local_ctrl.c"
          "src/esp_local_ctrl_handler.c"
          "proto-c/esp_local_ctrl.pb-c.c")

if(CONFIG_ESP_HTTPS_SERVER_ENABLE)
    list(APPEND srcs
         "src/esp_local_ctrl_transport_httpd.c")
endif()

if(CONFIG_BT_ENABLED)
    if(CONFIG_BT_BLUEDROID_ENABLED)
        list(APPEND srcs
             "src/esp_local_ctrl_transport_ble.c")
    endif()
endif()


idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    REQUIRES protocomm esp_https_server
                    PRIV_REQUIRES protobuf-c)

idf_component_optional_requires(PRIVATE espressif__mdns mdns)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
