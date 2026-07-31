/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* Correctness tests for esp_aes_crypt_ecb_blocks().
 *
 * The function submits a whole buffer to the AES accelerator in one operation,
 * so the cases below cover both code paths it can take: the DMA path, and the
 * block-at-a-time path taken on targets without AES DMA or for short buffers
 * when CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM is enabled. Buffer
 * placements cover internal RAM, unaligned internal RAM, flash (DROM) and
 * PSRAM, which are the placements that decide whether the driver can hand the
 * caller's pointers to the DMA or has to bounce them through aligned internal
 * memory.
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_macros.h"
#include "soc/soc_caps.h"
#include "soc/lldesc.h"

#if CONFIG_MBEDTLS_HARDWARE_AES

#include "aes/esp_aes.h"

#define ECB_INTERNAL_CAPS (MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)
#define ECB_PSRAM_CAPS    (MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM)

/* NIST SP 800-38A, F.1.5 and F.1.6: ECB-AES256 over four distinct blocks.
 * The blocks differ from each other, so a reordered or repeated block fails
 * this vector. A vector built from one repeated block cannot do that.
 */
static const uint8_t ecb_key_256[32] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
    0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4,
};

static const uint8_t ecb_kat_plaintext[64] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10,
};

static const uint8_t ecb_kat_ciphertext[64] = {
    0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8,
    0x59, 0x1c, 0xcb, 0x10, 0xd4, 0x10, 0xed, 0x26, 0xdc, 0x5b, 0xa7, 0x4a, 0x31, 0x36, 0x28, 0x70,
    0xb6, 0xed, 0x21, 0xb9, 0x9c, 0xa6, 0xf4, 0xf9, 0xf1, 0x53, 0xe7, 0xb1, 0xbe, 0xaf, 0xed, 0x1d,
    0x23, 0x30, 0x4b, 0x7a, 0x39, 0xf9, 0xf3, 0xff, 0x06, 0x7d, 0x8d, 0x8f, 0x9e, 0x24, 0xec, 0xc7,
};

/* Lengths chosen to straddle every branch the driver takes:
 *   16 .. 128   block path when CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM is on
 *   512 .. 1024 DMA path on every target
 *   1600, 1616 the chunk size used when a buffer has to be bounced
 *   3200       more than two of those chunks, so the bounce loop runs twice
 */
static const size_t ecb_lengths[] = { 16, 32, 64, 128, 144, 512, 1024, 1600, 1616, 3200 };
#define ECB_MAX_LEN 3200

/* Distinct bytes everywhere, so a block that is dropped, repeated or swapped
 * changes the result.
 */
static void ecb_fill_pattern(uint8_t *buf, size_t length, uint8_t seed)
{
    for (size_t i = 0; i < length; i++) {
        buf[i] = (uint8_t)(seed + i * 7u + (i >> 4));
    }
}

static void ecb_blocks_run(int mode, const uint8_t *input, uint8_t *output, size_t length)
{
    esp_aes_context ctx;

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, ecb_key_256, 256));
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_ecb_blocks(&ctx, mode, length, input, output));
    esp_aes_free(&ctx);
}

/* The reference is one esp_aes_crypt_ecb() call per block, which is what the
 * bulk call replaces. Input and output stay in internal aligned memory so the
 * reference itself never depends on the placement under test.
 */
static void ecb_reference_run(int mode, const uint8_t *input, uint8_t *output, size_t length)
{
    esp_aes_context ctx;

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, ecb_key_256, 256));
    for (size_t offset = 0; offset < length; offset += 16) {
        TEST_ASSERT_EQUAL(0, esp_aes_crypt_ecb(&ctx, mode, input + offset, output + offset));
    }
    esp_aes_free(&ctx);
}

