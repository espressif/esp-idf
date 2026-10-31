/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <string.h>
#include "esp_sm2.h"

#if SOC_ECC_SUPPORT_CURVE_SM2 && SOC_ECC_EXTENDED_MODES_SUPPORTED

#include "esp_crypto_lock.h"
#include "esp_crypto_periph_clk.h"
#include "esp_random.h"
#include "hal/ecc_hal.h"
#include "hal/ecc_types.h"
#if SOC_EFUSE_DIS_SM_CRYPT
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#endif

#define SM2_LEN ESP_SM2_PARAM_LEN

/* This component must not depend on mbedtls, because mbedtls already
   requires it. Zeroize locally, in the same way as esp_ds.c. */
static void esp_sm2_zeroize(void *buf, size_t len)
{
    volatile uint8_t *p = (volatile uint8_t *)buf;
    for (size_t i = 0; i < len; i++) {
        *p++ = 0;
    }
}

/* Recommended SM2 curve parameters (GB/T 32918.5), big-endian. */
static const uint8_t sm2_n_minus_1[SM2_LEN] = {
    0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x72, 0x03, 0xdf, 0x6b, 0x21, 0xc6, 0x05, 0x2b,
    0x53, 0xbb, 0xf4, 0x09, 0x39, 0xd5, 0x41, 0x22,
};
static const uint8_t sm2_n[SM2_LEN] = {
    0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x72, 0x03, 0xdf, 0x6b, 0x21, 0xc6, 0x05, 0x2b,
    0x53, 0xbb, 0xf4, 0x09, 0x39, 0xd5, 0x41, 0x23,
};
static const uint8_t sm2_gx[SM2_LEN] = {
    0x32, 0xc4, 0xae, 0x2c, 0x1f, 0x19, 0x81, 0x19,
    0x5f, 0x99, 0x04, 0x46, 0x6a, 0x39, 0xc9, 0x94,
    0x8f, 0xe3, 0x0b, 0xbf, 0xf2, 0x66, 0x0b, 0xe1,
    0x71, 0x5a, 0x45, 0x89, 0x33, 0x4c, 0x74, 0xc7,
};
static const uint8_t sm2_gy[SM2_LEN] = {
    0xbc, 0x37, 0x36, 0xa2, 0xf4, 0xf6, 0x77, 0x9c,
    0x59, 0xbd, 0xce, 0xe3, 0x6b, 0x69, 0x21, 0x53,
    0xd0, 0xa9, 0x87, 0x7c, 0xc6, 0x2a, 0x47, 0x40,
    0x02, 0xdf, 0x32, 0xe5, 0x21, 0x39, 0xf0, 0xa0,
};

static void sm2_reverse(const uint8_t *in, uint8_t *out)
{
    for (int i = 0; i < SM2_LEN; i++) {
        out[i] = in[SM2_LEN - 1 - i];
    }
}

static bool sm2_is_zero(const uint8_t *buf)
{
    uint8_t acc = 0;
    for (int i = 0; i < SM2_LEN; i++) {
        acc |= buf[i];
    }
    return acc == 0;
}

