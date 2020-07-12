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

#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"

#include "esp32s2/rom/crc.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"

#include "esp32s2/rom/efuse.h"
#include "esp32s2/rom/secure_boot.h"

static const char *TAG = "secure_boot_v2";
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

#define SIG_BLOCK_MAGIC_BYTE 0xe7
#define CRC_SIGN_BLOCK_LEN 1196
#define SIG_BLOCK_PADDING 4096

#define DIGEST_LEN 32

/* A signature block is valid when it has correct magic byte, crc and image digest. */
static esp_err_t validate_signature_block(const ets_secure_boot_sig_block_t *block, int block_num, const uint8_t *image_digest)
{
    uint32_t crc = crc32_le(0, (uint8_t *)block, CRC_SIGN_BLOCK_LEN);
    if (block->magic_byte != SIG_BLOCK_MAGIC_BYTE) {
        // All signature blocks have been parsed, no new signature block present.
        ESP_LOGD(TAG, "Signature block(%d) invalid/absent.", block_num);
        return ESP_FAIL;
    }
    if (block->block_crc != crc) {
        ESP_LOGE(TAG, "Magic byte correct but incorrect crc.");
        return ESP_FAIL;
    }
    if (memcmp(image_digest, block->image_digest, DIGEST_LEN)) {
        ESP_LOGE(TAG, "Magic byte & CRC correct but incorrect image digest.");
        return ESP_FAIL;
    } else {
        ESP_LOGD(TAG, "valid signature block(%d) found", block_num);
        return ESP_OK;
    }

    return ESP_FAIL;
}

/* Structure to hold public key digests calculated from the signature blocks of a single image.

   Each image can have one or more signature blocks (up to SECURE_BOOT_NUM_BLOCKS). Each signature block
   includes a public key.

   Different to the ROM ets_secure_boot_key_digests_t structure which holds pointers to eFuse data with digests,
   in this data structure the digest data is included.
*/
typedef struct {
    uint8_t key_digests[SECURE_BOOT_NUM_BLOCKS][DIGEST_LEN];
    unsigned num_digests; /* Number of valid digests, starting at index 0 */
} image_sig_public_key_digests_t;

