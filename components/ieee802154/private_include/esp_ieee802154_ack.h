/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_ieee802154_frame.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The radio pending table, which is utilized to determine whether the received frame should be responded to with pending bit enabled.
 */

#define IEEE802154_PENDING_TABLE_MASK_BITS (8)
#define IEEE802154_PENDING_TABLE_MASK_SIZE (((CONFIG_IEEE802154_PENDING_TABLE_SIZE - 1) / IEEE802154_PENDING_TABLE_MASK_BITS) + 1)
typedef struct {
    uint8_t short_addr[CONFIG_IEEE802154_PENDING_TABLE_SIZE][IEEE802154_FRAME_SHORT_ADDR_SIZE]; /*!< Short address table */
    uint8_t ext_addr[CONFIG_IEEE802154_PENDING_TABLE_SIZE][IEEE802154_FRAME_EXT_ADDR_SIZE];     /*!< Extend address table */
    uint8_t short_addr_mask[IEEE802154_PENDING_TABLE_MASK_SIZE];                                /*!< The mask which the index of short address table is used */
    uint8_t ext_addr_mask[IEEE802154_PENDING_TABLE_MASK_SIZE];                                  /*!< The mask which the index of extended address table is used */
} ieee802154_pending_table_t;

/**
 * @brief  Add an address to the pending table.
 *
 * @param[in]  addr  The pointer to the address needs to be added.
 * @param[in]  is_short  The type of address, true for short address, false for extended.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure due to the table is full.
 *
 */
esp_err_t ieee802154_add_pending_addr(const uint8_t *addr, bool is_short);

/**
 * @brief  Remove an address in pending table.
 *
 * @param[in]  addr  The pointer to the address needs to be cleared.
 * @param[in]  is_short  The type of address, true for short address, false for extended.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure if the given address is not present in the pending table.
 *
 */
esp_err_t ieee802154_clear_pending_addr(const uint8_t *addr, bool is_short);

/**
 * @brief  Reset the pending table, only clear the mask bits for finishing the process quickly.
 *
 * @param[in]  is_short  The type of address, true for resetting short address table, false for extended.
 *
 */
void ieee802154_reset_pending_table(bool is_short);

/**
 * @brief  Check whether the pending bit should be set or not in the ack frame.
 *
 * @param[in]  frame  The pointer to the received frame.
 *
 * @return
 *    - True The pending bit should be set, otherwise False.
 *
 */
bool ieee802154_ack_config_pending_bit(const uint8_t *frame);

#ifdef __cplusplus
}
#endif
