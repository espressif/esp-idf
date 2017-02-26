
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "tlk110_phy.h"

#define DEFAULT_PHY_CONFIG (AUTO_MDIX_ENABLE|AUTO_NEGOTIATION_ENABLE|AN_1|AN_0|LED_CFG)
void phy_dump_tlk110_registers();

static const char *TAG = "tlk110";


void phy_tlk110_check_phy_init(void)
{
    phy_dump_tlk110_registers();

    while((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & AUTO_NEGOTIATION_COMPLETE ) != AUTO_NEGOTIATION_COMPLETE)
    {};
    while((esp_eth_smi_read(PHY_STATUS_REG) & AUTO_NEGTIATION_STATUS ) != AUTO_NEGTIATION_STATUS)
    {};
    while((esp_eth_smi_read(CABLE_DIAGNOSTIC_CONTROL_REG) & DIAGNOSTIC_DONE ) != DIAGNOSTIC_DONE)
    {};
}

eth_speed_mode_t phy_tlk110_get_speed_mode(void)
{
    if((esp_eth_smi_read(PHY_STATUS_REG) & SPEED_STATUS ) != SPEED_STATUS) {
        ESP_LOGD(TAG, "phy_tlk110_get_speed_mode(100)");
        return ETH_SPEED_MODE_100M;
    } else {
        ESP_LOGD(TAG, "phy_tlk110_get_speed_mode(10)");
        return ETH_SPEED_MODE_10M;
    }
}

eth_duplex_mode_t phy_tlk110_get_duplex_mode(void)
{
    if((esp_eth_smi_read(PHY_STATUS_REG) & DUPLEX_STATUS ) == DUPLEX_STATUS) {
        ESP_LOGD(TAG, "phy_tlk110_get_duplex_mode(FULL)");
        return ETH_MDOE_FULLDUPLEX;
    } else {
        ESP_LOGD(TAG, "phy_tlk110_get_duplex_mode(HALF)");
        return ETH_MODE_HALFDUPLEX;
    }
}

bool phy_tlk110_check_phy_link_status(void)
{
    if ((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & LINK_STATUS) == LINK_STATUS) {
        ESP_LOGD(TAG, "phy_tlk110_check_phy_link_status(UP)");
        return true;
    } else {
        ESP_LOGD(TAG, "phy_tlk110_check_phy_link_status(DOWN)");
        return false;
    }
}

bool phy_tlk110_get_partner_pause_enable(void)
{
    if((esp_eth_smi_read(PHY_LINK_PARTNER_ABILITY_REG) & PARTNER_PAUSE) == PARTNER_PAUSE) {
        ESP_LOGD(TAG, "phy_tlk110_get_partner_pause_enable(TRUE)");
        return true;
    } else {
        ESP_LOGD(TAG, "phy_tlk110_get_partner_pause_enable(FALSE)");
        return false;
    }
}

void phy_enable_flow_ctrl(void)
{
    uint32_t data = 0;
    data = esp_eth_smi_read(AUTO_NEG_ADVERTISEMENT_REG);
    esp_eth_smi_write(AUTO_NEG_ADVERTISEMENT_REG,data|ASM_DIR|PAUSE);
}

void phy_tlk110_init(void)
{
    ESP_LOGD(TAG, "phy_tlk110_init()");
    phy_dump_tlk110_registers();

    esp_eth_smi_write(PHY_RESET_CONTROL_REG, SOFTWARE_RESET);

    while (esp_eth_smi_read(PHY_IDENTIFIER_REG) != OUI_MSB_21TO6_DEF) {
    }

    esp_eth_smi_write(SOFTWARE_STRAP_CONTROL_REG, DEFAULT_PHY_CONFIG |SW_STRAP_CONFIG_DONE);

    ets_delay_us(300);

    //if config.flow_ctrl_enable == true ,enable this
    phy_enable_flow_ctrl();
}

