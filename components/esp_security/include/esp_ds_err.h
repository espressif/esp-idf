/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL           (ESP_ERR_HW_CRYPTO_BASE + 0x1) /*!< HMAC peripheral problem */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_KEY         (ESP_ERR_HW_CRYPTO_BASE + 0x2) /*!< given HMAC key isn't correct,
                                                                                HMAC peripheral problem */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST      (ESP_ERR_HW_CRYPTO_BASE + 0x4) /*!< message digest check failed,
                                                                                result is invalid */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING     (ESP_ERR_HW_CRYPTO_BASE + 0x5) /*!< padding check failed, but result
                                                                                   is produced anyway and can be read*/

#ifdef __cplusplus
}
#endif
