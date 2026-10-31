/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "soc/soc_caps.h"

#if SOC_ECC_SUPPORT_CURVE_SM2 && SOC_ECC_EXTENDED_MODES_SUPPORTED

#include "unity.h"
#include "esp_sm2.h"

/* Provenance of every vector in this file: self-generated, then cross-checked
   with independent Python EC math against the GB/T 32918.5 recommended-curve
   parameters. None of them is taken from the standard's worked-example annex. */

/* KAT on the recommended SM2 curve. */
static const uint8_t sm2_kat_d[32] = {
    0x12, 0x8b, 0x2f, 0xa8, 0xbd, 0x43, 0x3c, 0x6c,
    0x06, 0x8c, 0x8d, 0x80, 0x3d, 0xff, 0x79, 0x79,
    0x2a, 0x51, 0x9a, 0x55, 0x17, 0x1b, 0x1b, 0x65,
    0x0c, 0x23, 0x66, 0x1d, 0x15, 0x89, 0x72, 0x63,
};
static const uint8_t sm2_kat_pub_x[32] = {
    0xd5, 0x54, 0x8c, 0x78, 0x25, 0xcb, 0xb5, 0x61,
    0x50, 0xa3, 0x50, 0x6c, 0xd5, 0x74, 0x64, 0xaf,
    0x8a, 0x1a, 0xe0, 0x51, 0x9d, 0xfa, 0xf3, 0xc5,
    0x82, 0x21, 0xdc, 0x81, 0x0c, 0xaf, 0x28, 0xdd,
};
static const uint8_t sm2_kat_pub_y[32] = {
    0x92, 0x10, 0x73, 0x76, 0x8f, 0xe3, 0xd5, 0x9c,
    0xe5, 0x4e, 0x79, 0xa4, 0x94, 0x45, 0xcf, 0x73,
    0xfe, 0xd2, 0x30, 0x86, 0x53, 0x70, 0x27, 0x26,
    0x4d, 0x16, 0x89, 0x46, 0xd4, 0x79, 0x53, 0x3e,
};

/* Curve order minus one: the first private key value the API must reject. */
static const uint8_t sm2_n_minus_1[32] = {
    0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x72, 0x03, 0xdf, 0x6b, 0x21, 0xc6, 0x05, 0x2b,
    0x53, 0xbb, 0xf4, 0x09, 0x39, 0xd5, 0x41, 0x22,
};

/* Signature over sm2_kat_e by sm2_kat_d. */
static const uint8_t sm2_kat_e[32] = {
    0xb5, 0x24, 0xf5, 0x52, 0xcd, 0x82, 0xb8, 0xb0,
    0x28, 0x47, 0x6e, 0x00, 0x5c, 0x37, 0x7f, 0xb1,
    0x9a, 0x87, 0xe6, 0xfc, 0x68, 0x2d, 0x48, 0xbb,
    0x5d, 0x42, 0xe3, 0xd9, 0xb9, 0xef, 0xfe, 0x76,
};
static const uint8_t sm2_kat_r[32] = {
    0xab, 0xcb, 0x7c, 0xff, 0x24, 0xc7, 0x8e, 0x7b,
    0xe2, 0x64, 0x67, 0x3d, 0xe0, 0x6d, 0xc1, 0x21,
    0x9e, 0x47, 0xb6, 0x55, 0x1d, 0xc9, 0xc4, 0xdd,
    0x5f, 0xda, 0x04, 0x80, 0x75, 0x95, 0x47, 0x4c,
};
static const uint8_t sm2_kat_s[32] = {
    0x14, 0x5c, 0x15, 0xa7, 0x7d, 0x6a, 0x5b, 0x69,
    0x2f, 0xf0, 0x7b, 0x34, 0xcc, 0x8c, 0xa1, 0x81,
    0xdd, 0x23, 0x08, 0x44, 0xf8, 0xcc, 0x22, 0xd2,
    0x66, 0x1c, 0x60, 0x5e, 0x59, 0x51, 0x9d, 0x53,
};

/* Digest above the curve order: e_big = n + 5. The signature below is valid
   for e = 5, so verify must reduce e_big first. This is the only vector that
   takes the subtract branch of the reduction on an accepted signature. */
static const uint8_t sm2_red_e_big[32] = {
    0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x72, 0x03, 0xdf, 0x6b, 0x21, 0xc6, 0x05, 0x2b,
    0x53, 0xbb, 0xf4, 0x09, 0x39, 0xd5, 0x41, 0x28,
};
static const uint8_t sm2_red_r[32] = {
    0xa0, 0x13, 0x37, 0x29, 0xad, 0x64, 0x26, 0x19,
    0xe0, 0x93, 0x90, 0xc8, 0x5f, 0x24, 0x43, 0x5d,
    0x2d, 0xa8, 0x2f, 0xbc, 0x4c, 0x28, 0xf3, 0x51,
    0x7d, 0x0c, 0xf8, 0xf0, 0xb8, 0x3a, 0x4d, 0x54,
};
static const uint8_t sm2_red_s[32] = {
    0x04, 0x51, 0x7a, 0x10, 0x50, 0x09, 0x78, 0x3b,
    0xd0, 0x31, 0xdc, 0x57, 0xe2, 0xf5, 0xc6, 0x8e,
    0x73, 0x1b, 0xc5, 0xfb, 0xa1, 0x51, 0x87, 0x86,
    0xda, 0x36, 0xb6, 0x6c, 0x74, 0x8a, 0x08, 0x8e,
};

