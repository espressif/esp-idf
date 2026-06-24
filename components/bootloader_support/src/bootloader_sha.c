/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>

#include "bootloader_sha.h"
#include "soc/soc_caps.h"
#include "rom/sha.h"
#include "sdkconfig.h"

#if NON_OS_BUILD || CONFIG_APP_BUILD_TYPE_RAM
#if !CONFIG_IDF_TARGET_ESP32
static SHA_CTX ctx;

bootloader_sha256_handle_t bootloader_sha256_start()
{
    // Enable SHA hardware
    ets_sha_enable();
    ets_sha_init(&ctx, SHA2_256);
    return &ctx; // Meaningless non-NULL value
}

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    ets_sha_update(&ctx, data, data_len, false);
}

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);

    if (digest == NULL) {
        bzero(&ctx, sizeof(ctx));
        return;
    }
    ets_sha_finish(&ctx, digest);
}

#if SOC_SHA_SUPPORT_SHA512
bootloader_sha_handle_t bootloader_sha512_start(bool is384)
{
    // Enable SHA hardware
    ets_sha_enable();
    ets_sha_init(&ctx, is384 ? SHA2_384 : SHA2_512);
    return &ctx; // Meaningless non-NULL value
}

void bootloader_sha512_data(bootloader_sha_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    ets_sha_update(&ctx, data, data_len, false);
}

void bootloader_sha512_finish(bootloader_sha_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);

    if (digest == NULL) {
        bzero(&ctx, sizeof(ctx));
        return;
    }
    ets_sha_finish(&ctx, digest);
}
#endif /* SOC_SHA_SUPPORT_SHA512 */
#else /* !CONFIG_IDF_TARGET_ESP32 */

#include "soc/dport_reg.h"
#include "soc/hwcrypto_periph.h"

static uint32_t words_hashed;

// Words per SHA256 block
static const size_t BLOCK_WORDS = (64 / sizeof(uint32_t));
// Words in final SHA256 digest
static const size_t DIGEST_WORDS = (32 / sizeof(uint32_t));

bootloader_sha256_handle_t bootloader_sha256_start(void)
{
    // Enable SHA hardware
    ets_sha_enable();
    words_hashed = 0;
    return (bootloader_sha256_handle_t)&words_hashed; // Meaningless non-NULL value
}

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    assert(data_len % 4 == 0);

    const uint32_t *w = (const uint32_t *)data;
    size_t word_len = data_len / 4;
    uint32_t *sha_text_reg = (uint32_t *)(SHA_TEXT_BASE);

    while (word_len > 0) {
        size_t block_count = words_hashed % BLOCK_WORDS;
        size_t copy_words = (BLOCK_WORDS - block_count);

        copy_words = MIN(word_len, copy_words);

        // Wait for SHA engine idle
        while (_DPORT_REG_READ(SHA_256_BUSY_REG) != 0) { }

        // Copy to memory block
        for (size_t i = 0; i < copy_words; i++) {
            sha_text_reg[block_count + i] = __builtin_bswap32(w[i]);
        }
        asm volatile ("memw");

        // Update counters
        words_hashed += copy_words;
        block_count += copy_words;
        word_len -= copy_words;
        w += copy_words;

        // If we loaded a full block, run the SHA engine
        if (block_count == BLOCK_WORDS) {
            if (words_hashed == BLOCK_WORDS) {
                _DPORT_REG_WRITE(SHA_256_START_REG, 1);
            } else {
                _DPORT_REG_WRITE(SHA_256_CONTINUE_REG, 1);
            }
            block_count = 0;
        }
    }
}

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);

    if (digest == NULL) {
        return; // We'd free resources here, but there are none to free
    }

    uint32_t data_words = words_hashed;

    // Pad to a 55 byte long block loaded in the engine
    // (leaving 1 byte 0x80 plus variable padding plus 8 bytes of length,
    // to fill a 64 byte block.)
    int block_bytes = (words_hashed % BLOCK_WORDS) * 4;
    int pad_bytes = 55 - block_bytes;
    if (pad_bytes < 0) {
        pad_bytes += 64;
    }
    static const uint8_t padding[64] = { 0x80, 0, };

    pad_bytes += 5; // 1 byte for 0x80 plus first 4 bytes of the 64-bit length
    assert(pad_bytes % 4 == 0); // should be, as (block_bytes % 4 == 0)

    bootloader_sha256_data(handle, padding, pad_bytes);

    assert(words_hashed % BLOCK_WORDS == 60 / 4); // 32-bits left in block

    // Calculate 32-bit length for final 32 bits of data
    uint32_t bit_count = __builtin_bswap32( data_words * 32 );
    bootloader_sha256_data(handle, &bit_count, sizeof(bit_count));

    assert(words_hashed % BLOCK_WORDS == 0);

    while (_DPORT_REG_READ(SHA_256_BUSY_REG) == 1) { }
    _DPORT_REG_WRITE(SHA_256_LOAD_REG, 1);
    while (_DPORT_REG_READ(SHA_256_BUSY_REG) == 1) { }

    uint32_t *digest_words = (uint32_t *)digest;
    uint32_t *sha_text_reg = (uint32_t *)(SHA_TEXT_BASE);
    for (size_t i = 0; i < DIGEST_WORDS; i++) {
        digest_words[i] = __builtin_bswap32(sha_text_reg[i]);
    }
    asm volatile ("memw");
}
#endif /* CONFIG_IDF_TARGET_ESP32 */
#else /* NON_OS_BUILD || CONFIG_APP_BUILD_TYPE_RAM */

