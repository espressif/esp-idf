/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BLE_EAD_H__
#define __ESP_BLE_EAD_H__

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BLE Encrypted Advertising Data (EAD)
 *
 * Based on Bluetooth Core Specification Version 5.4 and Core Specification
 * Supplement v11, Part A 1.23.
 *
 * Enable CONFIG_BT_BLE_FEAT_ENC_ADV_DATA to compile the encrypt/decrypt APIs.
 * A GATT server that publishes the session key should also enable
 * CONFIG_BT_GATTS_KEY_MATERIAL_CHAR and call esp_ble_gap_set_key_material().
 */

#define ESP_BLE_EAD_KEY_SIZE           16   /*!< 128-bit session key */
#define ESP_BLE_EAD_IV_SIZE            8    /*!< 64-bit Initialization Vector */
#define ESP_BLE_EAD_RANDOMIZER_SIZE    5    /*!< 40-bit Randomizer */
#define ESP_BLE_EAD_MIC_SIZE           4    /*!< 32-bit Message Integrity Check */
#define ESP_BLE_EAD_NONCE_SIZE         13   /*!< 104-bit Nonce (Randomizer + IV) */
#define ESP_BLE_EAD_AAD_SIZE           1    /*!< Additional Authenticated Data size */

/**
 * Direction bit position in Randomizer (MSB of last byte).
 * Per Bluetooth Core Spec Supplement v11, Part A 1.23.3.
 */
#define ESP_BLE_EAD_RANDOMIZER_DIRECTION_BIT  7

/**
 * @brief Calculate encrypted payload size from plaintext size
 *
 * Encrypted payload layout: Randomizer || Ciphertext || MIC
 */
#define ESP_BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(payload_size) \
    (ESP_BLE_EAD_RANDOMIZER_SIZE + (payload_size) + ESP_BLE_EAD_MIC_SIZE)

/**
 * @brief Calculate decrypted payload size from encrypted payload size
 */
#define ESP_BLE_EAD_DECRYPTED_PAYLOAD_SIZE(encrypted_size) \
    ((encrypted_size) - ESP_BLE_EAD_RANDOMIZER_SIZE - ESP_BLE_EAD_MIC_SIZE)

/**
 * @brief Key material structure for EAD
 */
typedef struct {
    uint8_t session_key[ESP_BLE_EAD_KEY_SIZE];  /*!< 128-bit session key */
    uint8_t iv[ESP_BLE_EAD_IV_SIZE];            /*!< 64-bit Initialization Vector */
} esp_ble_ead_key_material_t;

/**
 * @brief Encrypt advertising data using AES-CCM
 *
 * The resulting data in @p encrypted_payload has the following layout:
 * - Randomizer in the first ESP_BLE_EAD_RANDOMIZER_SIZE bytes
 * - Encrypted payload of @p payload_size bytes
 * - MIC in the last ESP_BLE_EAD_MIC_SIZE bytes
 *
 * The function must be called each time the RPA is updated or the advertising
 * data are modified. @p payload may contain one or more concatenated advertising
 * structures (length + type + data).
 *
 * @param[in]  session_key        16-byte session key
 * @param[in]  iv                 8-byte Initialization Vector. Must be changed
 *                                each time the session key changes
 * @param[in]  payload            Plaintext advertising data to encrypt
 * @param[in]  payload_size       Size of plaintext data. Must be greater than 0
 * @param[out] encrypted_payload  Output buffer for encrypted data.
 *                                Size must be at least ESP_BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(payload_size)
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: encryption failed
 */
esp_err_t esp_ble_ead_encrypt(const uint8_t session_key[ESP_BLE_EAD_KEY_SIZE],
                              const uint8_t iv[ESP_BLE_EAD_IV_SIZE],
                              const uint8_t *payload, size_t payload_size,
                              uint8_t *encrypted_payload);

/**
 * @brief Decrypt advertising data using AES-CCM
 *
 * @param[in]  session_key            16-byte session key
 * @param[in]  iv                     8-byte Initialization Vector
 * @param[in]  encrypted_payload      Encrypted advertising data (includes randomizer and MIC).
 *                                    This should only contain the advertising data from the
 *                                    received advertising structure, neither the length nor the type
 * @param[in]  encrypted_payload_size Size of encrypted data
 * @param[out] payload                Output buffer for decrypted data.
 *                                    Use ESP_BLE_EAD_DECRYPTED_PAYLOAD_SIZE to get the right size
 * @param[in]  payload_capacity       Size of @p payload in bytes; must be >=
 *                                    ESP_BLE_EAD_DECRYPTED_PAYLOAD_SIZE(encrypted_payload_size)
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: decryption or authentication failed
 */
esp_err_t esp_ble_ead_decrypt(const uint8_t session_key[ESP_BLE_EAD_KEY_SIZE],
                              const uint8_t iv[ESP_BLE_EAD_IV_SIZE],
                              const uint8_t *encrypted_payload, size_t encrypted_payload_size,
                              uint8_t *payload, size_t payload_capacity);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BLE_EAD_H__ */