/* Signature components shared by the two vectors below: r = 3 and s = 2, so
   t = (r + s) mod n = 5. Both are in range, so verify does the point math. */
static const uint8_t sm2_small_r[32] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
};
static const uint8_t sm2_small_s[32] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
};

/* Public key (2 * 5^-1 mod n) * G, so t*P == s*G and verify must double the
   point instead of adding two equal addends. An attacker can reach this, so
   the correct answer is to accept. sm2_dbl_e makes the equation hold. */
static const uint8_t sm2_dbl_pub_x[32] = {
    0x8a, 0xbb, 0xb1, 0xd8, 0x9a, 0xac, 0xa1, 0xae,
    0x12, 0xd9, 0xf4, 0xac, 0xe4, 0x9d, 0xda, 0xfb,
    0x88, 0x01, 0x76, 0x9f, 0x2c, 0x33, 0xa9, 0x4e,
    0x12, 0x19, 0x6c, 0x47, 0x31, 0x93, 0x80, 0xd2,
};
static const uint8_t sm2_dbl_pub_y[32] = {
    0x1e, 0x84, 0xf9, 0x95, 0xa1, 0x72, 0xa1, 0x69,
    0x8c, 0x53, 0xb6, 0x99, 0x75, 0x1c, 0xe9, 0x98,
    0x06, 0xf1, 0x7f, 0x90, 0x9b, 0xd0, 0x09, 0x0b,
    0x4d, 0x3d, 0xe4, 0x6d, 0x2c, 0xf0, 0xc0, 0xd4,
};
static const uint8_t sm2_dbl_e[32] = {
    0x3d, 0xc6, 0xaf, 0x8d, 0xfa, 0x39, 0x7c, 0xdb,
    0xd5, 0x7e, 0xfa, 0xd0, 0x09, 0xbe, 0x12, 0x96,
    0x71, 0x69, 0xd7, 0x20, 0x4b, 0xf9, 0x71, 0xad,
    0xa1, 0xa5, 0xad, 0x3c, 0x05, 0x34, 0x72, 0x51,
};

/* Negation of the key above: (-2 * 5^-1 mod n) * G. The X coordinate is the
   same, so the test reuses sm2_dbl_pub_x. Now t*P == -s*G, the sum is the
   point at infinity, and verify must reject. */
static const uint8_t sm2_inf_pub_y[32] = {
    0xe1, 0x7b, 0x06, 0x69, 0x5e, 0x8d, 0x5e, 0x96,
    0x73, 0xac, 0x49, 0x66, 0x8a, 0xe3, 0x16, 0x67,
    0xf9, 0x0e, 0x80, 0x6e, 0x64, 0x2f, 0xf6, 0xf5,
    0xb2, 0xc2, 0x1b, 0x92, 0xd3, 0x0f, 0x3f, 0x2b,
};

TEST_CASE("SM2 public key derivation matches KAT", "[sm2]")
{
    uint8_t x[32] = {0};
    uint8_t y[32] = {0};
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_get_pubkey(sm2_kat_d, x, y));
    TEST_ASSERT_EQUAL_MEMORY(sm2_kat_pub_x, x, 32);
    TEST_ASSERT_EQUAL_MEMORY(sm2_kat_pub_y, y, 32);
}

TEST_CASE("SM2 get_pubkey rejects invalid private keys", "[sm2]")
{
    uint8_t x[32];
    uint8_t y[32];
    uint8_t bad[32] = {0};                       /* d = 0 */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_get_pubkey(bad, x, y));
    memset(bad, 0xff, sizeof(bad));              /* d >= n - 1 */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_get_pubkey(bad, x, y));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_get_pubkey(NULL, x, y));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_get_pubkey(sm2_kat_d, NULL, y));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_get_pubkey(sm2_kat_d, x, NULL));
}

TEST_CASE("SM2 get_pubkey accepts d = n-2 and rejects d = n-1", "[sm2]")
{
    uint8_t x[32];
    uint8_t y[32];
    uint8_t d[32];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_get_pubkey(sm2_n_minus_1, x, y));
    memcpy(d, sm2_n_minus_1, sizeof(d));
    d[31] = 0x21;                                /* d = n - 2, the largest valid key */
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_get_pubkey(d, x, y));
}

