/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

typedef enum {
    ECC_MODE_POINT_MUL       = 0x0,    // (Rx, Ry) = K * (Px, Py)
    ECC_MODE_INVERSE_MUL,              // R = K^(-1) * Py
    ECC_MODE_VERIFY,                   // Check if (Px, Py) are points on the curve
    ECC_MODE_VERIFY_THEN_POINT_MUL,    // Verify and then perform point multiplication
} ecc_mode_t;

typedef enum {
    ECC_CURVE_SECP192R1           = 0x0,
    ECC_CURVE_SECP256R1,
} ecc_curve_t;
