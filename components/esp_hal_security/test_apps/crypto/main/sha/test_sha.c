/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_types.h"
#include "soc/soc_caps.h"
#include "unity.h"
#include "esp_heap_caps.h"

#include "memory_checks.h"
#include "unity_fixture.h"
#include "sha_block.h"
#include "sha_dma.h"

#if SOC_SHA_SUPPORT_SM3
#include "esp_sm3.h"
#include "hal/sha_hal.h"
#endif

#if SOC_SHA_SUPPORTED

#if SOC_SHA_SUPPORT_SHA1

static void test_sha1(bool is_dma)
{
    uint8_t sha1_result[20] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha1_expected[20] = { 0x09, 0x23, 0x02, 0xfb, 0x2d, 0x36, 0x42, 0xec,
                                        0xc5, 0xfa, 0xd5, 0x8f, 0xdb, 0xc3, 0x8d, 0x5c,
                                        0x97, 0xd6, 0x17, 0xee
                                      };

#if SOC_SHA_SUPPORT_DMA
    if (is_dma) {
        sha1_dma(SHA1, buffer, BUFFER_SZ, sha1_result);
    } else
#endif
    {
        sha1_block(SHA1, buffer, BUFFER_SZ, sha1_result);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha1_expected, sha1_result, sizeof(sha1_expected));
    heap_caps_free(buffer);
}

#endif /* SOC_SHA_SUPPORT_SHA1 */

#if SOC_SHA_SUPPORT_SHA224

static void test_sha224(bool is_dma)
{
    uint8_t sha224_result[28] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha224_expected[28] = { 0x69, 0xfd, 0x84, 0x30, 0xd9, 0x4a, 0x44, 0x96,
                                          0x41, 0xc4, 0xab, 0xab, 0x89, 0x53, 0xa9, 0x1f,
                                          0x4b, 0xfa, 0x5f, 0x2c, 0xa0, 0x72, 0x5f, 0x6b,
                                          0xec, 0xd1, 0x47, 0xf9
                                        };

#if SOC_SHA_SUPPORT_DMA
    if (is_dma) {
        sha256_dma(SHA2_224, buffer, BUFFER_SZ, sha224_result);
    } else
#endif
    {
        sha256_block(SHA2_224, buffer, BUFFER_SZ, sha224_result);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha224_expected, sha224_result, sizeof(sha224_expected));
    heap_caps_free(buffer);
}

#endif /* SOC_SHA_SUPPORT_SHA224 */

#if SOC_SHA_SUPPORT_SHA256

static void test_sha256(bool is_dma)
{
    uint8_t sha256_result[32] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha256_expected[32] = { 0x0c, 0x67, 0x8d, 0x7b, 0x8a, 0x3e, 0x9e, 0xc0,
                                          0xb5, 0x61, 0xaa, 0x51, 0xd8, 0xfd, 0x42, 0x70,
                                          0xd6, 0x11, 0x2a, 0xec, 0x4c, 0x72, 0x9b, 0x2c,
                                          0xa4, 0xc6, 0x04, 0x80, 0x93, 0x4d, 0xc9, 0x99
                                        };

#if SOC_SHA_SUPPORT_DMA
    if (is_dma) {
        sha256_dma(SHA2_256, buffer, BUFFER_SZ, sha256_result);
    } else
#endif
    {
        sha256_block(SHA2_256, buffer, BUFFER_SZ, sha256_result);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha256_expected, sha256_result, sizeof(sha256_expected));
    heap_caps_free(buffer);
}

#endif /*SOC_SHA_SUPPORT_SHA256 */

#if SOC_SHA_SUPPORT_SHA384

