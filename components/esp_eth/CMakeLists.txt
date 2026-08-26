idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(include)
set(priv_include_dirs)
set(ld_fragments linker.lf)
# As CONFIG_ETH_ENABLED comes from Kconfig, it is not evaluated yet
# when components are being registered.
# Thus, always add the (private) requirements, regardless of Kconfig
set(priv_requires log esp_timer esp_driver_spi esp_driver_gpio esp_hal_clock)

# If Ethernet disabled in Kconfig, this is a config-only component
if(CONFIG_ETH_ENABLED)
    set(srcs "src/esp_eth.c" "src/phy/esp_eth_phy_802_3.c")
    set(include "include")

    if(NOT CMAKE_BUILD_EARLY_EXPANSION AND NOT IDF_BUILD_V2)
        # esp_netif related
        idf_build_get_property(components_to_build BUILD_COMPONENTS)
        if(esp_netif IN_LIST components_to_build)
            list(APPEND srcs "src/esp_eth_netif_glue.c")
            if(CONFIG_ETH_SUBLAYER_SUPPORT)
                list(APPEND srcs "src/sublayer/esp_eth_sublayer.c"
                                 "src/sublayer/esp_eth_sublayer_vlan_child.c")
                if(CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT)
                    list(APPEND srcs "src/sublayer/esp_eth_sublayer_switch.c")
                endif()
                list(APPEND priv_include_dirs "src/sublayer")
            endif()
        endif()
    endif()

    if(CONFIG_ETH_USE_ESP32_EMAC)
        list(APPEND srcs "src/mac/esp_eth_mac_esp.c"
                         "src/mac/esp_eth_mac_esp_dma.c"
                         "src/mac/esp_eth_mac_esp_gpio.c"
                         "src/phy/esp_eth_phy_generic.c")
        list(APPEND priv_include_dirs "src/mac")
        if(CONFIG_SOC_PAU_SUPPORTED)
            list(APPEND srcs "src/mac/${target}/emac_retention.c")
        endif()
    endif()

    if(CONFIG_SOC_EMAC_IEEE1588V2_SUPPORTED)
        list(APPEND srcs "src/eth_clock/esp_eth_clock.c")
    endif()

    if(CONFIG_ETH_USE_OPENETH)
        list(APPEND srcs "src/openeth/esp_eth_mac_openeth.c"
                         "src/phy/esp_eth_phy_generic.c")
    endif()
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${include}
                       PRIV_INCLUDE_DIRS ${priv_include_dirs}
                       LDFRAGMENTS ${ld_fragments}
                       REQUIRES esp_event # For using "ESP_EVENT_DECLARE_BASE" in header file
                                esp_hal_emac
                       PRIV_REQUIRES ${priv_requires})

if(CONFIG_ETH_ENABLED)
    if(IDF_BUILD_V2)
        target_sources(${COMPONENT_TARGET} PRIVATE
            "$<$<TARGET_EXISTS:idf::esp_netif>:src/esp_eth_netif_glue.c>")
        if(CONFIG_ETH_SUBLAYER_SUPPORT)
            target_sources(${COMPONENT_TARGET} PRIVATE
                "$<$<TARGET_EXISTS:idf::esp_netif>:src/sublayer/esp_eth_sublayer.c>"
                "$<$<TARGET_EXISTS:idf::esp_netif>:src/sublayer/esp_eth_sublayer_vlan_child.c>")
            if(CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT)
                target_sources(${COMPONENT_TARGET} PRIVATE
                    "$<$<TARGET_EXISTS:idf::esp_netif>:src/sublayer/esp_eth_sublayer_switch.c>")
            endif()
            target_include_directories(${COMPONENT_TARGET} PRIVATE "src/sublayer")
        endif()
    endif()
    if(CONFIG_ETH_USE_SPI_ETHERNET)
        idf_component_optional_requires(PUBLIC esp_driver_spi)
    endif()
    idf_component_optional_requires(PRIVATE esp_netif esp_pm esp_mm)
endif()
