/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_AES_DRIVER_ENABLED)

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_cmac_contexts.h"


psa_status_t esp_cmac_mac_compute(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length);

psa_status_t esp_cmac_mac_abort(esp_cmac_operation_t *operation);

psa_status_t esp_cmac_mac_setup(esp_cmac_operation_t *operation,
                                  const psa_key_attributes_t *attributes,
                                  const uint8_t *key_buffer,
                                  size_t key_buffer_size,
                                  psa_algorithm_t alg);

psa_status_t esp_cmac_mac_update(esp_cmac_operation_t *cmac,
                                            const uint8_t *data,
                                            size_t data_length);

psa_status_t esp_cmac_mac_finish(
    esp_cmac_operation_t *hmac,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length);

psa_status_t esp_cmac_mac_verify_finish(
    esp_cmac_operation_t *operation,
    const uint8_t *mac,
    size_t mac_length);
#endif /* ESP_AES_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