static void test_sha384(bool is_dma)
{
    uint8_t sha384_result[48] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha384_expected[48] = { 0xf2, 0x7c, 0x75, 0x16, 0xa9, 0xe6, 0xe5, 0xe2,
                                          0x4d, 0x8b, 0xe4, 0x6b, 0xc5, 0xb3, 0x25, 0xb1,
                                          0x10, 0xc2, 0xb4, 0x7d, 0xb7, 0xe1, 0xee, 0x1c,
                                          0xbd, 0xde, 0x52, 0x9d, 0xaa, 0x31, 0xda, 0x88,
                                          0xfe, 0xec, 0xd5, 0x38, 0x59, 0x28, 0x93, 0xc7,
                                          0x1c, 0x1a, 0x0b, 0x3b, 0x4e, 0x06, 0x48, 0xa7
                                        };

#if SOC_SHA_SUPPORT_DMA
    if (is_dma) {
        sha512_dma(SHA2_384, buffer, BUFFER_SZ, sha384_result);
    } else
#endif
    {
        sha512_block(SHA2_384, buffer, BUFFER_SZ, sha384_result);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha384_expected, sha384_result, sizeof(sha384_expected));
    heap_caps_free(buffer);
}

#endif /* SOC_SHA_SUPPORT_SHA384 */

#if SOC_SHA_SUPPORT_SHA512

static void test_sha512(bool is_dma)
{
    uint8_t sha512_result[64] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha512_expected[64] = { 0x7f, 0xca, 0x1c, 0x81, 0xc6, 0xc7, 0x1e, 0x49,
                                          0x1f, 0x4a, 0x35, 0x50, 0xb0, 0x0c, 0xd9, 0xbf,
                                          0x3e, 0xba, 0x90, 0x31, 0x08, 0xc7, 0xb3, 0xf0,
                                          0x58, 0x11, 0xd3, 0x29, 0xee, 0xa0, 0x4f, 0x3b,
                                          0xe4, 0x60, 0xd2, 0xc7, 0x2e, 0x50, 0x39, 0x68,
                                          0xf7, 0x27, 0x2e, 0x71, 0xbc, 0x9f, 0x10, 0xfc,
                                          0x9d, 0x75, 0xb5, 0x57, 0x74, 0x8d, 0xb9, 0x4b,
                                          0x69, 0x1a, 0x9c, 0x5f, 0x30, 0x61, 0xca, 0x3b
                                        };

#if SOC_SHA_SUPPORT_DMA
    if (is_dma) {
        sha512_dma(SHA2_512, buffer, BUFFER_SZ, sha512_result);
    } else
#endif
    {
        sha512_block(SHA2_512, buffer, BUFFER_SZ, sha512_result);
    }
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha512_expected, sha512_result, sizeof(sha512_expected));
    heap_caps_free(buffer);
}

#endif /* SOC_SHA_SUPPORT_SHA512 */

#if SOC_SHA_SUPPORT_SHA512_T

static void test_sha512t(bool is_dma)
{
    unsigned char sha512[64], k;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            k = i * 2 + j;
            if (i > 1) {
                k = (i - 2) * 2 + j;
            }

#if SOC_SHA_SUPPORT_DMA
            if (is_dma) {
                sha512t_dma(sha512T_algo[i], sha512T_test_buf[j], sha512T_test_buflen[j], sha512, sha512T_t_len[i]);
            } else
#endif
            {
                sha512t_block(sha512T_algo[i], sha512T_test_buf[j], sha512T_test_buflen[j], sha512, sha512T_t_len[i]);
            }
            TEST_ASSERT_EQUAL_HEX8_ARRAY(sha512_test_sum[k], sha512, sha512T_t_len[i] / 8);
        }
    }
}

#endif /* SOC_SHA_SUPPORT_SHA512_T */

TEST_GROUP(sha);

TEST_SETUP(sha)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(400, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(sha)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

#if SOC_SHA_SUPPORT_SHA1

TEST(sha, test_sha1_block)
{
    test_sha1(0);
}

#if SOC_SHA_SUPPORT_DMA
TEST(sha, test_sha1_dma)
{
    test_sha1(1);
}
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA1 */

#if SOC_SHA_SUPPORT_SHA224

TEST(sha, test_sha224_block)
{
    test_sha224(0);
}

#if SOC_SHA_SUPPORT_DMA
TEST(sha, test_sha224_dma)
{
    test_sha224(1);
}
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA224 */

#if SOC_SHA_SUPPORT_SHA256

TEST(sha, test_sha256_block)
{
    test_sha256(0);
}

#if SOC_SHA_SUPPORT_DMA
TEST(sha, test_sha256_dma)
{
    test_sha256(1);
}
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA256 */

#if SOC_SHA_SUPPORT_SM3

/* Reference digests for SM3 test inputs (cross-checked with OpenSSL dgst -sm3):
 *   [0] "abc"              (GM/T 0004-2012 spec vector)
 *   [1] "abcd" x 16         (one full 64-byte block)
 *   [2] 0xEE x 1030         (multi-block, non-aligned tail)
 */
static const uint8_t sm3_expected[3][32] = {
    {
        0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
        0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
        0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
        0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0,
    },
    {
        0xde, 0xbe, 0x9f, 0xf9, 0x22, 0x75, 0xb8, 0xa1,
        0x38, 0x60, 0x48, 0x89, 0xc1, 0x8e, 0x5a, 0x4d,
        0x6f, 0xdb, 0x70, 0xe5, 0x38, 0x7e, 0x57, 0x65,
        0x29, 0x3d, 0xcb, 0xa3, 0x9c, 0x0c, 0x57, 0x32,
    },
    {
        0xfb, 0x7d, 0xd6, 0x4a, 0xd5, 0x0d, 0x5a, 0x16,
        0x2e, 0x92, 0x90, 0xa0, 0xd8, 0x3f, 0xa6, 0x01,
        0x86, 0x9b, 0x79, 0xf2, 0xda, 0xff, 0xbe, 0x16,
        0xac, 0x6b, 0x6c, 0x06, 0x52, 0x21, 0x56, 0x0a,
    },
};

/* Skip the esp_sm3 API tests when the chip forbids the SM crypto functions. */
static void sm3_api_skip_if_disabled(void)
{
    if (!sha_hal_is_sm3_supported()) {
        TEST_IGNORE_MESSAGE("SM crypto disabled by eFuse (DIS_SM_CRYPT)");
    }
}

TEST(sha, test_sm3_api_one_shot)
{
    uint8_t digest[ESP_SM3_DIGEST_LEN];

    sm3_api_skip_if_disabled();

    TEST_ASSERT_EQUAL(ESP_OK, esp_sm3("abc", 3, digest, sizeof(digest)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sm3_expected[0], digest, sizeof(digest));

    uint8_t one_block_input[ESP_SM3_BLOCK_LEN];
    for (size_t i = 0; i < sizeof(one_block_input); i += 4) {
        memcpy(&one_block_input[i], "abcd", 4);
    }
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm3(one_block_input, sizeof(one_block_input), digest, sizeof(digest)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sm3_expected[1], digest, sizeof(digest));

    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm3(buffer, BUFFER_SZ, digest, sizeof(digest)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sm3_expected[2], digest, sizeof(digest));
    heap_caps_free(buffer);
}

/* Feed the message in uneven pieces. Every piece that does not end on a block
   boundary forces the driver to save and restore the digest state. */
TEST(sha, test_sm3_api_streaming)
{
    static const size_t chunk_sizes[] = { 1, 62, 1, 64, 3, 200, 128, 129 };
    uint8_t digest[ESP_SM3_DIGEST_LEN];
    esp_sm3_ctx_handle_t ctx;

    sm3_api_skip_if_disabled();

    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    TEST_ASSERT_EQUAL(ESP_OK, esp_sm3_create(&ctx));

    size_t offset = 0;
    size_t index = 0;
    while (offset < BUFFER_SZ) {
        size_t chunk = chunk_sizes[index % (sizeof(chunk_sizes) / sizeof(chunk_sizes[0]))];
        if (chunk > BUFFER_SZ - offset) {
            chunk = BUFFER_SZ - offset;
        }
        TEST_ASSERT_EQUAL(ESP_OK, esp_sm3_update(ctx, buffer + offset, chunk));
        offset += chunk;
        index++;
    }

    TEST_ASSERT_EQUAL(ESP_OK, esp_sm3_finish(ctx, digest, sizeof(digest)));
    esp_sm3_delete(ctx);
    heap_caps_free(buffer);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(sm3_expected[2], digest, sizeof(digest));
}

TEST(sha, test_sm3_api_bad_args)
{
    uint8_t digest[ESP_SM3_DIGEST_LEN];

    sm3_api_skip_if_disabled();

    /* A short output buffer must be rejected before any byte reaches it. */
    memset(digest, 0xA5, sizeof(digest));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE,
                      esp_sm3("abc", 3, digest, ESP_SM3_DIGEST_LEN - 1));
    for (size_t i = 0; i < sizeof(digest); i++) {
        TEST_ASSERT_EQUAL_HEX8(0xA5, digest[i]);
    }

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm3("abc", 3, NULL, sizeof(digest)));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm3(NULL, 3, digest, sizeof(digest)));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm3_create(NULL));

    /* An empty message is valid input. The digest comes from the padding
       block alone, so it proves that the pad-only path works. */
    static const uint8_t sm3_expected_empty[ESP_SM3_DIGEST_LEN] = {
        0x1a, 0xb2, 0x1d, 0x83, 0x55, 0xcf, 0xa1, 0x7f,
        0x8e, 0x61, 0x19, 0x48, 0x31, 0xe8, 0x1a, 0x8f,
        0x22, 0xbe, 0xc8, 0xc7, 0x28, 0xfe, 0xfb, 0x74,
        0x7e, 0xd0, 0x35, 0xeb, 0x50, 0x82, 0xaa, 0x2b,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm3(NULL, 0, digest, sizeof(digest)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sm3_expected_empty, digest, sizeof(digest));
}