TEST_CASE("esp_aes_crypt_ecb_blocks known answer vector", "[aes]")
{
    uint8_t *input = heap_caps_malloc(sizeof(ecb_kat_plaintext), ECB_INTERNAL_CAPS);
    uint8_t *output = heap_caps_malloc(sizeof(ecb_kat_plaintext), ECB_INTERNAL_CAPS);

    TEST_ASSERT_NOT_NULL(input);
    TEST_ASSERT_NOT_NULL(output);

    memcpy(input, ecb_kat_plaintext, sizeof(ecb_kat_plaintext));
    ecb_blocks_run(ESP_AES_ENCRYPT, input, output, sizeof(ecb_kat_plaintext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecb_kat_ciphertext, output, sizeof(ecb_kat_ciphertext));

    memcpy(input, ecb_kat_ciphertext, sizeof(ecb_kat_ciphertext));
    ecb_blocks_run(ESP_AES_DECRYPT, input, output, sizeof(ecb_kat_ciphertext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecb_kat_plaintext, output, sizeof(ecb_kat_plaintext));

    free(input);
    free(output);
}

TEST_CASE("esp_aes_crypt_ecb_blocks matches per block ECB", "[aes]")
{
    uint8_t *plaintext = heap_caps_malloc(ECB_MAX_LEN, ECB_INTERNAL_CAPS);
    uint8_t *bulk = heap_caps_malloc(ECB_MAX_LEN, ECB_INTERNAL_CAPS);
    uint8_t *reference = heap_caps_malloc(ECB_MAX_LEN, ECB_INTERNAL_CAPS);
    uint8_t *roundtrip = heap_caps_malloc(ECB_MAX_LEN, ECB_INTERNAL_CAPS);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(bulk);
    TEST_ASSERT_NOT_NULL(reference);
    TEST_ASSERT_NOT_NULL(roundtrip);

    for (size_t i = 0; i < sizeof(ecb_lengths) / sizeof(ecb_lengths[0]); i++) {
        size_t length = ecb_lengths[i];

        ecb_fill_pattern(plaintext, length, (uint8_t)i);

        ecb_blocks_run(ESP_AES_ENCRYPT, plaintext, bulk, length);
        ecb_reference_run(ESP_AES_ENCRYPT, plaintext, reference, length);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(reference, bulk, length);

        ecb_blocks_run(ESP_AES_DECRYPT, bulk, roundtrip, length);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, roundtrip, length);

        ecb_blocks_run(ESP_AES_DECRYPT, plaintext, bulk, length);
        ecb_reference_run(ESP_AES_DECRYPT, plaintext, reference, length);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(reference, bulk, length);
    }

    free(plaintext);
    free(bulk);
    free(reference);
    free(roundtrip);
}

TEST_CASE("esp_aes_crypt_ecb_blocks in place", "[aes]")
{
    uint8_t *buffer = heap_caps_malloc(ECB_MAX_LEN, ECB_INTERNAL_CAPS);
    uint8_t *plaintext = heap_caps_malloc(ECB_MAX_LEN, ECB_INTERNAL_CAPS);
    uint8_t *reference = heap_caps_malloc(ECB_MAX_LEN, ECB_INTERNAL_CAPS);

    TEST_ASSERT_NOT_NULL(buffer);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(reference);

    for (size_t i = 0; i < sizeof(ecb_lengths) / sizeof(ecb_lengths[0]); i++) {
        size_t length = ecb_lengths[i];

        ecb_fill_pattern(plaintext, length, (uint8_t)(0x40 + i));
        ecb_reference_run(ESP_AES_ENCRYPT, plaintext, reference, length);

        memcpy(buffer, plaintext, length);
        ecb_blocks_run(ESP_AES_ENCRYPT, buffer, buffer, length);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(reference, buffer, length);

        ecb_blocks_run(ESP_AES_DECRYPT, buffer, buffer, length);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, buffer, length);
    }

    free(buffer);
    free(plaintext);
    free(reference);
}

#if SOC_AES_SUPPORT_DMA

