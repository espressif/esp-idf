/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_secure_boot.h"
#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"
#include "esp_image_format.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "secure_boot_signature_priv.h"


/* The following API implementations are used only when called
 * from the bootloader code.
 */

#ifdef CONFIG_SECURE_BOOT_V2_ENABLED

#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
static const char *TAG = "secure_boot_v2";

/* A signature block is valid when it has correct magic byte, crc and image digest. */
static esp_err_t validate_signature_block(const ets_secure_boot_sig_block_t *block, int block_num, const uint8_t *image_digest)
{
    if (block->magic_byte != ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC) {
        // All signature blocks have been parsed, no new signature block present.
        ESP_LOGD(TAG, "Signature block(%d) invalid/absent.", block_num);
        return ESP_FAIL;
    }
    if (block->block_crc != esp_rom_crc32_le(0, (uint8_t *)block, CRC_SIGN_BLOCK_LEN)) {
        ESP_LOGE(TAG, "Magic byte correct but incorrect crc.");
        return ESP_FAIL;
    }
    if (memcmp(image_digest, block->image_digest, ESP_SECURE_BOOT_DIGEST_LEN)) {
        ESP_LOGE(TAG, "Magic byte & CRC correct but incorrect image digest.");
        return ESP_FAIL;
    } else {
        ESP_LOGD(TAG, "valid signature block(%d) found", block_num);
        return ESP_OK;
    }
    return ESP_FAIL;
}

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
static esp_err_t s_calculate_image_public_key_digests(uint32_t flash_offset, uint32_t flash_size, esp_image_sig_public_key_digests_t *public_key_digests)
{
    esp_err_t ret;
    uint8_t image_digest[ESP_SECURE_BOOT_DIGEST_LEN] = {0};
    uint8_t __attribute__((aligned(4))) key_digest[ESP_SECURE_BOOT_DIGEST_LEN] = {0};
    size_t sig_block_addr = flash_offset + ALIGN_UP(flash_size, FLASH_SECTOR_SIZE);

    ESP_LOGD(TAG, "calculating public key digests for sig blocks of image offset 0x%" PRIx32 " (sig block offset 0x%x)", flash_offset, sig_block_addr);

    bzero(public_key_digests, sizeof(esp_image_sig_public_key_digests_t));

    ret = bootloader_sha256_flash_contents(flash_offset, sig_block_addr - flash_offset, image_digest);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "error generating image digest, %d", ret);
        return ret;
    }

    ESP_LOGD(TAG, "reading signature(s)");
    const ets_secure_boot_signature_t *signatures = bootloader_mmap(sig_block_addr, sizeof(ets_secure_boot_signature_t));
    if (signatures == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", sig_block_addr, sizeof(ets_secure_boot_signature_t));
        return ESP_FAIL;
    }

    /* Validating Signature block */
    for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        const ets_secure_boot_sig_block_t *block = &signatures->block[i];

        ret = validate_signature_block(block, i, image_digest);
        if (ret != ESP_OK) {
            ret = ESP_OK;  // past the last valid signature block
            break;
        }

        /* Generating the SHA of the public key components in the signature block */
        bootloader_sha256_handle_t sig_block_sha;
        sig_block_sha = bootloader_sha256_start();
#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
        bootloader_sha256_data(sig_block_sha, &block->key, sizeof(block->key));
#elif CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
        bootloader_sha256_data(sig_block_sha, &block->ecdsa.key, sizeof(block->ecdsa.key));
#endif
        bootloader_sha256_finish(sig_block_sha, key_digest);

        // Check we can verify the image using this signature and this key
        uint8_t temp_verified_digest[ESP_SECURE_BOOT_DIGEST_LEN];
#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
        bool verified = ets_rsa_pss_verify(&block->key, block->signature, image_digest, temp_verified_digest);
#elif CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
        bool verified = ets_ecdsa_verify(&block->ecdsa.key.point[0], block->ecdsa.signature, block->ecdsa.key.curve_id, image_digest, temp_verified_digest);
