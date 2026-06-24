idf_component_register(SRCS "vlan_support_main.c" "eth_vlan_utils.c"
                    PRIV_REQUIRES esp_netif esp_eth lwip
                    INCLUDE_DIRS ".")
