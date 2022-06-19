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
#include "esp_secure_boot.h"
#include "esp_spi_flash.h"
#include "esp_fault.h"
#include "esp32/rom/sha.h"
#include "uECC_verify_antifault.h"

#include <sys/param.h>
#include <string.h>

static const char *TAG = "secure_boot";
#define DIGEST_LEN 32

#ifdef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[DIGEST_LEN];
    uint8_t verified_digest[DIGEST_LEN] = { 0 }; /* ignored in this function */
    const esp_secure_boot_sig_block_t *sigblock;

    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

    esp_err_t err = bootloader_sha256_flash_contents(src_addr, length, digest);
    if (err != ESP_OK) {
        return err;
    }

    // Map the signature block
    sigblock = (const esp_secure_boot_sig_block_t *) bootloader_mmap(src_addr + length, sizeof(esp_secure_boot_sig_block_t));
    if(!sigblock) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", src_addr + length, sizeof(esp_secure_boot_sig_block_t));
        return ESP_FAIL;
    }
    // Verify the signature
    err = esp_secure_boot_verify_ecdsa_signature_block(sigblock, digest, verified_digest);
    // Unmap
    bootloader_munmap(sigblock);

    return err;
}

esp_err_t esp_secure_boot_verify_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest)
{
    uint8_t verified_digest[DIGEST_LEN] = { 0 };
    return esp_secure_boot_verify_ecdsa_signature_block(sig_block, image_digest, verified_digest);
}

esp_err_t esp_secure_boot_verify_ecdsa_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    ptrdiff_t keylen;

    keylen = signature_verification_key_end - signature_verification_key_start;
    if (keylen != SIGNATURE_VERIFICATION_KEYLEN) {
        ESP_LOGE(TAG, "Embedded public verification key has wrong length %d", keylen);
        return ESP_FAIL;
    }

    if (sig_block->version != 0) {
        ESP_LOGE(TAG, "image has invalid signature version field 0x%08x", sig_block->version);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Verifying secure boot signature");

    bool is_valid;
    is_valid = uECC_verify_antifault(signature_verification_key_start,
                           image_digest,
                           DIGEST_LEN,
                           sig_block->signature,
                           uECC_secp256r1(),
                           verified_digest);
    ESP_LOGD(TAG, "Verification result %d", is_valid);

    return is_valid ? ESP_OK : ESP_ERR_IMAGE_INVALID;
}

#elif CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[DIGEST_LEN] = {0};
    uint8_t verified_digest[DIGEST_LEN] = {0}; // ignored in this function
    const uint8_t *data;

    /* Padding to round off the input to the nearest 4k boundary */
    int padded_length = ALIGN_UP(length, FLASH_SECTOR_SIZE);
    ESP_LOGD(TAG, "verifying src_addr 0x%x length", src_addr, padded_length);

    data = bootloader_mmap(src_addr, padded_length + sizeof(ets_secure_boot_signature_t));
    if (data == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", src_addr, padded_length);
        return ESP_FAIL;
    }

    /* Calculate digest of main image */
    esp_err_t err = bootloader_sha256_flash_contents(src_addr, padded_length, digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Digest calculation failed 0x%x, 0x%x", src_addr, padded_length);
        bootloader_munmap(data);
        return err;
    }

    const ets_secure_boot_signature_t *sig_block = (const ets_secure_boot_signature_t *)(data + padded_length);
    err = esp_secure_boot_verify_rsa_signature_block(sig_block, digest, verified_digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Secure Boot V2 verification failed.");
    }

    bootloader_munmap(data);
    return err;
}

esp_err_t esp_secure_boot_verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    secure_boot_v2_status_t r;
    uint8_t efuse_trusted_digest[DIGEST_LEN] = {0}, sig_block_trusted_digest[DIGEST_LEN] = {0};

    memcpy(efuse_trusted_digest, (uint8_t *)EFUSE_BLK2_RDATA0_REG, DIGEST_LEN); /* EFUSE_BLK2_RDATA0_REG - Stores the Secure Boot Public Key Digest */

    if (!ets_use_secure_boot_v2()) {
        ESP_LOGI(TAG, "Secure Boot eFuse bit(ABS_DONE_1) not yet programmed.");

        /* Generating the SHA of the public key components in the signature block */
        bootloader_sha256_handle_t sig_block_sha;
        sig_block_sha = bootloader_sha256_start();
        bootloader_sha256_data(sig_block_sha, &sig_block->block[0].key, sizeof(sig_block->block[0].key));
        bootloader_sha256_finish(sig_block_sha, (unsigned char *)sig_block_trusted_digest);

#if CONFIG_SECURE_BOOT_V2_ENABLED
        if (memcmp(efuse_trusted_digest, sig_block_trusted_digest, DIGEST_LEN) != 0) {
            /* Most likely explanation for this is that BLK2 is empty, and we're going to burn it
               after we verify that the signature is valid. However, if BLK2 is not empty then we need to
               fail here.
            */
            bool all_zeroes = true;
            for (int i = 0; i < DIGEST_LEN; i++) {
                all_zeroes = all_zeroes && (efuse_trusted_digest[i] == 0);
            }
            if (!all_zeroes) {
                ESP_LOGE(TAG, "Different public key digest burned to eFuse BLK2");
                return ESP_ERR_INVALID_STATE;
            }
        }

        ESP_FAULT_ASSERT(!ets_use_secure_boot_v2());
#endif

        memcpy(efuse_trusted_digest, sig_block_trusted_digest, DIGEST_LEN);
    }

    ESP_LOGI(TAG, "Verifying with RSA-PSS...");
    r = ets_secure_boot_verify_signature(sig_block, image_digest, efuse_trusted_digest, verified_digest);
    if (r != SBV2_SUCCESS) {
        ESP_LOGE(TAG, "Secure Boot V2 verification failed.");
    }

    return (r == SBV2_SUCCESS) ? ESP_OK : ESP_ERR_IMAGE_INVALID;
}
#endif