static esp_err_t sm2_check_available(void)
{
#if SOC_EFUSE_DIS_SM_CRYPT
    if (esp_efuse_read_field_bit(ESP_EFUSE_DIS_SM_CRYPT)) {
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif
    return ESP_OK;
}

/* Constant-time big-endian compare. Returns 1 when v < upper, else 0.
   The first differing byte decides; later bytes cannot change the answer.
   v may be a private key or a nonce, so no byte is allowed to end the loop
   early and no branch is allowed to depend on its value. */
static int sm2_ct_lt(const uint8_t *v, const uint8_t *upper)
{
    unsigned int lt = 0;
    unsigned int decided = 0;
    for (int i = 0; i < SM2_LEN; i++) {
        unsigned int a = v[i];
        unsigned int b = upper[i];
        unsigned int is_lt = ((a - b) >> 8) & 1;   /* borrow out of a - b */
        unsigned int is_gt = ((b - a) >> 8) & 1;
        lt |= is_lt & ~decided;
        decided |= is_lt | is_gt;
    }
    return lt & 1;
}

/* Valid scalar check: 1 <= v < upper. Buffers are big-endian. */
static bool sm2_scalar_in_range(const uint8_t *v, const uint8_t *upper)
{
    /* Bitwise AND, not &&: the compare is not skipped for a zero scalar. */
    return ((int)!sm2_is_zero(v) & sm2_ct_lt(v, upper)) != 0;
}

/* Reduce a little-endian value below n with one conditional subtraction.
   Valid for any v < 2n; every input here is < p < 2n or < 2^256 < 2n. */
static void sm2_cond_sub_n_le(uint8_t *v_le)
{
    for (int i = SM2_LEN - 1; i >= 0; i--) {
        uint8_t n_byte = sm2_n[SM2_LEN - 1 - i];
        if (v_le[i] > n_byte) {
            break;                      /* v > n: subtract below */
        }
        if (v_le[i] < n_byte) {
            return;                     /* v < n: nothing to do */
        }
        if (i == 0) {
            break;                      /* v == n: subtract to zero */
        }
    }
    int borrow = 0;
    for (int i = 0; i < SM2_LEN; i++) {
        int diff = (int)v_le[i] - (int)sm2_n[SM2_LEN - 1 - i] - borrow;
        borrow = diff < 0;
        v_le[i] = (uint8_t)diff;
    }
}

static void sm2_acquire(void)
{
    esp_crypto_ecc_lock_acquire();
    esp_crypto_ecc_enable_periph_clk(true);
#if SOC_ECC_CONSTANT_TIME_POINT_MUL
    ecc_hal_enable_constant_time_point_mul(true);
#endif
}

static void sm2_release(void)
{
    esp_crypto_ecc_enable_periph_clk(false);
    esp_crypto_ecc_lock_release();
}

static void sm2_hw_start(ecc_mode_t mode)
{
    ecc_hal_set_mode(mode);
    ecc_hal_start_calc();
    while (!ecc_hal_is_calc_finished()) {
    }
}

/* (rx, ry) = k * (x, y). All little-endian. The caller holds the lock.
   verify_first also checks that (x, y) is on the curve. */
static int sm2_hw_point_mul(const uint8_t *k_le, const uint8_t *x_le, const uint8_t *y_le,
                            bool verify_first, uint8_t *rx_le, uint8_t *ry_le)
{
    ecc_hal_write_mul_param(k_le, x_le, y_le, SM2_LEN, ECC_CURVE_SM2);
    sm2_hw_start(verify_first ? ECC_MODE_VERIFY_THEN_POINT_MUL : ECC_MODE_POINT_MUL);
    return ecc_hal_read_mul_result(rx_le, ry_le, SM2_LEN);
}

esp_err_t esp_sm2_get_pubkey(const uint8_t priv_key[ESP_SM2_PARAM_LEN],
                             uint8_t pub_x[ESP_SM2_PARAM_LEN],
                             uint8_t pub_y[ESP_SM2_PARAM_LEN])
{
    if (priv_key == NULL || pub_x == NULL || pub_y == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!sm2_scalar_in_range(priv_key, sm2_n_minus_1)) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = sm2_check_available();
    if (err != ESP_OK) {
        return err;
    }

    uint8_t d_le[SM2_LEN];
    uint8_t gx_le[SM2_LEN];
    uint8_t gy_le[SM2_LEN];
    uint8_t x_le[SM2_LEN];
    uint8_t y_le[SM2_LEN];
    sm2_reverse(priv_key, d_le);
    sm2_reverse(sm2_gx, gx_le);
    sm2_reverse(sm2_gy, gy_le);

    sm2_acquire();
    int ret = sm2_hw_point_mul(d_le, gx_le, gy_le, false, x_le, y_le);
    sm2_release();

    esp_sm2_zeroize(d_le, SM2_LEN);
    if (ret != 0) {
        return ESP_FAIL;
    }
    sm2_reverse(x_le, pub_x);
    sm2_reverse(y_le, pub_y);
    return ESP_OK;
}

/* r = op(a, b) mod base. All little-endian. The caller holds the lock. */
static int sm2_hw_mod_op(ecc_mode_t mode, ecc_mod_base_t base,
                         const uint8_t *a_le, const uint8_t *b_le, uint8_t *r_le)
{
    ecc_hal_set_mod_base(base);
    ecc_hal_write_mod_op_param(a_le, b_le, SM2_LEN, ECC_CURVE_SM2);
    sm2_hw_start(mode);
    return ecc_hal_read_mod_op_result(r_le, SM2_LEN);
}

/* r = num / den mod base. All little-endian. The caller holds the lock.
   INVERSE_MUL takes its operands in the same two registers as the other
   mod operations, so it uses the mod-op writer that pairs with the
   mod-op reader below. */
static int sm2_hw_mod_div(ecc_mod_base_t base,
                          const uint8_t *num_le, const uint8_t *den_le, uint8_t *r_le)
{
    ecc_hal_set_mod_base(base);
    ecc_hal_write_mod_op_param(num_le, den_le, SM2_LEN, ECC_CURVE_SM2);
    sm2_hw_start(ECC_MODE_INVERSE_MUL);
    return ecc_hal_read_mod_op_result(r_le, SM2_LEN);
}

/* Compute x1 = affine X of (s*G + t*P) into x1_le, reduced below n.
   Returns 0 on success, -1 on an invalid point or the point at infinity.
   The caller holds the lock.
   Only the on-curve check inside VERIFY_THEN_POINT_MUL can make a HAL read
   fail for the modes used here. The other return-value checks below are
   defensive, so that a future HAL that can fail is not ignored. */
static int sm2_hw_sum_point_x(const uint8_t *s_le, const uint8_t *t_le,
                              const uint8_t *px_le, const uint8_t *py_le, uint8_t *x1_le)
{
    uint8_t ax[SM2_LEN], ay[SM2_LEN], bx[SM2_LEN], by[SM2_LEN];
    uint8_t jx[SM2_LEN], jy[SM2_LEN], jz[SM2_LEN], z2[SM2_LEN];
    uint8_t gx_le[SM2_LEN], gy_le[SM2_LEN], one[SM2_LEN] = {1};

    sm2_reverse(sm2_gx, gx_le);
    sm2_reverse(sm2_gy, gy_le);
    if (sm2_hw_point_mul(s_le, gx_le, gy_le, false, ax, ay) != 0) {
        return -1;
    }
    /* verify_first validates the public key on the curve. */
    if (sm2_hw_point_mul(t_le, px_le, py_le, true, bx, by) != 0) {
        return -1;
    }
    if (memcmp(ax, bx, SM2_LEN) == 0) {
        if (memcmp(ay, by, SM2_LEN) != 0) {
            return -1;                        /* A + (-A): point at infinity */
        }
        /* A == B: double explicitly instead of feeding equal addends to the
           add mode. */
        uint8_t two[SM2_LEN] = {2};
        if (sm2_hw_point_mul(two, ax, ay, false, jx, jy) != 0) {
            return -1;
        }
        memcpy(x1_le, jx, SM2_LEN);
        sm2_cond_sub_n_le(x1_le);
        return 0;
    }
    ecc_hal_write_point_add_param(ax, ay, bx, by, one, SM2_LEN, ECC_CURVE_SM2);
    sm2_hw_start(ECC_MODE_POINT_ADD);
    if (ecc_hal_read_point_add_result(jx, jy, jz, SM2_LEN, true) != 0) {
        return -1;
    }
    if (sm2_is_zero(jz)) {
        return -1;                            /* point at infinity */
    }
    if (sm2_hw_mod_op(ECC_MODE_MOD_MUL, ECC_MOD_P, jz, jz, z2) != 0) {
        return -1;
    }
    if (sm2_hw_mod_div(ECC_MOD_P, jx, z2, x1_le) != 0) {
        return -1;
    }
    sm2_cond_sub_n_le(x1_le);                 /* x1 < p < 2n: reduce below n */
    return 0;
}

esp_err_t esp_sm2_verify(const uint8_t digest[ESP_SM2_PARAM_LEN],
                         const uint8_t pub_x[ESP_SM2_PARAM_LEN],
                         const uint8_t pub_y[ESP_SM2_PARAM_LEN],
                         const uint8_t sig_r[ESP_SM2_PARAM_LEN],
                         const uint8_t sig_s[ESP_SM2_PARAM_LEN])
{
    if (digest == NULL || pub_x == NULL || pub_y == NULL || sig_r == NULL || sig_s == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!sm2_scalar_in_range(sig_r, sm2_n) || !sm2_scalar_in_range(sig_s, sm2_n)) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = sm2_check_available();
    if (err != ESP_OK) {
        return err;
    }

    uint8_t e_le[SM2_LEN], r_le[SM2_LEN], s_le[SM2_LEN];
    uint8_t px_le[SM2_LEN], py_le[SM2_LEN];
    uint8_t t_le[SM2_LEN], x1_le[SM2_LEN], rr_le[SM2_LEN];
    sm2_reverse(digest, e_le);
    sm2_cond_sub_n_le(e_le);                  /* e may reach 2^256 - 1 < 2n */
    sm2_reverse(sig_r, r_le);
    sm2_reverse(sig_s, s_le);
    sm2_reverse(pub_x, px_le);
    sm2_reverse(pub_y, py_le);

    sm2_acquire();
    int ret = sm2_hw_mod_op(ECC_MODE_MOD_ADD, ECC_MOD_N, r_le, s_le, t_le);
    if (ret == 0 && sm2_is_zero(t_le)) {
        ret = -1;                             /* t = (r + s) mod n must not be 0 */
    }
    if (ret == 0) {
        ret = sm2_hw_sum_point_x(s_le, t_le, px_le, py_le, x1_le);
    }
    if (ret == 0) {
        ret = sm2_hw_mod_op(ECC_MODE_MOD_ADD, ECC_MOD_N, e_le, x1_le, rr_le);
    }
    sm2_release();

    if (ret != 0 || memcmp(rr_le, r_le, SM2_LEN) != 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/* One signing attempt with a fresh random k. Every buffer here is
   little-endian, the outputs r_le and s_le included; the caller converts
   them back to big-endian.
   Returns 0 on success, 1 when the attempt must be retried with a new k,
   -1 on a hardware error. The caller holds the lock. */
static int sm2_sign_attempt(const uint8_t *e_le, const uint8_t *d_le,
                            uint8_t *r_le, uint8_t *s_le)
{
    uint8_t k_be[SM2_LEN], k_le[SM2_LEN];
    uint8_t gx_le[SM2_LEN], gy_le[SM2_LEN];
    uint8_t x1[SM2_LEN], y1[SM2_LEN], tmp[SM2_LEN], num[SM2_LEN], den[SM2_LEN];
    uint8_t one[SM2_LEN] = {1};
    int ret = 1;

    esp_fill_random(k_be, SM2_LEN);
    if (!sm2_scalar_in_range(k_be, sm2_n)) {
        goto cleanup;                          /* retry */
    }
    sm2_reverse(k_be, k_le);
    sm2_reverse(sm2_gx, gx_le);
    sm2_reverse(sm2_gy, gy_le);

    ret = -1;
    if (sm2_hw_point_mul(k_le, gx_le, gy_le, false, x1, y1) != 0) {
        goto cleanup;
    }
    sm2_cond_sub_n_le(x1);                     /* x1 < p < 2n */
    if (sm2_hw_mod_op(ECC_MODE_MOD_ADD, ECC_MOD_N, e_le, x1, r_le) != 0) {
        goto cleanup;
    }
    /* Retry when r == 0 or r + k == n (sum mod n == 0 for nonzero r, k). */
    if (sm2_is_zero(r_le)) {
        ret = 1;
        goto cleanup;
    }
    if (sm2_hw_mod_op(ECC_MODE_MOD_ADD, ECC_MOD_N, r_le, k_le, tmp) != 0) {
        goto cleanup;
    }
    if (sm2_is_zero(tmp)) {
        ret = 1;
        goto cleanup;
    }
    if (sm2_hw_mod_op(ECC_MODE_MOD_MUL, ECC_MOD_N, r_le, d_le, tmp) != 0 ||
            sm2_hw_mod_op(ECC_MODE_MOD_SUB, ECC_MOD_N, k_le, tmp, num) != 0 ||
            sm2_hw_mod_op(ECC_MODE_MOD_ADD, ECC_MOD_N, d_le, one, den) != 0 ||
            sm2_hw_mod_div(ECC_MOD_N, num, den, s_le) != 0) {
        goto cleanup;
    }
    ret = sm2_is_zero(s_le) ? 1 : 0;           /* s == 0: retry */

cleanup:
    /* k, r*d, k - r*d and d + 1 each reveal the private key. */
    esp_sm2_zeroize(k_be, SM2_LEN);
    esp_sm2_zeroize(k_le, SM2_LEN);
    esp_sm2_zeroize(num, SM2_LEN);
    esp_sm2_zeroize(den, SM2_LEN);
    esp_sm2_zeroize(tmp, SM2_LEN);
    return ret;
}

esp_err_t esp_sm2_sign(const uint8_t digest[ESP_SM2_PARAM_LEN],
                       const uint8_t priv_key[ESP_SM2_PARAM_LEN],
                       uint8_t sig_r[ESP_SM2_PARAM_LEN],
                       uint8_t sig_s[ESP_SM2_PARAM_LEN])
{
    if (digest == NULL || priv_key == NULL || sig_r == NULL || sig_s == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* The n-2 bound is what keeps den = (d + 1) mod n nonzero, so the
       division by (1 + d) in sm2_sign_attempt() never divides by zero.
       Do not loosen this to sm2_n to match the verify path. */
    if (!sm2_scalar_in_range(priv_key, sm2_n_minus_1)) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = sm2_check_available();
    if (err != ESP_OK) {
        return err;
    }

    uint8_t e_le[SM2_LEN], d_le[SM2_LEN], r_le[SM2_LEN], s_le[SM2_LEN];
    sm2_reverse(digest, e_le);
    sm2_cond_sub_n_le(e_le);
    sm2_reverse(priv_key, d_le);

    int ret = 1;
    sm2_acquire();
    /* Retries are astronomically rare; the bound only guards a broken RNG. */
    for (int attempt = 0; ret == 1 && attempt < 16; attempt++) {
        ret = sm2_sign_attempt(e_le, d_le, r_le, s_le);
    }
    sm2_release();

    esp_sm2_zeroize(d_le, SM2_LEN);
    if (ret != 0) {
        return ESP_FAIL;
    }
    sm2_reverse(r_le, sig_r);
    sm2_reverse(s_le, sig_s);
    return ESP_OK;
}

#endif /* SOC_ECC_SUPPORT_CURVE_SM2 && SOC_ECC_EXTENDED_MODES_SUPPORTED */
