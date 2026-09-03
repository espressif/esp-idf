/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "psa/crypto.h"
#include "soc/soc_caps.h"
#include "hal/ecdsa_types.h"
#if SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_hal.h"
#endif /* SOC_ECDSA_SUPPORTED */
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
 * @brief Digest length that the peripheral expects for a curve.
 *
 * esp_ecdsa_validate_sha_alg pins the hash algorithm to the curve, so the
 * expected digest length follows from the curve alone.
 */
static inline size_t esp_ecdsa_expected_hash_len(esp_ecdsa_curve_t curve)
{
#if SOC_ECDSA_SUPPORT_CURVE_P384
    if (curve == ESP_ECDSA_CURVE_SECP384R1) {
        return ECDSA_SHA_LEN_P384;
    }
#else
    (void)curve;
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
    return ECDSA_SHA_LEN;
}

/**
 * @brief Acquire the ECDSA hardware (locks and peripheral clocks)
 */
void esp_ecdsa_acquire_hardware(void);

/**
 * @brief Release the ECDSA hardware
 */
void esp_ecdsa_release_hardware(void);

#if defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED)
/**
 * @brief Generate a signature on the ECDSA peripheral
 *
 * Shared by the opaque and the transparent sign drivers. The caller sets the curve,
 * the nonce type and the key source in conf. This function takes the hardware,
 * retries while the peripheral fails, returns a zero r or s, or (with the software
 * deterministic loop) fails the k check, releases the hardware and writes r || s
 * in big-endian format to signature.
 *
 * @param conf          Peripheral configuration (mode, curve, key source, nonce type)
 * @param sha           Hash in little-endian format, len bytes
 * @param r             Scratch buffer for r in little-endian format, len bytes
 * @param s             Scratch buffer for s in little-endian format, len bytes
 * @param len           Curve component length in bytes
 * @param max_attempts  Upper bound on the attempts, 0 for no bound
 * @param signature     Output buffer, at least 2 * len bytes
 *
 * @return PSA_SUCCESS, PSA_ERROR_INVALID_ARGUMENT if len is 0 or larger than
 *         MAX_ECDSA_COMPONENT_LEN, or PSA_ERROR_GENERIC_ERROR if all attempts failed
 */
psa_status_t esp_ecdsa_hw_sign(ecdsa_hal_config_t *conf, const uint8_t *sha, uint8_t *r, uint8_t *s,
                               uint16_t len, unsigned int max_attempts, uint8_t *signature);

/**
 * @brief Check a sign request against the curve
 *
 * Shared by the opaque and the transparent sign drivers. Checks that the hash
 * algorithm matches the curve, that the eFuse allows the curve and that the
 * digest length matches the curve.
 *
 * @return PSA_SUCCESS, PSA_ERROR_INVALID_ARGUMENT for a wrong digest length,
 *         or PSA_ERROR_NOT_SUPPORTED for the other failures
 */
psa_status_t esp_ecdsa_check_sign_request(psa_algorithm_t alg, esp_ecdsa_curve_t curve, size_t hash_length);
#endif /* ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
