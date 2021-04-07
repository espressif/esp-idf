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

#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"
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

// Secure boot V2 for app

_Static_assert(SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS == SECURE_BOOT_NUM_BLOCKS,
               "Parts of this code rely on the max number of signatures appended to an image"
               "being the same as the max number of trusted keys.");

#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

static const char *TAG = "secure_boot_v2";
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

/* A signature block is valid when it has correct magic byte, crc. */
static esp_err_t validate_signature_block(const ets_secure_boot_sig_block_t *block)
{
    if (block->magic_byte != ETS_SECURE_BOOT_V2_SIGNATURE_MAGIC
        || block->block_crc != esp_rom_crc32_le(0, (uint8_t *)block, CRC_SIGN_BLOCK_LEN)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_secure_boot_get_signature_blocks_for_running_app(bool digest_public_keys, esp_image_sig_public_key_digests_t *public_key_digests)
{
    esp_image_metadata_t metadata;
    const esp_partition_t* running_app_part = esp_ota_get_running_partition();
    if (running_app_part == NULL) {
        ESP_LOGE(TAG, "Cannot get running partition");
        return ESP_FAIL;
    }
    const esp_partition_pos_t part_pos = {
        .offset = running_app_part->address,
        .size   = running_app_part->size,
    };
    esp_err_t err = esp_image_get_metadata(&part_pos, &metadata);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading metadata from running app (err=0x%x)", err);
        return ESP_FAIL;
    }

    memset(public_key_digests, 0, sizeof(esp_image_sig_public_key_digests_t));

    // Generating the SHA of the public key components in the signature block

    // metadata.image_len doesn't include any padding to start of the signature sector, so pad it here
    size_t sig_block_addr = metadata.start_addr + ALIGN_UP(metadata.image_len, FLASH_SECTOR_SIZE);
    ESP_LOGD(TAG, "reading signatures for app address 0x%x sig block address 0x%x", part_pos.offset, sig_block_addr);
    for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        ets_secure_boot_sig_block_t block;
        size_t addr = sig_block_addr + sizeof(ets_secure_boot_sig_block_t) * i;
        esp_err_t err = bootloader_flash_read(addr, &block, sizeof(ets_secure_boot_sig_block_t), true);
        if (err == ESP_OK) {
            if (validate_signature_block(&block) == ESP_OK) {
                if (digest_public_keys) {
                    bootloader_sha256_handle_t sig_block_sha = bootloader_sha256_start();
                    bootloader_sha256_data(sig_block_sha, &block.key, sizeof(block.key));
                    bootloader_sha256_finish(sig_block_sha, public_key_digests->key_digests[i]);
                }
                public_key_digests->num_digests++;
            }
        } else {
            ESP_LOGE(TAG, "Secure boot sign blocks cannot be read from a running app (err=0x%x)", err);
            return ESP_FAIL;
        }
    }
    if (public_key_digests->num_digests > 0) {
        return ESP_OK;
    }
    ESP_LOGE(TAG, "No signatures were found for the running app");
    return ESP_ERR_NOT_FOUND;
}

