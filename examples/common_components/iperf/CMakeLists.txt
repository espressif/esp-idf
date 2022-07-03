idf_component_register(SRCS "iperf.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lwip
                    PRIV_REQUIRES esp_timer)