#endif

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
        memcpy((void *)public_key_digests->key_digests[i], key_digest, ESP_SECURE_BOOT_DIGEST_LEN);
        public_key_digests->num_digests++;
    }

    if (ret == ESP_OK && public_key_digests->num_digests > 0) {
        ESP_LOGI(TAG, "Digests successfully calculated, %d valid signatures (image offset 0x%" PRIx32 ")",
                 public_key_digests->num_digests, flash_offset);
    }

    bootloader_munmap(signatures);
    return ret;
}

static esp_err_t check_and_generate_secure_boot_keys(const esp_image_metadata_t *image_data)
{
    esp_err_t ret;
#ifdef CONFIG_IDF_TARGET_ESP32
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK_SECURE_BOOT);
    if (coding_scheme != EFUSE_CODING_SCHEME_NONE) {
        ESP_LOGE(TAG, "No coding schemes are supported in secure boot v2.(Detected scheme: 0x%x)", coding_scheme);
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif // CONFIG_IDF_TARGET_ESP32

    esp_efuse_purpose_t secure_boot_key_purpose[SECURE_BOOT_NUM_BLOCKS] = {
#if SECURE_BOOT_NUM_BLOCKS == 1
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2,
#else
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2,
#endif
    };

    /* Verify the bootloader */
    esp_image_metadata_t bootloader_data = { 0 };
    ret = esp_image_verify_bootloader_data(&bootloader_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "bootloader image appears invalid! error %d", ret);
        return ret;
    }

    /* Initialize all efuse block entries to invalid (max) value */
    esp_efuse_block_t blocks[SECURE_BOOT_NUM_BLOCKS] = {[0 ... SECURE_BOOT_NUM_BLOCKS-1] = EFUSE_BLK_KEY_MAX};
    /* Check if secure boot digests are present */
    bool has_secure_boot_digest = false;
    for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        bool tmp_has_key = esp_efuse_find_purpose(secure_boot_key_purpose[i], &blocks[i]);
        if (tmp_has_key) { // For ESP32: esp_efuse_find_purpose() always returns True, need to check whether the key block is used or not.
            tmp_has_key &= !esp_efuse_key_block_unused(blocks[i]);
        }
        has_secure_boot_digest |= tmp_has_key;
    }

    esp_image_sig_public_key_digests_t boot_key_digests = {0};
    esp_image_sig_public_key_digests_t app_key_digests = {0};
    ESP_LOGI(TAG, "Secure boot digests %s", has_secure_boot_digest ? "already present":"absent, generating..");

    if (!has_secure_boot_digest) {
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

        ESP_LOGI(TAG, "Burning public key hash to eFuse");
        ret = esp_efuse_write_keys(secure_boot_key_purpose, boot_key_digests.key_digests, boot_key_digests.num_digests);
        if (ret != ESP_OK) {
            if (ret == ESP_ERR_NOT_ENOUGH_UNUSED_KEY_BLOCKS) {
                ESP_LOGE(TAG, "Bootloader signatures(%d) more than available key slots.", boot_key_digests.num_digests);
            } else {
                ESP_LOGE(TAG, "Failed to write efuse block with purpose (err=0x%x). Can't continue.", ret);
            }
            return ret;
        }
    } else {
        for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
            /* Check if corresponding digest slot is used or not */
            if (blocks[i] == EFUSE_BLK_KEY_MAX) {
                ESP_LOGD(TAG, "SECURE_BOOT_DIGEST%d slot is not used", i);
                continue;
            }

#if SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS
            if (esp_efuse_get_digest_revoke(i)) {
                continue;
            }
#endif
#ifndef CONFIG_SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK
            if (esp_efuse_get_key_dis_read(blocks[i])) {
                ESP_LOGE(TAG, "Key digest (BLK%d) read protected, aborting...", blocks[i]);
                return ESP_FAIL;
            }
#endif
            if (esp_efuse_block_is_empty(blocks[i])) {
                ESP_LOGE(TAG, "%d eFuse block is empty, aborting...", blocks[i]);
                return ESP_FAIL;
            }
            esp_efuse_set_key_dis_write(blocks[i]);
#ifdef CONFIG_SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK
            size_t offset = 128;
#else
            size_t offset = 0;
#endif
             ret = esp_efuse_read_block(blocks[i], boot_key_digests.key_digests[boot_key_digests.num_digests], offset,
                                            ESP_SECURE_BOOT_KEY_DIGEST_LEN * 8);
            if (ret) {
                ESP_LOGE(TAG, "Error during reading %d eFuse block (err=0x%x)", blocks[i], ret);
                return ret;
            }
            boot_key_digests.num_digests++;
        }
        if (boot_key_digests.num_digests == 0) {
            ESP_LOGE(TAG, "No valid pre-loaded public key digest in eFuse");
            return ESP_FAIL;
        }
        ESP_LOGW(TAG, "Using pre-loaded public key digest in eFuse");
    }

    /* Generate the application public key digests */
    ret = s_calculate_image_public_key_digests(image_data->start_addr, image_data->image_len - SIG_BLOCK_PADDING, &app_key_digests);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Application signature block is invalid.");
        return ret;
    }

    if (app_key_digests.num_digests == 0) {
        ESP_LOGE(TAG, "No valid applications signature blocks found.");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "%d signature block(s) found appended to the app.", app_key_digests.num_digests);
    if (app_key_digests.num_digests > boot_key_digests.num_digests) {
        ESP_LOGW(TAG, "App has %d signature blocks but bootloader only has %d. Some keys missing from bootloader?", app_key_digests.num_digests, boot_key_digests.num_digests);
    }

    /* Confirm if at least one public key from the application matches a public key in the bootloader
        (Also, ensure if that public revoke bit is not set for the matched key) */
    bool match = false;

    for (unsigned i = 0; i < boot_key_digests.num_digests; i++) {
#if SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS
        if (esp_efuse_get_digest_revoke(i)) {
            ESP_LOGI(TAG, "Key block(%d) has been revoked.", i);
            continue; // skip if the key block is revoked
        }
#endif // SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS
        for (unsigned j = 0; j < app_key_digests.num_digests; j++) {
            if (!memcmp(boot_key_digests.key_digests[i], app_key_digests.key_digests[j], ESP_SECURE_BOOT_KEY_DIGEST_LEN)) {
                ESP_LOGI(TAG, "Application key(%d) matches with bootloader key(%d).", j, i);
                match = true;
            }
        }
    }

    if (match == false) {
        ESP_LOGE(TAG, "No application key digest matches the bootloader key digest.");
        return ESP_FAIL;
    }

