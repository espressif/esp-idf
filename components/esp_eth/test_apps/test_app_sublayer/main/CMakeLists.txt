idf_component_register(SRCS "test_main.c"
                            "test_sublayer_common.c"
                            "test_sublayer_events.c"
                            "test_sublayer_hooks.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_eth esp_netif esp_event lwip
                       WHOLE_ARCHIVE)
