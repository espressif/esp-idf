
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "lan8720_phy.h"

void phy_dump_lan8720_registers();

static const char *TAG = "lan8720";


void phy_lan8720_check_phy_init(void)
{
    phy_dump_lan8720_registers();

    while((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & AUTO_NEGOTIATION_COMPLETE ) != AUTO_NEGOTIATION_COMPLETE)
    {};
    while((esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & AUTO_NEGOTIATION_DONE ) != AUTO_NEGOTIATION_DONE)
    {};
}

eth_speed_mode_t phy_lan8720_get_speed_mode(void)
{
    if(esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & SPEED_INDICATION_100T ) {
        ESP_LOGD(TAG, "phy_lan8720_get_speed_mode(100)");
        return ETH_SPEED_MODE_100M;
    } else {
        ESP_LOGD(TAG, "phy_lan8720_get_speed_mode(10)");
        return ETH_SPEED_MODE_10M;
    }
}

eth_duplex_mode_t phy_lan8720_get_duplex_mode(void)
{
    if(esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & DUPLEX_INDICATION_FULL ) {
        ESP_LOGD(TAG, "phy_lan8720_get_duplex_mode(FULL)");
        return ETH_MDOE_FULLDUPLEX;
    } else {
        ESP_LOGD(TAG, "phy_lan8720_get_duplex_mode(HALF)");
        return ETH_MODE_HALFDUPLEX;
    }
}

bool phy_lan8720_check_phy_link_status(void)
{
    if ((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & LINK_STATUS) == LINK_STATUS) {
        ESP_LOGD(TAG, "phy_lan8720_check_phy_link_status(UP)");
        return true;
    } else {
        ESP_LOGD(TAG, "phy_lan8720_check_phy_link_status(DOWN)");
        return false;
    }
}

bool phy_lan8720_get_partner_pause_enable(void)
{
    if((esp_eth_smi_read(PHY_LINK_PARTNER_ABILITY_REG) & PARTNER_PAUSE) == PARTNER_PAUSE) {
        ESP_LOGD(TAG, "phy_lan8720_get_partner_pause_enable(TRUE)");
        return true;
    } else {
        ESP_LOGD(TAG, "phy_lan8720_get_partner_pause_enable(FALSE)");
        return false;
    }
}

void phy_enable_flow_ctrl(void)
{
    uint32_t data = 0;
    data = esp_eth_smi_read(AUTO_NEG_ADVERTISEMENT_REG);
    esp_eth_smi_write(AUTO_NEG_ADVERTISEMENT_REG,data|ASM_DIR|PAUSE);
}

void phy_lan8720_init(void)
{
    ESP_LOGD(TAG, "phy_lan8720_init()");
    phy_dump_lan8720_registers();

    esp_eth_smi_write(BASIC_MODE_CONTROL_REG, SOFTWARE_RESET);

    while (esp_eth_smi_read(PHY_IDENTIFIER_REG) != OUI_MSB_21TO6_DEF) {
    }

    ets_delay_us(300);

    //if config.flow_ctrl_enable == true ,enable this
    phy_enable_flow_ctrl();
}

const eth_config_t lan8720_default_ethernet_phy_config = {
    .phy_addr = CONFIG_PHY_ID,
    .mac_mode = ETH_MODE_RMII,
    //Only FULLDUPLEX mode support flow ctrl now!
    .flow_ctrl_enable = true,
    .phy_init = phy_lan8720_init,
    .phy_check_init = phy_lan8720_check_phy_init,
    .phy_check_link = phy_lan8720_check_phy_link_status,
    .phy_get_speed_mode = phy_lan8720_get_speed_mode,
    .phy_get_duplex_mode = phy_lan8720_get_duplex_mode,
    .phy_get_partner_pause_enable = phy_lan8720_get_partner_pause_enable
};

void phy_dump_lan8720_registers()
{
    ESP_LOGD(TAG, "LAN8720 Registers:");
    ESP_LOGD(TAG, "BCR    0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGD(TAG, "BSR    0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGD(TAG, "PHY1   0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGD(TAG, "PHY2   0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGD(TAG, "ANAR   0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGD(TAG, "ANLPAR 0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGD(TAG, "ANER   0x%04x", esp_eth_smi_read(0x6));
    ESP_LOGD(TAG, "MCSR   0x%04x", esp_eth_smi_read(0x17));
    ESP_LOGD(TAG, "SM     0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGD(TAG, "SECR   0x%04x", esp_eth_smi_read(0x26));
    ESP_LOGD(TAG, "CSIR   0x%04x", esp_eth_smi_read(0x27));
    ESP_LOGD(TAG, "ISR    0x%04x", esp_eth_smi_read(0x29));
    ESP_LOGD(TAG, "IMR    0x%04x", esp_eth_smi_read(0x30));
    ESP_LOGD(TAG, "PSCSR  0x%04x", esp_eth_smi_read(0x31));
}
