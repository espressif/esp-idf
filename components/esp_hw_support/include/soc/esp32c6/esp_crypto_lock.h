/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Acquire lock for HMAC cryptography peripheral
 *
 * Internally also locks the SHA peripheral, as the HMAC depends on the SHA peripheral
 */
void esp_crypto_hmac_lock_acquire(void);

/**
 * @brief Release lock for HMAC cryptography peripheral
 *
 * Internally also releases the SHA peripheral, as the HMAC depends on the SHA peripheral
 */
void esp_crypto_hmac_lock_release(void);

/**
 * @brief Acquire lock for DS cryptography peripheral
 *
 * Internally also locks the HMAC (which locks SHA), AES and MPI  peripheral, as the DS depends on these peripherals
 */
void esp_crypto_ds_lock_acquire(void);

/**
 * @brief Release lock for DS cryptography peripheral
 *
 * Internally also releases the HMAC (which locks SHA), AES and MPI  peripheral, as the DS depends on these peripherals
 */
void esp_crypto_ds_lock_release(void);

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
 * @brief Acquire lock for the mpi cryptography peripheral.
 *
 */
void esp_crypto_mpi_lock_acquire(void);

/**
 * @brief Release lock for the mpi/rsa cryptography peripheral.
 *
 */
void esp_crypto_mpi_lock_release(void);

/**
 * @brief Acquire lock for the ECC cryptography peripheral.
 *
 */
void esp_crypto_ecc_lock_acquire(void);

/**
 * @brief Release lock for the ECC cryptography peripheral.
 *
 */
void esp_crypto_ecc_lock_release(void);

#ifdef __cplusplus
}
#endif
