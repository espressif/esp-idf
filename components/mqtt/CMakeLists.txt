set(COMPONENT_ADD_INCLUDEDIRS esp-mqtt/include)
set(COMPONENT_PRIV_INCLUDEDIRS "esp-mqtt/lib/include")
set(COMPONENT_SRCS "esp-mqtt/mqtt_client.c"
                   "esp-mqtt/lib/mqtt_msg.c"
                   "esp-mqtt/lib/mqtt_outbox.c"
                   "esp-mqtt/lib/platform_esp32_idf.c"
                   "esp-mqtt/lib/transport_ws.c")

set(COMPONENT_REQUIRES lwip nghttp mbedtls tcp_transport)

register_component()

if(GCC_NOT_5_2_0)
    # Temporary suppress "format-overflow" warning until we are fixed in esp-mqtt repo
    set_source_files_properties(
        esp-mqtt/lib/transport_ws.c
        PROPERTIES COMPILE_FLAGS
        -Wno-format-overflow)
endif()
