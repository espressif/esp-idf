/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_crypto_lock.h"
#include "esp_efuse_chip.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "esp_random.h"
#include "esp_err.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "hal/ecc_ll.h"
#include "hal/ecdsa_hal.h"
#include "hal/efuse_ll.h"
#include "hal/ecdsa_ll.h"
#include "hal/ecdsa_types.h"
#ifdef SOC_MPI_SUPPORTED
#include "hal/mpi_ll.h"
#endif
#include "soc/soc_caps.h"

#include "memory_checks.h"
#include "unity_fixture.h"

#include "ecdsa_params.h"
#include "hal_crypto_common.h"

__attribute__((unused)) static const char * TAG = "crypto_test";

static void ecdsa_enable_and_reset(void)
{
    ECDSA_RCC_ATOMIC() {
        ecdsa_ll_enable_bus_clock(true);
        ecdsa_ll_reset_register();
    }

    ECC_RCC_ATOMIC() {
        ecc_ll_enable_bus_clock(true);
        ecc_ll_power_up();
        ecc_ll_reset_register();
    }

#ifdef SOC_ECDSA_USES_MPI
    MPI_RCC_ATOMIC() {
        mpi_ll_enable_bus_clock(true);
        mpi_ll_reset_register();
    }
#endif
}

static void ecdsa_disable(void)
{
#ifdef SOC_ECDSA_USES_MPI
    MPI_RCC_ATOMIC() {
        mpi_ll_enable_bus_clock(false);
    }
#endif

    ECC_RCC_ATOMIC() {
        ecc_ll_enable_bus_clock(false);
        ecc_ll_power_down();
    }

    ECDSA_RCC_ATOMIC() {
        ecdsa_ll_enable_bus_clock(false);
    }
}

static void ecc_be_to_le(const uint8_t* be_point, uint8_t *le_point, uint8_t len)
{
    memset(le_point, 0x0, 32);
    for (int i = 0; i < len; i++) {
        le_point[i] = be_point[len - i - 1];
    }
}

int test_ecdsa_verify(bool is_p256, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, uint8_t *pub_x, uint8_t *pub_y)
{
    uint16_t len;
    uint8_t sha_le[32];

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_VERIFY,
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
    ecdsa_disable();
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

    // Randomly select a bit and corrupt its corresponding value
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

void test_ecdsa_sign(bool is_p256, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, bool use_km_key, ecdsa_sign_type_t k_type)
{
    uint8_t sha_le[32] = {0};
    uint8_t zeroes[32] = {0};
    uint16_t len;

#if !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
    uint16_t det_loop_number = 1;
#endif /* !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_GEN,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
        .use_km_key = use_km_key,
        .sign_type = k_type,
    };

    if (is_p256) {
        conf.curve = ECDSA_CURVE_SECP256R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_2;
        }
        len = 32;
    } else {
        conf.curve = ECDSA_CURVE_SECP192R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_1;
        }
        len = 24;
    }

    /* Set HASH */
    ecc_be_to_le(sha, sha_le, len);

    ecdsa_enable_and_reset();

    bool process_again = false;

    do {
#if !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        if (ecdsa_ll_is_deterministic_mode_supported() && k_type == ECDSA_K_TYPE_DETERMINISITIC) {
            conf.loop_number = det_loop_number++;
        }
#endif /* !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

        ecdsa_hal_gen_signature(&conf, sha_le, r_le, s_le, len);

        process_again = !ecdsa_hal_get_operation_result()
                        || !memcmp(r_le, zeroes, len)
                        || !memcmp(s_le, zeroes, len);

#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        if (ecdsa_ll_is_deterministic_mode_supported() && k_type == ECDSA_K_TYPE_DETERMINISITIC) {
            process_again |= !ecdsa_hal_det_signature_k_check();
        }
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

    } while(process_again);

    ecdsa_disable();
}

