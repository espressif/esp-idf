idf_component_register(SRCS "cmd_ethernet.c"
                            "ethernet_iperf_main.c"
                       PRIV_REQUIRES fatfs esp_netif esp_eth esp_psram
                       INCLUDE_DIRS ".")
