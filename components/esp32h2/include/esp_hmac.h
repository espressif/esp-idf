// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_HMAC_H_
#define _ESP_HMAC_H_

#include <stdbool.h>
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
 * SHA256 is used for the calculation (fixed on ESP32S2).
 *
 * @note Uses the HMAC peripheral in "upstream" mode.
 *
 * @param key_id Determines which of the 6 key blocks in the efuses should be used for the HMAC calcuation.
 *        The corresponding purpose field of the key block in the efuse must be set to the HMAC upstream purpose value.
 * @param message the message for which to calculate the HMAC
 * @param message_len message length
 *             return ESP_ERR_INVALID_STATE if unsuccessful
 * @param [out] hmac the hmac result; the buffer behind the provided pointer must be 32 bytes long
 *
 * @return
 *      * ESP_OK, if the calculation was successful,
 *      * ESP_FAIL, if the hmac calculation failed
 */
esp_err_t esp_hmac_calculate(hmac_key_id_t key_id,
        const void *message,
        size_t message_len,
        uint8_t *hmac);

#ifdef __cplusplus
}
#endif

#endif // _ESP_HMAC_H_