const eth_config_t tlk110_default_ethernet_phy_config = {
    .phy_addr = CONFIG_PHY_ID,
    .mac_mode = ETH_MODE_RMII,
    //Only FULLDUPLEX mode support flow ctrl now!
    .flow_ctrl_enable = true,
    .phy_init = phy_tlk110_init,
    .phy_check_init = phy_tlk110_check_phy_init,
    .phy_check_link = phy_tlk110_check_phy_link_status,
    .phy_get_speed_mode = phy_tlk110_get_speed_mode,
    .phy_get_duplex_mode = phy_tlk110_get_duplex_mode,
    .phy_get_partner_pause_enable = phy_tlk110_get_partner_pause_enable
};

void phy_dump_tlk110_registers()
{
    ESP_LOGD(TAG, "TLK110 Registers:");
    ESP_LOGD(TAG, "BMCR     0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGD(TAG, "BMSR     0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGD(TAG, "PHYIDR1  0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGD(TAG, "PHYIDR2  0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGD(TAG, "ANAR     0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGD(TAG, "ANLPAR   0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGD(TAG, "ANER     0x%04x", esp_eth_smi_read(0x6));
    ESP_LOGD(TAG, "ANNPTR   0x%04x", esp_eth_smi_read(0x7));
    ESP_LOGD(TAG, "ANLNPTR  0x%04x", esp_eth_smi_read(0x8));
    ESP_LOGD(TAG, "SWSCR1   0x%04x", esp_eth_smi_read(0x9));
    ESP_LOGD(TAG, "SWSCR2   0x%04x", esp_eth_smi_read(0xa));
    ESP_LOGD(TAG, "SWSCR3   0x%04x", esp_eth_smi_read(0xb));
    ESP_LOGD(TAG, "REGCR    0x%04x", esp_eth_smi_read(0xd));
    ESP_LOGD(TAG, "ADDAR    0x%04x", esp_eth_smi_read(0xe));
    ESP_LOGD(TAG, "PHYSTS   0x%04x", esp_eth_smi_read(0x10));
    ESP_LOGD(TAG, "PHYSCR   0x%04x", esp_eth_smi_read(0x11));
    ESP_LOGD(TAG, "MISR1    0x%04x", esp_eth_smi_read(0x12));
    ESP_LOGD(TAG, "MISR2    0x%04x", esp_eth_smi_read(0x13));
    ESP_LOGD(TAG, "FCSCR    0x%04x", esp_eth_smi_read(0x14));
    ESP_LOGD(TAG, "RECR     0x%04x", esp_eth_smi_read(0x15));
    ESP_LOGD(TAG, "BISCR    0x%04x", esp_eth_smi_read(0x16));
    ESP_LOGD(TAG, "RBR      0x%04x", esp_eth_smi_read(0x17));
    ESP_LOGD(TAG, "LEDCR    0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGD(TAG, "PHYCR    0x%04x", esp_eth_smi_read(0x19));
    ESP_LOGD(TAG, "10BTSCR  0x%04x", esp_eth_smi_read(0x1a));
    ESP_LOGD(TAG, "BICSR1   0x%04x", esp_eth_smi_read(0x1b));
    ESP_LOGD(TAG, "BICSR2   0x%04x", esp_eth_smi_read(0x1c));
    ESP_LOGD(TAG, "CDCR     0x%04x", esp_eth_smi_read(0x1e));
    ESP_LOGD(TAG, "TRXCPSR  0x%04x", esp_eth_smi_read(0x42));
    ESP_LOGD(TAG, "PWRBOCR  0x%04x", esp_eth_smi_read(0xae));
    ESP_LOGD(TAG, "VRCR     0x%04x", esp_eth_smi_read(0xD0));
    ESP_LOGD(TAG, "ALCDRR1  0x%04x", esp_eth_smi_read(0x155));
    ESP_LOGD(TAG, "CDSCR1   0x%04x", esp_eth_smi_read(0x170));
    ESP_LOGD(TAG, "CDSCR2   0x%04x", esp_eth_smi_read(0x171));
}
