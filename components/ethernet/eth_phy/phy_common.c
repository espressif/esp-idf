// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "eth_phy/phy.h"
#include "eth_phy/phy_reg.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "phy_common";

void phy_rmii_configure_data_interface_pins(void)
{
    // CRS_DRV to GPIO27
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO27_U, FUNC_GPIO27_EMAC_RX_DV);

    // TXD0 to GPIO19
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO19_U, FUNC_GPIO19_EMAC_TXD0);
    // TX_EN to GPIO21
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO21_U, FUNC_GPIO21_EMAC_TX_EN);
    // TXD1 to GPIO22
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO22_U, FUNC_GPIO22_EMAC_TXD1);
    // RXD0 to GPIO25
    gpio_set_direction(25, GPIO_MODE_INPUT);
    // RXD1 to GPIO26
    gpio_set_direction(26, GPIO_MODE_INPUT);
    // RMII CLK to GPIO0
    gpio_set_direction(0, GPIO_MODE_INPUT);
}

void phy_rmii_smi_configure_pins(uint8_t mdc_gpio, uint8_t mdio_gpio)
{
    gpio_matrix_out(mdc_gpio, EMAC_MDC_O_IDX, 0, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[mdc_gpio], PIN_FUNC_GPIO);
    gpio_matrix_out(mdio_gpio, EMAC_MDO_O_IDX, 0, 0);
    gpio_matrix_in(mdio_gpio, EMAC_MDI_I_IDX, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[mdio_gpio], PIN_FUNC_GPIO);
}

void phy_mii_enable_flow_ctrl(void)
{
    uint32_t data = esp_eth_smi_read(MII_AUTO_NEG_ADVERTISEMENT_REG);
    data |= MII_ASM_DIR | MII_PAUSE;
    esp_eth_smi_write(MII_AUTO_NEG_ADVERTISEMENT_REG, data);
}

bool phy_mii_check_link_status(void)
{
    if ((esp_eth_smi_read(MII_BASIC_MODE_STATUS_REG) & MII_LINK_STATUS)) {
        ESP_LOGD(TAG, "phy_mii_check_link_status(UP)");
        return true;
    } else {
        ESP_LOGD(TAG, "phy_mii_check_link_status(DOWN)");
        return false;
    }
}

bool phy_mii_get_partner_pause_enable(void)
{
    if((esp_eth_smi_read(MII_PHY_LINK_PARTNER_ABILITY_REG) & MII_PARTNER_PAUSE)) {
        ESP_LOGD(TAG, "phy_mii_get_partner_pause_enable(TRUE)");
        return true;
    } else {
        ESP_LOGD(TAG, "phy_mii_get_partner_pause_enable(FALSE)");
        return false;
    }
}