static esp_err_t get_secure_boot_key_digests(esp_image_sig_public_key_digests_t *public_key_digests)
{
#ifdef CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT
    // Gets key digests from running app
    ESP_LOGI(TAG, "Take trusted digest key(s) from running app");
    return esp_secure_boot_get_signature_blocks_for_running_app(true, public_key_digests);
#elif CONFIG_SECURE_BOOT_V2_ENABLED
    ESP_LOGI(TAG, "Take trusted digest key(s) from eFuse block(s)");
#if SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS > 1
    // Read key digests from efuse
    ets_secure_boot_key_digests_t efuse_trusted;
    if (ets_secure_boot_read_key_digests(&efuse_trusted) == ETS_OK) {
        for (unsigned i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
            if (efuse_trusted.key_digests[i] != NULL) {
                memcpy(public_key_digests->key_digests[i], (uint8_t *)efuse_trusted.key_digests[i], ESP_SECURE_BOOT_DIGEST_LEN);
                public_key_digests->num_digests++;
            }
        }
    }
    if (public_key_digests->num_digests > 0) {
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
#else
    memcpy(public_key_digests->key_digests[0], (uint8_t *)EFUSE_BLK2_RDATA0_REG, ESP_SECURE_BOOT_DIGEST_LEN);
    public_key_digests->num_digests = 1;
    return ESP_OK;
#endif // SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS
#endif // CONFIG_SECURE_BOOT_V2_ENABLED
}

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[ESP_SECURE_BOOT_DIGEST_LEN] = {0};
    uint8_t verified_digest[ESP_SECURE_BOOT_DIGEST_LEN] = {0};

    /* Rounding off length to the upper 4k boundary */
    uint32_t padded_length = ALIGN_UP(length, FLASH_SECTOR_SIZE);
    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

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

// This verify function is called only from app, during ota update.
// This function is compiled in case when CONFIG_SECURE_BOOT_V2_ENABLED==y or CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT==y.
// if CONFIG_SECURE_BOOT_V2_ENABLED==y and key digests from eFuse are missing, then FAIL (eFuse blocks should be set).
esp_err_t esp_secure_boot_verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    bool any_trusted_key = false;

    /* Note: in IDF verification we don't add any fault injection resistance, as we don't expect this to be called
        during boot-time verification. */
    memset(verified_digest, 0, ESP_SECURE_BOOT_DIGEST_LEN);

    esp_image_sig_public_key_digests_t trusted = {0};

    if (get_secure_boot_key_digests(&trusted) != ESP_OK) {
        ESP_LOGE(TAG, "Could not read secure boot digests!");
        return ESP_FAIL;
    }


    int ret = 0;
    mbedtls_rsa_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const unsigned rsa_key_size = sizeof(sig_block->block[0].signature);
    unsigned char *sig_be = calloc(1, rsa_key_size);
    unsigned char *buf = calloc(1, rsa_key_size);
    if (sig_be == NULL || buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned -0x%04x\n", ret);
        goto exit;
    }

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
        bootloader_sha256_data(sig_block_sha, &app_blk->key, sizeof(app_blk->key));
        bootloader_sha256_finish(sig_block_sha, app_blk_digest);

        /* Check if the key is one we trust */
        for (unsigned trusted_key_idx = 0; trusted_key_idx < secure_boot_num_blocks; trusted_key_idx++) {
            if (memcmp(app_blk_digest, trusted.key_digests[trusted_key_idx], ESP_SECURE_BOOT_DIGEST_LEN) == 0) {
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

        ESP_LOGI(TAG, "Verifying with RSA-PSS...");

        const mbedtls_mpi N = { .s = 1,
                                .n = sizeof(trusted_block->key.n)/sizeof(mbedtls_mpi_uint),
                                .p = (void *)trusted_block->key.n,
        };
        const mbedtls_mpi e = { .s = 1,
                                .n = sizeof(trusted_block->key.e)/sizeof(mbedtls_mpi_uint), // 1
                                .p = (void *)&trusted_block->key.e,
        };
        mbedtls_rsa_init(&pk, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
        ret = mbedtls_rsa_import(&pk, &N, NULL, NULL, NULL, &e);
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed mbedtls_rsa_import, err: %d", ret);
            goto exit;
        }

        ret = mbedtls_rsa_complete(&pk);
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed mbedtls_rsa_complete, err: %d", ret);
            goto exit;
        }

        ret = mbedtls_rsa_check_pubkey(&pk);
        if (ret != 0) {
            ESP_LOGI(TAG, "Key is not an RSA key -%0x", -ret);
            goto exit;
        }

        /* Signature needs to be byte swapped into BE representation */
        for (int j = 0; j < rsa_key_size; j++) {
            sig_be[rsa_key_size - j - 1] = trusted_block->signature[j];
        }

        ret = mbedtls_rsa_public( &pk, sig_be, buf);
        if (ret != 0) {
            ESP_LOGE(TAG, "mbedtls_rsa_public failed, err: %d", ret);
            goto exit;
        }

        ret = mbedtls_rsa_rsassa_pss_verify( &pk, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, ESP_SECURE_BOOT_DIGEST_LEN,
                                            image_digest, sig_be);
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed mbedtls_rsa_rsassa_pss_verify, err: %d", ret);
        } else {
            ESP_LOGI(TAG, "Signature verified successfully!");
        }
exit:
        mbedtls_rsa_free(&pk);
        if (ret == 0) {
            break;
        }
    }

    free(sig_be);
    free(buf);
    return (ret != 0 || any_trusted_key == false) ? ESP_ERR_IMAGE_INVALID: ESP_OK;
}
#endif // CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT
