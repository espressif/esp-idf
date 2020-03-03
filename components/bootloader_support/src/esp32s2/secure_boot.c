// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>

#include "esp_log.h"
#include "esp_secure_boot.h"
#include "soc/efuse_reg.h"

#include "bootloader_flash.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"

#include "esp_rom_crc.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"

#include "esp32s2/rom/secure_boot.h"

static const char *TAG = "secure_boot_v2";
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

#define SIG_BLOCK_MAGIC_BYTE 0xe7
#define CRC_SIGN_BLOCK_LEN 1196
#define SIG_BLOCK_PADDING 4096

#define DIGEST_LEN 32

/* A signature block is valid when it has correct magic byte, crc and image digest. */
static esp_err_t validate_signature_block(int block_num, const ets_secure_boot_signature_t *sig_block, uint8_t *digest)
{
    uint32_t crc = esp_rom_crc32_le(0, (uint8_t *)&sig_block->block[block_num], CRC_SIGN_BLOCK_LEN);
    if (sig_block->block[block_num].magic_byte != SIG_BLOCK_MAGIC_BYTE) {
        // All signature blocks have been parsed, no new signature block present.
        ESP_LOGD(TAG, "Signature block(%d) invalid/absent.", block_num);
        return ESP_FAIL;
    }
    if (sig_block->block[block_num].block_crc != crc) {
        ESP_LOGE(TAG, "Magic byte correct but incorrect crc.");
        return ESP_FAIL;
    }
    if (memcmp(digest, sig_block->block[block_num].image_digest, DIGEST_LEN)) {
        ESP_LOGE(TAG, "Magic byte & CRC correct but incorrect image digest.");
        return ESP_FAIL;
    } else {
        ESP_LOGD(TAG, "valid signature block(%d) found", block_num);
        return ESP_OK;
    }

    return ESP_FAIL;
}

// Inputs the flash_offset and length of an image(app or bootloader), validates & verifies its secure boot v2 signature.
// Generates the public key digests of the valid public keys in a signature block and writes it into trusted_keys.
// The key_digests in trusted keys whose signature blocks are invalid will be set to NULL.
static esp_err_t secure_boot_v2_digest_generate(uint32_t flash_offset, uint32_t flash_size, ets_secure_boot_key_digests_t * const trusted_keys)
{
    int i = 0;
    esp_err_t ret = ESP_FAIL;

    uint8_t image_digest[DIGEST_LEN] = {0};
    uint8_t public_key_digests[SECURE_BOOT_NUM_BLOCKS][DIGEST_LEN];
    size_t sig_block_addr = flash_offset + ALIGN_UP(flash_size, FLASH_SECTOR_SIZE);
    ret = bootloader_sha256_flash_contents(flash_offset, sig_block_addr - flash_offset, image_digest);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "error generating image digest, %d", ret);
        return ret;
    }

    ESP_LOGD(TAG, "reading signature block");
    const ets_secure_boot_signature_t *sig_block = bootloader_mmap(sig_block_addr, sizeof(ets_secure_boot_signature_t));
    if (sig_block == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", sig_block_addr, sizeof(ets_secure_boot_signature_t));
        return ESP_FAIL;
    }

    for (i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        ret = validate_signature_block(i, sig_block, image_digest);
        if (ret != ESP_OK) {
            break;
        }

        /* Generating the SHA of the public key components in the signature block */
        bootloader_sha256_handle_t sig_block_sha;
        sig_block_sha = bootloader_sha256_start();
        bootloader_sha256_data(sig_block_sha, &sig_block->block[i].key, sizeof(sig_block->block[i].key));
        bootloader_sha256_finish(sig_block_sha, public_key_digests[i]);

        memcpy((uint8_t *)trusted_keys->key_digests[0], public_key_digests[i], DIGEST_LEN); // Overwriting 0th index to verify each valid signature block
        /* A signature block is verified when it is valid and the signature in its signature block can be verified with a valid public key */
        uint8_t verified_digest[DIGEST_LEN] = {0};
        ets_secure_boot_status_t r = ets_secure_boot_verify_signature(sig_block, image_digest, trusted_keys, verified_digest);
        if (r != SB_SUCCESS) {
            ESP_LOGE(TAG, "Secure boot key (%d) verification failed.", i);
            ret = ESP_FAIL;
            goto exit;
        }
    }

    // At least 1 verified signature block found.
    if (i > 0) {
        // validate_signature_block returns ESP_FAIL when a sig block is absent, which isn't an error.
        while (--i) {
            trusted_keys->key_digests[i] = public_key_digests[i];
        }
        ret = ESP_OK;
    }

exit:
    /* Set the pointer to an invalid/absent block to NULL */
    while (i < SECURE_BOOT_NUM_BLOCKS) {
        trusted_keys->key_digests[i] = NULL;
        i++;
    }
    ESP_LOGI(TAG, "Secure boot verification success.");
    bootloader_munmap(sig_block);
    return ret;
}

/* Traverses ets_secure_boot_key_digests_t to find the number of non-null key_digests */
static uint8_t get_signature_block_count(ets_secure_boot_key_digests_t * const trusted_keys) {
    uint8_t bootloader_sig_block_count = 0;
    for (uint8_t i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        if (trusted_keys->key_digests[i] != NULL) {
            bootloader_sig_block_count++;

            for (uint8_t j = 0; j < DIGEST_LEN ; j++) {
                ESP_LOGD(TAG, "Secure Boot Digest %d: 0x%x", i, *(((uint8_t *)trusted_keys->key_digests[i]) + j));
            }
        }
    }
    return bootloader_sig_block_count;
}

esp_err_t esp_secure_boot_v2_permanently_enable(const esp_image_metadata_t *image_data)
{
    ESP_LOGI(TAG, "enabling secure boot v2 - ESP32-S2...");

    if (esp_secure_boot_enabled()) {
        ESP_LOGI(TAG, "secure boot v2 is already enabled, continuing..");
        return ESP_OK;
    }

    esp_err_t ret;
    /* Verify the bootloader */
    esp_image_metadata_t bootloader_data = { 0 };
    ret = esp_image_verify_bootloader_data(&bootloader_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "bootloader image appears invalid! error %d", ret);
        return ret;
    }

    /* Check if secure boot digests are present */
    bool has_secure_boot_digest = ets_efuse_find_purpose(ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0, NULL);
    has_secure_boot_digest |= ets_efuse_find_purpose(ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1, NULL);
    has_secure_boot_digest |= ets_efuse_find_purpose(ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2, NULL);
    ESP_LOGI(TAG, "Secure boot digests %s", has_secure_boot_digest ? "already present":"absent, generating..");


    ets_efuse_clear_program_registers();
    uint8_t i, j, bootloader_sig_block_count = 0;
    if (!has_secure_boot_digest) {
        ets_secure_boot_key_digests_t boot_trusted_keys, app_trusted_keys;
        uint8_t boot_trusted_key_data[SECURE_BOOT_NUM_BLOCKS][DIGEST_LEN] = {0}, app_trusted_key_data[SECURE_BOOT_NUM_BLOCKS][DIGEST_LEN] = {0};

        for(i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
            boot_trusted_keys.key_digests[i] = boot_trusted_key_data[i];
            app_trusted_keys.key_digests[i] = app_trusted_key_data[i];
        }

        /* Generate the bootloader public key digests */
        ret = secure_boot_v2_digest_generate(bootloader_data.start_addr, bootloader_data.image_len - SIG_BLOCK_PADDING, &boot_trusted_keys);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Public key digest generation failed.");
            return ret;
        }

        bootloader_sig_block_count = get_signature_block_count(&boot_trusted_keys);
        if (bootloader_sig_block_count <= 0) {
            ESP_LOGI(TAG, "No valid signature blocks found. %d signature block(s) found.", bootloader_sig_block_count);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "%d signature block(s) found appended to the bootloader.", bootloader_sig_block_count);

        int unused_key_slots = ets_efuse_count_unused_key_blocks();
        if (bootloader_sig_block_count > unused_key_slots) {
            ESP_LOGE(TAG, "Bootloader signatures(%d) more than available key slots(%d).", bootloader_sig_block_count, unused_key_slots);
            return ESP_FAIL;
        }

        for (i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
            if (boot_trusted_keys.key_digests[i] == NULL)  {
                break;
            }

            const uint32_t secure_boot_key_purpose[SECURE_BOOT_NUM_BLOCKS] = { ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0,
                                ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1, ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2 };

            ets_efuse_block_t block = ets_efuse_find_unused_key_block();
            if (block == ETS_EFUSE_BLOCK_MAX) {
                ESP_LOGE(TAG, "Key blocks not available.");
                return ESP_FAIL;
            }

            int r = ets_efuse_write_key(block, secure_boot_key_purpose[i], boot_trusted_keys.key_digests[i], DIGEST_LEN);
            if (r != 0) {
                ESP_LOGE(TAG, "Failed to write efuse block %d with purpose %d. Can't continue.", block, secure_boot_key_purpose[i]);
                return ESP_FAIL;
            }

            r = esp_efuse_set_write_protect(block);
            if (r != 0) {
                ESP_LOGE(TAG, "Failed to write protect efuse block %d. Can't continue.", block);
                return ESP_FAIL;
            }
        }

        /* Generate the application public key digests */
        ret = secure_boot_v2_digest_generate(image_data->start_addr, image_data->image_len - SIG_BLOCK_PADDING, &app_trusted_keys);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Application signature block is invalid.");
            return ret;
        }

        int app_sig_block_count = get_signature_block_count(&app_trusted_keys);
        ESP_LOGI(TAG, "%d signature block(s) found appended to the application.", app_sig_block_count);

        /* Confirm if atleast one the public key from the application matches a public key in the bootloader
        (Also, ensure if that public revoke bit is not set for the matched key) */
        bool match = false;
        const uint32_t revoke_bits[SECURE_BOOT_NUM_BLOCKS] = { EFUSE_SECURE_BOOT_KEY_REVOKE0,
                                EFUSE_SECURE_BOOT_KEY_REVOKE1, EFUSE_SECURE_BOOT_KEY_REVOKE2 };

        for (i = 0; i < SECURE_BOOT_NUM_BLOCKS && match == false; i++) {

            if (REG_GET_BIT(EFUSE_RD_REPEAT_DATA1_REG, revoke_bits[i])) {
                ESP_LOGI(TAG, "Key block(%d) has been revoked.", i);
                continue; // skip if the key block is revoked
            }

            for (j = 0; j < SECURE_BOOT_NUM_BLOCKS; j++) {
                if (!memcmp(boot_trusted_key_data[i], app_trusted_key_data[j], DIGEST_LEN)) {
                    ESP_LOGI(TAG, "Application key(%d) matches with bootloader key(%d).", j, i);
                    match = true;
                    break;
                }
            }
        }

        if (match == false) {
            ESP_LOGE(TAG, "No application key digest matches the bootloader key digest.");
            return ESP_FAIL;
        }

        /* Revoke the empty signature blocks */
        if (bootloader_sig_block_count < SECURE_BOOT_NUM_BLOCKS) {
            /* The revocation index can be 0, 1, 2. Bootloader count can be 1,2,3. */
            for (uint8_t i = bootloader_sig_block_count; i < SECURE_BOOT_NUM_BLOCKS; i++) {
                ESP_LOGI(TAG, "Revoking empty key digest slot (%d)...", i);
                ets_secure_boot_revoke_public_key_digest(i);
            }
        }
    }

    esp_err_t err = esp_efuse_batch_write_begin();
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Error batch programming security eFuses.");
        return err;
    }

    __attribute__((unused)) static const uint8_t enable = 1;

    esp_efuse_write_field_bit(ESP_EFUSE_DIS_BOOT_REMAP);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);

#ifdef CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    ESP_LOGI(TAG, "Enabling Security download mode...");
    esp_efuse_write_field_bit(ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD);
#else
    ESP_LOGW(TAG, "Not enabling Security download mode - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable hardware & software JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
    esp_efuse_write_field_bit(ESP_EFUSE_SOFT_DIS_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE);
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_EN);

    err = esp_efuse_batch_write_commit();
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Error programming security eFuses.");
        return err;
    }

#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    assert(ets_efuse_secure_boot_aggressive_revoke_enabled());
#endif

    assert(esp_rom_efuse_is_secure_boot_enabled());
    ESP_LOGI(TAG, "Secure boot permanently enabled");

    return ESP_OK;
}
