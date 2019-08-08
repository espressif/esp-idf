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

#include <stdbool.h>
#include <esp_err.h>
#include "soc/efuse_periph.h"

#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/efuse.h"
#endif

#ifdef CONFIG_SECURE_BOOT_ENABLED
#if !defined(CONFIG_SECURE_SIGNED_ON_BOOT) || !defined(CONFIG_SECURE_SIGNED_ON_UPDATE) || !defined(CONFIG_SECURE_SIGNED_APPS)
#error "internal sdkconfig error, secure boot should always enable all signature options"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Support functions for secure boot features.

   Can be compiled as part of app or bootloader code.
*/

/** @brief Is secure boot currently enabled in hardware?
 *
 * This means that the ROM bootloader code will only boot
 * a verified secure bootloader from now on.
 *
 * @return true if secure boot is enabled.
 */
static inline bool esp_secure_boot_enabled(void)
{
#if CONFIG_IDF_TARGET_ESP32
    return REG_READ(EFUSE_BLK0_RDATA6_REG) & EFUSE_RD_ABS_DONE_0;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    return ets_efuse_secure_boot_enabled();
#endif
}

/** @brief Generate secure digest from bootloader image
 *
 * @important This function is intended to be called from bootloader code only.
 *
 * If secure boot is not yet enabled for bootloader, this will:
 *     1) generate the secure boot key and burn it on EFUSE
 *        (without enabling R/W protection)
 *     2) generate the digest from bootloader and save it
 *        to flash address 0x0
 *
 * If first boot gets interrupted after calling this function
 * but before esp_secure_boot_permanently_enable() is called, then
 * the key burned on EFUSE will not be regenerated, unless manually
 * done using espefuse.py tool
 *
 * @return ESP_OK if secure boot digest is generated
 * successfully or found to be already present
 */
esp_err_t esp_secure_boot_generate_digest(void);

/** @brief Enable secure boot if it is not already enabled.
 *
 * @important If this function succeeds, secure boot is permanently
 * enabled on the chip via efuse.
 *
 * @important This function is intended to be called from bootloader code only.
 *
 * @important This will enable r/w protection of secure boot key on EFUSE,
 * therefore it is to be ensured that esp_secure_boot_generate_digest()
 * is called before this
 *
 * If secure boot is not yet enabled for bootloader, this will
 *     1) enable R/W protection of secure boot key on EFUSE
 *     2) enable secure boot by blowing the EFUSE_RD_ABS_DONE_0 efuse.
 *
 * This function does not verify secure boot of the bootloader (the
 * ROM bootloader does this.)
 *
 * Will fail if efuses have been part-burned in a way that indicates
 * secure boot should not or could not be correctly enabled.
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

/** @brief Verify the secure boot signature block (deterministic ECDSA w/ SHA256) based on the SHA256 hash of some data.
 *
 * Similar to esp_secure_boot_verify_signature(), but can be used when the digest is precalculated.
 * @param sig_block Pointer to signature block data
 * @param image_digest Pointer to 32 byte buffer holding SHA-256 hash.
 *
 */

/** @brief Secure boot verification block, on-flash data format. */
typedef struct {
    uint32_t version;
    uint8_t signature[64];
} esp_secure_boot_sig_block_t;

esp_err_t esp_secure_boot_verify_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest);

#define FLASH_OFFS_SECURE_BOOT_IV_DIGEST 0

/** @brief Secure boot IV+digest header */
typedef struct {
    uint8_t iv[128];
    uint8_t digest[64];
} esp_secure_boot_iv_digest_t;

#ifdef __cplusplus
}
#endif
