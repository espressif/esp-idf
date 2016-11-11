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
#ifndef _ESP_EFUSE_H
#define _ESP_EFUSE_H

#include "soc/efuse_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief Permanently update values written to the efuse write registers
 *
 * After updating EFUSE_BLKx_WDATAx_REG registers with new values to
 * write, call this function to permanently write them to efuse.
 *
 * @note Setting bits in efuse is permanent, they cannot be unset.
 *
 * @note Due to this restriction you don't need to copy values to
 * Efuse write registers from the matching read registers, bits which
 * are set in the read register but unset in the matching write
 * register will be unchanged when new values are burned.
 *
 * @note This function is not threadsafe, if calling code updates
 * efuse values from multiple tasks then this is caller's
 * responsibility to serialise.
 *
 * After burning new efuses, the read registers are updated to match
 * the new efuse values.
 */
void esp_efuse_burn_new_values(void);

/* @brief Reset efuse write registers
 *
 * Efuse write registers are written to zero, to negate
 * any changes that have been staged here.
 */
void esp_efuse_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_EFUSE_H */