#endif /* SOC_SHA_SUPPORT_SM3 */

#if SOC_SHA_SUPPORT_SHA384

TEST(sha, test_sha384_block)
{
    test_sha384(0);
}

#if SOC_SHA_SUPPORT_DMA
TEST(sha, test_sha384_dma)
{
    test_sha384(1);
}
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA384 */

#if SOC_SHA_SUPPORT_SHA512

TEST(sha, test_sha512_block)
{
    test_sha512(0);
}

#if SOC_SHA_SUPPORT_DMA
TEST(sha, test_sha512_dma)
{
    test_sha512(1);
}
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA512 */

#if SOC_SHA_SUPPORT_SHA512_T

TEST(sha, test_sha512t_block)
{
    test_sha512t(0);
}

#if SOC_SHA_SUPPORT_DMA
TEST(sha, test_sha512t_dma)
{
    test_sha512t(1);
}
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif // SOC_SHA_SUPPORT_SHA512_T

#endif // SOC_SHA_SUPPORTED

TEST_GROUP_RUNNER(sha)
{
#if SOC_SHA_SUPPORTED

#if SOC_SHA_SUPPORT_SHA1
    RUN_TEST_CASE(sha, test_sha1_block);
#if SOC_SHA_SUPPORT_DMA
    RUN_TEST_CASE(sha, test_sha1_dma);
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA1 */

#if SOC_SHA_SUPPORT_SHA224
    RUN_TEST_CASE(sha, test_sha224_block);
#if SOC_SHA_SUPPORT_DMA
    RUN_TEST_CASE(sha, test_sha224_dma);
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA224 */

#if SOC_SHA_SUPPORT_SHA256
    RUN_TEST_CASE(sha, test_sha256_block);
#if SOC_SHA_SUPPORT_DMA
    RUN_TEST_CASE(sha, test_sha256_dma);
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA256 */

#if SOC_SHA_SUPPORT_SM3
    RUN_TEST_CASE(sha, test_sm3_api_one_shot);
    RUN_TEST_CASE(sha, test_sm3_api_streaming);
    RUN_TEST_CASE(sha, test_sm3_api_bad_args);
#endif /* SOC_SHA_SUPPORT_SM3 */

#if SOC_SHA_SUPPORT_SHA384
    RUN_TEST_CASE(sha, test_sha384_block);
#if SOC_SHA_SUPPORT_DMA
    RUN_TEST_CASE(sha, test_sha384_dma);
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA384 */

#if SOC_SHA_SUPPORT_SHA512
    RUN_TEST_CASE(sha, test_sha512_block);
#if SOC_SHA_SUPPORT_DMA
    RUN_TEST_CASE(sha, test_sha512_dma);
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /* SOC_SHA_SUPPORT_SHA512 */

#if SOC_SHA_SUPPORT_SHA512_T
    RUN_TEST_CASE(sha, test_sha512t_block);
#if SOC_SHA_SUPPORT_DMA
    RUN_TEST_CASE(sha, test_sha512t_dma);
#endif /* SOC_SHA_SUPPORT_DMA*/
#endif // SOC_SHA_SUPPORT_SHA512_T

#endif /* SOC_SHA_SUPPORTED */
}