/* Generates the public key digests of the valid public keys in an image's
   signature block, verifies each signature, and stores the key digests in the
   public_key_digests structure.

   @param flash_offset Image offset in flash
   @param flash_size Image size in flash (not including signature block)
   @param[out] public_key_digests Pointer to structure to hold the key digests for valid sig blocks


   Note that this function doesn't read any eFuses, so it doesn't know if the
   keys are ultimately trusted by the hardware or not

   @return - ESP_OK if no signatures failed to verify, or if no valid signature blocks are found at all.
           - ESP_FAIL if there's a valid signature block that doesn't verify using the included public key (unexpected!)
*/
static esp_err_t s_calculate_image_public_key_digests(uint32_t flash_offset, uint32_t flash_size, image_sig_public_key_digests_t *public_key_digests)
{
    esp_err_t ret;
    uint8_t image_digest[DIGEST_LEN] = {0};
    uint8_t __attribute__((aligned(4))) key_digest[DIGEST_LEN] = {0};
    size_t sig_block_addr = flash_offset + ALIGN_UP(flash_size, FLASH_SECTOR_SIZE);

    ESP_LOGD(TAG, "calculating public key digests for sig blocks of image offset 0x%x (sig block offset 0x%x)", flash_offset, sig_block_addr);

    bzero(public_key_digests, sizeof(image_sig_public_key_digests_t));

    ret = bootloader_sha256_flash_contents(flash_offset, sig_block_addr - flash_offset, image_digest);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "error generating image digest, %d", ret);
        return ret;
    }

    ESP_LOGD(TAG, "reading signatures");
    const ets_secure_boot_signature_t *signatures = bootloader_mmap(sig_block_addr, sizeof(ets_secure_boot_signature_t));
    if (signatures == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", sig_block_addr, sizeof(ets_secure_boot_signature_t));
        return ESP_FAIL;
    }

    for (int i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        const ets_secure_boot_sig_block_t *block = &signatures->block[i];

        ret = validate_signature_block(block, i, image_digest);
        if (ret != ESP_OK) {
            ret = ESP_OK;  // past the last valid signature block
            break;
        }

        /* Generating the SHA of the public key components in the signature block */
        bootloader_sha256_handle_t sig_block_sha;
        sig_block_sha = bootloader_sha256_start();
        bootloader_sha256_data(sig_block_sha, &block->key, sizeof(block->key));
        bootloader_sha256_finish(sig_block_sha, key_digest);

        // Check we can verify the image using this signature and this key
        uint8_t temp_verified_digest[DIGEST_LEN];
        bool verified = ets_rsa_pss_verify(&block->key, block->signature, image_digest, temp_verified_digest);

        if (!verified) {
            /* We don't expect this: the signature blocks before we enable secure boot should all be verifiable or invalid,
               so this is a fatal error
            */
            ret = ESP_FAIL;
            ESP_LOGE(TAG, "Secure boot key (%d) verification failed.", i);
            break;
        }
        ESP_LOGD(TAG, "Signature block (%d) is verified", i);
        /* Copy the key digest to the buffer provided by the caller */
        memcpy((void *)public_key_digests->key_digests[i], key_digest, DIGEST_LEN);
        public_key_digests->num_digests++;
    }

    if (ret == ESP_OK && public_key_digests->num_digests > 0) {
        ESP_LOGI(TAG, "Digests successfully calculated, %d valid signatures (image offset 0x%x)",
                 public_key_digests->num_digests, flash_offset);
    }

    bootloader_munmap(signatures);
    return ret;
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
    if (!has_secure_boot_digest) {
        image_sig_public_key_digests_t boot_key_digests = {0};
        image_sig_public_key_digests_t app_key_digests = {0};

        /* Generate the bootloader public key digests */
        ret = s_calculate_image_public_key_digests(bootloader_data.start_addr, bootloader_data.image_len - SIG_BLOCK_PADDING, &boot_key_digests);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Bootloader signature block is invalid");
            return ret;
        }

        if (boot_key_digests.num_digests == 0) {
            ESP_LOGE(TAG, "No valid bootloader signature blocks found.");
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "%d signature block(s) found appended to the bootloader.", boot_key_digests.num_digests);

        int unused_key_slots = ets_efuse_count_unused_key_blocks();
        if (boot_key_digests.num_digests > unused_key_slots) {
            ESP_LOGE(TAG, "Bootloader signatures(%d) more than available key slots(%d).", boot_key_digests.num_digests, unused_key_slots);
            return ESP_FAIL;
        }

        for (int i = 0; i < boot_key_digests.num_digests; i++) {
            ets_efuse_block_t block;
            const uint32_t secure_boot_key_purpose[SECURE_BOOT_NUM_BLOCKS] = { ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0,
                                ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1, ETS_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2 };

            block = ets_efuse_find_unused_key_block();
            if (block == ETS_EFUSE_BLOCK_MAX) {
                ESP_LOGE(TAG, "No more unused key blocks available.");
                return ESP_FAIL;
            }

            int r = ets_efuse_write_key(block, secure_boot_key_purpose[i], boot_key_digests.key_digests[i], DIGEST_LEN);
            if (r != 0) {
                ESP_LOGE(TAG, "Failed to write efuse block %d with purpose %d. Can't continue.", block, secure_boot_key_purpose[i]);
                return ESP_FAIL;
            }

            // Note: write key will write protect both the block and the purpose eFuse, always
        }

        /* Generate the application public key digests */
        ret = s_calculate_image_public_key_digests(image_data->start_addr, image_data->image_len - SIG_BLOCK_PADDING, &app_key_digests);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "App signature block is invalid.");
            return ret;
        }

        if (app_key_digests.num_digests == 0) {
            ESP_LOGE(TAG, "No valid applications signature blocks found.");
            return ESP_FAIL;
        }

        ESP_LOGI(TAG, "%d signature block(s) found appended to the app.", app_key_digests.num_digests);
        if (app_key_digests.num_digests > boot_key_digests.num_digests) {
            ESP_LOGW(TAG, "App has %d signature blocks but bootloader only has %d. Some keys missing from bootloader?");
        }

        /* Confirm if at least one public key from the application matches a public key in the bootloader
           (Also, ensure if that public revoke bit is not set for the matched key) */
        bool match = false;
        const uint32_t revoke_bits[SECURE_BOOT_NUM_BLOCKS] = { EFUSE_SECURE_BOOT_KEY_REVOKE0,
                                EFUSE_SECURE_BOOT_KEY_REVOKE1, EFUSE_SECURE_BOOT_KEY_REVOKE2 };

        for (int i = 0; i < boot_key_digests.num_digests; i++) {

            if (REG_GET_BIT(EFUSE_RD_REPEAT_DATA1_REG, revoke_bits[i])) {
                ESP_LOGI(TAG, "Key block(%d) has been revoked.", i);
                continue; // skip if the key block is revoked
            }

            for (int j = 0; j < app_key_digests.num_digests; j++) {
                if (!memcmp(boot_key_digests.key_digests[i], app_key_digests.key_digests[j], DIGEST_LEN)) {
                    ESP_LOGI(TAG, "Application key(%d) matches with bootloader key(%d).", j, i);
                    match = true;
                }
            }
        }

        if (match == false) {
            ESP_LOGE(TAG, "No application key digest matches the bootloader key digest.");
            return ESP_FAIL;
        }

        /* Revoke the empty signature blocks */
        if (boot_key_digests.num_digests < SECURE_BOOT_NUM_BLOCKS) {
            /* The revocation index can be 0, 1, 2. Bootloader count can be 1,2,3. */
            for (uint8_t i = boot_key_digests.num_digests; i < SECURE_BOOT_NUM_BLOCKS; i++) {
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

    esp_err_t err = ESP_FAIL;
#ifdef CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    ESP_LOGI(TAG, "Enabling Security download mode...");
    err = esp_efuse_enable_rom_secure_download_mode();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not enable Security download mode...");
        return err;
    }
#elif CONFIG_SECURE_DISABLE_ROM_DL_MODE
    ESP_LOGI(TAG, "Disable ROM Download mode...");
    err = esp_efuse_disable_rom_download_mode();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not disable ROM Download mode...");
        return err;
    }
#else
    ESP_LOGW(TAG, "UART ROM Download mode kept enabled - SECURITY COMPROMISED");
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

#ifndef CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS
    bool rd_dis_now = true;
#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    /* If flash encryption is not enabled yet then don't read-disable efuses yet, do it later in the boot
       when Flash Encryption is being enabled */
    rd_dis_now = esp_flash_encryption_enabled();
#endif
    if (rd_dis_now) {
        ESP_LOGI(TAG, "Prevent read disabling of additional efuses...");
        esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
    }
#else
    ESP_LOGW(TAG, "Allowing read disabling of additional efuses - SECURITY COMPROMISED");
#endif

    err = esp_efuse_batch_write_commit();
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Error programming security eFuses.");
        return err;
    }

#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    assert(ets_efuse_secure_boot_aggressive_revoke_enabled());
#endif

    assert(ets_efuse_secure_boot_enabled());
    ESP_LOGI(TAG, "Secure boot permanently enabled");

    return ESP_OK;
}
