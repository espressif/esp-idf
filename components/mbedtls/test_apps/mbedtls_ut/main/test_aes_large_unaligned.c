/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* AES over external memory, longer than one DMA chunk, ending in a partial block.
 *
 * esp_aes_process_dma_ext_ram() splits the work into AES_MAX_CHUNK_WRITE_SIZE
 * pieces, 8 KiB on ESP32-P4. A length such as 16397 therefore ends with a chunk
 * of 13 bytes, which holds no full AES block. esp_mspi_buffer_alignment_satisfied()
 * rejects a zero length by design, so without a guard esp_aes_process_dma() hands
 * that chunk straight back, and with PSRAM ECC on the bounce buffer is itself in
 * PSRAM, so the two functions call each other until the stack runs out.
 *
 * A TLS 1.3 record is 16384 + 1 bytes, which is exactly this shape, so a peer
 * sending full size records reaches it. See GitHub issue #19099.
 *
 * The lengths below give tails of 1, 13 and 15 bytes, plus 16 and 0 as controls.
 */

#include <string.h>
#include <stdio.h>

#include "sdkconfig.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "aes/esp_aes.h"

#if CONFIG_SPIRAM_USE_MALLOC && CONFIG_MBEDTLS_HARDWARE_AES

#define AES_CHUNK           8192        /* AES_MAX_CHUNK_WRITE_SIZE on ESP32-P4 */
#define AES_MAX_TEST_LEN    (2 * AES_CHUNK + 16)
#define PSRAM_CAPS          (MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM)
#define INTERNAL_CAPS       (MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL)

/* tail = len % 8192: 1, 13, 15, then 16 and 0 as controls */
static const size_t k_lengths[] = { 8193, 16397, 16399, 16400, 16384 };
static const size_t k_offsets[] = { 0, 3 };
#define NUM_LENGTHS (sizeof(k_lengths) / sizeof(k_lengths[0]))
#define NUM_OFFSETS (sizeof(k_offsets) / sizeof(k_offsets[0]))

static void aes_ctr_once(const uint8_t *key, const uint8_t *input, uint8_t *output, size_t len)
{
    esp_aes_context ctx;
    unsigned char nonce[16];
    unsigned char stream_block[16];
    size_t nc_off = 0;

    memset(nonce, 0x2B, sizeof(nonce));
    memset(stream_block, 0, sizeof(stream_block));

    esp_aes_init(&ctx);
    TEST_ASSERT_EQUAL(0, esp_aes_setkey(&ctx, key, 256));
    TEST_ASSERT_EQUAL(0, esp_aes_crypt_ctr(&ctx, len, &nc_off, nonce, stream_block,
                                           input, output));
    esp_aes_free(&ctx);
}

TEST_CASE("mbedtls AES CTR over large unaligned PSRAM buffers", "[aes]")
{
    uint8_t key[32];
    memset(key, 0x6C, sizeof(key));

    uint8_t *plain = heap_caps_malloc(AES_MAX_TEST_LEN, INTERNAL_CAPS);
    uint8_t *expected = heap_caps_malloc(AES_MAX_TEST_LEN, INTERNAL_CAPS);
    uint8_t *psram_in = heap_caps_malloc(AES_MAX_TEST_LEN + 16, PSRAM_CAPS);
    uint8_t *psram_out = heap_caps_malloc(AES_MAX_TEST_LEN + 16, PSRAM_CAPS);
    TEST_ASSERT_NOT_NULL(plain);
    TEST_ASSERT_NOT_NULL(expected);
    TEST_ASSERT_NOT_NULL(psram_in);
    TEST_ASSERT_NOT_NULL(psram_out);
    TEST_ASSERT_TRUE(esp_ptr_external_ram(psram_in));
    TEST_ASSERT_TRUE(esp_ptr_external_ram(psram_out));

    for (size_t i = 0; i < AES_MAX_TEST_LEN; i++) {
        plain[i] = (uint8_t)(i * 7 + 1);
    }

    for (size_t l = 0; l < NUM_LENGTHS; l++) {
        size_t len = k_lengths[l];

        /* Reference from internal RAM, which never takes the external memory path. */
        aes_ctr_once(key, plain, expected, len);

        for (size_t o = 0; o < NUM_OFFSETS; o++) {
            uint8_t *input = psram_in + k_offsets[o];
            uint8_t *output = psram_out + k_offsets[o];

            memcpy(input, plain, len);
            memset(output, 0, len);
            aes_ctr_once(key, input, output, len);

            printf("len=%u tail=%u offset=%u ... ", (unsigned)len,
                   (unsigned)(len % AES_CHUNK), (unsigned)k_offsets[o]);
            TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, output, len);
            printf("ok\n");
        }
    }

    free(plain);
    free(expected);
    free(psram_in);
    free(psram_out);
}

#endif /* CONFIG_SPIRAM_USE_MALLOC && CONFIG_MBEDTLS_HARDWARE_AES */
