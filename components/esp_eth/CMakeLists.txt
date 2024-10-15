idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_build_get_property(components_to_build BUILD_COMPONENTS)

set(srcs)
set(include)
set(ld_fragments linker.lf)
# As CONFIG_ETH_ENABLED comes from Kconfig, it is not evaluated yet
# when components are being registered.
# Thus, always add the (private) requirements, regardless of Kconfig
set(priv_requires log esp_timer esp_driver_spi esp_driver_gpio)

# If Ethernet disabled in Kconfig, this is a config-only component
if(CONFIG_ETH_ENABLED)
    set(srcs "src/esp_eth.c" "src/phy/esp_eth_phy_802_3.c")
    set(include "include")

    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        # esp_netif related
        if(esp_netif IN_LIST components_to_build)
            list(APPEND srcs "src/esp_eth_netif_glue.c")
        endif()
    endif()

    if(CONFIG_ETH_USE_ESP32_EMAC)
        list(APPEND srcs "src/mac/esp_eth_mac_esp.c"
                         "src/mac/esp_eth_mac_esp_dma.c"
                         "src/mac/esp_eth_mac_esp_gpio.c"
                         "src/phy/esp_eth_phy_generic.c"
                         "src/phy/esp_eth_phy_dp83848.c"
                         "src/phy/esp_eth_phy_ip101.c"
                         "src/phy/esp_eth_phy_ksz80xx.c"
                         "src/phy/esp_eth_phy_lan87xx.c"
                         "src/phy/esp_eth_phy_rtl8201.c")
    endif()

    if(CONFIG_ETH_SPI_ETHERNET_DM9051)
        list(APPEND srcs "src/spi/dm9051/esp_eth_mac_dm9051.c"
                         "src/spi/dm9051/esp_eth_phy_dm9051.c")
    endif()

    if(CONFIG_ETH_SPI_ETHERNET_W5500)
        list(APPEND srcs "src/spi/w5500/esp_eth_mac_w5500.c"
                         "src/spi/w5500/esp_eth_phy_w5500.c")
    endif()

    if(CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL)
        list(APPEND srcs "src/spi/ksz8851snl/esp_eth_mac_ksz8851snl.c"
                         "src/spi/ksz8851snl/esp_eth_phy_ksz8851snl.c")
    endif()

    if(CONFIG_ETH_USE_OPENETH)
        list(APPEND srcs "src/openeth/esp_eth_mac_openeth.c"
                         "src/phy/esp_eth_phy_dp83848.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${include}
                       LDFRAGMENTS ${ld_fragments}
                       REQUIRES esp_event # For using "ESP_EVENT_DECLARE_BASE" in header file
                       PRIV_REQUIRES ${priv_requires})

if(CONFIG_ETH_ENABLED)
    if(CONFIG_ETH_USE_SPI_ETHERNET)
        idf_component_optional_requires(PUBLIC esp_driver_spi)
    endif()
    idf_component_optional_requires(PRIVATE esp_netif esp_pm esp_mm)
endif()
