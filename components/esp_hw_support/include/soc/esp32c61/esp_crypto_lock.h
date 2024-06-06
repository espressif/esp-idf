/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void esp_crypto_ecc_lock_acquire(void);

/**
 * @brief Release lock for the ECC cryptography peripheral.
 *
 */
void esp_crypto_ecc_lock_release(void);

/**
 * @brief Acquire lock for ECDSA cryptography peripheral
 *
 * Internally also locks the ECC and MPI peripheral, as the ECDSA depends on these peripherals
 */
void esp_crypto_ecdsa_lock_acquire(void);

/**
 * @brief Release lock for ECDSA cryptography peripheral
 *
 * Internally also releases the ECC and MPI peripheral, as the ECDSA depends on these peripherals
 */
void esp_crypto_ecdsa_lock_release(void);

#ifdef __cplusplus
}
#endif
