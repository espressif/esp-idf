
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "lan8720_phy.h"

extern char *demo_tag;


void phy_device_check_phy_init(void)
{
#ifdef CONFIG_PHY_DEBUG
    phy_dump_device_registers();
#endif

    while((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & AUTO_NEGOTIATION_COMPLETE ) != AUTO_NEGOTIATION_COMPLETE)
    {};
    while((esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & AUTO_NEGOTIATION_DONE ) != AUTO_NEGOTIATION_DONE)
    {};
}

eth_speed_mode_t phy_device_get_speed_mode(void)
{
    if(esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & SPEED_INDICATION_100T ) {
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
    if(esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & DUPLEX_INDICATION_FULL ) {
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

    esp_eth_smi_write(BASIC_MODE_CONTROL_REG, SOFTWARE_RESET);

    while (esp_eth_smi_read(PHY_IDENTIFIER_REG) != OUI_MSB_21TO6_DEF) {
    }

    ets_delay_us(300);

    //if config.flow_ctrl_enable == true ,enable this
    phy_enable_flow_ctrl();
}

void phy_dump_device_registers()
{
    ESP_LOGI(demo_tag, "LAN8720 Registers:");
    ESP_LOGI(demo_tag, "BCR    0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGI(demo_tag, "BSR    0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGI(demo_tag, "PHY1   0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGI(demo_tag, "PHY2   0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGI(demo_tag, "ANAR   0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGI(demo_tag, "ANLPAR 0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGI(demo_tag, "ANER   0x%04x", esp_eth_smi_read(0x6));
    ESP_LOGI(demo_tag, "MCSR   0x%04x", esp_eth_smi_read(0x17));
    ESP_LOGI(demo_tag, "SM     0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGI(demo_tag, "SECR   0x%04x", esp_eth_smi_read(0x26));
    ESP_LOGI(demo_tag, "CSIR   0x%04x", esp_eth_smi_read(0x27));
    ESP_LOGI(demo_tag, "ISR    0x%04x", esp_eth_smi_read(0x29));
    ESP_LOGI(demo_tag, "IMR    0x%04x", esp_eth_smi_read(0x30));
    ESP_LOGI(demo_tag, "PSCSR  0x%04x", esp_eth_smi_read(0x31));
}
