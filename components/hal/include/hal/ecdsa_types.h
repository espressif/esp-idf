/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ECDSA peripheral work modes
 */
typedef enum {
    ECDSA_MODE_SIGN_VERIFY,
    ECDSA_MODE_SIGN_GEN,
    ECDSA_MODE_EXPORT_PUBKEY,
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

/**
 * @brief ECDSA signature type
 */
typedef enum {
    ECDSA_K_TYPE_TRNG,
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    ECDSA_K_TYPE_DETERMINISITIC,
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
} ecdsa_sign_type_t;

#ifdef __cplusplus
}
#endif
