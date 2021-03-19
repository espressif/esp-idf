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
#include "sdkconfig.h"

#include <string.h>
#include "esp_fault.h"
#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"

// Secure boot V2 for bootloader.

#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_BOOT_V2_ENABLED

static const char* TAG = "secure_boot_v2";

#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[ESP_SECURE_BOOT_DIGEST_LEN] = {0};
    uint8_t verified_digest[ESP_SECURE_BOOT_DIGEST_LEN] = { 0 }; /* Note: this function doesn't do any anti-FI checks on this buffer */

    /* Rounding off length to the upper 4k boundary */
    uint32_t padded_length = ALIGN_UP(length, FLASH_SECTOR_SIZE);
    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

    /* Calculate digest of main image */
    esp_err_t err = bootloader_sha256_flash_contents(src_addr, padded_length, digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Digest calculation failed 0x%x, 0x%x", src_addr, padded_length);
        return err;
    }

    const ets_secure_boot_signature_t *sig_block = bootloader_mmap(src_addr + padded_length, sizeof(ets_secure_boot_signature_t));
    if (sig_block == NULL) {
        ESP_LOGE(TAG, "Failed to mmap data at offset 0x%x", src_addr + padded_length);
        return ESP_FAIL;
    }

    err = esp_secure_boot_verify_rsa_signature_block(sig_block, digest, verified_digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Secure Boot V2 verification failed.");
    }
    bootloader_munmap(sig_block);
    return err;
}

/* A signature block is valid when it has correct magic byte, crc. */
static esp_err_t validate_signature_block(const ets_secure_boot_sig_block_t *block)
{
    if (block->magic_byte != ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC
        || block->block_crc != esp_rom_crc32_le(0, (uint8_t *)block, CRC_SIGN_BLOCK_LEN)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t get_secure_boot_key_digests(esp_image_sig_public_key_digests_t *public_key_digests)
{
#if SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS > 1
    // Read key digests from efuse
    ets_secure_boot_key_digests_t trusted_keys;
    ets_secure_boot_key_digests_t trusted_key_copies[2];
    ETS_STATUS ets_ret;

    memset(&trusted_keys, 0, sizeof(ets_secure_boot_key_digests_t));
    memset(trusted_key_copies, 0, 2 * sizeof(ets_secure_boot_key_digests_t));


    ets_ret = ets_secure_boot_read_key_digests(&trusted_keys);

    // Create the copies for FI checks (assuming result is ETS_OK, if it's not then it'll fail the fault check anyhow)
    ets_secure_boot_read_key_digests(&trusted_key_copies[0]);
    ets_secure_boot_read_key_digests(&trusted_key_copies[1]);
    ESP_FAULT_ASSERT(memcmp(&trusted_keys, &trusted_key_copies[0], sizeof(ets_secure_boot_key_digests_t)) == 0);
    ESP_FAULT_ASSERT(memcmp(&trusted_keys, &trusted_key_copies[1], sizeof(ets_secure_boot_key_digests_t)) == 0);

    if (ets_ret == ETS_OK) {
        for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
            if (trusted_keys.key_digests[i] != NULL) {
                memcpy(public_key_digests->key_digests[i], (uint8_t *)trusted_keys.key_digests[i], ESP_SECURE_BOOT_DIGEST_LEN);
                public_key_digests->num_digests++;
            }
        }
        if (public_key_digests->num_digests > 0) {
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
#else
    bool all_zeroes = true;
    uint32_t *reg = (uint32_t*)&public_key_digests->key_digests[0];
    for (int i = 0; i < ESP_SECURE_BOOT_DIGEST_LEN / 4; i++) {
        *(reg + i) = REG_READ(EFUSE_BLK2_RDATA0_REG + i * 4);
        all_zeroes = all_zeroes && (*(reg + i) == 0);
    }
    if (all_zeroes) {
        return ESP_ERR_NOT_FOUND;
    }
    public_key_digests->num_digests = 1;
    return ESP_OK;
#endif // SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS
}

// if CONFIG_SECURE_BOOT_V2_ENABLED==y and key digests from eFuse are missing, then it is the first boot,
// trusted.key_digests are filled from app sig_block.
esp_err_t esp_secure_boot_verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    esp_image_sig_public_key_digests_t trusted = {0};
    bool efuse_keys_are_not_set = false;
    if (get_secure_boot_key_digests(&trusted) != ESP_OK) {
        if (esp_secure_boot_enabled()) {
            ESP_LOGE(TAG, "Could not read eFuse secure boot digests!");
            return ESP_FAIL;
        } else {
            ESP_LOGI(TAG, "Secure boot V2 is not enabled yet and eFuse digest keys are not set");
            efuse_keys_are_not_set = true;
            ESP_FAULT_ASSERT(!esp_secure_boot_enabled());
        }
    }

    if (!esp_secure_boot_enabled()) {
        // It is the first boot. eFuse secure boot bit is not set yet. eFuse block(s) can be written or not.
        // Generating the SHA of the public key components in the signature block
        for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
            if (validate_signature_block(&sig_block->block[i]) == ESP_OK) {
                if (efuse_keys_are_not_set) {
                    // if efuse key digests are not in eFuse yet due to it is the first boot
                    // then use digests from app to skip error in ets_secure_boot_verify_signature().
                    bootloader_sha256_handle_t sig_block_sha = bootloader_sha256_start();
                    bootloader_sha256_data(sig_block_sha, &sig_block->block[i].key, sizeof(sig_block->block[i].key));
                    bootloader_sha256_finish(sig_block_sha, trusted.key_digests[i]);
                }
            }
        }
        ESP_FAULT_ASSERT(!esp_secure_boot_enabled());
    }

    ESP_LOGI(TAG, "Verifying with RSA-PSS...");
#if SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS == 1
    int sb_result = ets_secure_boot_verify_signature(sig_block, image_digest, trusted.key_digests[0], verified_digest);
#else
    ets_secure_boot_key_digests_t trusted_key_digests;
    for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        trusted_key_digests.key_digests[i] = &trusted.key_digests[i];
    }
    int sb_result = ets_secure_boot_verify_signature(sig_block, image_digest, &trusted_key_digests, verified_digest);
#endif
    if (sb_result != SB_SUCCESS) {
        ESP_LOGE(TAG, "Secure Boot V2 verification failed.");
        return ESP_ERR_IMAGE_INVALID;
    } else {
        ESP_LOGI(TAG, "Signature verified successfully!");
        return ESP_OK;
    }
}
#endif // CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_BOOT_V2_ENABLED
