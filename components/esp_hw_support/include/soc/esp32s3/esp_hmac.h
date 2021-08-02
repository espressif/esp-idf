/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The possible efuse keys for the HMAC peripheral
 */
typedef enum {
    HMAC_KEY0 = 0,
    HMAC_KEY1,
    HMAC_KEY2,
    HMAC_KEY3,
    HMAC_KEY4,
    HMAC_KEY5,
    HMAC_KEY_MAX
} hmac_key_id_t;

/**
 * @brief
 * Calculate the HMAC of a given message.
 *
 * Calculate the HMAC \c hmac of a given message \c message with length \c message_len.
 * SHA256 is used for the calculation (fixed on ESP32S3).
 *
 * @note Uses the HMAC peripheral in "upstream" mode.
 *
 * @param key_id Determines which of the 6 key blocks in the efuses should be used for the HMAC calcuation.
 *        The corresponding purpose field of the key block in the efuse must be set to the HMAC upstream purpose value.
 * @param message the message for which to calculate the HMAC
 * @param message_len message length
 * @param [out] hmac the hmac result; the buffer behind the provided pointer must be 32 bytes long
 *
 * @return
 *      * ESP_OK, if the calculation was successful,
 *      * ESP_ERR_INVALID_ARG if message or hmac is a nullptr or if key_id out of range
 *      * ESP_FAIL, if the hmac calculation failed
 */
esp_err_t esp_hmac_calculate(hmac_key_id_t key_id,
        const void *message,
        size_t message_len,
        uint8_t *hmac);

#ifdef __cplusplus
}
#endif
