/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#define MIN_SEC_STG_SLOT_ID                 0    /*!< Minimum secure storage slot ID */
#define MAX_SEC_STG_SLOT_ID                 14   /*!< Maximum secure storage slot ID */

#define MAX_ECDSA_SUPPORTED_KEY_LEN         32   /*!< Maximum supported size for the ECDSA key */
#define MAX_AES_SUPPORTED_KEY_LEN           32   /*!< Maximum supported size for the AES key */

/**
 * @brief Enum to represent the type of key stored in the secure storage
 *
 */
typedef enum {
    ESP_SEC_STG_KEY_ECDSA_SECP256R1 = 0,
    ESP_SEC_STG_KEY_AES256 = 1,
    ESP_SEC_STG_KEY_ECDSA_SECP192R1 = 2,
    ESP_SEC_STG_MAX,
} esp_tee_sec_storage_type_t;

/**
 * @brief Structure holding the X and Y components
 *        of the ECDSA public key
 *
 */
typedef struct {
    uint8_t pub_x[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< X component */
    uint8_t pub_y[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< Y component */
} __attribute__((__packed__)) esp_tee_sec_storage_pubkey_t;

/**
 * @brief Structure holding the R and S components
 *        of the ECDSA signature
 *
 */
typedef struct {
    uint8_t sign_r[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< R component */
    uint8_t sign_s[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< S component */
} __attribute__((__packed__)) esp_tee_sec_storage_sign_t;

/**
 * @brief Initialize the TEE secure storage partition
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_init(void);

/**
 * @brief Generate a unique key and store it in the given secure storage slot
 *
 * @param slot_id secure storage slot ID
 * @param key_type secure storage key type to generate
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_gen_key(uint16_t slot_id, esp_tee_sec_storage_type_t key_type);

/**
 * @brief Generate and return the signature for the specified message digest using
 *        the key pair located in the given secure storage slot.
 *
 * @param[in]   slot_id   secure storage slot ID
 * @param[in]   key_type  secure storage key type
 * @param[in]   hash      Message digest
 * @param[in]   hlen      Digest length
 * @param[out]  out_sign  Output context holding the signature
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_get_signature(uint16_t slot_id, esp_tee_sec_storage_type_t key_type, uint8_t *hash, size_t hlen, esp_tee_sec_storage_sign_t *out_sign);

/**
 * @brief  Return the public key for the given secure storage slot
 *
 * @param[in]   slot_id  secure storage slot ID
 * @param[in]   key_type secure storage key type
 * @param[out]  pubkey   Output context holding the public key
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_get_pubkey(uint16_t slot_id, esp_tee_sec_storage_type_t key_type, esp_tee_sec_storage_pubkey_t *pubkey);

/**
 * @brief Check whether the given slot in the secure storage is empty or not
 *
 * @param slot_id  secure storage slot ID
 *
 * @return bool   true: slot is empty; false otherwise.
 */
bool esp_tee_sec_storage_is_slot_empty(uint16_t slot_id);

/**
 * @brief Erase the given secure storage slot
 *
 * @param slot_id secure storage slot ID
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_clear_slot(uint16_t slot_id);

/**
 * @brief Perform encryption using AES256-GCM with the key stored in the specified slot
 *
 * @param[in] slot_id    Secure storage slot ID containing the AES-GCM key
 * @param[in] input      Pointer to the input data buffer
 * @param[in] len        Length of the input data
 * @param[in] aad        Pointer to the Additional Authenticated Data (AAD)
 * @param[in] aad_len    Length of the AAD
 * @param[out] tag       Pointer to the authentication tag buffer
 * @param[out] tag_len   Length of the authentication tag
 * @param[out] output    Pointer to the output data buffer
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_encrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                      uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output);

/**
 * @brief Perform decryption using AES256-GCM with the key stored in the specified slot
 *
 * @param[in] slot_id   Secure storage slot ID containing the AES-GCM key
 * @param[in] input     Pointer to the input data buffer
 * @param[in] len       Length of the input data
 * @param[in] aad       Pointer to the Additional Authenticated Data (AAD)
 * @param[in] aad_len   Length of the AAD
 * @param[in] tag       Pointer to the authentication tag buffer
 * @param[in] tag_len   Length of the authentication tag
 * @param[out] output   Pointer to the output data buffer
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_decrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                      uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output);

#ifdef __cplusplus
}
#endif
