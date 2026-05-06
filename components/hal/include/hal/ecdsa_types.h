/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
    ECDSA_CURVE_SECP384R1,
    ECDSA_CURVE_SM2,
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

/**
 * @brief Macro to combine two key blocks into a single integer
 * @note Least significant 4 bits stores block number of the low key block, and the next 4 bits store the high key block number.
 */
#define HAL_ECDSA_COMBINE_KEY_BLOCKS(blk_high, blk_low) (((blk_high) << 4) | (blk_low))

/**
 * @brief Macro to extract high and low key block numbers from a combined integer
 * @note Extracts high block from bits 4-7 and low block from bits 0-3
 */
#define HAL_ECDSA_EXTRACT_KEY_BLOCKS(combined_blk, blk_high, blk_low) do { \
    (blk_high) = ((combined_blk) >> 4) & 0xF; \
    (blk_low) = (combined_blk) & 0xF; \
} while(0)

#ifdef __cplusplus
}
#endif
