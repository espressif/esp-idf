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
#include "soc/soc_caps.h"

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

/*
 * Curve orders n in little-endian (LSB first), for the FIPS-style scalar
 * range check below.
 */
static const uint8_t ecc_p192_n_le[24] = {
    0x31, 0x28, 0xd2, 0xb4, 0xb1, 0xc9, 0x6b, 0x14,
    0x36, 0xf8, 0xde, 0x99, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
static const uint8_t ecc_p256_n_le[32] = {
    0x51, 0x25, 0x63, 0xfc, 0xc2, 0xca, 0xb9, 0xf3,
    0x84, 0x9e, 0x17, 0xa7, 0xad, 0xfa, 0xe6, 0xbc,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
};
#if SOC_ECC_SUPPORT_CURVE_P384
static const uint8_t ecc_p384_n_le[48] = {
    0x73, 0x29, 0xc5, 0xcc, 0x6a, 0x19, 0xec, 0xec,
    0x7a, 0xa7, 0xb0, 0x48, 0xb2, 0x0d, 0x1a, 0x58,
    0xdf, 0x2d, 0x37, 0xf4, 0x81, 0x4d, 0x63, 0xc7,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
#endif

static const uint8_t ecc_p192_p_le[24] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
static const uint8_t ecc_p256_p_le[32] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
};
#if SOC_ECC_SUPPORT_CURVE_P384
static const uint8_t ecc_p384_p_le[48] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
#endif

static const uint8_t *ecc_curve_order_for_len(uint16_t len)
{
    switch (len) {
    case 24: return ecc_p192_n_le;
    case 32: return ecc_p256_n_le;
#if SOC_ECC_SUPPORT_CURVE_P384
    case 48: return ecc_p384_n_le;
#endif
    default: return NULL;
    }
}

static const uint8_t *ecc_curve_prime_for_len(uint16_t len)
{
    switch (len) {
    case 24: return ecc_p192_p_le;
    case 32: return ecc_p256_p_le;
#if SOC_ECC_SUPPORT_CURVE_P384
    case 48: return ecc_p384_p_le;
#endif
    default: return NULL;
    }
}

static uint32_t ecc_le_less_than(const uint8_t *a_le, const uint8_t *b_le, uint16_t len)
{
    uint32_t borrow = 0;

    for (uint16_t i = 0; i < len; i++) {
        uint32_t diff = (uint32_t)a_le[i] - b_le[i] - borrow;
        borrow = (diff >> 8) & 1;
    }

    return borrow;
}

static bool ecc_scalar_bounds_ok(const uint8_t *scalar_le, const uint8_t *n_le, uint16_t len)
{
    uint8_t fold = 0;

    for (uint16_t i = 0; i < len; i++) {
        fold |= scalar_le[i];
    }

    return (fold != 0) & (ecc_le_less_than(scalar_le, n_le, len) == 1);
}

static bool ecc_point_coords_in_range(const ecc_point_t *point)
{
    const uint8_t *p_le = ecc_curve_prime_for_len(point->len);
    if (p_le == NULL) {
        return false;
    }

    return (ecc_le_less_than(point->x, p_le, point->len) & ecc_le_less_than(point->y, p_le, point->len)) == 1;
}

static bool ecc_scalar_in_range(const uint8_t *scalar_le, uint16_t len)
{
    const uint8_t *n_le = ecc_curve_order_for_len(len);

    return n_le != NULL && ecc_scalar_bounds_ok(scalar_le, n_le, len);
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

    if (!ecc_scalar_in_range(scalar, len)) {
        memset(result, 0, sizeof(ecc_point_t));
        return -1;
    }

    if (verify_first && !ecc_point_coords_in_range(point)) {
        memset(result, 0, sizeof(ecc_point_t));
        return -1;
    }

    esp_ecc_acquire_hardware();

    ecc_hal_write_mul_param(scalar, point->x, point->y, len);
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

    if (!ecc_point_coords_in_range(point)) {
        return 0;
    }

    esp_ecc_acquire_hardware();
    ecc_hal_write_verify_param(point->x, point->y, len);
    ecc_hal_set_mode(ECC_MODE_VERIFY);
    ecc_hal_start_calc();

    while (!ecc_hal_is_calc_finished()) {
        ;
    }

    result = ecc_hal_read_verify_result();

    esp_ecc_release_hardware();

    return result;
}
