/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This API should be used by all components which use the SHA, AES, HMAC and DS crypto hardware on the ESP32S3.
 * Not all of them can be used in parallel because they use the same underlying module.
 * E.g., HMAC uses SHA or DS uses HMAC and AES. See the ESP32S3 Technical Reference Manual for more details.
 *
 * Other unrelated components must not use it.
 */

/**
 * @brief Acquire lock for Digital Signature(DS) cryptography peripheral
 *
 * Internally also takes the HMAC lock, as the DS depends on the HMAC peripheral
 */
void esp_crypto_ds_lock_acquire(void);

/**
 * @brief Release lock for Digital Signature(DS) cryptography peripheral
 *
 * Internally also releases the HMAC lock, as the DS depends on the HMAC peripheral
 */
void esp_crypto_ds_lock_release(void);

/**
 * @brief Acquire lock for HMAC cryptography peripheral
 *
 * Internally also takes the SHA & AES lock, as the HMAC depends on the SHA peripheral
 */
void esp_crypto_hmac_lock_acquire(void);

/**
 * @brief Release lock for HMAC cryptography peripheral
 *
 * Internally also releases the SHA & AES lock, as the HMAC depends on the SHA peripheral
 */
void esp_crypto_hmac_lock_release(void);

/**
 * @brief Acquire lock for the SHA and AES cryptography peripheral.
 *
 */
void esp_crypto_sha_aes_lock_acquire(void);

/**
 * @brief Release lock for the SHA and AES cryptography peripheral.
 *
 */
void esp_crypto_sha_aes_lock_release(void);

/**
 * Acquire lock for the MPI/RSA cryptography peripheral
 */
void esp_crypto_mpi_lock_acquire(void);

/**
 * Release lock for the MPI/RSA cryptography peripheral
 */
void esp_crypto_mpi_lock_release(void);

#ifdef __cplusplus
}
#endif