void test_ecdsa_sign_and_verify(bool is_p256, uint8_t* sha, uint8_t* pub_x, uint8_t* pub_y, bool use_km_key, ecdsa_sign_type_t k_type)
{
    uint8_t r_le[32] = {0};
    uint8_t s_le[32] = {0};

    test_ecdsa_sign(is_p256, sha, r_le, s_le, use_km_key, k_type);
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(is_p256, sha, r_le, s_le, pub_x, pub_y));
}

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
void test_ecdsa_export_pubkey_inner(bool is_p256, uint8_t *exported_pub_x, uint8_t *exported_pub_y, bool use_km_key, uint16_t *len)
{

    uint8_t zeroes[32] = {0};
    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_EXPORT_PUBKEY,
        .use_km_key = use_km_key,
    };

    if (is_p256) {
        conf.curve = ECDSA_CURVE_SECP256R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_2;
        }
        *len = 32;
    } else {
        conf.curve = ECDSA_CURVE_SECP192R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_1;
        }
        *len = 24;
    }

    ecdsa_enable_and_reset();

    bool process_again = false;

    do {
        ecdsa_hal_export_pubkey(&conf, exported_pub_x, exported_pub_y, *len);

        process_again = !ecdsa_hal_get_operation_result()
                        || !memcmp(exported_pub_x, zeroes, *len)
                        || !memcmp(exported_pub_y, zeroes, *len);

    } while (process_again);

    ecdsa_disable();
}

void test_ecdsa_export_pubkey(bool is_p256, uint8_t *ecdsa_pub_x, uint8_t *ecdsa_pub_y, bool use_km_key)
{
    uint8_t pub_x[32] = {0};
    uint8_t pub_y[32] = {0};
    uint16_t len;
    test_ecdsa_export_pubkey_inner(is_p256, pub_x, pub_y, use_km_key, &len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecdsa_pub_x, pub_x, len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecdsa_pub_y, pub_y, len);

}
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */


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
    if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        TEST_ASSERT_EQUAL(0, test_ecdsa_verify(0, sha, ecdsa192_r, ecdsa192_s, ecdsa192_pub_x, ecdsa192_pub_y));
    }
}

TEST(ecdsa, ecdsa_SECP192R1_sign_and_verify)
{
    if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_sign_and_verify(0, sha, ecdsa192_pub_x, ecdsa192_pub_y, false, ECDSA_K_TYPE_TRNG);
    }
}

TEST(ecdsa, ecdsa_SECP192R1_corrupt_signature)
{
    if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_corrupt_data(0, sha, ecdsa192_r, ecdsa192_s, ecdsa192_pub_x, ecdsa192_pub_y);
    }
}

TEST(ecdsa, ecdsa_SECP256R1_signature_verification)
{
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y));
}

TEST(ecdsa, ecdsa_SECP256R1_sign_and_verify)
{
    test_ecdsa_sign_and_verify(1, sha, ecdsa256_pub_x, ecdsa256_pub_y, false, ECDSA_K_TYPE_TRNG);
}

TEST(ecdsa, ecdsa_SECP256R1_corrupt_signature)
{
    test_ecdsa_corrupt_data(1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y);
}

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
TEST(ecdsa, ecdsa_SECP192R1_det_sign_and_verify)
{
    if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_sign_and_verify(0, sha, ecdsa192_pub_x, ecdsa192_pub_y, false, ECDSA_K_TYPE_DETERMINISITIC);
    }
}

TEST(ecdsa, ecdsa_SECP256R1_det_sign_and_verify)
{
    if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else {
        test_ecdsa_sign_and_verify(1, sha, ecdsa256_pub_x, ecdsa256_pub_y, false, ECDSA_K_TYPE_DETERMINISITIC);
    }
}
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
TEST(ecdsa, ecdsa_SECP192R1_export_pubkey)
{
    if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_export_pubkey(0, ecdsa192_pub_x, ecdsa192_pub_y, 0);
    }
}

TEST(ecdsa, ecdsa_SECP256R1_export_pubkey)
{
    test_ecdsa_export_pubkey(1, ecdsa256_pub_x, ecdsa256_pub_y, 0);
}
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

TEST_GROUP_RUNNER(ecdsa)
{
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_signature_verification)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_corrupt_signature)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_signature_verification)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_corrupt_signature)
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_det_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_det_sign_and_verify)
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_export_pubkey)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_export_pubkey)
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */
}
