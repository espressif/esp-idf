#define BASIC_MODE_STATUS_REG           (0x1)
#define AUTO_NEGOTIATION_COMPLETE          BIT(5)
#define LINK_STATUS                        BIT(2)

#define PHY_IDENTIFIER_REG              (0x2)
#define OUI_MSB_21TO6_DEF                      0x2000

#define AUTO_NEG_ADVERTISEMENT_REG      (0x4)
#define ASM_DIR                            BIT(11)
#define PAUSE                              BIT(10)

#define PHY_LINK_PARTNER_ABILITY_REG    (0x5)
#define PARTNER_ASM_DIR                    BIT(11)
#define PARTNER_PAUSE                      BIT(10) 

#define SOFTWARE_STRAP_CONTROL_REG       (0x9)
#define SW_STRAP_CONFIG_DONE               BIT(15)
#define AUTO_MDIX_ENABLE                   BIT(14)
#define AUTO_NEGOTIATION_ENABLE            BIT(13)
#define AN_1                               BIT(12)
#define AN_0                               BIT(11)
#define LED_CFG                            BIT(10)
#define RMII_ENHANCED_MODE                 BIT(9) 

#define PHY_STATUS_REG                  (0x10)
#define AUTO_NEGTIATION_STATUS             BIT(4)
#define DUPLEX_STATUS                      BIT(2)
#define SPEED_STATUS                       BIT(1)       

#define CABLE_DIAGNOSTIC_CONTROL_REG    (0x1e)
#define DIAGNOSTIC_DONE                    BIT(1)

#define PHY_RESET_CONTROL_REG           (0x1f)
#define SOFTWARE_RESET                     BIT(15)


void phy_device_check_phy_init(void);
bool phy_device_check_phy_link_status(void);
bool phy_device_get_partner_pause_enable(void);
void phy_device_init(void);
void phy_device_power_enable(bool enable);
void phy_dump_device_registers();
void phy_enable_flow_ctrl(void);
eth_duplex_mode_t phy_device_get_duplex_mode(void);
eth_speed_mode_t phy_device_get_speed_mode(void);
