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
#include "soc/efuse_reg.h"

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
static inline bool esp_secure_boot_enabled(void) {
    return REG_READ(EFUSE_BLK0_RDATA6_REG) & EFUSE_RD_ABS_DONE_0;
}


/** @brief Enable secure boot if it is not already enabled.
 *
 * @important If this function succeeds, secure boot is permanently
 * enabled on the chip via efuse.
 *
 * @important This function is intended to be called from bootloader code only.
 *
 * If secure boot is not yet enabled for bootloader, this will
 * generate the secure boot digest and enable secure boot by blowing
 * the EFUSE_RD_ABS_DONE_0 efuse.
 *
 * This function does not verify secure boot of the bootloader (the
 * ROM bootloader does this.)
 *
 * Will fail if efuses have been part-burned in a way that indicates
 * secure boot should not or could not be correctly enabled.
 *
 *
 * @return ESP_ERR_INVALID_STATE if efuse state doesn't allow
 * secure boot to be enabled cleanly. ESP_OK if secure boot
 * is enabled on this chip from now on.
 */
esp_err_t esp_secure_boot_permanently_enable(void);

/** @brief Verify the secure boot signature (determinstic ECDSA w/ SHA256) appended to some binary data in flash.
 *
 * Public key is compiled into the calling program. See docs/security/secure-boot.rst for details.
 *
 * @param src_addr Starting offset of the data in flash.
 * @param length Length of data in bytes. Signature is appended -after- length bytes.
 *
 * If flash encryption is enabled, the image will be transparently decrypted while being verified.
 *
 * @return ESP_OK if signature is valid, ESP_ERR_INVALID_STATE if
 * signature fails, ESP_FAIL for other failures (ie can't read flash).
 */
esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length);

/** @brief Secure boot verification block, on-flash data format. */
typedef struct {
    uint32_t version;
    uint8_t signature[64];
} esp_secure_boot_sig_block_t;

#define FLASH_OFFS_SECURE_BOOT_IV_DIGEST 0

/** @brief Secure boot IV+digest header */
typedef struct {
    uint8_t iv[128];
    uint8_t digest[64];
} esp_secure_boot_iv_digest_t;

#endif
