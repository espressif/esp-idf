/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLE_EAD_H
#define BLE_EAD_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BLE Encrypted Advertising Data (EAD) definitions
 * Based on Bluetooth Core Specification Version 5.4
 */

#define BLE_EAD_KEY_SIZE           16   /* 128-bit session key */
#define BLE_EAD_IV_SIZE            8    /* 64-bit Initialization Vector */
#define BLE_EAD_RANDOMIZER_SIZE    5    /* 40-bit Randomizer */
#define BLE_EAD_MIC_SIZE           4    /* 32-bit Message Integrity Check */
#define BLE_EAD_NONCE_SIZE         13   /* 104-bit Nonce (Randomizer + IV) */
#define BLE_EAD_AAD_SIZE           1    /* Additional Authenticated Data size */

/* Direction bit position in Randomizer (MSB of last byte)
 * Per Bluetooth Core Spec Supplement v11, Part A 1.23.3
 */
#define BLE_EAD_RANDOMIZER_DIRECTION_BIT  7

/* AD Type for Encrypted Advertising Data (0x31) */
#define ESP_BLE_AD_TYPE_ENC_ADV_DATA  0x31

/**
 * @brief Calculate encrypted payload size from plaintext size
 */
#define BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(payload_size) \
    (BLE_EAD_RANDOMIZER_SIZE + (payload_size) + BLE_EAD_MIC_SIZE)

/**
 * @brief Calculate decrypted payload size from encrypted payload size
 */
#define BLE_EAD_DECRYPTED_PAYLOAD_SIZE(encrypted_size) \
    ((encrypted_size) - BLE_EAD_RANDOMIZER_SIZE - BLE_EAD_MIC_SIZE)

/**
 * @brief Key material structure for EAD
 */
typedef struct {
    uint8_t session_key[BLE_EAD_KEY_SIZE];  /* 128-bit session key */
    uint8_t iv[BLE_EAD_IV_SIZE];            /* 64-bit Initialization Vector */
} ble_ead_key_material_t;

/**
 * @brief Encrypt advertising data using AES-CCM
 *
 * @param session_key   16-byte session key
 * @param iv            8-byte Initialization Vector
 * @param payload       Plaintext advertising data to encrypt
 * @param payload_size  Size of plaintext data
 * @param encrypted_payload Output buffer for encrypted data
 *                          Size must be at least BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(payload_size)
 *
 * @return 0 on success, negative error code on failure
 */
int ble_ead_encrypt(const uint8_t session_key[BLE_EAD_KEY_SIZE],
                    const uint8_t iv[BLE_EAD_IV_SIZE],
                    const uint8_t *payload, size_t payload_size,
                    uint8_t *encrypted_payload);

/**
 * @brief Decrypt advertising data using AES-CCM
 *
 * @param session_key           16-byte session key
 * @param iv                    8-byte Initialization Vector
 * @param encrypted_payload     Encrypted advertising data (includes randomizer and MIC)
 * @param encrypted_payload_size Size of encrypted data
 * @param payload               Output buffer for decrypted data
 *                              Size must be at least BLE_EAD_DECRYPTED_PAYLOAD_SIZE(encrypted_payload_size)
 *
 * @return 0 on success, negative error code on failure
 */
int ble_ead_decrypt(const uint8_t session_key[BLE_EAD_KEY_SIZE],
                    const uint8_t iv[BLE_EAD_IV_SIZE],
                    const uint8_t *encrypted_payload, size_t encrypted_payload_size,
                    uint8_t *payload);

#ifdef __cplusplus
}
#endif

#endif /* BLE_EAD_H */
