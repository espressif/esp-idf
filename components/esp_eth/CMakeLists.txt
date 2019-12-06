set(srcs)  # If ethernet disabled in Kconfig, this is a config-only component
set(include)
set(linker)

if(CONFIG_ETH_ENABLED)
    set(srcs "src/esp_eth.c")
    set(include "include")
    set(linker "linker.lf")

    if(CONFIG_ETH_USE_ESP32_EMAC)
        list(APPEND srcs "src/esp_eth_mac_esp32.c"
                                 "src/esp_eth_phy_dp83848.c"
                                 "src/esp_eth_phy_ip101.c"
                                 "src/esp_eth_phy_lan8720.c"
                                 "src/esp_eth_phy_rtl8201.c"
            )
    endif()

    if(CONFIG_ETH_SPI_ETHERNET_DM9051)
        list(APPEND srcs "src/esp_eth_mac_dm9051.c"
                                 "src/esp_eth_phy_dm9051.c")
    endif()
endif()

if(CONFIG_ETH_USE_OPENETH)
    list(APPEND srcs "src/esp_eth_mac_openeth.c")
endif()

idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS ${include}
    LDFRAGMENTS ${linker}
    REQUIRES "esp_event"
    PRIV_REQUIRES "esp_netif" "driver" "log")