#if SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS
    /* Revoke the empty signature blocks */
    if (boot_key_digests.num_digests < SECURE_BOOT_NUM_BLOCKS) {
        /* The revocation index can be 0, 1, 2. Bootloader count can be 1,2,3. */
        for (unsigned i = boot_key_digests.num_digests; i < SECURE_BOOT_NUM_BLOCKS; i++) {
            ESP_LOGI(TAG, "Revoking empty key digest slot (%d)...", i);
            esp_efuse_set_digest_revoke(i);
        }
    }
#endif // SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS
    return ESP_OK;
}

esp_err_t esp_secure_boot_v2_permanently_enable(const esp_image_metadata_t *image_data)
{
    ESP_LOGI(TAG, "enabling secure boot v2...");

    if (esp_secure_boot_enabled()) {
        ESP_LOGI(TAG, "secure boot v2 is already enabled, continuing..");
        return ESP_OK;
    }

    esp_efuse_batch_write_begin(); /* Batch all efuse writes at the end of this function */

    esp_err_t err;
    err = check_and_generate_secure_boot_keys(image_data);
    if (err != ESP_OK) {
        esp_efuse_batch_write_cancel();
        return err;
    }

    ESP_LOGI(TAG, "blowing secure boot efuse...");
    err = esp_secure_boot_enable_secure_features();
    if (err != ESP_OK) {
        esp_efuse_batch_write_cancel();
        return err;
    }

    err = esp_efuse_batch_write_commit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error programming security eFuses (err=0x%x).", err);
        return err;
    }

#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    assert(esp_efuse_read_field_bit(ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE));
#endif

#ifndef CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER
    assert(esp_secure_boot_enabled());
    ESP_LOGI(TAG, "Secure boot permanently enabled");
#endif

    return ESP_OK;
}

#endif // CONFIG_SECURE_BOOT_V2_ENABLED
