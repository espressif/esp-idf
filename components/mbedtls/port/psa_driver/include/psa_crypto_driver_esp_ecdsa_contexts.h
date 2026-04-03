/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#pragma once

#include "esp_types.h"
#include "soc/soc_caps.h"
#include "psa/crypto_driver_common.h"
#include "sdkconfig.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#if defined(ESP_ECDSA_DRIVER_ENABLED)
#if SOC_ECDSA_SUPPORT_CURVE_P384
#define MAX_ECDSA_COMPONENT_LEN     48
#define MAX_ECDSA_SHA_LEN           48
#else
#define MAX_ECDSA_COMPONENT_LEN     32
#define MAX_ECDSA_SHA_LEN           32
#endif

#define ECDSA_SHA_LEN               32
#if SOC_ECDSA_SUPPORT_CURVE_P384
#define ECDSA_SHA_LEN_P384          48
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

/**
 * @brief ECDSA curve options
 */
typedef enum {
    ESP_ECDSA_CURVE_SECP192R1,
    ESP_ECDSA_CURVE_SECP256R1,
    ESP_ECDSA_CURVE_SECP384R1,
    ESP_ECDSA_CURVE_MAX,
} esp_ecdsa_curve_t;

/**
 * @brief Structure to store opaque key metadata.
 */
typedef struct {
    esp_ecdsa_curve_t curve;                 /**< ECDSA curve */
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    const char *tee_key_id;                 /**< TEE secure storage key id */
#endif
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *key_recovery_info;                /**< Pointer to the key recovery info for ECDSA key */
#endif
    uint8_t efuse_block;                    /**< eFuse block id for ECDSA private key e.g. EFUSE_BLK_KEY0, EFUSE_BLK_KEY1 */
} esp_ecdsa_opaque_key_t;

#if !(__DOXYGEN__) // No need to document these structures, these are internal to the driver
/* The buffers are stored in the little-endian format */
typedef struct {
    esp_ecdsa_curve_t curve;
    uint8_t r[MAX_ECDSA_COMPONENT_LEN];
    uint8_t s[MAX_ECDSA_COMPONENT_LEN];
    uint8_t qx[MAX_ECDSA_COMPONENT_LEN];
    uint8_t qy[MAX_ECDSA_COMPONENT_LEN];
    uint8_t sha[MAX_ECDSA_SHA_LEN];
    size_t sha_len;
    size_t key_len;
} esp_ecdsa_transparent_verify_hash_operation_t;

/* The buffers are stored in the little-endian format */
typedef struct {
    psa_algorithm_t alg;
    const esp_ecdsa_opaque_key_t *opaque_key;
    uint8_t r[MAX_ECDSA_COMPONENT_LEN];
    uint8_t s[MAX_ECDSA_COMPONENT_LEN];
    uint8_t sha[MAX_ECDSA_SHA_LEN];
    size_t sha_len;
    size_t key_len;
} esp_ecdsa_opaque_sign_hash_operation_t;
#endif /* !(__DOXYGEN__) */
#endif /* ESP_ECDSA_DRIVER_ENABLED */
#ifdef __cplusplus
}
#endif
