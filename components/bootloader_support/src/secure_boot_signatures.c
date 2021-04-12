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

#include "bootloader_flash.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"

#ifdef BOOTLOADER_BUILD
#include "uECC_verify_antifault.h"
#else
#include "mbedtls/sha256.h"
#include "mbedtls/x509.h"
#include "mbedtls/md.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#endif

#include <sys/param.h>

static const char* TAG = "secure_boot";

extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64

#define DIGEST_LEN 32

/* Mmap source address mask */
#define MMAP_ALIGNED_MASK 0x0000FFFF

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

#ifdef BOOTLOADER_BUILD
esp_err_t esp_secure_boot_verify_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest)
{
    uint8_t verified_digest[DIGEST_LEN] = { 0 };
    return esp_secure_boot_verify_ecdsa_signature_block(sig_block, image_digest, verified_digest);
}

esp_err_t esp_secure_boot_verify_ecdsa_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    ptrdiff_t keylen;
    bool is_valid;

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

    is_valid = uECC_verify_antifault(signature_verification_key_start,
                                image_digest,
                                DIGEST_LEN,
                                sig_block->signature,
                                uECC_secp256r1(),
                                verified_digest);
    ESP_LOGD(TAG, "Verification result %d", is_valid);
    return is_valid ? ESP_OK : ESP_ERR_IMAGE_INVALID;
}

#else // BOOTLOADER_BUILD

esp_err_t esp_secure_boot_verify_ecdsa_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
#if !(defined(CONFIG_MBEDTLS_ECDSA_C) && defined(CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED))
    ESP_LOGE(TAG, "Signature verification requires ECDSA & SECP256R1 curve enabled");
    return ESP_ERR_NOT_SUPPORTED;
#else
    ptrdiff_t keylen;

    /* Note: in IDF app image verification we don't add any fault injection resistance, boot-time checks only */
    memset(verified_digest, 0, DIGEST_LEN);

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

    mbedtls_ecp_group_load(&ecdsa_context.grp, MBEDTLS_ECP_DP_SECP256R1);
    size_t plen = mbedtls_mpi_size(&ecdsa_context.grp.P);
    if (keylen != 2 * plen) {
        ESP_LOGE(TAG, "Incorrect ECDSA key length %d", keylen);
        ret = ESP_FAIL;
        goto cleanup;
    }

    /* Extract X and Y components from ECDSA public key */
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ecdsa_context.Q.X, signature_verification_key_start, plen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ecdsa_context.Q.Y, signature_verification_key_start + plen, plen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&ecdsa_context.Q.Z, 1));

    ret = mbedtls_ecdsa_verify(&ecdsa_context.grp, image_digest, DIGEST_LEN, &ecdsa_context.Q, &r, &s);
    ESP_LOGD(TAG, "Verification result %d", ret);

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecdsa_free(&ecdsa_context);
    return ret == 0 ? ESP_OK : ESP_ERR_IMAGE_INVALID;
#endif // CONFIG_MBEDTLS_ECDSA_C && CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED
}
#endif // BOOTLOADER_BUILD
