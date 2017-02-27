
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "tlk110_phy.h"

#define DEFAULT_PHY_CONFIG (AUTO_MDIX_ENABLE|AUTO_NEGOTIATION_ENABLE|AN_1|AN_0|LED_CFG)

extern char *demo_tag;


void phy_device_check_phy_init(void)
{
#ifdef CONFIG_PHY_DEBUG
    phy_dump_device_registers();
#endif

    while((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & AUTO_NEGOTIATION_COMPLETE ) != AUTO_NEGOTIATION_COMPLETE)
    {};
    while((esp_eth_smi_read(PHY_STATUS_REG) & AUTO_NEGTIATION_STATUS ) != AUTO_NEGTIATION_STATUS)
    {};
    while((esp_eth_smi_read(CABLE_DIAGNOSTIC_CONTROL_REG) & DIAGNOSTIC_DONE ) != DIAGNOSTIC_DONE)
    {};
}

eth_speed_mode_t phy_device_get_speed_mode(void)
{
    if((esp_eth_smi_read(PHY_STATUS_REG) & SPEED_STATUS ) != SPEED_STATUS) {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_get_speed_mode(100)");
#endif
        return ETH_SPEED_MODE_100M;
    } else {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_get_speed_mode(10)");
#endif
        return ETH_SPEED_MODE_10M;
    }
}

eth_duplex_mode_t phy_device_get_duplex_mode(void)
{
    if((esp_eth_smi_read(PHY_STATUS_REG) & DUPLEX_STATUS ) == DUPLEX_STATUS) {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_get_duplex_mode(FULL)");
#endif
        return ETH_MDOE_FULLDUPLEX;
    } else {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_get_duplex_mode(HALF)");
#endif
        return ETH_MODE_HALFDUPLEX;
    }
}

bool phy_device_check_phy_link_status(void)
{
    if ((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & LINK_STATUS) == LINK_STATUS) {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_check_phy_link_status(UP)");
#endif
        return true;
    } else {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_check_phy_link_status(DOWN)");
#endif
        return false;
    }
}

bool phy_device_get_partner_pause_enable(void)
{
    if((esp_eth_smi_read(PHY_LINK_PARTNER_ABILITY_REG) & PARTNER_PAUSE) == PARTNER_PAUSE) {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_get_partner_pause_enable(TRUE)");
#endif
        return true;
    } else {
#ifdef CONFIG_PHY_DEBUG
        ESP_LOGI(demo_tag, "phy_device_get_partner_pause_enable(FALSE)");
#endif
        return false;
    }
}

void phy_enable_flow_ctrl(void)
{
    uint32_t data = 0;
    data = esp_eth_smi_read(AUTO_NEG_ADVERTISEMENT_REG);
    esp_eth_smi_write(AUTO_NEG_ADVERTISEMENT_REG,data|ASM_DIR|PAUSE);
}

void phy_device_init(void)
{
#ifdef CONFIG_PHY_DEBUG
    ESP_LOGI(demo_tag, "phy_device_init()");
    phy_dump_device_registers();
#endif
    esp_eth_smi_write(PHY_RESET_CONTROL_REG, SOFTWARE_RESET);

    while (esp_eth_smi_read(PHY_IDENTIFIER_REG) != OUI_MSB_21TO6_DEF) {
    }

    esp_eth_smi_write(SOFTWARE_STRAP_CONTROL_REG, DEFAULT_PHY_CONFIG |SW_STRAP_CONFIG_DONE);

    ets_delay_us(300);

    //if config.flow_ctrl_enable == true ,enable this
    phy_enable_flow_ctrl();
}

void phy_dump_device_registers()
{
    ESP_LOGI(demo_tag, "TLK110 Registers:");
    ESP_LOGI(demo_tag, "BMCR     0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGI(demo_tag, "BMSR     0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGI(demo_tag, "PHYIDR1  0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGI(demo_tag, "PHYIDR2  0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGI(demo_tag, "ANAR     0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGI(demo_tag, "ANLPAR   0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGI(demo_tag, "ANER     0x%04x", esp_eth_smi_read(0x6));
    ESP_LOGI(demo_tag, "ANNPTR   0x%04x", esp_eth_smi_read(0x7));
    ESP_LOGI(demo_tag, "ANLNPTR  0x%04x", esp_eth_smi_read(0x8));
    ESP_LOGI(demo_tag, "SWSCR1   0x%04x", esp_eth_smi_read(0x9));
    ESP_LOGI(demo_tag, "SWSCR2   0x%04x", esp_eth_smi_read(0xa));
    ESP_LOGI(demo_tag, "SWSCR3   0x%04x", esp_eth_smi_read(0xb));
    ESP_LOGI(demo_tag, "REGCR    0x%04x", esp_eth_smi_read(0xd));
    ESP_LOGI(demo_tag, "ADDAR    0x%04x", esp_eth_smi_read(0xe));
    ESP_LOGI(demo_tag, "PHYSTS   0x%04x", esp_eth_smi_read(0x10));
    ESP_LOGI(demo_tag, "PHYSCR   0x%04x", esp_eth_smi_read(0x11));
    ESP_LOGI(demo_tag, "MISR1    0x%04x", esp_eth_smi_read(0x12));
    ESP_LOGI(demo_tag, "MISR2    0x%04x", esp_eth_smi_read(0x13));
    ESP_LOGI(demo_tag, "FCSCR    0x%04x", esp_eth_smi_read(0x14));
    ESP_LOGI(demo_tag, "RECR     0x%04x", esp_eth_smi_read(0x15));
    ESP_LOGI(demo_tag, "BISCR    0x%04x", esp_eth_smi_read(0x16));
    ESP_LOGI(demo_tag, "RBR      0x%04x", esp_eth_smi_read(0x17));
    ESP_LOGI(demo_tag, "LEDCR    0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGI(demo_tag, "PHYCR    0x%04x", esp_eth_smi_read(0x19));
    ESP_LOGI(demo_tag, "10BTSCR  0x%04x", esp_eth_smi_read(0x1a));
    ESP_LOGI(demo_tag, "BICSR1   0x%04x", esp_eth_smi_read(0x1b));
    ESP_LOGI(demo_tag, "BICSR2   0x%04x", esp_eth_smi_read(0x1c));
    ESP_LOGI(demo_tag, "CDCR     0x%04x", esp_eth_smi_read(0x1e));
    ESP_LOGI(demo_tag, "TRXCPSR  0x%04x", esp_eth_smi_read(0x42));
    ESP_LOGI(demo_tag, "PWRBOCR  0x%04x", esp_eth_smi_read(0xae));
    ESP_LOGI(demo_tag, "VRCR     0x%04x", esp_eth_smi_read(0xD0));
    ESP_LOGI(demo_tag, "ALCDRR1  0x%04x", esp_eth_smi_read(0x155));
    ESP_LOGI(demo_tag, "CDSCR1   0x%04x", esp_eth_smi_read(0x170));
    ESP_LOGI(demo_tag, "CDSCR2   0x%04x", esp_eth_smi_read(0x171));
}
