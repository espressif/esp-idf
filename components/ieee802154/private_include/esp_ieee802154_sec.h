/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#define IEEE802154_SECURITY_ADDR_SIZE 8
#define IEEE802154_SECURITY_KEY_SIZE  16
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Configure the encryption of transmission.
 *
 * @param[in]  frame  The frame needs to be encrypted.
 * @param[in]  key  The key using in the encryption process.
 * @param[in]  addr  The address using in the encryption process.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_ERR_INVALID_ARG if the security payload offset cannot be parsed.
 *
 */
esp_err_t ieee802154_transmit_security_config(uint8_t *frame, uint8_t *key, uint8_t *addr);

/**
 * @brief  Clear the encryption enabled configuration.
 *
 */
void ieee802154_sec_clear(void);

#ifdef __cplusplus
}
#endif
