/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_tinycrypt_port.h"

#include <string.h>

#include "esp_crypto_lock.h"
#include "esp_crypto_periph_clk.h"
#include <tinycrypt/ecc.h>

#if SOC_ECC_SUPPORTED
#include "hal/ecc_hal.h"
#endif /* SOC_ECC_SUPPORTED */

#define ECC_MAX_PARAM_BYTES 48

#if SOC_ECC_SUPPORTED
static void uecc_vli_native_to_le(uint8_t *le, const uECC_word_t *native, uint16_t len)
{
    uint8_t be[ECC_MAX_PARAM_BYTES];

    uECC_vli_nativeToBytes(be, len, native);
    for (uint16_t i = 0; i < len; i++) {
        le[i] = be[len - 1 - i];
    }
}

static void uecc_vli_le_to_native(uECC_word_t *native, const uint8_t *le, uint16_t len)
{
    uint8_t be[ECC_MAX_PARAM_BYTES];

    for (uint16_t i = 0; i < len; i++) {
        be[i] = le[len - 1 - i];
    }
    uECC_vli_bytesToNative(native, be, len);
}

static void esp_tinycrypt_acquire_ecc_hardware(void)
{
    esp_crypto_ecc_lock_acquire();
    esp_crypto_ecc_enable_periph_clk(true);
}

static void esp_tinycrypt_release_ecc_hardware(void)
{
    esp_crypto_ecc_enable_periph_clk(false);
    esp_crypto_ecc_lock_release();
}
#endif /* SOC_ECC_SUPPORTED */

int esp_tinycrypt_verify_ecc_point(const uint8_t *pk_x, const uint8_t *pk_y, uint8_t length)
{
#if SOC_ECC_SUPPORTED
    int result;
    uint8_t px_le[ECC_MAX_PARAM_BYTES];
    uint8_t py_le[ECC_MAX_PARAM_BYTES];

    uecc_vli_native_to_le(px_le, (const uECC_word_t *)pk_x, length);
    uecc_vli_native_to_le(py_le, (const uECC_word_t *)pk_y, length);

    esp_tinycrypt_acquire_ecc_hardware();

    ecc_hal_write_verify_param(px_le, py_le, length);
    ecc_hal_set_mode(ECC_MODE_VERIFY);
    ecc_hal_start_calc();
    while (!ecc_hal_is_calc_finished());
    result = ecc_hal_read_verify_result();

    esp_tinycrypt_release_ecc_hardware();

    if (result == 1) {
        return 0;
    } else {
        return -1;
    }
#else
    (void)pk_x;
    (void)pk_y;
    (void)length;
    return -1;
#endif /* SOC_ECC_SUPPORTED */
}

int esp_tinycrypt_calc_ecc_mult(const uECC_word_t *point_x, const uECC_word_t *point_y,
                                const uECC_word_t *scalar, uECC_word_t *result_x,
                                uECC_word_t *result_y, uint8_t num_bytes, bool verify_first)
{
#if SOC_ECC_SUPPORTED
    int ret;
    ecc_mode_t work_mode = verify_first ? ECC_MODE_VERIFY_THEN_POINT_MUL : ECC_MODE_POINT_MUL;
    uint8_t k_le[ECC_MAX_PARAM_BYTES];
    uint8_t px_le[ECC_MAX_PARAM_BYTES];
    uint8_t py_le[ECC_MAX_PARAM_BYTES];
    uint8_t rx_le[ECC_MAX_PARAM_BYTES];
    uint8_t ry_le[ECC_MAX_PARAM_BYTES];

    memset(k_le, 0, sizeof(k_le));
    memset(px_le, 0, sizeof(px_le));
    memset(py_le, 0, sizeof(py_le));
    memset(rx_le, 0, sizeof(rx_le));
    memset(ry_le, 0, sizeof(ry_le));

    uecc_vli_native_to_le(k_le, scalar, num_bytes);
    uecc_vli_native_to_le(px_le, point_x, num_bytes);
    uecc_vli_native_to_le(py_le, point_y, num_bytes);

    esp_tinycrypt_acquire_ecc_hardware();

    ecc_hal_write_mul_param(k_le, px_le, py_le, num_bytes);
    ecc_hal_set_mode(work_mode);
    /*
     * Enable constant-time point multiplication operations for the ECC hardware accelerator,
     * if supported for the given target. This protects the ECC multiplication operation from
     * timing attacks. This increases the time taken (by almost 50%) for some point
     * multiplication operations performed by the ECC hardware accelerator.
     */
    ecc_hal_enable_constant_time_point_mul(true);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished());

    ret = ecc_hal_read_mul_result(rx_le, ry_le, num_bytes);

    esp_tinycrypt_release_ecc_hardware();

    if (ret != 0) {
        return -1;
    }

    uecc_vli_le_to_native(result_x, rx_le, num_bytes);
    uecc_vli_le_to_native(result_y, ry_le, num_bytes);
    return 0;
#else
    (void)point_x;
    (void)point_y;
    (void)scalar;
    (void)result_x;
    (void)result_y;
    (void)num_bytes;
    (void)verify_first;
    return -1;
#endif /* SOC_ECC_SUPPORTED */
}
