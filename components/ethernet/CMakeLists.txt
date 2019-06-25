idf_component_register(SRCS "emac_dev.c"
                            "emac_main.c"
                            "eth_phy/phy_common.c"
                            "eth_phy/phy_lan8720.c"
                            "eth_phy/phy_tlk110.c"
                            "eth_phy/phy_ip101.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES tcpip_adapter esp_event soc)
