/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_private/periph_ctrl.h"
#include "esp_random.h"
#include "hal/clk_gate_ll.h"
#include "hal/ecdsa_hal.h"
#include "hal/ecdsa_types.h"

#include "memory_checks.h"
#include "unity_fixture.h"

#include "ecdsa_params.h"


static void ecdsa_enable_and_reset(void)
{
    periph_ll_enable_clk_clear_rst(PERIPH_ECDSA_MODULE);
}

static void ecdsa_disable_and_reset(void)
{
    periph_ll_disable_clk_set_rst(PERIPH_ECDSA_MODULE);
}

static void ecc_be_to_le(const uint8_t* be_point, uint8_t *le_point, uint8_t len)
{
    memset(le_point, 0x0, 32);
    for (int i = 0; i < len; i++) {
        le_point[i] = be_point[len - i - 1];
    }
}

static int test_ecdsa_verify(bool is_p256, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, uint8_t *pub_x, uint8_t *pub_y)
{
    uint16_t len;
    uint8_t sha_le[32];

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_VERIFY,
        .k_mode = ECDSA_K_USE_TRNG,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
    };

    if (is_p256) {
        conf.curve = ECDSA_CURVE_SECP256R1;
        len = 32;
    } else {
        conf.curve = ECDSA_CURVE_SECP192R1;
        len = 24;
    }

    /* Set HASH */
    ecc_be_to_le(sha, sha_le, len);

    ecdsa_enable_and_reset();
    int ret = ecdsa_hal_verify_signature(&conf, sha_le, r_le, s_le, pub_x, pub_y, len);
    ecdsa_disable_and_reset();
    return ret;
}

static void test_ecdsa_corrupt_data(bool is_p256, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, uint8_t *pub_x, uint8_t *pub_y)
{
    int len;

    if (is_p256) {
        len = 32;
    } else {
        len = 24;
    }

    // Randomly select a bit and corrupt its correpsonding value
    uint16_t r_bit = esp_random() % len * 8;

    printf("Corrupting SHA bit %d...\n", r_bit);
    sha[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(1, sha, r_le, s_le, pub_x, pub_y));
    sha[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting R bit %d...\n", r_bit);
    r_le[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(1, sha, r_le, s_le, pub_x, pub_y));
    r_le[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting S bit %d...\n", r_bit);
    s_le[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(1, sha, r_le, s_le, pub_x, pub_y));
    s_le[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting pub_x bit %d...\n", r_bit);
    pub_x[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(1, sha, r_le, s_le, pub_x, pub_y));
    pub_x[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting pub_y bit %d...\n", r_bit);
    pub_y[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(1, sha, r_le, s_le, pub_x, pub_y));
    pub_y[r_bit / 8] ^= 1 << (r_bit % 8);

}

static void test_ecdsa_sign(bool is_p256, uint8_t* sha, uint8_t* r_le, uint8_t* s_le)
{
    uint8_t sha_le[32] = {0};
    uint8_t zeroes[32] = {0};
    uint16_t len;

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_GEN,
        .k_mode = ECDSA_K_USE_TRNG,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
    };

    if (is_p256) {
        conf.curve = ECDSA_CURVE_SECP256R1;
        conf.efuse_key_blk = 6;
        len = 32;
    } else {
        conf.curve = ECDSA_CURVE_SECP192R1;
        conf.efuse_key_blk = 5;
        len = 24;
    }

    /* Set HASH */
    ecc_be_to_le(sha, sha_le, len);

    ecdsa_enable_and_reset();

    do {
        ecdsa_hal_gen_signature(&conf, NULL, sha_le, r_le, s_le, len);
    } while(!memcmp(r_le, zeroes, len) || !memcmp(s_le, zeroes, len));

    ecdsa_disable_and_reset();
}

static void test_ecdsa_sign_and_verify(bool is_p256, uint8_t* sha, uint8_t* pub_x, uint8_t* pub_y)
{
    uint8_t r_le[32] = {0};
    uint8_t s_le[32] = {0};

    test_ecdsa_sign(is_p256, sha, r_le, s_le);

    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(is_p256, sha, r_le, s_le, pub_x, pub_y));
}

TEST_GROUP(ecdsa);

TEST_SETUP(ecdsa)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(ecdsa)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(ecdsa, ecdsa_SECP192R1_signature_verification)
{
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(0, sha, ecdsa192_r, ecdsa192_s, ecdsa192_pub_x, ecdsa192_pub_y));
}


TEST(ecdsa, ecdsa_SECP192R1_sign_and_verify)
{
    test_ecdsa_sign_and_verify(0, sha, ecdsa192_pub_x, ecdsa192_pub_y);
}


TEST(ecdsa, ecdsa_SECP192R1_corrupt_signature)
{
    test_ecdsa_corrupt_data(0, sha, ecdsa192_r, ecdsa192_s, ecdsa192_pub_x, ecdsa192_pub_y);
}


TEST(ecdsa, ecdsa_SECP256R1_signature_verification)
{
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y));
}


TEST(ecdsa, ecdsa_SECP256R1_sign_and_verify)
{
    test_ecdsa_sign_and_verify(1, sha, ecdsa256_pub_x, ecdsa256_pub_y);
}


TEST(ecdsa, ecdsa_SECP256R1_corrupt_signature)
{
    test_ecdsa_corrupt_data(1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y);
}

TEST_GROUP_RUNNER(ecdsa)
{
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_signature_verification)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_corrupt_signature)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_signature_verification)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_corrupt_signature)
}
