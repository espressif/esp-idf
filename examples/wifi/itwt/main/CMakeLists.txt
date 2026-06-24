idf_component_register(SRCS "itwt_main.c"
                            "wifi_stats_cmd.c"
                            "wifi_itwt_cmd.c"
                    PRIV_REQUIRES lwip esp_wifi esp_timer esp_pm
                    INCLUDE_DIRS ".")
