// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "eth_phy/phy_tlk110.h"
#include "eth_phy/phy_reg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Value of MII_PHY_IDENTIFIER_REG for TI TLK110,
   Excluding bottom 4 bytes of ID2, used for model revision
 */
#define TLK110_PHY_ID1 0x2000
#define TLK110_PHY_ID2 0xa210
#define TLK110_PHY_ID2_MASK 0xFFF0

/* TLK110-specific registers */
#define SW_STRAP_CONTROL_REG            (0x9)
#define SW_STRAP_CONFIG_DONE               BIT(15)
#define AUTO_MDIX_ENABLE                   BIT(14)
#define AUTO_NEGOTIATION_ENABLE            BIT(13)
#define AN_1                               BIT(12)
#define AN_0                               BIT(11)
#define LED_CFG                            BIT(10)
#define RMII_ENHANCED_MODE                 BIT(9)

#define DEFAULT_STRAP_CONFIG (AUTO_MDIX_ENABLE|AUTO_NEGOTIATION_ENABLE|AN_1|AN_0|LED_CFG)

#define PHY_STATUS_REG                  (0x10)
#define AUTO_NEGOTIATION_STATUS             BIT(4)
#define DUPLEX_STATUS                      BIT(2)
#define SPEED_STATUS                       BIT(1)

#define CABLE_DIAGNOSTIC_CONTROL_REG    (0x1e)
#define DIAGNOSTIC_DONE                    BIT(1)

#define PHY_RESET_CONTROL_REG           (0x1f)
#define SOFTWARE_RESET                     BIT(15)

static const char *TAG = "tlk110";

void phy_tlk110_check_phy_init(void)
{
    phy_tlk110_dump_registers();

    esp_eth_smi_wait_set(MII_BASIC_MODE_STATUS_REG, MII_AUTO_NEGOTIATION_COMPLETE, 0);
    esp_eth_smi_wait_set(PHY_STATUS_REG, AUTO_NEGOTIATION_STATUS, 0);
    esp_eth_smi_wait_set(CABLE_DIAGNOSTIC_CONTROL_REG, DIAGNOSTIC_DONE, 0);
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
        return ETH_MODE_FULLDUPLEX;
    } else {
        ESP_LOGD(TAG, "phy_tlk110_get_duplex_mode(HALF)");
        return ETH_MODE_HALFDUPLEX;
    }
}

void phy_tlk110_power_enable(bool enable)
{
    if (enable) {
        esp_eth_smi_write(SW_STRAP_CONTROL_REG, DEFAULT_STRAP_CONFIG | SW_STRAP_CONFIG_DONE);

        // TODO: only do this if config.flow_ctrl_enable == true
        phy_mii_enable_flow_ctrl();
    }
}

void phy_tlk110_init(void)
{
    ESP_LOGD(TAG, "phy_tlk110_init()");
    phy_tlk110_dump_registers();

    esp_eth_smi_write(PHY_RESET_CONTROL_REG, SOFTWARE_RESET);

    esp_err_t res1, res2;
    do {
        // Call esp_eth_smi_wait_value() with a timeout so it prints an error periodically
        res1 = esp_eth_smi_wait_value(MII_PHY_IDENTIFIER_1_REG, TLK110_PHY_ID1, UINT16_MAX, 1000);
        res2 = esp_eth_smi_wait_value(MII_PHY_IDENTIFIER_2_REG, TLK110_PHY_ID2, TLK110_PHY_ID2_MASK, 1000);
    } while(res1 != ESP_OK || res2 != ESP_OK);

    esp_eth_smi_write(SW_STRAP_CONTROL_REG,
                      DEFAULT_STRAP_CONFIG | SW_STRAP_CONFIG_DONE);

    ets_delay_us(300);

    // TODO: only do this if config.flow_ctrl_enable == true
    phy_mii_enable_flow_ctrl();
}

const eth_config_t phy_tlk110_default_ethernet_config = {
    // PHY address configured by PHYADx pins. Default value of 0x1
    // is used if all pins are unconnected.
    .phy_addr = 0x1,
    .mac_mode = ETH_MODE_RMII,
    .clock_mode = ETH_CLOCK_GPIO0_IN,
    //Only FULLDUPLEX mode support flow ctrl now!
    .flow_ctrl_enable = true,
    .phy_init = phy_tlk110_init,
    .phy_check_init = phy_tlk110_check_phy_init,
    .phy_check_link = phy_mii_check_link_status,
    .phy_get_speed_mode = phy_tlk110_get_speed_mode,
    .phy_get_duplex_mode = phy_tlk110_get_duplex_mode,
    .phy_get_partner_pause_enable = phy_mii_get_partner_pause_enable,
    .phy_power_enable = phy_tlk110_power_enable,
};

void phy_tlk110_dump_registers()
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