/* Only the DMA path accepts a buffer that is not word aligned. The block path
 * reads the buffer as uint32_t, which faults on an unaligned address, so the
 * length used here stays above the small-data threshold on every target.
 */
#define ECB_UNALIGNED_LEN 1024

TEST_CASE("esp_aes_crypt_ecb_blocks with unaligned internal buffers", "[aes]")
{
    const size_t offsets[] = { 1, 2, 3, 8, 15 };

    uint8_t *input_alloc = heap_caps_malloc(ECB_UNALIGNED_LEN + 16, ECB_INTERNAL_CAPS);
    uint8_t *output_alloc = heap_caps_malloc(ECB_UNALIGNED_LEN + 16, ECB_INTERNAL_CAPS);
    uint8_t *plaintext = heap_caps_malloc(ECB_UNALIGNED_LEN, ECB_INTERNAL_CAPS);
    uint8_t *reference = heap_caps_malloc(ECB_UNALIGNED_LEN, ECB_INTERNAL_CAPS);

    TEST_ASSERT_NOT_NULL(input_alloc);
    TEST_ASSERT_NOT_NULL(output_alloc);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(reference);

    ecb_fill_pattern(plaintext, ECB_UNALIGNED_LEN, 0x11);
    ecb_reference_run(ESP_AES_ENCRYPT, plaintext, reference, ECB_UNALIGNED_LEN);

    for (size_t i = 0; i < sizeof(offsets) / sizeof(offsets[0]); i++) {
        uint8_t *input = input_alloc + offsets[i];
        uint8_t *output = output_alloc + offsets[i];

        memcpy(input, plaintext, ECB_UNALIGNED_LEN);
        memset(output, 0, ECB_UNALIGNED_LEN);

        ecb_blocks_run(ESP_AES_ENCRYPT, input, output, ECB_UNALIGNED_LEN);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(reference, output, ECB_UNALIGNED_LEN);
    }

    free(input_alloc);
    free(output_alloc);
    free(plaintext);
    free(reference);
}

TEST_CASE("esp_aes_crypt_ecb_blocks across multiple DMA descriptors", "[aes]")
{
    /* One DMA descriptor cannot carry more than LLDESC_MAX_NUM_PER_DESC bytes,
       so this length forces the driver to build a descriptor chain. */
    const size_t length = ESP_ALIGN_DOWN(2 * LLDESC_MAX_NUM_PER_DESC, 16);

    uint8_t *plaintext = heap_caps_malloc(length, ECB_INTERNAL_CAPS);
    uint8_t *bulk = heap_caps_malloc(length, ECB_INTERNAL_CAPS);
    uint8_t *reference = heap_caps_malloc(length, ECB_INTERNAL_CAPS);

    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(bulk);
    TEST_ASSERT_NOT_NULL(reference);

    ecb_fill_pattern(plaintext, length, 0x5c);

    ecb_blocks_run(ESP_AES_ENCRYPT, plaintext, bulk, length);
    ecb_reference_run(ESP_AES_ENCRYPT, plaintext, reference, length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(reference, bulk, length);

    ecb_blocks_run(ESP_AES_DECRYPT, bulk, reference, length);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, reference, length);

    free(plaintext);
    free(bulk);
    free(reference);
}

#endif /* SOC_AES_SUPPORT_DMA */

TEST_CASE("esp_aes_crypt_ecb_blocks with input in flash", "[aes]")
{
    /* ecb_kat_plaintext is const, so on targets that keep rodata in flash the
       input is in DROM. The DMA cannot reach it and the driver has to copy the
       input into internal memory before it starts. */
    uint8_t *output = heap_caps_malloc(sizeof(ecb_kat_plaintext), ECB_INTERNAL_CAPS);

    TEST_ASSERT_NOT_NULL(output);

    ecb_blocks_run(ESP_AES_ENCRYPT, ecb_kat_plaintext, output, sizeof(ecb_kat_plaintext));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecb_kat_ciphertext, output, sizeof(ecb_kat_ciphertext));

    free(output);
}

