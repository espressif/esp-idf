/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Tests for MD5 and HMAC-MD5 functions.
 *
 * These tests verify the basic functionality of md5_vector, hmac_md5, and
 * hmac_md5_vector APIs. On ESP32-C3 and ESP32-S3, these may use ROM implementations
 * which need verification after PSA migration changes.
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "crypto/crypto.h"
#include "crypto/md5.h"
#include "test_wpa_supplicant_common.h"

/* MD5 output length */
#define MD5_DIGEST_LEN 16

/*
 * RFC 1321 MD5 Test Vectors
 */

/* MD5("") = d41d8cd98f00b204e9800998ecf8427e */
static const uint8_t md5_empty_expected[MD5_DIGEST_LEN] = {
    0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
    0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e
};

/* MD5("abc") = 900150983cd24fb0d6963f7d28e17f72 */
static const uint8_t md5_abc_input[] = "abc";
static const uint8_t md5_abc_expected[MD5_DIGEST_LEN] = {
    0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
    0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
};

/*
 * RFC 2104 HMAC-MD5 Test Vectors
 */

/* Test case 1: key = 0x0b repeated 16 times, data = "Hi There" */
static const uint8_t hmac_md5_key1[16] = {
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};
static const uint8_t hmac_md5_data1[] = "Hi There";
static const uint8_t hmac_md5_expected1[MD5_DIGEST_LEN] = {
    0x92, 0x94, 0x72, 0x7a, 0x36, 0x38, 0xbb, 0x1c,
    0x13, 0xf4, 0x8e, 0xf8, 0x15, 0x8b, 0xfc, 0x9d
};

/* Test case 2: key = "Jefe", data = "what do ya want for nothing?" */
static const uint8_t hmac_md5_key2[] = "Jefe";
static const uint8_t hmac_md5_data2[] = "what do ya want for nothing?";
static const uint8_t hmac_md5_expected2[MD5_DIGEST_LEN] = {
    0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03,
    0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38
};

TEST_CASE("Test md5_vector function", "[wpa_crypto][rom_md5]")
{
    set_leak_threshold(100);

    uint8_t digest[MD5_DIGEST_LEN];
    int ret;

    /* Test 1: MD5 of empty string */
    {
        const uint8_t *addr[1] = { (const uint8_t *)"" };
        size_t len[1] = { 0 };

        memset(digest, 0, sizeof(digest));
        ret = md5_vector(1, addr, len, digest);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(md5_empty_expected, digest, MD5_DIGEST_LEN);
    }

    /* Test 2: MD5 of "abc" */
    {
        const uint8_t *addr[1] = { md5_abc_input };
        size_t len[1] = { 3 };

        memset(digest, 0, sizeof(digest));
        ret = md5_vector(1, addr, len, digest);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(md5_abc_expected, digest, MD5_DIGEST_LEN);
    }

    /* Test 3: MD5 with multiple elements (should produce same result as concatenated) */
    {
        /* "abc" split into "a" + "bc" */
        const uint8_t *addr[2] = { (const uint8_t *)"a", (const uint8_t *)"bc" };
        size_t len[2] = { 1, 2 };

        memset(digest, 0, sizeof(digest));
        ret = md5_vector(2, addr, len, digest);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(md5_abc_expected, digest, MD5_DIGEST_LEN);
    }
}

TEST_CASE("Test hmac_md5 function", "[wpa_crypto][rom_md5]")
{
    set_leak_threshold(100);

    uint8_t mac[MD5_DIGEST_LEN];
    int ret;

    /* Test 1: RFC 2104 test vector - key=0x0b*16, data="Hi There" */
    {
        memset(mac, 0, sizeof(mac));
        ret = hmac_md5(hmac_md5_key1, sizeof(hmac_md5_key1),
                       hmac_md5_data1, strlen((const char *)hmac_md5_data1),
                       mac);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(hmac_md5_expected1, mac, MD5_DIGEST_LEN);
    }

    /* Test 2: RFC 2104 test vector - key="Jefe", data="what do ya want..." */
    {
        memset(mac, 0, sizeof(mac));
        ret = hmac_md5(hmac_md5_key2, strlen((const char *)hmac_md5_key2),
                       hmac_md5_data2, strlen((const char *)hmac_md5_data2),
                       mac);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(hmac_md5_expected2, mac, MD5_DIGEST_LEN);
    }
}

TEST_CASE("Test hmac_md5_vector function", "[wpa_crypto][rom_md5]")
{
    set_leak_threshold(100);

    uint8_t mac[MD5_DIGEST_LEN];
    int ret;

    /* Test 1: Single element (should match hmac_md5 result) */
    {
        const uint8_t *addr[1] = { hmac_md5_data1 };
        size_t len[1] = { strlen((const char *)hmac_md5_data1) };

        memset(mac, 0, sizeof(mac));
        ret = hmac_md5_vector(hmac_md5_key1, sizeof(hmac_md5_key1),
                              1, addr, len, mac);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(hmac_md5_expected1, mac, MD5_DIGEST_LEN);
    }

    /* Test 2: Multiple elements (data split into parts) */
    {
        /* "Hi There" split into "Hi " + "There" */
        const uint8_t *addr[2] = { (const uint8_t *)"Hi ", (const uint8_t *)"There" };
        size_t len[2] = { 3, 5 };

        memset(mac, 0, sizeof(mac));
        ret = hmac_md5_vector(hmac_md5_key1, sizeof(hmac_md5_key1),
                              2, addr, len, mac);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(hmac_md5_expected1, mac, MD5_DIGEST_LEN);
    }

    /* Test 3: Three elements */
    {
        /* "Hi There" split into "Hi" + " " + "There" */
        const uint8_t *addr[3] = { (const uint8_t *)"Hi", (const uint8_t *)" ", (const uint8_t *)"There" };
        size_t len[3] = { 2, 1, 5 };

        memset(mac, 0, sizeof(mac));
        ret = hmac_md5_vector(hmac_md5_key1, sizeof(hmac_md5_key1),
                              3, addr, len, mac);
        TEST_ASSERT_EQUAL_INT(0, ret);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(hmac_md5_expected1, mac, MD5_DIGEST_LEN);
    }
}
