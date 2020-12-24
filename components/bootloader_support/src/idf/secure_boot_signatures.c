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
#include "mbedtls/sha256.h"
#include "mbedtls/x509.h"
#include "mbedtls/md.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#include <sys/param.h>
#include "esp_secure_boot.h"

#ifdef CONFIG_IDF_TARGET_ESP32S2
#include <esp32s2/rom/secure_boot.h>
#endif

#define DIGEST_LEN 32

#ifdef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
static const char *TAG = "secure_boot_v1";

extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[DIGEST_LEN];
    uint8_t verified_digest[DIGEST_LEN];
    const esp_secure_boot_sig_block_t *sigblock;

    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

    esp_err_t err = bootloader_sha256_flash_contents(src_addr, length, digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Digest calculation failed 0x%x, 0x%x", src_addr, length);
        return err;
    }

    // Map the signature block and verify the signature
    sigblock = (const esp_secure_boot_sig_block_t *)bootloader_mmap(src_addr + length, sizeof(esp_secure_boot_sig_block_t));
    if (sigblock == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", src_addr + length, sizeof(esp_secure_boot_sig_block_t));
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

#elif CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME

static const char *TAG = "secure_boot_v2";
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#define RSA_KEY_SIZE 384 /* RSA 3072 Bits */

#if CONFIG_IDF_TARGET_ESP32S2
inline static bool digest_matches(const void *trusted, const void *computed)
{
    if (trusted == NULL) {
        return false;
    }

    // 'trusted' is probably a pointer to read-only efuse registers,
    // which only support word reads. memcmp() cannot be guaranteed
    // to do word reads, so we make a local copy here (we know that
    // memcpy() will do word operations if it can).
    uint8_t __attribute__((aligned(4))) trusted_local[ETS_DIGEST_LEN];
    uint8_t __attribute__((aligned(4))) computed_local[ETS_DIGEST_LEN];

    memcpy(trusted_local, trusted, ETS_DIGEST_LEN);
    memcpy(computed_local, computed, ETS_DIGEST_LEN);
    return memcmp(trusted_local, computed_local, ETS_DIGEST_LEN) == 0;
}
#endif /* CONFIG_IDF_TARGET_ESP32S2 */

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[DIGEST_LEN] = {0};
    uint8_t verified_digest[DIGEST_LEN] = {0};

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

esp_err_t esp_secure_boot_verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    uint8_t i = 0;
#if CONFIG_SECURE_BOOT_V2_ENABLED /* Verify key against efuse block */
    uint8_t sig_block_key_digest[SECURE_BOOT_NUM_BLOCKS][DIGEST_LEN] = {0};

    /* Note: in IDF verification we don't add any fault injection resistance, as we don't expect this to be called
       during boot-time verification. */
    memset(verified_digest, 0, DIGEST_LEN);

    /* Generating the SHA of the public key components in the signature block */
    for (i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        bootloader_sha256_handle_t sig_block_sha;
        sig_block_sha = bootloader_sha256_start();
        bootloader_sha256_data(sig_block_sha, &sig_block->block[i].key, sizeof(sig_block->block[i].key));
        bootloader_sha256_finish(sig_block_sha, (unsigned char *)sig_block_key_digest[i]);
    }

#if CONFIG_IDF_TARGET_ESP32
    uint8_t efuse_trusted_digest[DIGEST_LEN] = {0};
    memcpy(efuse_trusted_digest, (uint8_t *) EFUSE_BLK2_RDATA0_REG, sizeof(efuse_trusted_digest));

    if (memcmp(efuse_trusted_digest, sig_block_key_digest[0], DIGEST_LEN) != 0) {
        const uint8_t zeroes[DIGEST_LEN] = {0};
        /* Can't continue if secure boot is enabled, OR if a different digest is already written in efuse BLK2

           (If BLK2 is empty and Secure Boot is disabled then we assume that it will be enabled later.)
         */
        if (esp_secure_boot_enabled() || memcmp(efuse_trusted_digest, zeroes, DIGEST_LEN) != 0) {
            ESP_LOGE(TAG, "Public key digest in eFuse BLK2 and the signature block don't match.");
            return ESP_FAIL;
        }
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    bool match = false;
    ets_secure_boot_key_digests_t efuse_trusted_digest;
    ETS_STATUS r;
    r = ets_secure_boot_read_key_digests(&efuse_trusted_digest);
    if (r != 0) {
        ESP_LOGI(TAG, "Could not read secure boot digests!");
        return ESP_FAIL;
    }
#endif /* CONFIG_IDF_TARGET_ESP32 */
#endif /* CONFIG_SECURE_BOOT_V2_ENABLED */

    ESP_LOGI(TAG, "Verifying with RSA-PSS...");
    int ret = 0;
    mbedtls_rsa_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    unsigned char *sig_be = calloc(1, RSA_KEY_SIZE);
    unsigned char *buf = calloc(1, RSA_KEY_SIZE);
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

    for (i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
#if CONFIG_IDF_TARGET_ESP32S2
        for (uint8_t j = 0; j < SECURE_BOOT_NUM_BLOCKS; j++) {
            if (digest_matches(efuse_trusted_digest.key_digests[j], sig_block_key_digest[i])) {
                ESP_LOGI(TAG, "eFuse key matches(%d) matches the application key(%d).", j, i);
                match = true;
                break;
            }
        }
        if (match == false) {
            continue; // Skip the public keys whose digests don't match.
        }
# endif

        const mbedtls_mpi N = { .s = 1,
                                .n = sizeof(sig_block->block[i].key.n)/sizeof(mbedtls_mpi_uint),
                                .p = (void *)sig_block->block[i].key.n,
        };
        const mbedtls_mpi e = { .s = 1,
                                .n = sizeof(sig_block->block[i].key.e)/sizeof(mbedtls_mpi_uint), // 1
                                .p = (void *)&sig_block->block[i].key.e,
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
        for (int j = 0; j < RSA_KEY_SIZE; j++) {
            sig_be[RSA_KEY_SIZE- j - 1] = sig_block->block[i].signature[j];
        }

        ret = mbedtls_rsa_public( &pk, sig_be, buf);
        if (ret != 0) {
            ESP_LOGE(TAG, "mbedtls_rsa_public failed, err: %d", ret);
            goto exit;
        }

        ret = mbedtls_rsa_rsassa_pss_verify( &pk, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, DIGEST_LEN,
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
#if CONFIG_IDF_TARGET_ESP32
    return (ret != 0) ? ESP_ERR_IMAGE_INVALID: ESP_OK;
#elif CONFIG_IDF_TARGET_ESP32S2
    return (ret != 0 || match == false) ? ESP_ERR_IMAGE_INVALID: ESP_OK;
#endif /* CONFIG_IDF_TARGET_ESP32 */
}
#endif
