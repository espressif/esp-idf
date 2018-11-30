set(COMPONENT_SRCS "emac_dev.c"
                   "emac_main.c"
                   "eth_phy/phy_common.c"
                   "eth_phy/phy_lan8720.c"
                   "eth_phy/phy_tlk110.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES tcpip_adapter esp_event)

register_component()
