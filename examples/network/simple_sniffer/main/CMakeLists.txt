idf_component_register(SRCS "simple_sniffer_example_main.c"
                            "cmd_sniffer.c"
                            "cmd_pcap.c"
                    PRIV_REQUIRES console esp_wifi fatfs esp_eth app_trace nvs_flash
                    INCLUDE_DIRS ".")
