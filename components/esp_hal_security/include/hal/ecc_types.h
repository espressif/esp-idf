/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* The values match the ECC work-mode register encoding on every chip that
   implements the mode. Raw value 1 is reserved in hardware. */
typedef enum {
    ECC_MODE_POINT_MUL                 = 0,     // (Rx, Ry) = K * (Px, Py)
    ECC_MODE_VERIFY                    = 2,     // Check if (Px, Py) are points on the curve
    ECC_MODE_VERIFY_THEN_POINT_MUL     = 3,     // Verify and then perform point multiplication
    ECC_MODE_JACOBIAN_POINT_MUL        = 4,
    ECC_MODE_POINT_ADD                 = 5,
    ECC_MODE_JACOBIAN_POINT_VERIFY     = 6,
    ECC_MODE_POINT_VERIFY_JACOBIAN_MUL = 7,
    ECC_MODE_MOD_ADD                   = 8,
    ECC_MODE_MOD_SUB                   = 9,
    ECC_MODE_MOD_MUL                   = 10,
    ECC_MODE_INVERSE_MUL               = 11,    // R = K^(-1) * Py
} ecc_mode_t;

typedef enum {
    ECC_CURVE_SECP192R1           = 0x0,
    ECC_CURVE_SECP256R1,
    ECC_CURVE_SECP384R1,
    ECC_CURVE_SM2,
} ecc_curve_t;

typedef enum {
    ECC_MOD_N,                  // Order of the curve
    ECC_MOD_P,                  // Prime modulus
} ecc_mod_base_t;

#ifdef __cplusplus
}
#endif
