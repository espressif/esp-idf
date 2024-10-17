/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"
#include "bootloader_signature.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "mbedtls/sha256.h"
#include "mbedtls/x509.h"
#include "mbedtls/md.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#include <sys/param.h>
#include "esp_secure_boot.h"
#include "esp_ota_ops.h"
#include "esp_efuse.h"
#include "esp_efuse_chip.h"

#include "secure_boot_signature_priv.h"

// Secure boot V2 for app

_Static_assert(SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS == SECURE_BOOT_NUM_BLOCKS,
               "Parts of this code rely on the max number of signatures appended to an image"
               "being the same as the max number of trusted keys.");

#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME || CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

static const char *TAG = "secure_boot_v2";
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

/* A signature block is valid when it has correct magic byte, crc. */
static esp_err_t validate_signature_block(const ets_secure_boot_sig_block_t *block)
{
    if (block->magic_byte != ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC
        || block->block_crc != esp_rom_crc32_le(0, (uint8_t *)block, CRC_SIGN_BLOCK_LEN)) {
        return ESP_FAIL;
    }
    if (block->version != ESP_SECURE_BOOT_SCHEME) {
        ESP_LOGE(TAG, "%s signing scheme selected but signature block generated for %s scheme", esp_secure_boot_get_scheme_name(ESP_SECURE_BOOT_SCHEME), esp_secure_boot_get_scheme_name(block->version));
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t calculate_image_public_key_digests(bool verify_image_digest, bool digest_public_keys, esp_image_sig_public_key_digests_t *public_key_digests, esp_partition_pos_t *part_pos)
{
    esp_image_metadata_t img_metadata = {0};
    esp_err_t ret = esp_image_get_metadata(part_pos, &img_metadata);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error reading metadata from running app (err=0x%x)", ret);
        return ESP_FAIL;
    }

    uint8_t image_digest[ESP_SECURE_BOOT_DIGEST_LEN] = {0};
    uint8_t __attribute__((aligned(4))) key_digest[ESP_SECURE_BOOT_DIGEST_LEN] = {0};
    size_t sig_block_addr = img_metadata.start_addr + ALIGN_UP(img_metadata.image_len, FLASH_SECTOR_SIZE);

    ESP_LOGD(TAG, "calculating public key digests for sig blocks of image offset 0x%"PRIu32" (sig block offset 0x%u)", img_metadata.start_addr, sig_block_addr);

    bzero(public_key_digests, sizeof(esp_image_sig_public_key_digests_t));

    if (verify_image_digest) {
        ret = bootloader_sha256_flash_contents(img_metadata.start_addr, sig_block_addr - img_metadata.start_addr, image_digest);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "error generating image digest, %d", ret);
            return ret;
        }
    }

    ESP_LOGD(TAG, "reading signature(s)");
    for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        ets_secure_boot_sig_block_t block;
        size_t addr = sig_block_addr + sizeof(ets_secure_boot_sig_block_t) * i;
        esp_err_t err = bootloader_flash_read(addr, &block, sizeof(ets_secure_boot_sig_block_t), true);
        if (err == ESP_OK) {
            if (validate_signature_block(&block) == ESP_OK) {
                if (digest_public_keys) {
                    bootloader_sha256_handle_t sig_block_sha = bootloader_sha256_start();
#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
                    bootloader_sha256_data(sig_block_sha, &block.key, sizeof(block.key));
#elif CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
                    bootloader_sha256_data(sig_block_sha, &block.ecdsa.key, sizeof(block.ecdsa.key));
#endif
                    bootloader_sha256_finish(sig_block_sha, key_digest);
                    if (verify_image_digest) {
                        // Check we can verify the image using this signature and this key
                        uint8_t temp_verified_digest[ESP_SECURE_BOOT_DIGEST_LEN];
    #if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
                        bool verified = ets_rsa_pss_verify(&block.key, block.signature, image_digest, temp_verified_digest);
    #elif CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
                        bool verified = ets_ecdsa_verify(&block.ecdsa.key.point[0], block.ecdsa.signature, block.ecdsa.key.curve_id, image_digest, temp_verified_digest);
    #endif
                        if (!verified) {
                            ESP_LOGE(TAG, "Secure boot key (%d) verification failed.", i);
                            continue;
                        }
                        ESP_LOGD(TAG, "Signature block (%d) is verified", i);
                    }
                    /* Copy the key digest to the buffer provided by the caller */
                    memcpy((void *)public_key_digests->key_digests[public_key_digests->num_digests], key_digest, ESP_SECURE_BOOT_DIGEST_LEN);
                }
                public_key_digests->num_digests++;
            }
        } else {
            ESP_LOGE(TAG, "Secure boot sign blocks cannot be read from image at %lx (err=0x%x)", part_pos->offset, err);
            return ESP_FAIL;
        }
    }

    if (ret == ESP_OK && public_key_digests->num_digests > 0) {
        ESP_LOGD(TAG, "Digests successfully calculated, %d valid signatures (image offset 0x%"PRIu32")",
                 public_key_digests->num_digests, img_metadata.start_addr);
    }

    if (public_key_digests->num_digests == 0) {
        return ESP_ERR_NOT_FOUND;
    }
    return ret;
}

