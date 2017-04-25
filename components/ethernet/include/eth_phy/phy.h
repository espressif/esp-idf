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

/* Common PHY-management functions.

   These are not enough to drive any particular Ethernet PHY, but they provide a common configuration structure and
   management functions.
*/

/* Configure fixed pins for RMII data interface.

   This configures GPIOs 0, 19, 22, 25, 26, 27 for use with RMII
   data interface. These pins cannot be changed, and must be wired to
   ethernet functions.

   This is not sufficient to fully configure the Ethernet PHY,
   MDIO configuration interface pins (such as SMI MDC, MDO, MDI)
   must also be configured correctly in the GPIO matrix.
*/
void phy_rmii_configure_data_interface_pins(void);

/* Configure variable pins for SMI (MDIO) ethernet functions.

   Calling this function along with mii_configure_default_pins() will
   fully configure the GPIOs for the ethernet PHY.
 */
void phy_rmii_smi_configure_pins(uint8_t mdc_gpio, uint8_t mdio_gpio);


/* Enable flow control in standard PHY MII register.
 */
void phy_mii_enable_flow_ctrl(void);

bool phy_mii_check_link_status(void);

bool phy_mii_get_partner_pause_enable(void);

#ifdef __cplusplus
}
#endif
