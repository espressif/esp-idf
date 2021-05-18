#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

ifndef CONFIG_ETH_USE_ESP32_EMAC
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_esp.o
endif

ifndef CONFIG_ETH_SPI_ETHERNET_DM9051
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_dm9051.o src/esp_eth_phy_dm9051.o
endif

ifndef CONFIG_ETH_SPI_ETHERNET_W5500
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_w5500.o src/esp_eth_phy_w5500.o
endif

ifndef CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_ksz8851snl.o src/esp_eth_phy_ksz8851snl.o
endif

ifndef CONFIG_ETH_USE_OPENETH
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_openeth.o
endif

# uses C11 atomic feature
src/esp_eth.o: CFLAGS += -std=gnu11
