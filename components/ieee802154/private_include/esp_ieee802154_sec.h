/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

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
 */
void ieee802154_transmit_security_config(uint8_t *frame, uint8_t *key, uint8_t *addr);

/**
 * @brief  Update the encryption enabled configuration of the next transmission.
 *
 *
 */
void ieee802154_sec_update(void);

#ifdef __cplusplus
}
#endif
