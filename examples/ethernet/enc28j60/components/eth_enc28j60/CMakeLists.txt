idf_component_register(SRCS "esp_eth_mac_enc28j60.c"
                            "esp_eth_phy_enc28j60.c"
                       PRIV_REQUIRES driver esp_eth
                       INCLUDE_DIRS ".")