TEST_CASE("SM2 verify accepts the KAT signature", "[sm2]")
{
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_verify(sm2_kat_e, sm2_kat_pub_x, sm2_kat_pub_y,
                                             sm2_kat_r, sm2_kat_s));
}

TEST_CASE("SM2 verify rejects tampered inputs", "[sm2]")
{
    uint8_t bad[32];

    memcpy(bad, sm2_kat_r, 32);
    bad[7] ^= 0x01;
    TEST_ASSERT_EQUAL(ESP_FAIL, esp_sm2_verify(sm2_kat_e, sm2_kat_pub_x, sm2_kat_pub_y,
                                               bad, sm2_kat_s));

    memcpy(bad, sm2_kat_e, 32);
    bad[0] ^= 0x80;
    TEST_ASSERT_EQUAL(ESP_FAIL, esp_sm2_verify(bad, sm2_kat_pub_x, sm2_kat_pub_y,
                                               sm2_kat_r, sm2_kat_s));

    memset(bad, 0xff, sizeof(bad));              /* digest > n: reduced before use */
    TEST_ASSERT_EQUAL(ESP_FAIL, esp_sm2_verify(bad, sm2_kat_pub_x, sm2_kat_pub_y,
                                               sm2_kat_r, sm2_kat_s));

    memcpy(bad, sm2_kat_pub_x, 32);              /* off-curve public key */
    bad[10] ^= 0xff;
    TEST_ASSERT_EQUAL(ESP_FAIL, esp_sm2_verify(sm2_kat_e, bad, sm2_kat_pub_y,
                                               sm2_kat_r, sm2_kat_s));
}

TEST_CASE("SM2 verify rejects out-of-range r and s", "[sm2]")
{
    uint8_t zero[32] = {0};
    uint8_t big[32];
    memset(big, 0xff, sizeof(big));              /* >= n */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_verify(sm2_kat_e, sm2_kat_pub_x,
                                                          sm2_kat_pub_y, zero, sm2_kat_s));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_verify(sm2_kat_e, sm2_kat_pub_x,
                                                          sm2_kat_pub_y, sm2_kat_r, big));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_verify(NULL, sm2_kat_pub_x,
                                                          sm2_kat_pub_y, sm2_kat_r, sm2_kat_s));
}

TEST_CASE("SM2 verify reduces a digest above the curve order", "[sm2]")
{
    uint8_t e_red[32] = {0};
    e_red[31] = 5;                               /* e_big - n */

    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_verify(sm2_red_e_big, sm2_kat_pub_x, sm2_kat_pub_y,
                                             sm2_red_r, sm2_red_s));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_verify(e_red, sm2_kat_pub_x, sm2_kat_pub_y,
                                             sm2_red_r, sm2_red_s));
}

TEST_CASE("SM2 verify accepts a signature that doubles s*G", "[sm2]")
{
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_verify(sm2_dbl_e, sm2_dbl_pub_x, sm2_dbl_pub_y,
                                             sm2_small_r, sm2_small_s));
}

TEST_CASE("SM2 verify rejects a sum at the point at infinity", "[sm2]")
{
    TEST_ASSERT_EQUAL(ESP_FAIL, esp_sm2_verify(sm2_kat_e, sm2_dbl_pub_x, sm2_inf_pub_y,
                                               sm2_small_r, sm2_small_s));
}

TEST_CASE("SM2 sign then verify round-trip", "[sm2]")
{
    uint8_t r[32], s[32];
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_sign(sm2_kat_e, sm2_kat_d, r, s));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_verify(sm2_kat_e, sm2_kat_pub_x, sm2_kat_pub_y, r, s));
    /* Tamper the least significant byte: the change never carries, so the
       tampered r stays in range and the failure comes from the math. */
    r[31] ^= 0x40;
    TEST_ASSERT_EQUAL(ESP_FAIL, esp_sm2_verify(sm2_kat_e, sm2_kat_pub_x, sm2_kat_pub_y, r, s));
}

TEST_CASE("SM2 signatures use a fresh random nonce", "[sm2]")
{
    uint8_t r1[32], s1[32], r2[32], s2[32];
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_sign(sm2_kat_e, sm2_kat_d, r1, s1));
    TEST_ASSERT_EQUAL(ESP_OK, esp_sm2_sign(sm2_kat_e, sm2_kat_d, r2, s2));
    TEST_ASSERT_TRUE(memcmp(r1, r2, 32) != 0);
}

TEST_CASE("SM2 sign rejects invalid arguments", "[sm2]")
{
    uint8_t r[32], s[32];
    uint8_t bad[32] = {0};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_sign(sm2_kat_e, bad, r, s));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_sign(NULL, sm2_kat_d, r, s));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_sm2_sign(sm2_kat_e, sm2_kat_d, NULL, s));
}

#endif /* SOC_ECC_SUPPORT_CURVE_SM2 && SOC_ECC_EXTENDED_MODES_SUPPORTED */
