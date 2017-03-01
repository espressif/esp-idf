#define BASIC_MODE_CONTROL_REG          (0x0)
#define SOFTWARE_RESET                     BIT(15)

#define BASIC_MODE_STATUS_REG           (0x1)
#define AUTO_NEGOTIATION_COMPLETE          BIT(5)
#define LINK_STATUS                        BIT(2)

#define PHY_IDENTIFIER_REG              (0x2)
#define OUI_MSB_21TO6_DEF                      0x0007

#define AUTO_NEG_ADVERTISEMENT_REG      (0x4)
#define ASM_DIR                            BIT(11)
#define PAUSE                              BIT(10)

#define PHY_LINK_PARTNER_ABILITY_REG    (0x5)
#define PARTNER_PAUSE                      BIT(10) 

#define SOFTWARE_STRAP_CONTROL_REG       (0x9)
#define SW_STRAP_CONFIG_DONE               BIT(15)
#define AUTO_MDIX_ENABLE                   BIT(14)
#define AUTO_NEGOTIATION_ENABLE            BIT(13)
#define AN_1                               BIT(12)
#define AN_0                               BIT(11)
#define LED_CFG                            BIT(10)
#define RMII_ENHANCED_MODE                 BIT(9) 

#define PHY_SPECIAL_CONTROL_STATUS_REG    (0x1f)
#define AUTO_NEGOTIATION_DONE              BIT(12)
#define SPEED_DUPLEX_INDICATION_10T_HALF   0x04
#define SPEED_DUPLEX_INDICATION_10T_FULL   0x14
#define SPEED_DUPLEX_INDICATION_100T_HALF  0x08
#define SPEED_DUPLEX_INDICATION_100T_FULL  0x18
#define SPEED_INDICATION_100T              BIT(3)
#define SPEED_INDICATION_10T               BIT(2)
#define DUPLEX_INDICATION_FULL             BIT(4)

extern const eth_config_t lan8720_default_ethernet_phy_config;
