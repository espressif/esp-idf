/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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
