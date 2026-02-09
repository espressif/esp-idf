/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_crypto_periph_clk.h"
#include "esp_log.h"
#include "ecc_params.h"
#include "soc/soc_caps.h"
#include "hal/ecc_hal.h"
#include "hal/ecc_ll.h"

#include "memory_checks.h"
#include "unity_fixture.h"
#include "ccomp_timer.h"

/* ECC curve size constants in bytes */
#define ECC_P192_SIZE_BYTES                 24
#define ECC_P256_SIZE_BYTES                 32
#define ECC_P384_SIZE_BYTES                 48

/* Maximum ECC buffer size for all supported curves */
#define ECC_MAX_BUFFER_SIZE                 48

/* ECC operation support flags */
#define SOC_ECC_SUPPORT_POINT_MULT          1
#define SOC_ECC_SUPPORT_POINT_VERIFY        1

#if SOC_ECC_EXTENDED_MODES_SUPPORTED
#define SOC_ECC_SUPPORT_POINT_DIVISION            1
#define SOC_ECC_SUPPORT_JACOB_POINT_MULT          1
#define SOC_ECC_SUPPORT_JACOB_POINT_VERIFY        1
#define SOC_ECC_SUPPORT_POINT_ADDITION            1
#define SOC_ECC_SUPPORT_MOD_ADD                   1
#define SOC_ECC_SUPPORT_MOD_SUB                   1
#define SOC_ECC_SUPPORT_MOD_MUL                   1
#endif

static void ecc_be_to_le(const uint8_t* be_point, uint8_t *le_point, uint8_t len)
{
    /* When the size is 24 bytes, it should be padded with 0 bytes*/
    memset(le_point, 0x0, ECC_MAX_BUFFER_SIZE);

    for (int i = 0; i < len; i++) {
        le_point[i] = be_point[len - i - 1];
    }
}

TEST_GROUP(ecc);

TEST_SETUP(ecc)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(ecc)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

#if SOC_ECC_SUPPORT_POINT_MULT
static void ecc_point_mul(const uint8_t *k_le, const uint8_t *x_le, const uint8_t *y_le, uint8_t len, bool verify_first,
                          uint8_t *res_x_le, uint8_t *res_y_le)
{
    esp_crypto_ecc_enable_periph_clk(true);

    ecc_hal_write_mul_param(k_le, x_le, y_le, len);
    if (verify_first) {
        ecc_hal_set_mode(ECC_MODE_VERIFY_THEN_POINT_MUL);
    } else {
        ecc_hal_set_mode(ECC_MODE_POINT_MUL);
    }
    ecc_hal_enable_constant_time_point_mul(true);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_mul_result(res_x_le, res_y_le, len);
    esp_crypto_ecc_enable_periph_clk(false);
}

