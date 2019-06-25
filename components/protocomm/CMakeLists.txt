set(include_dirs include/common
                include/security
                include/transports)
set(priv_include_dirs proto-c src/common src/simple_ble)
set(srcs 
    "src/common/protocomm.c"
    "src/security/security0.c"
    "src/security/security1.c"
    "proto-c/constants.pb-c.c"
    "proto-c/sec0.pb-c.c"
    "proto-c/sec1.pb-c.c"
    "proto-c/session.pb-c.c"
    "src/transports/protocomm_console.c"
    "src/transports/protocomm_httpd.c")

if(CONFIG_BT_ENABLED)
    if(CONFIG_BT_BLUEDROID_ENABLED)
        list(APPEND srcs
            "src/simple_ble/simple_ble.c"
            "src/transports/protocomm_ble.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    PRIV_REQUIRES protobuf-c mbedtls console esp_http_server bt)
