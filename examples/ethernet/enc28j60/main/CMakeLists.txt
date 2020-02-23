set(srcs "enc28j60_example_main.c"
         "esp_eth_mac_enc28j60.c"
         "esp_eth_phy_enc28j60.c")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
