idf_build_get_property(components_to_build BUILD_COMPONENTS)

set(srcs)
set(include)
set(priv_requires)

# If Ethernet disabled in Kconfig, this is a config-only component
if(CONFIG_ETH_ENABLED)
    set(srcs "src/esp_eth.c" "src/esp_eth_phy.c")
    set(include "include")
    set(priv_requires "driver" "log" "esp_timer") # require "driver" for using some GPIO APIs

    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        # esp_netif related
        if(esp_netif IN_LIST components_to_build)
            list(APPEND srcs "src/esp_eth_netif_glue.c")
            list(APPEND priv_requires esp_netif esp_pm)
        endif()
    endif()

    if(CONFIG_ETH_USE_ESP32_EMAC)
        list(APPEND srcs "src/esp_eth_mac_esp.c"
                         "src/esp_eth_phy_dp83848.c"
                         "src/esp_eth_phy_ip101.c"
                         "src/esp_eth_phy_ksz80xx.c"
                         "src/esp_eth_phy_lan87xx.c"
                         "src/esp_eth_phy_rtl8201.c")
    endif()

    if(CONFIG_ETH_SPI_ETHERNET_DM9051)
        list(APPEND srcs "src/esp_eth_mac_dm9051.c"
                         "src/esp_eth_phy_dm9051.c")
    endif()

    if(CONFIG_ETH_SPI_ETHERNET_W5500)
        list(APPEND srcs "src/esp_eth_mac_w5500.c"
                         "src/esp_eth_phy_w5500.c")
    endif()

    if(CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL)
        list(APPEND srcs "src/esp_eth_mac_ksz8851snl.c"
                         "src/esp_eth_phy_ksz8851snl.c")
    endif()

    if(CONFIG_ETH_USE_OPENETH)
        list(APPEND srcs "src/esp_eth_mac_openeth.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${include}
                       REQUIRES "esp_event" # For using "ESP_EVENT_DECLARE_BASE" in header file
                       PRIV_REQUIRES ${priv_requires})

# uses C11 atomic feature
set_source_files_properties(src/esp_eth.c PROPERTIES COMPILE_FLAGS -std=gnu11)