#include "bootloader_flash_priv.h"
#include "psa/crypto.h"

bootloader_sha256_handle_t bootloader_sha256_start(void)
{
    psa_hash_operation_t *op = (psa_hash_operation_t *)malloc(sizeof(psa_hash_operation_t));
    if (!op) {
        return NULL;
    }

    *op = psa_hash_operation_init();
    psa_status_t status = psa_hash_setup(op, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS) {
        free(op);
        return NULL;
    }

    return (bootloader_sha256_handle_t)op;
}

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    psa_hash_operation_t *op = (psa_hash_operation_t *)handle;

    psa_status_t status = psa_hash_update(op, data, data_len);
    assert(status == PSA_SUCCESS);
    (void)status; // Suppress unused variable warning in release builds
}

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);
    psa_hash_operation_t *op = (psa_hash_operation_t *)handle;

    if (digest != NULL) {
        size_t hash_len;
        psa_status_t status = psa_hash_finish(op, digest, PSA_HASH_LENGTH(PSA_ALG_SHA_256), &hash_len);
        assert(status == PSA_SUCCESS);
        assert(hash_len == PSA_HASH_LENGTH(PSA_ALG_SHA_256));
        (void)status; // Suppress unused variable warning in release builds
        (void)hash_len; // Suppress unused variable warning in release builds
    } else {
        psa_hash_abort(op);
    }

    free(handle);
    handle = NULL;
}

#if SOC_SHA_SUPPORT_SHA512

typedef struct {
    psa_hash_operation_t *hash_op;
    int psa_alg;
} bootloader_psa_sha_handle_t;

bootloader_sha_handle_t bootloader_sha512_start(bool is384)
{
    psa_status_t status;
    bootloader_psa_sha_handle_t *op = (bootloader_psa_sha_handle_t *)malloc(sizeof(bootloader_psa_sha_handle_t));
    if (!op) {
        return NULL;
    }

    op->hash_op = (psa_hash_operation_t *)malloc(sizeof(psa_hash_operation_t));
    if (!op->hash_op) {
        free(op);
        return NULL;
    }

    op->psa_alg = is384 ? PSA_ALG_SHA_384 : PSA_ALG_SHA_512;

    *op->hash_op = psa_hash_operation_init();
    status = psa_hash_setup(op->hash_op, op->psa_alg);
    if (status != PSA_SUCCESS) {
        free(op->hash_op);
        free(op);
        return NULL;
    }

    return (bootloader_psa_sha_handle_t *)op;
}

void bootloader_sha512_data(bootloader_sha_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    bootloader_psa_sha_handle_t *op = (bootloader_psa_sha_handle_t *)handle;

    psa_status_t status = psa_hash_update(op->hash_op, data, data_len);
    assert(status == PSA_SUCCESS);
    (void)status; // Suppress unused variable warning in release builds
}

void bootloader_sha512_finish(bootloader_sha_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);
    bootloader_psa_sha_handle_t *op = (bootloader_psa_sha_handle_t *)handle;

    if (digest != NULL) {
        size_t hash_len;
        psa_status_t status = psa_hash_finish(op->hash_op, digest, PSA_HASH_LENGTH(op->psa_alg), &hash_len);
        assert(status == PSA_SUCCESS);
        assert(hash_len == PSA_HASH_LENGTH(op->psa_alg));
        (void)status; // Suppress unused variable warning in release builds
        (void)hash_len; // Suppress unused variable warning in release builds
    } else {
        psa_hash_abort(op->hash_op);
    }

    free(op->hash_op);
    free(op);
    handle = NULL;
}
#endif /* SOC_SHA_SUPPORT_SHA512 */
#endif /* !(NON_OS_BUILD || CONFIG_APP_BUILD_TYPE_RAM) */
