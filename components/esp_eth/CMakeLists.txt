set(esp_eth_srcs "src/esp_eth.c"
                 "src/esp_eth_phy_dp83848.c"
                 "src/esp_eth_phy_ip101.c"
                 "src/esp_eth_phy_lan8720.c"
                 "src/esp_eth_phy_rtl8201.c")

if(CONFIG_ETH_USE_ESP32_EMAC)
    list(APPEND esp_eth_srcs "src/esp_eth_mac_esp32.c")
endif()

if(CONFIG_ETH_SPI_ETHERNET_DM9051)
    list(APPEND esp_eth_srcs "src/esp_eth_mac_dm9051.c"
                             "src/esp_eth_phy_dm9051.c")
endif()

idf_component_register(SRCS "${esp_eth_srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES "esp_event"
                       PRIV_REQUIRES "tcpip_adapter" "driver" "log")

# uses C11 atomic feature
set_source_files_properties(src/esp_eth.c PROPERTIES COMPILE_FLAGS -std=gnu11)
