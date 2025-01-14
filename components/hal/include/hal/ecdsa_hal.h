/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/ecdsa_types.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_HAL_ECDSA_GEN_SIG_CM

#define ECDSA_SIGN_MAX_DUMMY_OP_COUNT 0x7

/* This value defines the maximum dummy operation count for the ECDSA signature countermeasure.
   Higher the number, better the countermeasure's effectiveness against attacks.
   At the same time higher number leads to slower performance.
   After the countermeasure is enabled, hardware ECDSA signature operation
   shall take time approximately equal to original time multiplied by this number.
   If you observe that the reduced performance is affecting your use-case then you may try reducing this time to the minimum. */
#endif /* CONFIG_HAL_ECDSA_GEN_SIG_CM */
/*
 * ECDSA peripheral config structure
 */
typedef struct {
    ecdsa_mode_t mode;              /* Mode of operation */
    ecdsa_curve_t curve;            /* Curve to use for operation */
    ecdsa_sha_mode_t sha_mode;      /* Source of SHA that needs to be signed */
    int efuse_key_blk;              /* Efuse block to use as ECDSA key (The purpose of the efuse block must be ECDSA_KEY) */
} ecdsa_hal_config_t;

/**
 * @brief Generate ECDSA signature
 *
 * @param conf Configuration for ECDSA operation, see ``ecdsa_hal_config_t``
 * @param hash Hash that is to be signed
 * @param r_out Buffer that will contain `R` component of ECDSA signature
 * @param s_out Buffer that will contain `S` component of ECDSA signature
 * @param len Length of the r_out and s_out buffer (32 bytes for SECP256R1, 24 for SECP192R1)
 */
void ecdsa_hal_gen_signature(ecdsa_hal_config_t *conf, const uint8_t *hash,
                            uint8_t *r_out, uint8_t *s_out, uint16_t len);

/**
 * @brief Verify given ECDSA signature
 *
 * @param conf Configuration for ECDSA operation, see ``ecdsa_hal_config_t``
 * @param hash Hash that was signed
 * @param r `R` component of ECDSA signature
 * @param s `S` component of ECDSA signature
 * @param pub_x X coordinate of public key
 * @param pub_y Y coordinate of public key
 * @param len Length of r and s buffer (32 bytes for SECP256R1, 24 for SECP192R1)
 *
 * @return - 0, if the signature matches
 *         - -1, if verification fails
 */
int ecdsa_hal_verify_signature(ecdsa_hal_config_t *conf, const uint8_t *hash, const uint8_t *r, const uint8_t *s,
                               const uint8_t *pub_x, const uint8_t *pub_y, uint16_t len);

/**
 * @brief Check if the ECDSA operation is successful
 *
 * @return - true, if the ECDSA operation is successful
 *         - false, if the ECDSA operation fails
 */
bool ecdsa_hal_get_operation_result(void);

#ifdef __cplusplus
}
#endif
