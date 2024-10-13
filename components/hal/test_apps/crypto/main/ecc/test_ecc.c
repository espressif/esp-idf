/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "esp_log.h"
#include "ecc_params.h"
#include "soc/soc_caps.h"
#include "hal/ecc_hal.h"
#include "hal/ecc_ll.h"

#include "memory_checks.h"
#include "unity_fixture.h"
#include "ccomp_timer.h"

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
    memset(le_point, 0x0, 32);

    for (int i = 0; i < len; i++) {
        le_point[i] = be_point[len - i - 1];
    }
}

static void ecc_enable_and_reset(void)
{
    ECC_RCC_ATOMIC() {
        ecc_ll_enable_bus_clock(true);
        ecc_ll_power_up();
        ecc_ll_reset_register();
    }
}

static void ecc_disable(void)
{
    ECC_RCC_ATOMIC() {
        ecc_ll_enable_bus_clock(false);
        ecc_ll_power_down();
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
    ecc_enable_and_reset();

    ecc_hal_write_mul_param(k_le, x_le, y_le, len);
    if (verify_first) {
        ecc_hal_set_mode(ECC_MODE_VERIFY_THEN_POINT_MUL);
    } else {
        ecc_hal_set_mode(ECC_MODE_POINT_MUL);
    }
#ifdef SOC_ECC_CONSTANT_TIME_POINT_MUL
    ecc_hal_enable_constant_time_point_mul(true);
#endif /* SOC_ECC_CONSTANT_TIME_POINT_MUL */
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_mul_result(res_x_le, res_y_le, len);
    ecc_disable();
}

static void test_ecc_point_mul_inner(bool verify_first)
{
    uint8_t scalar_le[32];
    uint8_t x_le[32];
    uint8_t y_le[32];

    /* P256 */
    ecc_be_to_le(ecc_p256_scalar, scalar_le, 32);
    ecc_be_to_le(ecc_p256_point_x, x_le, 32);
    ecc_be_to_le(ecc_p256_point_y, y_le, 32);

    uint8_t x_res_le[32];
    uint8_t y_res_le[32];

    ecc_point_mul(scalar_le, x_le, y_le, 32, verify_first, x_res_le, y_res_le);

    uint8_t x_mul_le[32];
    uint8_t y_mul_le[32];

    ecc_be_to_le(ecc_p256_mul_res_x, x_mul_le, 32);
    ecc_be_to_le(ecc_p256_mul_res_y, y_mul_le, 32);

    ESP_LOG_BUFFER_HEXDUMP("Expected X:", x_mul_le, 32, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got X:", x_res_le, 32, ESP_LOG_DEBUG);

    ESP_LOG_BUFFER_HEXDUMP("Expected Y:", y_mul_le, 32, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got Y:", y_res_le, 32, ESP_LOG_DEBUG);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_mul_le, x_res_le, 32, "X coordinate of P256 point multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_mul_le, y_res_le, 32, "Y coordinate of P256 point multiplication ");

    memset(x_res_le, 0x0, 32);
    memset(y_res_le, 0x0, 32);
    memset(x_mul_le, 0x0, 32);
    memset(y_mul_le, 0x0, 32);

    /* P192 */
    ecc_be_to_le(ecc_p192_scalar, scalar_le, 24);
    ecc_be_to_le(ecc_p192_point_x, x_le, 24);
    ecc_be_to_le(ecc_p192_point_y, y_le, 24);

    ecc_point_mul(scalar_le, x_le, y_le, 24, verify_first, x_res_le, y_res_le);

    ecc_be_to_le(ecc_p192_mul_res_x, x_mul_le, 24);
    ecc_be_to_le(ecc_p192_mul_res_y, y_mul_le, 24);

    ESP_LOG_BUFFER_HEXDUMP("Expected X:", x_mul_le, 32, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got X:", x_res_le, 32, ESP_LOG_DEBUG);

    ESP_LOG_BUFFER_HEXDUMP("Expected Y:", y_mul_le, 32, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("Got Y:", y_res_le, 32, ESP_LOG_DEBUG);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_mul_le, x_res_le, 24, "X coordinate of P192 point multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_mul_le, y_res_le, 24, "Y coordinate of P192 point multiplication ");
}

TEST(ecc, ecc_point_multiplication_on_SECP192R1_and_SECP256R1)
{
    test_ecc_point_mul_inner(false);
}

#if SOC_ECC_CONSTANT_TIME_POINT_MUL

#define CONST_TIME_DEVIATION_PERCENT 0.002

static void test_ecc_point_mul_inner_constant_time(void)
{
    uint8_t scalar_le[32];
    uint8_t x_le[32];
    uint8_t y_le[32];

    /* P256 */
    ecc_be_to_le(ecc_p256_scalar, scalar_le, 32);
    ecc_be_to_le(ecc_p256_point_x, x_le, 32);
    ecc_be_to_le(ecc_p256_point_y, y_le, 32);

    uint8_t x_res_le[32];
    uint8_t y_res_le[32];

    double deviation = 0;
    uint32_t elapsed_time, mean_elapsed_time, total_elapsed_time = 0;
    uint32_t max_time = 0, min_time = UINT32_MAX;
    int loop_count = 10;

    for (int i = 0; i < loop_count; i++) {
        ccomp_timer_start();
        ecc_point_mul(scalar_le, x_le, y_le, 32, 0, x_res_le, y_res_le);
        elapsed_time = ccomp_timer_stop();

        max_time = MAX(elapsed_time, max_time);
        min_time = MIN(elapsed_time, min_time);
        total_elapsed_time += elapsed_time;
    }
    mean_elapsed_time = total_elapsed_time / loop_count;
    deviation = ((double)(max_time - mean_elapsed_time) / mean_elapsed_time);

    TEST_ASSERT_LESS_THAN_DOUBLE(CONST_TIME_DEVIATION_PERCENT, deviation);

    /* P192 */
    ecc_be_to_le(ecc_p192_scalar, scalar_le, 24);
    ecc_be_to_le(ecc_p192_point_x, x_le, 24);
    ecc_be_to_le(ecc_p192_point_y, y_le, 24);

    max_time = 0;
    min_time = UINT32_MAX;
    total_elapsed_time = 0;

    for (int i = 0; i < loop_count; i++) {
        ccomp_timer_start();
        ecc_point_mul(scalar_le, x_le, y_le, 24, 0, x_res_le, y_res_le);
        elapsed_time = ccomp_timer_stop();

        max_time = MAX(elapsed_time, max_time);
        min_time = MIN(elapsed_time, min_time);
        total_elapsed_time += elapsed_time;
    }
    mean_elapsed_time = total_elapsed_time / loop_count;
    deviation = ((double)(max_time - mean_elapsed_time) / mean_elapsed_time);

    TEST_ASSERT_LESS_THAN_DOUBLE(CONST_TIME_DEVIATION_PERCENT, deviation);
}

TEST(ecc, ecc_point_multiplication_const_time_check_on_SECP192R1_and_SECP256R1)
{
    test_ecc_point_mul_inner_constant_time();
}
#endif

#endif

#if SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
static int ecc_point_verify(const uint8_t *x_le, const uint8_t *y_le, uint8_t len)
{
    ecc_enable_and_reset();

    ecc_hal_write_verify_param(x_le, y_le, len);
    ecc_hal_set_mode(ECC_MODE_VERIFY);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    int ret = ecc_hal_read_verify_result();
    ecc_disable();

    return ret;
}

TEST(ecc, ecc_point_verification_on_SECP192R1_and_SECP256R1)
{
    int res;
    uint8_t x_le[32];
    uint8_t y_le[32];

    /* P256 */
    ecc_be_to_le(ecc_p256_point_x, x_le, 32);
    ecc_be_to_le(ecc_p256_point_y, y_le, 32);

    // Case 1: Correct point on curve
    res = ecc_point_verify(x_le, y_le, 32);
    TEST_ASSERT_EQUAL(1, res);

    // Case 2: Modify one byte from the point
    x_le[6] = x_le[6] ^ 0xFF;
    res = ecc_point_verify(x_le, y_le, 32);
    TEST_ASSERT_EQUAL(0, res);

    /* P192 */
    ecc_be_to_le(ecc_p192_point_x, x_le, 24);
    ecc_be_to_le(ecc_p192_point_y, y_le, 24);

    // Case 1: Correct point on curve
    res = ecc_point_verify(x_le, y_le, 24);
    TEST_ASSERT_EQUAL(1, res);

    // Case 2: Modify one byte from the point
    x_le[6] = x_le[6] ^ 0xFF;
    res = ecc_point_verify(x_le, y_le, 24);
    TEST_ASSERT_EQUAL(0, res);
}
#endif

#if SOC_ECC_SUPPORT_POINT_MULT && SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
TEST(ecc, ecc_point_verification_and_multiplication_on_SECP192R1_and_SECP256R1)
{
    test_ecc_point_mul_inner(true);
}
#endif

#if SOC_ECC_SUPPORT_POINT_DIVISION
static void ecc_point_inv_mul(const uint8_t *num_le, const uint8_t *deno_le, uint8_t len, uint8_t *res_le)
{
    ecc_enable_and_reset();

    uint8_t zero[32] = {0};
    ecc_hal_write_mul_param(zero, num_le, deno_le, len);

    ecc_hal_set_mode(ECC_MODE_INVERSE_MUL);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_mul_result(zero, res_le, len);
    ecc_disable();
}

TEST(ecc, ecc_inverse_multiplication_or_mod_division_using_SECP192R1_and_SECP256R1_order_of_curve)
{
    uint8_t res[32] = {0};
    ecc_point_inv_mul(ecc256_num, ecc256_den, 32, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_inv_mul_res, res, 32, "P256 Inverse multiplication (or Mod division)");

    ecc_point_inv_mul(ecc192_num, ecc192_den, 24, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_inv_mul_res, res, 24, "P192 Inverse multiplication (or Mod division)");
}
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT
static void ecc_jacob_mul(uint8_t *k_le, uint8_t *x_le, uint8_t *y_le, uint8_t len, bool verify_first,
                          uint8_t *res_x_le, uint8_t *res_y_le, uint8_t *res_z_le)
{
    ecc_enable_and_reset();

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
    ecc_disable();
}

static void test_ecc_jacob_mul_inner(bool verify_first)
{
    uint8_t scalar_le[32];
    uint8_t x_le[32];
    uint8_t y_le[32];

    /* P256 */
    ecc_be_to_le(ecc_p256_scalar, scalar_le, 32);
    ecc_be_to_le(ecc_p256_point_x, x_le, 32);
    ecc_be_to_le(ecc_p256_point_y, y_le, 32);

    uint8_t x_res_le[32];
    uint8_t y_res_le[32];
    uint8_t z_res_le[32];

    ecc_jacob_mul(scalar_le, x_le, y_le, 32, verify_first, x_res_le, y_res_le, z_res_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p256_jacob_mul_res_x_le, x_res_le, 32, "X coordinate of P256 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p256_jacob_mul_res_y_le, y_res_le, 32, "Y coordinate of P256 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p256_jacob_mul_res_z_le, z_res_le, 32, "Z coordinate of P256 point jacobian multiplication ");

    memset(x_res_le, 0x0, 32);
    memset(y_res_le, 0x0, 32);
    memset(z_res_le, 0x0, 32);

    /* P192 */
    ecc_be_to_le(ecc_p192_scalar, scalar_le, 24);
    ecc_be_to_le(ecc_p192_point_x, x_le, 24);
    ecc_be_to_le(ecc_p192_point_y, y_le, 24);

    ecc_jacob_mul(scalar_le, x_le, y_le, 24, verify_first, x_res_le, y_res_le, z_res_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p192_jacob_mul_res_x_le, x_res_le, 24, "X coordinate of P192 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p192_jacob_mul_res_y_le, y_res_le, 24, "Y coordinate of P192 point jacobian multiplication ");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc_p192_jacob_mul_res_z_le, z_res_le, 24, "Z coordinate of P192 point jacobian multiplication ");
}

TEST(ecc, ecc_jacobian_point_multiplication_on_SECP192R1_and_SECP256R1)
{
    test_ecc_jacob_mul_inner(false);
}
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
static int ecc_jacob_verify(const uint8_t *x_le, const uint8_t *y_le, const uint8_t *z_le, uint8_t len)
{
    ecc_enable_and_reset();

    ecc_hal_write_jacob_verify_param(x_le, y_le, z_le, len);

    ecc_hal_set_mode(ECC_MODE_JACOBIAN_POINT_VERIFY);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    int ret = ecc_hal_read_verify_result();
    ecc_disable();

    return ret;
}

TEST(ecc, ecc_jacobian_point_verification_on_SECP192R1_and_SECP256R1)
{
    int res;
    /* P256 */
    res = ecc_jacob_verify(ecc_p256_jacob_mul_res_x_le, ecc_p256_jacob_mul_res_y_le, ecc_p256_jacob_mul_res_z_le, 32);
    TEST_ASSERT_EQUAL(1, res);

    /* P192 */
    res = ecc_jacob_verify(ecc_p192_jacob_mul_res_x_le, ecc_p192_jacob_mul_res_y_le, ecc_p192_jacob_mul_res_z_le, 24);
    TEST_ASSERT_EQUAL(1, res);
}
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT && SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
TEST(ecc, ecc_point_verification_and_jacobian_point_multiplication_on_SECP192R1_and_SECP256R1)
{
    test_ecc_jacob_mul_inner(true);
}
#endif

#if SOC_ECC_SUPPORT_POINT_ADDITION
static void ecc_point_addition(uint8_t *px_le, uint8_t *py_le, uint8_t *qx_le, uint8_t *qy_le, uint8_t *qz_le,
                               uint8_t len, bool jacob_output,
                               uint8_t *x_res_le, uint8_t *y_res_le, uint8_t *z_res_le)
{
    ecc_enable_and_reset();

    ecc_hal_write_point_add_param(px_le, py_le, qx_le, qy_le, qz_le, len);

    ecc_hal_set_mode(ECC_MODE_POINT_ADD);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_point_add_result(x_res_le, y_res_le, z_res_le, len, jacob_output);
    ecc_disable();
}

TEST(ecc, ecc_point_addition_on_SECP192R1_and_SECP256R1)
{
    uint8_t scalar_le[32] = {0};
    uint8_t x_le[32] = {0};
    uint8_t y_le[32] = {0};
    uint8_t z_le[32] = {0};

    /* P256
     * R = 2 * P
     */
    ecc_be_to_le(ecc_p256_point_x, x_le, 32);
    ecc_be_to_le(ecc_p256_point_y, y_le, 32);

    scalar_le[0] = 0x2;

    uint8_t x_res_le[32];
    uint8_t y_res_le[32];
    uint8_t z_res_le[32];

    ecc_jacob_mul(scalar_le, x_le, y_le, 32, 0, x_res_le, y_res_le, z_res_le);

    uint8_t x_add_le[32];
    uint8_t y_add_le[32];
    uint8_t z_add_le[32];

    z_le[0] = 0x1;

    ecc_point_addition(x_le, y_le, x_le, y_le, z_le, 32, 1, x_add_le, y_add_le, z_add_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_add_le, x_res_le, 32, "X coordinate of P256 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_add_le, y_res_le, 32, "Y coordinate of P256 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(z_add_le, z_res_le, 32, "Z coordinate of P256 point addition");

    /* P192
     * R = 2 * P
     */
    ecc_be_to_le(ecc_p192_point_x, x_le, 24);
    ecc_be_to_le(ecc_p192_point_y, y_le, 24);

    scalar_le[0] = 0x2;

    ecc_jacob_mul(scalar_le, x_le, y_le, 24, 0, x_res_le, y_res_le, z_res_le);

    z_le[0] = 0x1;

    ecc_point_addition(x_le, y_le, x_le, y_le, z_le, 24, 1, x_add_le, y_add_le, z_add_le);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(x_add_le, x_res_le, 24, "X coordinate of P192 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(y_add_le, y_res_le, 24, "Y coordinate of P192 point addition");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(z_add_le, z_res_le, 24, "Z coordinate of P192 point addition");
}
#endif

#if SOC_ECC_SUPPORT_MOD_ADD || SOC_ECC_SUPPORT_MOD_SUB || SOC_ECC_SUPPORT_MOD_MUL
static void ecc_mod_op(ecc_mode_t mode, const uint8_t *a, const uint8_t *b, uint8_t len, uint8_t *res_le)
{
    ecc_enable_and_reset();

    ecc_hal_write_mod_op_param(a, b, len);

    ecc_hal_set_mode(mode);

    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ecc_hal_read_mod_op_result(res_le, len);
    ecc_disable();
}
#endif

#if SOC_ECC_SUPPORT_MOD_ADD
TEST(ecc, ecc_mod_addition_using_SECP192R1_and_SECP256R1_order_of_curve)
{
    uint8_t res[32] = {0};
    ecc_mod_op(ECC_MODE_MOD_ADD, ecc256_x, ecc256_y, 32, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_add_res, res, 32, "P256 mod addition");

    ecc_mod_op(ECC_MODE_MOD_ADD, ecc192_x, ecc192_y, 24, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_add_res, res, 24, "P192 mod addition");
}
#endif

#if SOC_ECC_SUPPORT_MOD_SUB
TEST(ecc, ecc_mod_subtraction_using_SECP192R1_and_SECP256R1_order_of_curve)
{
    uint8_t res[32] = {0};
    ecc_mod_op(ECC_MODE_MOD_SUB, ecc256_x, ecc256_y, 32, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_sub_res, res, 32, "P256 mod subtraction");

    ecc_mod_op(ECC_MODE_MOD_SUB, ecc192_x, ecc192_y, 24, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_sub_res, res, 24, "P192 mod subtraction");
}
#endif

#if SOC_ECC_SUPPORT_MOD_MUL
TEST(ecc, ecc_mod_multiplication_using_SECP192R1_and_SECP256R1_order_of_curve)
{
    uint8_t res[32] = {0};
    ecc_mod_op(ECC_MODE_MOD_MUL, ecc256_x, ecc256_y, 32, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc256_mul_res, res, 32, "P256 mod multiplication");

    ecc_mod_op(ECC_MODE_MOD_MUL, ecc192_x, ecc192_y, 24, res);
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ecc192_mul_res, res, 24, "P192 mod multiplication");
}
#endif

TEST_GROUP_RUNNER(ecc)
{
#if SOC_ECC_SUPPORT_POINT_MULT
    RUN_TEST_CASE(ecc, ecc_point_multiplication_on_SECP192R1_and_SECP256R1);
#if SOC_ECC_CONSTANT_TIME_POINT_MUL
    RUN_TEST_CASE(ecc, ecc_point_multiplication_const_time_check_on_SECP192R1_and_SECP256R1);
#endif
#endif

#if SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
    RUN_TEST_CASE(ecc, ecc_point_verification_on_SECP192R1_and_SECP256R1);
#endif

#if SOC_ECC_SUPPORT_POINT_MULT && SOC_ECC_SUPPORT_POINT_VERIFY && !defined(SOC_ECC_SUPPORT_POINT_VERIFY_QUIRK)
    RUN_TEST_CASE(ecc, ecc_point_verification_and_multiplication_on_SECP192R1_and_SECP256R1);
#endif

#if SOC_ECC_SUPPORT_POINT_DIVISION
    RUN_TEST_CASE(ecc, ecc_inverse_multiplication_or_mod_division_using_SECP192R1_and_SECP256R1_order_of_curve);
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT
    RUN_TEST_CASE(ecc, ecc_jacobian_point_multiplication_on_SECP192R1_and_SECP256R1);
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
    RUN_TEST_CASE(ecc, ecc_jacobian_point_verification_on_SECP192R1_and_SECP256R1);
#endif

#if SOC_ECC_SUPPORT_JACOB_POINT_MULT && SOC_ECC_SUPPORT_JACOB_POINT_VERIFY
    RUN_TEST_CASE(ecc, ecc_point_verification_and_jacobian_point_multiplication_on_SECP192R1_and_SECP256R1);
#endif

#if SOC_ECC_SUPPORT_POINT_ADDITION
    RUN_TEST_CASE(ecc, ecc_point_addition_on_SECP192R1_and_SECP256R1);
#endif

#if SOC_ECC_SUPPORT_MOD_ADD
    RUN_TEST_CASE(ecc, ecc_mod_addition_using_SECP192R1_and_SECP256R1_order_of_curve);
#endif

#if SOC_ECC_SUPPORT_MOD_SUB
    RUN_TEST_CASE(ecc, ecc_mod_subtraction_using_SECP192R1_and_SECP256R1_order_of_curve);
#endif

#if SOC_ECC_SUPPORT_MOD_MUL
    RUN_TEST_CASE(ecc, ecc_mod_multiplication_using_SECP192R1_and_SECP256R1_order_of_curve);
#endif
}