#if CONFIG_SPIRAM_USE_MALLOC && SOC_AES_SUPPORT_DMA

/* Above the small-data threshold on every target, so these cases stay on the
 * DMA path where the buffer placement actually matters.
 */
#define ECB_PSRAM_LEN 1024

static void ecb_psram_case(const char *name, uint8_t *input, uint8_t *output,
                           const uint8_t *plaintext, const uint8_t *reference)
{
    memcpy(input, plaintext, ECB_PSRAM_LEN);
    memset(output, 0, ECB_PSRAM_LEN);

    ecb_blocks_run(ESP_AES_ENCRYPT, input, output, ECB_PSRAM_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(reference, output, ECB_PSRAM_LEN, name);

    ecb_blocks_run(ESP_AES_DECRYPT, output, output, ECB_PSRAM_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(plaintext, output, ECB_PSRAM_LEN, name);
}

TEST_CASE("esp_aes_crypt_ecb_blocks with PSRAM buffers", "[aes][psram]")
{
    uint8_t *psram_input = heap_caps_malloc(ECB_PSRAM_LEN + 16, ECB_PSRAM_CAPS);
    uint8_t *psram_output = heap_caps_malloc(ECB_PSRAM_LEN + 16, ECB_PSRAM_CAPS);
    uint8_t *internal = heap_caps_malloc(ECB_PSRAM_LEN, ECB_INTERNAL_CAPS);
    uint8_t *plaintext = heap_caps_malloc(ECB_PSRAM_LEN, ECB_INTERNAL_CAPS);
    uint8_t *reference = heap_caps_malloc(ECB_PSRAM_LEN, ECB_INTERNAL_CAPS);

    TEST_ASSERT_NOT_NULL(psram_input);
    TEST_ASSERT_NOT_NULL(psram_output);
    TEST_ASSERT_NOT_NULL(internal);
    TEST_ASSERT_NOT_NULL(plaintext);
    TEST_ASSERT_NOT_NULL(reference);

    TEST_ASSERT_TRUE(esp_ptr_external_ram(psram_input));
    TEST_ASSERT_TRUE(esp_ptr_external_ram(psram_output));

    ecb_fill_pattern(plaintext, ECB_PSRAM_LEN, 0x77);
    ecb_reference_run(ESP_AES_ENCRYPT, plaintext, reference, ECB_PSRAM_LEN);

    /* Both buffers in PSRAM, aligned as the allocator returned them. */
    ecb_psram_case("psram to psram", psram_input, psram_output, plaintext, reference);

    /* Both buffers in PSRAM and deliberately off alignment. With flash
       encryption or PSRAM ECC enabled this is the case the driver has to
       bounce through aligned internal memory. */
    ecb_psram_case("psram to psram, unaligned", psram_input + 1, psram_output + 3, plaintext, reference);

    /* One buffer in each memory, in both directions. */
    ecb_psram_case("psram to internal", psram_input, internal, plaintext, reference);
    ecb_psram_case("internal to psram", internal, psram_output, plaintext, reference);

    /* In place inside PSRAM. */
    memcpy(psram_input, plaintext, ECB_PSRAM_LEN);
    ecb_blocks_run(ESP_AES_ENCRYPT, psram_input, psram_input, ECB_PSRAM_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(reference, psram_input, ECB_PSRAM_LEN);
    ecb_blocks_run(ESP_AES_DECRYPT, psram_input, psram_input, ECB_PSRAM_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext, psram_input, ECB_PSRAM_LEN);

    free(psram_input);
    free(psram_output);
    free(internal);
    free(plaintext);
    free(reference);
}

#endif /* CONFIG_SPIRAM_USE_MALLOC && SOC_AES_SUPPORT_DMA */

#endif /* CONFIG_MBEDTLS_HARDWARE_AES */
