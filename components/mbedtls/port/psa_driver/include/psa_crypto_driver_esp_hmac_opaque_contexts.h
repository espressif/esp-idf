/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_types.h"
#include "soc/soc_caps.h"
#include "psa/crypto_driver_common.h"

#ifdef __cplusplus
extern "C" {
#endif
#if defined(ESP_HMAC_OPAQUE_DRIVER_ENABLED)
/**
 * @brief Size of HMAC result in bytes (the opaque driver only supports SHA-256 based HMAC)
 */
#define ESP_HMAC_RESULT_SIZE PSA_HASH_LENGTH(PSA_ALG_HMAC(PSA_ALG_SHA_256))

/**
 * @brief Structure to store opaque HMAC key.
 */
typedef struct {
    bool use_km_key;                        /**< Use key deployed in the key manager */
    uint8_t efuse_block;                    /**< eFuse block id for HMAC key */
} esp_hmac_opaque_key_t;

/**
 * @brief Structure to store opaque HMAC operation context.
 */
typedef struct {
    const esp_hmac_opaque_key_t *opaque_key;    /**< Pointer to the opaque key structure */
    uint8_t hmac[ESP_HMAC_RESULT_SIZE];         /**< Buffer to store the HMAC result */
} esp_hmac_opaque_operation_t;

#endif /* ESP_HMAC_OPAQUE_DRIVER_ENABLED */
#ifdef __cplusplus
}
#endif
