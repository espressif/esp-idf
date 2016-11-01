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
#ifndef __ESP32_SECUREBOOT_H
#define __ESP32_SECUREBOOT_H

#include <stdbool.h>
#include <esp_err.h>

/* Support functions for secure boot features.

   Can be compiled as part of app or bootloader code.
*/

/** @brief Is secure boot currently enabled in hardware?
 *
 * Secure boot is enabled if the ABS_DONE_0 efuse is blown. This means
 * that the ROM bootloader code will only boot a verified secure
 * bootloader digest from now on.
 *
 * @return true if secure boot is enabled.
 */
bool esp_secure_boot_enabled(void);


/** @brief Enable secure boot if it isw not already enabled.
 *
 * @important If this function succeeds, secure boot is permanentl
 * enabled on the chip via efuse.
 *
 * This function is intended to be called from bootloader code.
 *
 * @return ESP_ERR_INVALID_STATE if efuse state doesn't allow
 * secure boot to be enabled cleanly. ESP_OK if secure boot
 * is enabled on this chip from now on.
 */
esp_err_t esp_secure_boot_enable(void);



#endif