esp_err_t esp_secure_boot_get_signature_blocks_for_running_app(bool digest_public_keys, esp_image_sig_public_key_digests_t *public_key_digests)
{
    esp_partition_pos_t part_pos;
    const esp_partition_t* running_app_part = esp_ota_get_running_partition();
    if (running_app_part == NULL) {
        ESP_LOGE(TAG, "Cannot get running partition");
        return ESP_FAIL;
    }
    part_pos.offset = running_app_part->address;
    part_pos.size   = running_app_part->size;

    esp_err_t err = calculate_image_public_key_digests(false, digest_public_keys, public_key_digests, &part_pos);
    if (public_key_digests->num_digests == 0) {
        ESP_LOGE(TAG, "No signatures were found for the running app");
    }
    return err;
}

static esp_err_t get_secure_boot_key_digests(esp_image_sig_public_key_digests_t *public_key_digests)
{
    if (!esp_secure_boot_enabled()) { // CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT
        // Gets key digests from running app
        ESP_LOGI(TAG, "Take trusted digest key(s) from running app");
        return esp_secure_boot_get_signature_blocks_for_running_app(true, public_key_digests);
     } else { // CONFIG_SECURE_BOOT_V2_ENABLED
        ESP_LOGI(TAG, "Take trusted digest key(s) from eFuse block(s)");
        // Read key digests from efuse
        esp_secure_boot_key_digests_t efuse_trusted;
        if (esp_secure_boot_read_key_digests(&efuse_trusted) == ESP_OK) {
            for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
                if (efuse_trusted.key_digests[i] != NULL) {
                    memcpy(public_key_digests->key_digests[i], (uint8_t *)efuse_trusted.key_digests[i], ESP_SECURE_BOOT_KEY_DIGEST_LEN);
                    public_key_digests->num_digests++;
                }
            }
        }
        if (public_key_digests->num_digests > 0) {
            return ESP_OK;
        }
     }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[ESP_SECURE_BOOT_KEY_DIGEST_LEN] = {0};
    uint8_t verified_digest[ESP_SECURE_BOOT_KEY_DIGEST_LEN] = {0};

    /* Rounding off length to the upper 4k boundary */
    uint32_t padded_length = ALIGN_UP(length, FLASH_SECTOR_SIZE);
    ESP_LOGD(TAG, "verifying signature src_addr 0x%"PRIx32" length 0x%"PRIx32, src_addr, length);

    esp_err_t err = bootloader_sha256_flash_contents(src_addr, padded_length, digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Digest calculation failed 0x%"PRIx32", 0x%"PRIx32, src_addr, padded_length);
        return err;
    }

    const ets_secure_boot_signature_t *sig_block = bootloader_mmap(src_addr + padded_length, sizeof(ets_secure_boot_signature_t));
    if (sig_block == NULL) {
        ESP_LOGE(TAG, "Failed to mmap data at offset 0x%"PRIx32, src_addr + padded_length);
        return ESP_FAIL;
    }

    err = esp_secure_boot_verify_sbv2_signature_block(sig_block, digest, verified_digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Secure Boot V2 verification failed.");
    }
    bootloader_munmap(sig_block);
    return err;
}

