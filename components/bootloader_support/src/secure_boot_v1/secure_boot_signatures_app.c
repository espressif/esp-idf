/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
#include "mbedtls/sha256.h"
#include "mbedtls/x509.h"
#include "mbedtls/md.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#include <sys/param.h>

#ifdef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
static const char *TAG = "secure_boot_v1";

extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64

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

    int ret;
    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    /* Extract r and s components from RAW ECDSA signature of 64 bytes */
#define ECDSA_INTEGER_LEN 32
    ret = mbedtls_mpi_read_binary(&r, &sig_block->signature[0], ECDSA_INTEGER_LEN);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed mbedtls_mpi_read_binary(1), err:%d", ret);
        return ESP_FAIL;
    }

    ret = mbedtls_mpi_read_binary(&s, &sig_block->signature[ECDSA_INTEGER_LEN], ECDSA_INTEGER_LEN);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed mbedtls_mpi_read_binary(2), err:%d", ret);
        mbedtls_mpi_free(&r);
        return ESP_FAIL;
    }

    /* Initialise ECDSA context */
    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);
    size_t plen = mbedtls_mpi_size(&ecdsa_context.MBEDTLS_PRIVATE(grp).P);
    if (keylen != 2 * plen) {
        ESP_LOGE(TAG, "Incorrect ECDSA key length %d", keylen);
        ret = ESP_FAIL;
        goto cleanup;
    }

    /* Extract X and Y components from ECDSA public key */
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), signature_verification_key_start, plen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), signature_verification_key_start + plen, plen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Z), 1));

    ret = mbedtls_ecdsa_verify(&ecdsa_context.MBEDTLS_PRIVATE(grp), image_digest, ESP_SECURE_BOOT_DIGEST_LEN, &ecdsa_context.MBEDTLS_PRIVATE(Q), &r, &s);
    ESP_LOGD(TAG, "Verification result %d", ret);

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecdsa_free(&ecdsa_context);
    return ret == 0 ? ESP_OK : ESP_ERR_IMAGE_INVALID;
#endif // CONFIG_MBEDTLS_ECDSA_C && CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED
}
#endif // CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
