idf_component_register(SRCS "dns_over_https.c" "dns_utils.c"
                    INCLUDE_DIRS "include" "."
                    PRIV_REQUIRES nvs_flash lwip esp_event esp-tls esp_http_client)

if(CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_CUSTOM)
    target_link_libraries(${COMPONENT_LIB} "-u lwip_hook_netconn_external_resolve")
endif()
