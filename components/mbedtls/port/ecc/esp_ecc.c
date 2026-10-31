/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>

#include "esp_crypto_lock.h"
#include "esp_crypto_periph_clk.h"
#include "ecc_impl.h"
#include "hal/ecc_hal.h"
#include "hal/ecc_types.h"
#include "soc/soc_caps.h"

#define ECC_P192_SIZE_BYTES  P192_LEN
#define ECC_P256_SIZE_BYTES  P256_LEN
#define ECC_P384_SIZE_BYTES  P384_LEN

static void esp_ecc_acquire_hardware(void)
{
    esp_crypto_ecc_lock_acquire();

    esp_crypto_ecc_enable_periph_clk(true);
}

static void esp_ecc_release_hardware(void)
{
    esp_crypto_ecc_enable_periph_clk(false);

    esp_crypto_ecc_lock_release();
}

/* Infer the NIST curve from the coordinate length.
   esp_ecc_point_multiply() / esp_ecc_point_verify() only expose the NIST
   SECP family; SM2 (which shares the 32-byte size with SECP256R1) is a
   distinct curve family and is selectable only at the HAL level via
   ecc_hal_write_*_param() with ECC_CURVE_SM2. */
static inline ecc_curve_t curve_from_len(uint16_t len)
{
    if (len == ECC_P384_SIZE_BYTES) {
        return ECC_CURVE_SECP384R1;
    } else if (len == ECC_P256_SIZE_BYTES) {
        return ECC_CURVE_SECP256R1;
    } else {
        return ECC_CURVE_SECP192R1;
    }
}

int esp_ecc_point_multiply(const ecc_point_t *point, const uint8_t *scalar, ecc_point_t *result, bool verify_first)
{
    int ret = -1;
    uint16_t len = point->len;
    ecc_mode_t work_mode = verify_first ? ECC_MODE_VERIFY_THEN_POINT_MUL : ECC_MODE_POINT_MUL;

    /* len is used as the HW read/write byte count for the fixed-size ecc_point_t buffers;
     * reject any value that is not a supported curve length before touching hardware. On the
     * TEE secure-service path this field is attacker-controlled (CWE-20 -> OOB read/write). */
    if (len != P192_LEN && len != P256_LEN
#if SOC_ECC_SUPPORT_CURVE_P384
            && len != P384_LEN
#endif
       ) {
        return -1;
    }

    esp_ecc_acquire_hardware();

    ecc_hal_write_mul_param(scalar, point->x, point->y, len, curve_from_len(len));
    ecc_hal_set_mode(work_mode);
    /*
     * Enable constant-time point multiplication operations for the ECC hardware accelerator,
     * if supported for the given target. This protects the ECC multiplication operation from
     * timing attacks. This increases the time taken (by almost 50%) for some point
     * multiplication operations performed by the ECC hardware accelerator.
     */
    ecc_hal_enable_constant_time_point_mul(true);
    ecc_hal_start_calc();

    memset(result, 0, sizeof(ecc_point_t));

    result->len = len;

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    ret = ecc_hal_read_mul_result(result->x, result->y, len);

    esp_ecc_release_hardware();

    return ret;
}

int esp_ecc_point_verify(const ecc_point_t *point)
{
    int result;
    const unsigned len = point->len;

    if (len != P192_LEN && len != P256_LEN
#if SOC_ECC_SUPPORT_CURVE_P384
            && len != P384_LEN
#endif
       ) {
        return 0;
    }

    esp_ecc_acquire_hardware();
    ecc_hal_write_verify_param(point->x, point->y, point->len, curve_from_len(point->len));
    ecc_hal_set_mode(ECC_MODE_VERIFY);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    result = ecc_hal_read_verify_result();

    esp_ecc_release_hardware();

    return result;
}
