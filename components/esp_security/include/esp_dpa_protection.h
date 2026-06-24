/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_CRYPTO_DPA_SEC_LEVEL_OFF = 0,      /*!< DPA protection disabled */
    ESP_CRYPTO_DPA_SEC_LEVEL_LOW,          /*!< DPA protection level low */
    ESP_CRYPTO_DPA_SEC_LEVEL_MIDDLE,       /*!< DPA protection level medium */
    ESP_CRYPTO_DPA_SEC_LEVEL_HIGH,         /*!< DPA protection level high */
} esp_crypto_dpa_sec_level_t;

/**
 * @brief Enable DPA (Differential Power Analysis) related protection
 *
 * @note
 * Enabling the DPA protection can help to make it difficult to perform SCA
 * attacks on the crypto peripherals. However, based on the security level
 * set there will be a performance impact, higher the level higher the impact.
 * Please refer to the TRM for more details.
 *
 * @param level DPA Security Level of type `esp_crypto_dpa_sec_level_t`
 */
void esp_crypto_dpa_protection_enable(esp_crypto_dpa_sec_level_t level);

/**
 * @brief Disable DPA (Differential Power Analysis) related protection
 */
void esp_crypto_dpa_protection_disable(void);

#ifdef __cplusplus
}
#endif
