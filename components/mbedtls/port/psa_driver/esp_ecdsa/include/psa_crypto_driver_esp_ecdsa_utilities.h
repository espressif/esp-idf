/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "psa/crypto.h"
#include "hal/ecdsa_types.h"
#include "psa_crypto_driver_esp_ecdsa_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reverse the byte order of a buffer (the ECDSA peripheral uses little endian)
 */
void esp_ecdsa_change_endianness(const uint8_t *old_point, uint8_t *new_point, uint8_t len);

/**
 * @brief Map a key length in bytes to the driver's curve enum
 */
esp_ecdsa_curve_t esp_ecdsa_bits_to_curve(size_t key_len);

/**
 * @brief Check that the hash algorithm matches the curve
 */
psa_status_t esp_ecdsa_validate_sha_alg(psa_algorithm_t alg, const esp_ecdsa_curve_t curve);

/**
 * @brief Map the driver's curve enum to the HAL's curve enum
 */
ecdsa_curve_t esp_ecdsa_curve_to_hal_curve(esp_ecdsa_curve_t curve);

/**
 * @brief Acquire the ECDSA hardware (locks and peripheral clocks)
 */
void esp_ecdsa_acquire_hardware(void);

/**
 * @brief Release the ECDSA hardware
 */
void esp_ecdsa_release_hardware(void);

#ifdef __cplusplus
}
#endif
