idf_build_get_property(target IDF_TARGET)

if("${CONFIG_EXAMPLE_IPV4}" STREQUAL y)
    set(tcp_client_ip tcp_client_v4.c)
else()
    set(tcp_client_ip tcp_client_v6.c)
endif()

idf_component_register(SRCS "tcp_client_main.c" "${tcp_client_ip}"
                                INCLUDE_DIRS "."
                                PRIV_REQUIRES unity nvs_flash esp_netif)