// This verify function is called only from app, during ota update.
// This function is compiled in case when CONFIG_SECURE_BOOT_V2_ENABLED==y or CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT==y.
// if CONFIG_SECURE_BOOT_V2_ENABLED==y and key digests from eFuse are missing, then FAIL (eFuse blocks should be set).
esp_err_t esp_secure_boot_verify_sbv2_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    bool any_trusted_key = false;

    /* Note: in IDF verification we don't add any fault injection resistance, as we don't expect this to be called
        during boot-time verification. */
    memset(verified_digest, 0, ESP_SECURE_BOOT_KEY_DIGEST_LEN);

    esp_image_sig_public_key_digests_t trusted = {0};

    if (get_secure_boot_key_digests(&trusted) != ESP_OK) {
        ESP_LOGE(TAG, "Could not read secure boot digests!");
        return ESP_FAIL;
    }

    int ret = 0;
#ifdef CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT
    const unsigned secure_boot_num_blocks = 1;
#else
    const unsigned secure_boot_num_blocks = SECURE_BOOT_NUM_BLOCKS;
#endif

    for (unsigned app_blk_idx = 0; app_blk_idx < secure_boot_num_blocks; app_blk_idx++) {
        uint8_t app_blk_digest[ESP_SECURE_BOOT_DIGEST_LEN] = { 0 };
        const ets_secure_boot_sig_block_t *app_blk = &sig_block->block[app_blk_idx];
        const ets_secure_boot_sig_block_t *trusted_block = NULL;

        if (validate_signature_block(app_blk) != ESP_OK) {
            continue; // Skip invalid signature blocks
        }

        /* Generate the SHA of the public key components in the signature block */
        bootloader_sha256_handle_t sig_block_sha = bootloader_sha256_start();
#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
        bootloader_sha256_data(sig_block_sha, &app_blk->key, sizeof(app_blk->key));
#elif CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
        bootloader_sha256_data(sig_block_sha, &app_blk->ecdsa.key, sizeof(app_blk->ecdsa.key));
#endif

        bootloader_sha256_finish(sig_block_sha, app_blk_digest);

        /* Check if the key is one we trust */
        for (unsigned trusted_key_idx = 0; trusted_key_idx < secure_boot_num_blocks; trusted_key_idx++) {
            if (memcmp(app_blk_digest, trusted.key_digests[trusted_key_idx], ESP_SECURE_BOOT_KEY_DIGEST_LEN) == 0) {
                ESP_LOGI(TAG, "#%d app key digest == #%d trusted key digest", app_blk_idx, trusted_key_idx);
                trusted_block = app_blk;
                any_trusted_key = true;
                break;
            }
            ESP_LOGV(TAG, "not trusting app sig %d trust idx %d", app_blk_idx, trusted_key_idx);
        }

        if (trusted_block == NULL) {
            continue; // Skip the signature blocks with no trusted digest
        }

#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
        ESP_LOGI(TAG, "Verifying with RSA-PSS...");
        ret = verify_rsa_signature_block(sig_block, image_digest, trusted_block);
#elif CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME
        ESP_LOGI(TAG, "Verifying with ECDSA...");
        ret = verify_ecdsa_signature_block(sig_block, image_digest, trusted_block);
#endif
        if (ret == 0) {
            break;
        }
    }
    return (ret != 0 || any_trusted_key == false) ? ESP_ERR_IMAGE_INVALID: ESP_OK;
}

#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
// To maintain backward compatibility
esp_err_t esp_secure_boot_verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    return esp_secure_boot_verify_sbv2_signature_block(sig_block, image_digest, verified_digest);
}
#endif

#endif // CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME || CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY && CONFIG_SECURE_BOOT_V2_ENABLED

esp_err_t esp_secure_boot_verify_with_efuse_digest_index(int efuse_digest_index, esp_partition_pos_t *part_pos)
{
    if (!part_pos || (efuse_digest_index < 0 || efuse_digest_index >= SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS)) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_image_sig_public_key_digests_t img_key_digests = {0};
    esp_err_t ret = calculate_image_public_key_digests(true, true, &img_key_digests, part_pos);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
    if (esp_efuse_get_digest_revoke(efuse_digest_index)) {
        return ESP_FAIL;
    }

    // Read key digests from efuse
    esp_secure_boot_key_digests_t efuse_key_digests;
    memset(&efuse_key_digests, 0, sizeof(esp_secure_boot_key_digests_t));
    esp_secure_boot_read_key_digests(&efuse_key_digests);

    for (int i = 0; i < img_key_digests.num_digests; i++) {
        if (!memcmp(img_key_digests.key_digests[i], efuse_key_digests.key_digests[efuse_digest_index], ESP_SECURE_BOOT_KEY_DIGEST_LEN)) {
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}
#endif // SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY && CONFIG_SECURE_BOOT_V2_ENABLED
