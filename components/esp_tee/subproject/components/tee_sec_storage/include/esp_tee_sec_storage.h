/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_bit_defs.h"

#define MAX_ECDSA_SUPPORTED_KEY_LEN         32   /*!< Maximum supported size for the ECDSA key */
#define MAX_AES_SUPPORTED_KEY_LEN           32   /*!< Maximum supported size for the AES key */

#define SEC_STORAGE_FLAG_NONE               0      /*!< No flags */
#define SEC_STORAGE_FLAG_WRITE_ONCE         BIT(0) /*!< Data can only be written once */

/**
 * @brief Enum to represent the type of key stored in the secure storage
 *
 */
typedef enum {
    ESP_SEC_STG_KEY_AES256 = 0,
    ESP_SEC_STG_KEY_ECDSA_SECP256R1 = 1,
    ESP_SEC_STG_KEY_ECDSA_SECP192R1 = 2,
    ESP_SEC_STG_TYPE_MAX,
} esp_tee_sec_storage_type_t;

/**
 * @brief Configuration structure for key generation/import
 *
 */
typedef struct {
    const char *id;                          /*!< Unique identifier for the key */
    esp_tee_sec_storage_type_t type;         /*!< Type of key (AES256, ECDSA_SECP256R1, etc.) */
    uint32_t flags;                          /*!< Key flags (e.g. WRITE_ONCE) */
} esp_tee_sec_storage_key_cfg_t;

/**
 * @brief Context structure for AES-GCM AEAD encryption/decryption operations
 *
 */
typedef struct {
    const char *key_id;                      /*!< Identifier of the key to use */
    const uint8_t *aad;                      /*!< Additional authenticated data */
    size_t aad_len;                          /*!< Length of additional authenticated data */
    const uint8_t *input;                    /*!< Input data buffer */
    size_t input_len;                        /*!< Length of input data */
} esp_tee_sec_storage_aead_ctx_t;

/**
 * @brief Context structure for ECDSA signing with PBKDF2 (HMAC) derived key
 *
 */
typedef struct {
    const uint8_t *salt;                    /*!< Salt for PBKDF2 */
    size_t salt_len;                        /*!< Length of the salt */
    esp_tee_sec_storage_type_t key_type;    /*!< Key type to be generated and used */
} esp_tee_sec_storage_pbkdf2_ctx_t;

/**
 * @brief Structure holding the X and Y components of the ECDSA public key
 *
 */
typedef struct {
    uint8_t pub_x[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< X component */
    uint8_t pub_y[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< Y component */
} __attribute__((__packed__)) esp_tee_sec_storage_ecdsa_pubkey_t;

/**
 * @brief Structure holding the R and S components of the ECDSA signature
 *
 */
typedef struct {
    uint8_t sign_r[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< R component */
    uint8_t sign_s[MAX_ECDSA_SUPPORTED_KEY_LEN];    /*!< S component */
} __attribute__((__packed__)) esp_tee_sec_storage_ecdsa_sign_t;

#if ESP_TEE_BUILD && !(__DOXYGEN__)
/**
 * @brief Initialize the secure storage
 *
 * @note Must be invoked as part of the TEE initialization sequence.
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_init(void);
#endif

/**
 * @brief Clear a key from secure storage
 *
 * @param key_id    Key identifier string
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_clear_key(const char *key_id);

/**
 * @brief Generate a unique key and store it in the secure storage
 *
 * @param cfg       Pointer to the key configuration
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_gen_key(const esp_tee_sec_storage_key_cfg_t *cfg);

/**
 * @brief Generate and return the signature for the specified message digest using
 *        the key pair located in the secure storage.
 *
 * @param[in]   cfg       Pointer to the key configuration
 * @param[in]   hash      Message digest
 * @param[in]   hlen      Digest length
 * @param[out]  out_sign  Output context holding the signature
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_ecdsa_sign(const esp_tee_sec_storage_key_cfg_t *cfg, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign);

/**
 * @brief  Return the public key from secure storage
 *
 * @param[in]   cfg        Pointer to the key configuration
 * @param[out]  out_pubkey Output context holding the public key
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_ecdsa_get_pubkey(const esp_tee_sec_storage_key_cfg_t *cfg, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey);

/**
 * @brief Perform encryption using AES256-GCM with the key from secure storage
 *
 * @param[in]  ctx      Pointer to the AEAD operation context
 * @param[out] tag      Pointer to the authentication tag buffer
 * @param[in]  tag_len  Length of the authentication tag
 * @param[out] output   Pointer to the output data buffer
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_aead_encrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, uint8_t *tag, size_t tag_len, uint8_t *output);

/**
 * @brief Perform decryption using AES256-GCM with the key from secure storage
 *
 * @param[in]  ctx      Pointer to the AEAD operation context
 * @param[in]  tag      Pointer to the authentication tag buffer
 * @param[in]  tag_len  Length of the authentication tag
 * @param[out] output   Pointer to the output data buffer
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_aead_decrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, const uint8_t *tag, size_t tag_len, uint8_t *output);

/**
 * @brief Generate and return the signature for the specified message digest using
 *        the key pair derived via PBKDF2-HMAC-SHA256 using the HMAC peripheral
 *        with the given salt and the configured HMAC eFuse key ID
 *        (`CONFIG_SECURE_TEE_PBKDF2_EFUSE_HMAC_KEY_ID`).
 *
 * @param[in]   ctx        Pointer to the PBKDF2 context
 * @param[in]   hash       Message digest
 * @param[in]   hlen       Digest length
 * @param[out]  out_sign   Output context holding the signature
 * @param[out]  out_pubkey Output context holding the public key
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise.
 */
esp_err_t esp_tee_sec_storage_ecdsa_sign_pbkdf2(const esp_tee_sec_storage_pbkdf2_ctx_t *ctx, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey);

#ifdef __cplusplus
}
#endif
