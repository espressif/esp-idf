set(requires esp_https_server nvs_flash esp_timer esp_netif)
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs protocol_examples_common)
else()
    list(APPEND requires esp_wifi esp_eth)
endif()

idf_component_register(SRCS "wss_server_example.c" "keep_alive.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ${requires}
                    EMBED_TXTFILES "certs/servercert.pem"
                                   "certs/prvtkey.pem")
