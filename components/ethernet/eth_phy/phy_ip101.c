// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "esp_log.h"
#include "esp_eth.h"
#include "eth_phy/phy_reg.h"
#include "eth_phy/phy_ip101.h"

#define IP101_PHY_ID1 0x243
#define IP101_PHY_ID2 0xc54
#define IP101_PHY_ID2_MASK 0xFFF0

#define PHY_STATUS_REG (0x1e)
#define DUPLEX_STATUS BIT(2)
#define SPEED_STATUS BIT(1)

static const char *TAG = "ip101";

void phy_ip101_check_phy_init(void)
{
    phy_ip101_dump_registers();
    esp_eth_smi_wait_set(MII_BASIC_MODE_STATUS_REG, MII_AUTO_NEGOTIATION_COMPLETE, 0);
}

eth_speed_mode_t phy_ip101_get_speed_mode(void)
{
    if ((esp_eth_smi_read(PHY_STATUS_REG) & SPEED_STATUS) == SPEED_STATUS) {
        ESP_LOGD(TAG, "phy_ip101_get_speed_mode(100)");
        return ETH_SPEED_MODE_100M;
    } else {
        ESP_LOGD(TAG, "phy_ip101_get_speed_mode(10)");
        return ETH_SPEED_MODE_10M;
    }
}

eth_duplex_mode_t phy_ip101_get_duplex_mode(void)
{
    if ((esp_eth_smi_read(PHY_STATUS_REG) & DUPLEX_STATUS) == DUPLEX_STATUS) {
        ESP_LOGD(TAG, "phy_ip101_get_duplex_mode(FULL)");
        return ETH_MODE_FULLDUPLEX;
    } else {
        ESP_LOGD(TAG, "phy_ip101_get_duplex_mode(HALF)");
        return ETH_MODE_HALFDUPLEX;
    }
}

void phy_ip101_power_enable(bool enable)
{
    if (enable) {
        uint32_t data = esp_eth_smi_read(MII_BASIC_MODE_CONTROL_REG);
        data |= MII_AUTO_NEGOTIATION_ENABLE | MII_RESTART_AUTO_NEGOTIATION;
        esp_eth_smi_write(MII_BASIC_MODE_CONTROL_REG, data);
        // TODO: only do this if config.flow_ctrl_enable == true
        phy_mii_enable_flow_ctrl();
    }
}

esp_err_t phy_ip101_init(void)
{
    esp_err_t res1, res2;
    ESP_LOGD(TAG, "phy_ip101_init()");
    phy_ip101_dump_registers();

    esp_eth_smi_write(MII_BASIC_MODE_CONTROL_REG, MII_SOFTWARE_RESET);

    do {
        // Call esp_eth_smi_wait_value() with a timeout so it prints an error periodically
        res1 = esp_eth_smi_wait_value(MII_PHY_IDENTIFIER_1_REG, IP101_PHY_ID1, UINT16_MAX, 1000);
        res2 = esp_eth_smi_wait_value(MII_PHY_IDENTIFIER_2_REG, IP101_PHY_ID2, IP101_PHY_ID2_MASK, 1000);
    } while (res1 != ESP_OK || res2 != ESP_OK);

    uint32_t data = esp_eth_smi_read(MII_BASIC_MODE_CONTROL_REG);
    data |= MII_AUTO_NEGOTIATION_ENABLE | MII_RESTART_AUTO_NEGOTIATION;
    esp_eth_smi_write(MII_BASIC_MODE_CONTROL_REG, data);

    ets_delay_us(300);
    // TODO: only do this if config.flow_ctrl_enable == true
    phy_mii_enable_flow_ctrl();
    if (res1 == ESP_OK && res2 == ESP_OK) {
        return ESP_OK;
    } else {
        return ESP_ERR_TIMEOUT;
    }
}

const eth_config_t phy_ip101_default_ethernet_config = {
    .phy_addr = 0x1,
    .mac_mode = ETH_MODE_RMII,
    .clock_mode = ETH_CLOCK_GPIO0_OUT,
    .flow_ctrl_enable = true,
    .phy_init = phy_ip101_init,
    .phy_check_init = phy_ip101_check_phy_init,
    .phy_check_link = phy_mii_check_link_status,
    .phy_get_speed_mode = phy_ip101_get_speed_mode,
    .phy_get_duplex_mode = phy_ip101_get_duplex_mode,
    .phy_get_partner_pause_enable = phy_mii_get_partner_pause_enable,
    .phy_power_enable = phy_ip101_power_enable,
    .reset_timeout_ms = 1000,
    .promiscuous_enable = false,
};

void phy_ip101_dump_registers()
{
    ESP_LOGD(TAG, "IP101 Registers:");
    ESP_LOGD(TAG, "BCR    0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGD(TAG, "BSR    0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGD(TAG, "PHY1   0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGD(TAG, "PHY2   0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGD(TAG, "ANAR   0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGD(TAG, "ANLPAR 0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGD(TAG, "ANER   0x%04x", esp_eth_smi_read(0x6));
    ESP_LOGD(TAG, "PSCR   0x%04x", esp_eth_smi_read(0x16));
    ESP_LOGD(TAG, "ISR    0x%04x", esp_eth_smi_read(0x17));
    ESP_LOGD(TAG, "ICR    0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGD(TAG, "CSSR   0x%04x", esp_eth_smi_read(0x30));
}
