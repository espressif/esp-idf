/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "soc/soc_caps.h"

#if !SOC_HMAC_SUPPORTED && !CI_HEADER_CHECK
#error "HMAC peripheral is not supported for the selected target"
#endif

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
 * SHA256 is used for the calculation.
 *
 * @note Uses the HMAC peripheral in "upstream" mode.
 *
 * @param key_id Determines which of the 6 key blocks in the efuses should be used for the HMAC calcuation.
 *        The corresponding purpose field of the key block in the efuse must be set to the HMAC upstream purpose value.
 * @param message the message for which to calculate the HMAC
 * @param message_len message length
 *                         return ESP_ERR_INVALID_STATE if unsuccessful
 * @param [out] hmac the hmac result; the buffer behind the provided pointer must be a writeable buffer of 32 bytes
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
 *      * ESP_OK, if the key_purpose of the key_id matches to HMAC downstread mode,
 *                The API returns success even if calculated HMAC does not match with the provided token.
 *                However, The JTAG will be re-enabled only if the calculated HMAC value matches with provided token,
 *                otherwise JTAG will remain disabled.
 *      * ESP_FAIL, if the key_purpose of the key_id is not set to HMAC downstream purpose
 *                  or JTAG is permanently disabled by EFUSE_HARD_DIS_JTAG eFuse parameter.
 *      * ESP_ERR_INVALID_ARG, invalid input arguments
 *
 * @note  Return value of the API does not indicate the JTAG status.
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
