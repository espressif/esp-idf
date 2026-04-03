/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if defined(ESP_RSA_DS_DRIVER_ENABLED)
#include "psa/crypto_driver_common.h"
#include "esp_ds.h"
#include "hal/hmac_types.h"

#include "soc/soc_caps.h"
#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RSA-DS padding options
 */
typedef enum {
    ESP_RSA_DS_PADDING_PKCS_V15,
    ESP_RSA_DS_PADDING_PSS,
    ESP_RSA_DS_PADDING_OAEP,
    ESP_RSA_DS_PADDING_INVALID = -1
} esp_rsa_ds_padding_t;

/**
 * @brief ESP DS data context
 * This context is used to store the ESP DS data.
 *
 * When passed to psa_import_key() for PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE, the key material
 * (this struct and the esp_ds_data_t pointed to by esp_ds_data) must remain valid
 * until psa_destroy_key() is called on the imported key.
 */
typedef struct {
    esp_ds_data_t *esp_ds_data;     /**< Pointer to the esp ds data */
    uint8_t efuse_key_id;           /**< efuse block id in which the HMAC key for the DS peripheral is stored e.g. 0,1*/
    uint16_t rsa_length_bits;       /**< length of RSA private key in bits e.g. 2048 */
} esp_ds_data_ctx_t;

typedef struct {
    esp_ds_data_ctx_t *ds_data_ctx;
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *key_recovery_info;        /**< Pointer to the key recovery info for DS key */
#endif /* SOC_KEY_MANAGER_SUPPORTED */
} esp_rsa_ds_opaque_key_t;

#if !(__DOXYGEN__) // No need to document these structures, these are internal to the driver
/* The buffers are stored in the little-endian format */
typedef struct {
    const esp_rsa_ds_opaque_key_t *esp_rsa_ds_opaque_key; /**< Pointer to the esp ds opaque key */
#if SOC_KEY_MANAGER_SUPPORTED
    bool is_km_key_active;                          /**< Flag indicating if the Key Manager key is active for this operation */
#endif /* SOC_KEY_MANAGER_SUPPORTED */
    psa_algorithm_t alg;                            /**< Algorithm used in the sign operation */
    uint32_t *sig_buffer;                           /**< Buffer to hold the signature */
    size_t sig_buffer_size;                         /**< Size of the signature buffer */
    esp_ds_context_t *esp_rsa_ds_ctx;               /**< Pointer to the esp ds context */
} esp_rsa_ds_opaque_sign_hash_operation_t;
#endif /* !(__DOXYGEN__) */

#ifdef __cplusplus
}
#endif

#endif /* ESP_RSA_DS_DRIVER_ENABLED */
