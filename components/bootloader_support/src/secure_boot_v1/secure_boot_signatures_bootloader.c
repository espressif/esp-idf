/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "spi_flash_mmap.h"
#include "esp_fault.h"
#include "esp32/rom/sha.h"
#include "uECC_verify_antifault.h"

#include <sys/param.h>
#include <string.h>

static const char *TAG = "secure_boot";

#ifdef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[ESP_SECURE_BOOT_DIGEST_LEN];
    uint8_t verified_digest[ESP_SECURE_BOOT_DIGEST_LEN] = { 0 }; /* ignored in this function */
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
    uint8_t verified_digest[ESP_SECURE_BOOT_DIGEST_LEN] = { 0 };
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
        ESP_LOGE(TAG, "image has invalid signature version field 0x%08x (image without a signature?)", sig_block->version);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Verifying secure boot signature");

    bool is_valid;
    is_valid = uECC_verify_antifault(signature_verification_key_start,
                           image_digest,
                           ESP_SECURE_BOOT_DIGEST_LEN,
                           sig_block->signature,
                           uECC_secp256r1(),
                           verified_digest);
    ESP_LOGD(TAG, "Verification result %d", is_valid);

    return is_valid ? ESP_OK : ESP_ERR_IMAGE_INVALID;
}

#endif // CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
