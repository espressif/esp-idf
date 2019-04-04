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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_eth.h"

/**
 * @brief Common PHY-management functions.
 *
 * @note These are not enough to drive any particular Ethernet PHY.
 * They provide a common configuration structure and management functions.
 *
 */

/**
 * @brief Configure fixed pins for RMII data interface.
 *
 * @note This configures GPIOs 0, 19, 22, 25, 26, 27 for use with RMII data interface.
 * These pins cannot be changed, and must be wired to ethernet functions.
 * This is not sufficient to fully configure the Ethernet PHY.
 * MDIO configuration interface pins (such as SMI MDC, MDO, MDI) must also be configured correctly in the GPIO matrix.
 *
 */
void phy_rmii_configure_data_interface_pins(void);

/**
 * @brief Configure variable pins for SMI ethernet functions.
 *
 * @param mdc_gpio MDC GPIO Pin number
 * @param mdio_gpio MDIO GPIO Pin number
 *
 * @note Calling this function along with mii_configure_default_pins() will fully configure the GPIOs for the ethernet PHY.
 */
void phy_rmii_smi_configure_pins(uint8_t mdc_gpio, uint8_t mdio_gpio);

/**
 * @brief Enable flow control in standard PHY MII register.
 *
 */
void phy_mii_enable_flow_ctrl(void);

/**
 * @brief Check Ethernet link status via MII interface
 *
 * @return true Link is on
 * @return false Link is off
 */
bool phy_mii_check_link_status(void);

/**
 * @brief Check pause frame ability of partner via MII interface
 *
 * @return true Partner is able to process pause frame
 * @return false Partner can not process pause frame
 */
bool phy_mii_get_partner_pause_enable(void);

#ifdef __cplusplus
}
#endif
