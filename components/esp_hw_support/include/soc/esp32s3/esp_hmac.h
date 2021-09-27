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

/**
 * @brief Use HMAC peripheral in Downstream mode to re-enable the JTAG, if it is not permanently disabled by HW.
 *        In downstream mode, HMAC calculations performed by peripheral are used internally and not provided back to user.
 *
 * @param key_id Determines which of the 6 key blocks in the efuses should be used for the HMAC calculation.
 *        The corresponding purpose field of the key block in the efuse must be set to HMAC downstream purpose.
 *
 * @param token Pre calculated HMAC value of the 32-byte 0x00 using SHA-256 and the known private HMAC key. The key is already
 *        programmed to a eFuse key block. The key block number is provided as the first parameter to this function.
 *
 * @return
 *      * ESP_OK, if the calculation was successful,
 *                if the calculated HMAC value matches with provided token,
 *                JTAG will be re-enable otherwise JTAG will remain disabled.
 *                Return value does not indicate the JTAG status.
 *      * ESP_FAIL, if the hmac calculation failed or JTAG is permanently disabled by EFUSE_HARD_DIS_JTAG eFuse parameter.
 *      * ESP_ERR_INVALID_ARG, invalid input arguments
 */
esp_err_t esp_hmac_jtag_enable(hmac_key_id_t key_id, const uint8_t *token);

/**
 *  @brief Disable the JTAG which might be enabled using the HMAC downstream mode. This function just clears the result generated
 *         by calling esp_hmac_jtag_enable() API.
 *
 *  @return
 *       * ESP_OK return ESP_OK after writing the HMAC_SET_INVALIDATE_JTAG_REG with value 1.
 */
esp_err_t esp_hmac_jtag_disable(void);

#ifdef __cplusplus
}
#endif
