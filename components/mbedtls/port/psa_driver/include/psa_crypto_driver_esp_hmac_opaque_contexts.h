/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_types.h"
#include "soc/soc_caps.h"
#include "psa/crypto_driver_common.h"
#include "hal/hmac_types.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif

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
    uint8_t efuse_key_id;                                              /**< eFuse key block id for HMAC key */
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *key_recovery_info;                /**< Pointer to the key recovery info for HMAC key */
#endif /* SOC_KEY_MANAGER_SUPPORTED */
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
