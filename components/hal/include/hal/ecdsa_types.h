/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ECDSA peripheral work modes
 */
typedef enum {
    ECDSA_MODE_SIGN_VERIFY,
    ECDSA_MODE_SIGN_GEN,
} ecdsa_mode_t;

/**
 * @brief ECDSA curve options
 */
typedef enum {
    ECDSA_CURVE_SECP192R1,
    ECDSA_CURVE_SECP256R1,
} ecdsa_curve_t;

/**
 * @brief Source of SHA message that is to be signed/verified
 */
typedef enum {
    ECDSA_Z_USE_SHA_PERI,
    ECDSA_Z_USER_PROVIDED,
} ecdsa_sha_mode_t;

#ifdef __cplusplus
}
#endif
