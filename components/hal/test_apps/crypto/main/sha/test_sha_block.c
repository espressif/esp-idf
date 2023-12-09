/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_types.h"
#include "soc/soc_caps.h"
#include "esp_heap_caps.h"
#include "unity.h"
#include "esp_heap_caps.h"

#include "memory_checks.h"
#include "unity_fixture.h"
#include "sha_block.h"

#if SOC_SHA_SUPPORTED

TEST_GROUP(sha);

TEST_SETUP(sha)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(sha)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

#if SOC_SHA_SUPPORT_SHA1

TEST(sha, test_sha1)
{
    uint8_t sha1_result[20] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha1_expected[20] = { 0x09, 0x23, 0x02, 0xfb, 0x2d, 0x36, 0x42, 0xec,
                                        0xc5, 0xfa, 0xd5, 0x8f, 0xdb, 0xc3, 0x8d, 0x5c,
                                        0x97, 0xd6, 0x17, 0xee };

    sha1_block(SHA1, buffer, BUFFER_SZ, sha1_result);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha1_expected, sha1_result, sizeof(sha1_expected));
    heap_caps_free(buffer);
}

#endif /* SOC_SHA_SUPPORT_SHA1 */


#if SOC_SHA_SUPPORT_SHA224

TEST(sha, test_sha224)
{
    uint8_t sha224_result[28] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha224_expected[28] = { 0x69, 0xfd, 0x84, 0x30, 0xd9, 0x4a, 0x44, 0x96,
                                        0x41, 0xc4, 0xab, 0xab, 0x89, 0x53, 0xa9, 0x1f,
                                        0x4b, 0xfa, 0x5f, 0x2c, 0xa0, 0x72, 0x5f, 0x6b,
                                        0xec, 0xd1, 0x47, 0xf9};

    sha256_block(SHA2_224, buffer, BUFFER_SZ, sha224_result);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha224_expected, sha224_result, sizeof(sha224_expected));
    heap_caps_free(buffer);
}

#endif /* SOC_SHA_SUPPORT_SHA224 */


#if SOC_SHA_SUPPORT_SHA256

TEST(sha, test_sha256)
{
    uint8_t sha256_result[32] = { 0 };
    uint8_t *buffer = heap_caps_calloc(BUFFER_SZ, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(buffer);
    memset(buffer, 0xEE, BUFFER_SZ);

    const uint8_t sha256_expected[32] = { 0x0c, 0x67, 0x8d, 0x7b, 0x8a, 0x3e, 0x9e, 0xc0,
                                        0xb5, 0x61, 0xaa, 0x51, 0xd8, 0xfd, 0x42, 0x70,
                                        0xd6, 0x11, 0x2a, 0xec, 0x4c, 0x72, 0x9b, 0x2c,
                                        0xa4, 0xc6, 0x04, 0x80, 0x93, 0x4d, 0xc9, 0x99 };

    sha256_block(SHA2_256, buffer, BUFFER_SZ, sha256_result);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha256_expected, sha256_result, sizeof(sha256_expected));
    heap_caps_free(buffer);
}

#endif /* SOC_SHA_SUPPORT_SHA256 */


#if SOC_SHA_SUPPORT_SHA384

TEST(sha, test_sha384)
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
                                        0x1c, 0x1a, 0x0b, 0x3b, 0x4e, 0x06, 0x48, 0xa7 };

    sha512_block(SHA2_384, buffer, BUFFER_SZ, sha384_result);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha384_expected, sha384_result, sizeof(sha384_expected));
    heap_caps_free(buffer);
}
#endif /* SOC_SHA_SUPPORT_SHA384 */

#if SOC_SHA_SUPPORT_SHA512

TEST(sha, test_sha512)
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
                                        0x69, 0x1a, 0x9c, 0x5f, 0x30, 0x61, 0xca, 0x3b };


    sha512_block(SHA2_512, buffer, BUFFER_SZ, sha512_result);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(sha512_expected, sha512_result, sizeof(sha512_expected));
    heap_caps_free(buffer);
}
#endif /* SOC_SHA_SUPPORT_SHA512 */


#if SOC_SHA_SUPPORT_SHA512_T

TEST(sha, test_sha512t)
{
    unsigned char sha512[64], k;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            k = i * 2 + j;
            if (i > 1) {
                k = (i - 2) * 2 + j;
            }

            sha512t_block(sha512T_algo[i], sha512T_test_buf[j], sha512T_test_buflen[j], sha512, sha512T_t_len[i]);

            TEST_ASSERT_EQUAL_HEX8_ARRAY(sha512_test_sum[k], sha512, sha512T_t_len[i] / 8);
        }
    }
}

#endif // SOC_SHA_SUPPORT_SHA512_T

#endif // SOC_SHA_SUPPORTED

TEST_GROUP_RUNNER(sha)
{
#if SOC_SHA_SUPPORTED

#if SOC_SHA_SUPPORT_SHA1
    RUN_TEST_CASE(sha, test_sha1);
#endif /* SOC_SHA_SUPPORT_SHA1 */

#if SOC_SHA_SUPPORT_SHA224
    RUN_TEST_CASE(sha, test_sha224);
#endif /* SOC_SHA_SUPPORT_SHA224 */

#if SOC_SHA_SUPPORT_SHA256
    RUN_TEST_CASE(sha, test_sha256);
#endif /* SOC_SHA_SUPPORT_SHA256 */

#if SOC_SHA_SUPPORT_SHA384
    RUN_TEST_CASE(sha, test_sha384);
#endif /* SOC_SHA_SUPPORT_SHA384 */

#if SOC_SHA_SUPPORT_SHA512
    RUN_TEST_CASE(sha, test_sha512);
#endif /* SOC_SHA_SUPPORT_SHA512 */

#if SOC_SHA_SUPPORT_SHA512_T
    RUN_TEST_CASE(sha, test_sha512t);
#endif // SOC_SHA_SUPPORT_SHA512_T

#endif /* SOC_SHA_SUPPORTED */
}
