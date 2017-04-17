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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "phy.h"


/* @brief Dump all LAN8720 PHY SMI configuration registers
 *
 * @note These registers are dumped at 'debug' level, so output
 * may not be visible depending on default log levels.
 */
void phy_lan8720_dump_registers();

/* @brief Default LAN8720 phy_check_init function.
 */
void phy_lan8720_check_phy_init(void);

/* @brief Default LAN8720 phy_get_speed_mode function.
 */
eth_speed_mode_t phy_lan8720_get_speed_mode(void);

/* @brief Default LAN8720 phy_get_duplex_mode function.
 */
eth_duplex_mode_t phy_lan8720_get_duplex_mode(void);

/* @brief Default LAN8720 phy_power_enable function.
 *
 * @note This function may need to be replaced with a custom function
 * if the PHY has a GPIO to enable power or start a clock.
 *
 * Consult the ethernet example to see how this is done.
 */
void phy_lan8720_power_enable(bool);

/* @brief Default LAN8720 phy_init function.
 */
void phy_lan8720_init(void);

/* @brief Default LAN8720 PHY configuration
 *
 * This configuration is not suitable for use as-is, it will need
 * to be modified for your particular PHY hardware setup.
 *
 * Consult the Ethernet example to see how this is done.
 */
extern const eth_config_t phy_lan8720_default_ethernet_config;

#ifdef __cplusplus
}
#endif
