#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src
COMPONENT_ADD_LDFRAGMENTS += linker.lf

ifndef CONFIG_ETH_USE_ESP32_EMAC
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_esp32.o
endif

ifndef CONFIG_ETH_SPI_ETHERNET_DM9051
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_dm9051.o src/esp_eth_phy_dm9051.o
endif

ifndef CONFIG_ETH_USE_OPENETH
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_openeth.o
endif