static void test_ecc_point_mul_inner(bool verify_first)
{
    uint8_t scalar_le[ECC_MAX_BUFFER_SIZE];
    uint8_t x_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_le[ECC_MAX_BUFFER_SIZE];

    /* P256 */
    ecc_be_to_le(ecc_p256_scalar, scalar_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_x, x_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_y, y_le, ECC_P256_SIZE_BYTES);

    uint8_t x_res_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_res_le[ECC_MAX_BUFFER_SIZE];

    ecc_point_mul(scalar_le, x_le, y_le, ECC_P256_SIZE_BYTES, verify_first, x_res_le, y_res_le);

    uint8_t x_mul_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_mul_le[ECC_MAX_BUFFER_SIZE];

    ecc_be_to_le(ecc_p256_mul_res_x, x_mul_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_mul_res_y, y_mul_le, ECC_P256_SIZE_BYTES);

    ESP_LOG_BUFFER_HEXDUMP("Expected X:", x_mul_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got X:", x_res_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);

    ESP_LOG_BUFFER_HEXDUMP("Expected Y:", y_mul_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got Y:", y_res_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_mul_le, x_res_le, ECC_P256_SIZE_BYTES, "X coordinate of P256 point multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_mul_le, y_res_le, ECC_P256_SIZE_BYTES, "Y coordinate of P256 point multiplication ");

    memset(x_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
    memset(y_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
    memset(x_mul_le, 0x0, ECC_MAX_BUFFER_SIZE);
    memset(y_mul_le, 0x0, ECC_MAX_BUFFER_SIZE);

    /* P192 */
    ecc_be_to_le(ecc_p192_scalar, scalar_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_x, x_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_y, y_le, ECC_P192_SIZE_BYTES);

    ecc_point_mul(scalar_le, x_le, y_le, ECC_P192_SIZE_BYTES, verify_first, x_res_le, y_res_le);

    ecc_be_to_le(ecc_p192_mul_res_x, x_mul_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_mul_res_y, y_mul_le, ECC_P192_SIZE_BYTES);

    ESP_LOG_BUFFER_HEXDUMP("Expected X:", x_mul_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got X:", x_res_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);

    ESP_LOG_BUFFER_HEXDUMP("Expected Y:", y_mul_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got Y:", y_res_le, ECC_P256_SIZE_BYTES, ESP_LOG_DEBUG);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_mul_le, x_res_le, ECC_P192_SIZE_BYTES, "X coordinate of P192 point multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_mul_le, y_res_le, ECC_P192_SIZE_BYTES, "Y coordinate of P192 point multiplication ");

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        memset(x_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
        memset(y_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
        memset(x_mul_le, 0x0, ECC_MAX_BUFFER_SIZE);
        memset(y_mul_le, 0x0, ECC_MAX_BUFFER_SIZE);

        /* P384 */
        ecc_be_to_le(ecc_p384_scalar, scalar_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_x, x_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_y, y_le, ECC_P384_SIZE_BYTES);

        ecc_point_mul(scalar_le, x_le, y_le, ECC_P384_SIZE_BYTES, verify_first, x_res_le, y_res_le);

        ecc_be_to_le(ecc_p384_mul_res_x, x_mul_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_mul_res_y, y_mul_le, ECC_P384_SIZE_BYTES);

        ESP_LOG_BUFFER_HEXDUMP("Expected Result X:", x_mul_le, ECC_P384_SIZE_BYTES, ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEXDUMP("Got Result X:", x_res_le, ECC_P384_SIZE_BYTES, ESP_LOG_DEBUG);

        ESP_LOG_BUFFER_HEXDUMP("Expected Result Y:", y_mul_le, ECC_P384_SIZE_BYTES, ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEXDUMP("Got Result Y:", y_res_le, ECC_P384_SIZE_BYTES, ESP_LOG_DEBUG);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_mul_le, x_res_le, ECC_P384_SIZE_BYTES, "X coordinate of P384 point multiplication ");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_mul_le, y_res_le, ECC_P384_SIZE_BYTES, "Y coordinate of P384 point multiplication ");
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}

TEST(ecc, ecc_point_multiplication_on_SECP192R1_SECP256R1_SECP384R1)
{
    test_ecc_point_mul_inner(false);
}

#if SOC_ECC_CONSTANT_TIME_POINT_MUL

#define CONST_TIME_DEVIATION_PERCENT 0.0025

static void test_ecc_point_mul_inner_constant_time(void)
{
#if CONFIG_IDF_TARGET_ESP32H2
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
        TEST_IGNORE_MESSAGE("Skipping test, not supported on ESP32-H2 <v1.2\n");
        return;
    }
#endif
    uint8_t scalar_le[ECC_MAX_BUFFER_SIZE];
    uint8_t x_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_le[ECC_MAX_BUFFER_SIZE];

    /* P256 */
    ecc_be_to_le(ecc_p256_scalar, scalar_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_x, x_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_y, y_le, ECC_P256_SIZE_BYTES);

    uint8_t x_res_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_res_le[ECC_MAX_BUFFER_SIZE];

    double deviation = 0;
    uint32_t elapsed_time, mean_elapsed_time, total_elapsed_time = 0;
    uint32_t max_time = 0, min_time = UINT32_MAX;
    int loop_count = 10;

    for (int i = 0; i < loop_count; i++) {
        ccomp_timer_start();
        ecc_point_mul(scalar_le, x_le, y_le, ECC_P256_SIZE_BYTES, 0, x_res_le, y_res_le);
        elapsed_time = ccomp_timer_stop();

        max_time = MAX(elapsed_time, max_time);
        min_time = MIN(elapsed_time, min_time);
        total_elapsed_time += elapsed_time;
    }
    mean_elapsed_time = total_elapsed_time / loop_count;
    deviation = ((double)(max_time - mean_elapsed_time) / mean_elapsed_time);

    TEST_ASSERT_LESS_THAN_DOUBLE(CONST_TIME_DEVIATION_PERCENT, deviation);

    /* P192 */
    ecc_be_to_le(ecc_p192_scalar, scalar_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_x, x_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_y, y_le, ECC_P192_SIZE_BYTES);

    max_time = 0;
    min_time = UINT32_MAX;
    total_elapsed_time = 0;

    for (int i = 0; i < loop_count; i++) {
        ccomp_timer_start();
        ecc_point_mul(scalar_le, x_le, y_le, ECC_P192_SIZE_BYTES, 0, x_res_le, y_res_le);
        elapsed_time = ccomp_timer_stop();

        max_time = MAX(elapsed_time, max_time);
        min_time = MIN(elapsed_time, min_time);
        total_elapsed_time += elapsed_time;
    }
    mean_elapsed_time = total_elapsed_time / loop_count;
    deviation = ((double)(max_time - mean_elapsed_time) / mean_elapsed_time);

    TEST_ASSERT_LESS_THAN_DOUBLE(CONST_TIME_DEVIATION_PERCENT, deviation);

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        /* P384 */
        ecc_be_to_le(ecc_p384_scalar, scalar_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_x, x_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_y, y_le, ECC_P384_SIZE_BYTES);

        max_time = 0;
        min_time = UINT32_MAX;
        total_elapsed_time = 0;

        for (int i = 0; i < loop_count; i++) {
            ccomp_timer_start();
            ecc_point_mul(scalar_le, x_le, y_le, ECC_P384_SIZE_BYTES, 0, x_res_le, y_res_le);
            elapsed_time = ccomp_timer_stop();

            max_time = MAX(elapsed_time, max_time);
            min_time = MIN(elapsed_time, min_time);
            total_elapsed_time += elapsed_time;
        }
        mean_elapsed_time = total_elapsed_time / loop_count;
        deviation = ((double)(max_time - mean_elapsed_time) / mean_elapsed_time);

        TEST_ASSERT_LESS_THAN_DOUBLE(CONST_TIME_DEVIATION_PERCENT, deviation);
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}

TEST(ecc, ecc_point_multiplication_const_time_check_on_SECP192R1_SECP256R1_SECP384R1)
{
    test_ecc_point_mul_inner_constant_time();
}
#endif

#endif

#if SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
static int ecc_point_verify(const uint8_t *x_le, const uint8_t *y_le, uint8_t len)
{
    esp_crypto_ecc_enable_periph_clk(true);

    ecc_hal_write_verify_param(x_le, y_le, len);
    ecc_hal_set_mode(ECC_MODE_VERIFY);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    int ret = ecc_hal_read_verify_result();
    esp_crypto_ecc_enable_periph_clk(false);

    return ret;
}

TEST(ecc, ecc_point_verification_on_SECP192R1_SECP256R1_SECP384R1)
{
    int res;
    uint8_t x_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_le[ECC_MAX_BUFFER_SIZE];

    /* P256 */
    ecc_be_to_le(ecc_p256_point_x, x_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_y, y_le, ECC_P256_SIZE_BYTES);

    // Case 1: Correct point on curve
    res = ecc_point_verify(x_le, y_le, ECC_P256_SIZE_BYTES);
    TEST_ASSERT_EQUAL(1, res);

    // Case 2: Modify one byte from the point
    x_le[6] = x_le[6] ^ 0xFF;
    res = ecc_point_verify(x_le, y_le, ECC_P256_SIZE_BYTES);
    TEST_ASSERT_EQUAL(0, res);

    /* P192 */
    ecc_be_to_le(ecc_p192_point_x, x_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_y, y_le, ECC_P192_SIZE_BYTES);

    // Case 1: Correct point on curve
    res = ecc_point_verify(x_le, y_le, ECC_P192_SIZE_BYTES);
    TEST_ASSERT_EQUAL(1, res);

    // Case 2: Modify one byte from the point
    x_le[6] = x_le[6] ^ 0xFF;
    res = ecc_point_verify(x_le, y_le, ECC_P192_SIZE_BYTES);
    TEST_ASSERT_EQUAL(0, res);

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        /* P384 */
        ecc_be_to_le(ecc_p384_point_x, x_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_y, y_le, ECC_P384_SIZE_BYTES);

        // Case 1: Correct point on curve
        res = ecc_point_verify(x_le, y_le, ECC_P384_SIZE_BYTES);
        TEST_ASSERT_EQUAL(1, res);

        // Case 2: Modify one byte from the point
        x_le[6] = x_le[6] ^ 0xFF;
        res = ecc_point_verify(x_le, y_le, ECC_P384_SIZE_BYTES);
        TEST_ASSERT_EQUAL(0, res);
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}
#endif

#if SOC_ECC_SUPPORT_POINT_MULT && SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
TEST(ecc, ecc_point_verification_and_multiplication_on_SECP192R1_SECP256R1_SECP384R1)
{
    test_ecc_point_mul_inner(true);
}
#endif

#if SOC_ECC_SUPPORT_POINT_DIVISION
static void ecc_point_inv_mul(const uint8_t *num_le, const uint8_t *deno_le, uint8_t len, uint8_t *res_le)
{
    esp_crypto_ecc_enable_periph_clk(true);

    uint8_t zero[ECC_MAX_BUFFER_SIZE] = {0};
    ecc_hal_write_mul_param(zero, num_le, deno_le, len);

    ecc_hal_set_mode(ECC_MODE_INVERSE_MUL);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_mul_result(zero, res_le, len);
    esp_crypto_ecc_enable_periph_clk(false);
}

TEST(ecc, ecc_inverse_multiplication_or_mod_division_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve)
{
    uint8_t res[ECC_MAX_BUFFER_SIZE] = {0};
    ecc_point_inv_mul(ecc256_num, ecc256_den, ECC_P256_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_inv_mul_res, res, ECC_P256_SIZE_BYTES, "P256 Inverse multiplication (or Mod division)");

    ecc_point_inv_mul(ecc192_num, ecc192_den, ECC_P192_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_inv_mul_res, res, ECC_P192_SIZE_BYTES, "P192 Inverse multiplication (or Mod division)");

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        ecc_point_inv_mul(ecc384_num, ecc384_den, ECC_P384_SIZE_BYTES, res);
        ESP_LOG_BUFFER_HEXDUMP("Expected Result X:", ecc384_inv_mul_res, ECC_P384_SIZE_BYTES, ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEXDUMP("Got Result X:", res, ECC_P384_SIZE_BYTES, ESP_LOG_DEBUG);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc384_inv_mul_res, res, ECC_P384_SIZE_BYTES, "P384 Inverse multiplication (or Mod division)");
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT
static void ecc_jacob_mul(uint8_t *k_le, uint8_t *x_le, uint8_t *y_le, uint8_t len, bool verify_first,
                          uint8_t *res_x_le, uint8_t *res_y_le, uint8_t *res_z_le)
{
    esp_crypto_ecc_enable_periph_clk(true);

    ecc_hal_write_mul_param(k_le, x_le, y_le, len);
    if (verify_first) {
        ecc_hal_set_mode(ECC_MODE_POINT_VERIFY_JACOBIAN_MUL);
    } else {
        ecc_hal_set_mode(ECC_MODE_JACOBIAN_POINT_MUL);
    }
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_jacob_mul_result(res_x_le, res_y_le, res_z_le, len);
    esp_crypto_ecc_enable_periph_clk(false);
}

static void test_ecc_jacob_mul_inner(bool verify_first)
{
    uint8_t scalar_le[ECC_MAX_BUFFER_SIZE];
    uint8_t x_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_le[ECC_MAX_BUFFER_SIZE];

    /* P256 */
    ecc_be_to_le(ecc_p256_scalar, scalar_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_x, x_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_y, y_le, ECC_P256_SIZE_BYTES);

    uint8_t x_res_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_res_le[ECC_MAX_BUFFER_SIZE];
    uint8_t z_res_le[ECC_MAX_BUFFER_SIZE];

    ecc_jacob_mul(scalar_le, x_le, y_le, ECC_P256_SIZE_BYTES, verify_first, x_res_le, y_res_le, z_res_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p256_jacob_mul_res_x_le, x_res_le, ECC_P256_SIZE_BYTES, "X coordinate of P256 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p256_jacob_mul_res_y_le, y_res_le, ECC_P256_SIZE_BYTES, "Y coordinate of P256 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p256_jacob_mul_res_z_le, z_res_le, ECC_P256_SIZE_BYTES, "Z coordinate of P256 point jacobian multiplication ");

    memset(x_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
    memset(y_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
    memset(z_res_le, 0x0, ECC_MAX_BUFFER_SIZE);

    /* P192 */
    ecc_be_to_le(ecc_p192_scalar, scalar_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_x, x_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_y, y_le, ECC_P192_SIZE_BYTES);

    ecc_jacob_mul(scalar_le, x_le, y_le, ECC_P192_SIZE_BYTES, verify_first, x_res_le, y_res_le, z_res_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p192_jacob_mul_res_x_le, x_res_le, ECC_P192_SIZE_BYTES, "X coordinate of P192 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p192_jacob_mul_res_y_le, y_res_le, ECC_P192_SIZE_BYTES, "Y coordinate of P192 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p192_jacob_mul_res_z_le, z_res_le, ECC_P192_SIZE_BYTES, "Z coordinate of P192 point jacobian multiplication ");

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        memset(x_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
        memset(y_res_le, 0x0, ECC_MAX_BUFFER_SIZE);
        memset(z_res_le, 0x0, ECC_MAX_BUFFER_SIZE);

        /* P384 */
        ecc_be_to_le(ecc_p384_scalar, scalar_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_x, x_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_y, y_le, ECC_P384_SIZE_BYTES);

        ecc_jacob_mul(scalar_le, x_le, y_le, ECC_P384_SIZE_BYTES, verify_first, x_res_le, y_res_le, z_res_le);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p384_jacob_mul_res_x_le, x_res_le, ECC_P384_SIZE_BYTES, "X coordinate of P384 point jacobian multiplication ");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p384_jacob_mul_res_y_le, y_res_le, ECC_P384_SIZE_BYTES, "Y coordinate of P384 point jacobian multiplication ");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p384_jacob_mul_res_z_le, z_res_le, ECC_P384_SIZE_BYTES, "Z coordinate of P384 point jacobian multiplication ");
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}

TEST(ecc, ecc_jacobian_point_multiplication_on_SECP192R1_SECP256R1_SECP384R1)
{
    test_ecc_jacob_mul_inner(false);
}
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
static int ecc_jacob_verify(const uint8_t *x_le, const uint8_t *y_le, const uint8_t *z_le, uint8_t len)
{
    esp_crypto_ecc_enable_periph_clk(true);

    ecc_hal_write_jacob_verify_param(x_le, y_le, z_le, len);

    ecc_hal_set_mode(ECC_MODE_JACOBIAN_POINT_VERIFY);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    int ret = ecc_hal_read_verify_result();
    esp_crypto_ecc_enable_periph_clk(false);

    return ret;
}

TEST(ecc, ecc_jacobian_point_verification_on_SECP192R1_SECP256R1_SECP384R1)
{
    int res;
    /* P256 */
    res = ecc_jacob_verify(ecc_p256_jacob_mul_res_x_le, ecc_p256_jacob_mul_res_y_le, ecc_p256_jacob_mul_res_z_le, ECC_P256_SIZE_BYTES);
    TEST_ASSERT_EQUAL(1, res);

    /* P192 */
    res = ecc_jacob_verify(ecc_p192_jacob_mul_res_x_le, ecc_p192_jacob_mul_res_y_le, ecc_p192_jacob_mul_res_z_le, ECC_P192_SIZE_BYTES);
    TEST_ASSERT_EQUAL(1, res);

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        /* P384 */
        res = ecc_jacob_verify(ecc_p384_jacob_mul_res_x_le, ecc_p384_jacob_mul_res_y_le, ecc_p384_jacob_mul_res_z_le, ECC_P384_SIZE_BYTES);
        TEST_ASSERT_EQUAL(1, res);
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT && SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
TEST(ecc, ecc_point_verification_and_jacobian_point_multiplication_on_SECP192R1_SECP256R1_SECP384R1)
{
    test_ecc_jacob_mul_inner(true);
}
#endif

#if SOC_ECC_SUPPORT_POINT_ADDITION
static void ecc_point_addition(uint8_t *px_le, uint8_t *py_le, uint8_t *qx_le, uint8_t *qy_le, uint8_t *qz_le,
                               uint8_t len, bool jacob_output,
                               uint8_t *x_res_le, uint8_t *y_res_le, uint8_t *z_res_le)
{
    esp_crypto_ecc_enable_periph_clk(true);

    ecc_hal_write_point_add_param(px_le, py_le, qx_le, qy_le, qz_le, len);

    ecc_hal_set_mode(ECC_MODE_POINT_ADD);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_point_add_result(x_res_le, y_res_le, z_res_le, len, jacob_output);
    esp_crypto_ecc_enable_periph_clk(false);
}

TEST(ecc, ecc_point_addition_on_SECP192R1_SECP256R1_SECP384R1)
{
    uint8_t scalar_le[ECC_MAX_BUFFER_SIZE] = {0};
    uint8_t x_le[ECC_MAX_BUFFER_SIZE] = {0};
    uint8_t y_le[ECC_MAX_BUFFER_SIZE] = {0};
    uint8_t z_le[ECC_MAX_BUFFER_SIZE] = {0};

    /* P256
     * R = 2 * P
     */
    ecc_be_to_le(ecc_p256_point_x, x_le, ECC_P256_SIZE_BYTES);
    ecc_be_to_le(ecc_p256_point_y, y_le, ECC_P256_SIZE_BYTES);

    scalar_le[0] = 0x2;

    uint8_t x_res_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_res_le[ECC_MAX_BUFFER_SIZE];
    uint8_t z_res_le[ECC_MAX_BUFFER_SIZE];

    ecc_jacob_mul(scalar_le, x_le, y_le, ECC_P256_SIZE_BYTES, 0, x_res_le, y_res_le, z_res_le);

    uint8_t x_add_le[ECC_MAX_BUFFER_SIZE];
    uint8_t y_add_le[ECC_MAX_BUFFER_SIZE];
    uint8_t z_add_le[ECC_MAX_BUFFER_SIZE];

    z_le[0] = 0x1;

    ecc_point_addition(x_le, y_le, x_le, y_le, z_le, ECC_P256_SIZE_BYTES, 1, x_add_le, y_add_le, z_add_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_add_le, x_res_le, ECC_P256_SIZE_BYTES, "X coordinate of P256 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_add_le, y_res_le, ECC_P256_SIZE_BYTES, "Y coordinate of P256 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(z_add_le, z_res_le, ECC_P256_SIZE_BYTES, "Z coordinate of P256 point addition");

    /* P192
     * R = 2 * P
     */
    ecc_be_to_le(ecc_p192_point_x, x_le, ECC_P192_SIZE_BYTES);
    ecc_be_to_le(ecc_p192_point_y, y_le, ECC_P192_SIZE_BYTES);

    scalar_le[0] = 0x2;

    ecc_jacob_mul(scalar_le, x_le, y_le, ECC_P192_SIZE_BYTES, 0, x_res_le, y_res_le, z_res_le);

    z_le[0] = 0x1;

    ecc_point_addition(x_le, y_le, x_le, y_le, z_le, ECC_P192_SIZE_BYTES, 1, x_add_le, y_add_le, z_add_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_add_le, x_res_le, ECC_P192_SIZE_BYTES, "X coordinate of P192 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_add_le, y_res_le, ECC_P192_SIZE_BYTES, "Y coordinate of P192 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(z_add_le, z_res_le, ECC_P192_SIZE_BYTES, "Z coordinate of P192 point addition");

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        /* P384
         * R = 2 * P
         */
        ecc_be_to_le(ecc_p384_point_x, x_le, ECC_P384_SIZE_BYTES);
        ecc_be_to_le(ecc_p384_point_y, y_le, ECC_P384_SIZE_BYTES);

        scalar_le[0] = 0x2;

        ecc_jacob_mul(scalar_le, x_le, y_le, ECC_P384_SIZE_BYTES, 0, x_res_le, y_res_le, z_res_le);

        z_le[0] = 0x1;

        ecc_point_addition(x_le, y_le, x_le, y_le, z_le, ECC_P384_SIZE_BYTES, 1, x_add_le, y_add_le, z_add_le);

        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_add_le, x_res_le, ECC_P384_SIZE_BYTES, "X coordinate of P384 point addition");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_add_le, y_res_le, ECC_P384_SIZE_BYTES, "Y coordinate of P384 point addition");
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(z_add_le, z_res_le, ECC_P384_SIZE_BYTES, "Z coordinate of P384 point addition");
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}
#endif

#if SOC_ECC_SUPPORT_MOD_ADD || SOC_ECC_SUPPORT_MOD_SUB || SOC_ECC_SUPPORT_MOD_MUL
static void ecc_mod_op(ecc_mode_t mode, const uint8_t *a, const uint8_t *b, uint8_t len, uint8_t *res_le)
{
    esp_crypto_ecc_enable_periph_clk(true);

    ecc_hal_write_mod_op_param(a, b, len);

    ecc_hal_set_mode(mode);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_mod_op_result(res_le, len);
    esp_crypto_ecc_enable_periph_clk(false);
}
#endif

#if SOC_ECC_SUPPORT_MOD_ADD
TEST(ecc, ecc_mod_addition_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve)
{
    uint8_t res[ECC_MAX_BUFFER_SIZE] = {0};
    ecc_mod_op(ECC_MODE_MOD_ADD, ecc256_x, ecc256_y, ECC_P256_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_add_res, res, ECC_P256_SIZE_BYTES, "P256 mod addition");

    ecc_mod_op(ECC_MODE_MOD_ADD, ecc192_x, ecc192_y, ECC_P192_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_add_res, res, ECC_P192_SIZE_BYTES, "P192 mod addition");

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        ecc_mod_op(ECC_MODE_MOD_ADD, ecc384_x, ecc384_y, ECC_P384_SIZE_BYTES, res);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc384_add_res, res, ECC_P384_SIZE_BYTES, "P384 mod addition");
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}
#endif

#if SOC_ECC_SUPPORT_MOD_SUB
TEST(ecc, ecc_mod_subtraction_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve)
{
    uint8_t res[ECC_MAX_BUFFER_SIZE] = {0};
    ecc_mod_op(ECC_MODE_MOD_SUB, ecc256_x, ecc256_y, ECC_P256_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_sub_res, res, ECC_P256_SIZE_BYTES, "P256 mod subtraction");

    ecc_mod_op(ECC_MODE_MOD_SUB, ecc192_x, ecc192_y, ECC_P192_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_sub_res, res, ECC_P192_SIZE_BYTES, "P192 mod subtraction");

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        ecc_mod_op(ECC_MODE_MOD_SUB, ecc384_x, ecc384_y, ECC_P384_SIZE_BYTES, res);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc384_sub_res, res, ECC_P384_SIZE_BYTES, "P384 mod subtraction");
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}
#endif

#if SOC_ECC_SUPPORT_MOD_MUL
TEST(ecc, ecc_mod_multiplication_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve)
{
    uint8_t res[ECC_MAX_BUFFER_SIZE] = {0};
    ecc_mod_op(ECC_MODE_MOD_MUL, ecc256_x, ecc256_y, ECC_P256_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_mul_res, res, ECC_P256_SIZE_BYTES, "P256 mod multiplication");

    ecc_mod_op(ECC_MODE_MOD_MUL, ecc192_x, ecc192_y, ECC_P192_SIZE_BYTES, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_mul_res, res, ECC_P192_SIZE_BYTES, "P192 mod multiplication");

#if SOC_ECC_SUPPORT_CURVE_P384
    if (ecc_ll_is_p384_curve_operations_supported()) {
        ecc_mod_op(ECC_MODE_MOD_MUL, ecc384_x, ecc384_y, ECC_P384_SIZE_BYTES, res);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc384_mul_res, res, ECC_P384_SIZE_BYTES, "P384 mod multiplication");
    }
#endif /* SOC_ECC_SUPPORT_CURVE_P384 */
}
#endif

TEST_GROUP_RUNNER(ecc)
{
#if SOC_ECC_SUPPORT_POINT_MULT
    RUN_TEST_CASE(ecc, ecc_point_multiplication_on_SECP192R1_SECP256R1_SECP384R1);
#if SOC_ECC_CONSTANT_TIME_POINT_MUL
    RUN_TEST_CASE(ecc, ecc_point_multiplication_const_time_check_on_SECP192R1_SECP256R1_SECP384R1);
#endif
#endif

#if SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
    RUN_TEST_CASE(ecc, ecc_point_verification_on_SECP192R1_SECP256R1_SECP384R1);
#endif

#if SOC_ECC_SUPPORT_POINT_MULT && SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
    RUN_TEST_CASE(ecc, ecc_point_verification_and_multiplication_on_SECP192R1_SECP256R1_SECP384R1);
#endif

#if SOC_ECC_SUPPORT_POINT_DIVISION
    RUN_TEST_CASE(ecc, ecc_inverse_multiplication_or_mod_division_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve);
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT
    RUN_TEST_CASE(ecc, ecc_jacobian_point_multiplication_on_SECP192R1_SECP256R1_SECP384R1);
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
    RUN_TEST_CASE(ecc, ecc_jacobian_point_verification_on_SECP192R1_SECP256R1_SECP384R1);
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT && SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
    RUN_TEST_CASE(ecc, ecc_point_verification_and_jacobian_point_multiplication_on_SECP192R1_SECP256R1_SECP384R1);
#endif

#if SOC_ECC_SUPPORT_POINT_ADDITION
    RUN_TEST_CASE(ecc, ecc_point_addition_on_SECP192R1_SECP256R1_SECP384R1);
#endif

#if SOC_ECC_SUPPORT_MOD_ADD
    RUN_TEST_CASE(ecc, ecc_mod_addition_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve);
#endif

#if SOC_ECC_SUPPORT_MOD_SUB
    RUN_TEST_CASE(ecc, ecc_mod_subtraction_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve);
#endif

#if SOC_ECC_SUPPORT_MOD_MUL
    RUN_TEST_CASE(ecc, ecc_mod_multiplication_using_SECP192R1_SECP256R1_SECP384R1_order_of_curve);
#endif
}
