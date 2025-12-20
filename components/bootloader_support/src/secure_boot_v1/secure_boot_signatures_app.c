/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#include "psa/crypto.h"
#include <string.h>
#include <sys/param.h>
#include "mbedtls/pk.h"


#ifdef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
ESP_LOG_ATTR_TAG(TAG, "secure_boot_v1");

extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64
#define PSA_ECDSA_PUB_KEY_SIZE_BITS 256
#define UNCOMPRESSED_SECP256R1_KEY_SIZE 65  // Size for uncompressed SECP256R1 (1 + 32 + 32)
#define ECC_UNCOMPRESSED_POINT_FORMAT_INDICATOR 0x04
esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[ESP_SECURE_BOOT_DIGEST_LEN];
    uint8_t verified_digest[ESP_SECURE_BOOT_DIGEST_LEN];
    const esp_secure_boot_sig_block_t *sigblock;

    ESP_LOGD(TAG, "verifying signature src_addr 0x%"PRIx32" length 0x%"PRIx32, src_addr, length);

    esp_err_t err = bootloader_sha256_flash_contents(src_addr, length, digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Digest calculation failed 0x%"PRIx32", 0x%"PRIx32, src_addr, length);
        return err;
    }

    // Map the signature block and verify the signature
    sigblock = (const esp_secure_boot_sig_block_t *)bootloader_mmap(src_addr + length, sizeof(esp_secure_boot_sig_block_t));
    if (sigblock == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%"PRIx32", 0x%x) failed", src_addr + length, sizeof(esp_secure_boot_sig_block_t));
        return ESP_FAIL;
    }
    err = esp_secure_boot_verify_ecdsa_signature_block(sigblock, digest, verified_digest);
    bootloader_munmap(sigblock);
    return err;
}

esp_err_t esp_secure_boot_verify_ecdsa_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
#if !(defined(CONFIG_MBEDTLS_ECDSA_C) && defined(CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED))
    ESP_LOGE(TAG, "Signature verification requires ECDSA & SECP256R1 curve enabled");
    return ESP_ERR_NOT_SUPPORTED;
#else
    ptrdiff_t keylen;

    /* Note: in IDF app image verification we don't add any fault injection resistance, boot-time checks only */
    memset(verified_digest, 0, ESP_SECURE_BOOT_DIGEST_LEN);

    keylen = signature_verification_key_end - signature_verification_key_start;
    if (keylen != SIGNATURE_VERIFICATION_KEYLEN) {
        ESP_LOGE(TAG, "Embedded public verification key has wrong length %d", keylen);
        return ESP_FAIL;
    }

    if (sig_block->version != ESP_SECURE_BOOT_SCHEME) {
        ESP_LOGE(TAG, "image has invalid signature version field 0x%08"PRIx32" (image without a signature?)", sig_block->version);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Verifying secure boot signature");
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_handle;

    // Format the public key for PSA import
    uint8_t formatted_key[UNCOMPRESSED_SECP256R1_KEY_SIZE];
    formatted_key[0] = ECC_UNCOMPRESSED_POINT_FORMAT_INDICATOR;

    // Copy X and Y coordinates
    if (keylen == 64) { // Raw coordinates without format byte
        memcpy(&formatted_key[1], signature_verification_key_start, 64);
    } else if (keylen == UNCOMPRESSED_SECP256R1_KEY_SIZE && signature_verification_key_start[0] == ECC_UNCOMPRESSED_POINT_FORMAT_INDICATOR) {
        // Key is already in correct format
        memcpy(formatted_key, signature_verification_key_start, UNCOMPRESSED_SECP256R1_KEY_SIZE);
    } else {
        ESP_LOGE(TAG, "Invalid key format or length");
        return ESP_FAIL;
    }

    // Set key attributes
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&key_attributes, PSA_ECDSA_PUB_KEY_SIZE_BITS);

    // Import the properly formatted public key
    status = psa_import_key(&key_attributes, formatted_key, sizeof(formatted_key), &key_handle);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to import key, status:%d", status);
        return ESP_FAIL;
    }

    // Verify the signature
    status = psa_verify_hash(key_handle, PSA_ALG_ECDSA(PSA_ALG_SHA_256), image_digest, ESP_SECURE_BOOT_DIGEST_LEN, sig_block->signature, SIGNATURE_VERIFICATION_KEYLEN);
    ESP_LOGI(TAG, "Verification result %d", status);

    // Destroy the key handle
    psa_destroy_key(key_handle);
    psa_reset_key_attributes(&key_attributes);

    return status == PSA_SUCCESS ? ESP_OK : ESP_ERR_IMAGE_INVALID;
#endif // CONFIG_MBEDTLS_ECDSA_C && CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED
}
#endif // CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
