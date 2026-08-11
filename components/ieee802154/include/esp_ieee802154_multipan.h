/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_ieee802154_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Get the device PAN ID for specific interface.
 *
 * @param[in]  index  The interface index.
 *
 * @return  The device PAN ID.
 *
 */
uint16_t esp_ieee802154_get_multipan_panid(esp_ieee802154_multipan_index_t index);

/**
 * @brief  Set the device PAN ID for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[in]  panid  The device PAN ID.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_panid(esp_ieee802154_multipan_index_t index, uint16_t panid);

/**
 * @brief  Get the device short address for specific interface.
 *
 * @param[in]  index  The interface index.
 *
 * @return  The device short address.
 *
 */
uint16_t esp_ieee802154_get_multipan_short_address(esp_ieee802154_multipan_index_t index);

/**
 * @brief  Set the device short address for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[in]  short_address  The device short address.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_short_address(esp_ieee802154_multipan_index_t index, uint16_t short_address);

/**
 * @brief  Get the device extended address for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[out]  ext_addr  The pointer to the device extended address. Must not be NULL.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_ERR_INVALID_ARG if ext_addr is NULL.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_get_multipan_extended_address(esp_ieee802154_multipan_index_t index, uint8_t *ext_addr);

/**
 * @brief  Set the device extended address for specific interface.
 *
 * @param[in]  index  The interface index.
 * @param[in]  ext_addr  The pointer to the device extended address. Must not be NULL.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_ERR_INVALID_ARG if addr is NULL.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_extended_address(esp_ieee802154_multipan_index_t index, const uint8_t *ext_addr);

/**
 * @brief  Get the device current multipan interface enable mask.
 *
 * @return  Current multipan interface enable mask.
 *
 */
uint8_t esp_ieee802154_get_multipan_enable(void);

/**
 * @brief Enable specific interface for the device.
 *
 * As an example, call `esp_ieee802154_set_multipan_enable(BIT(ESP_IEEE802154_MULTIPAN_0) | BIT(ESP_IEEE802154_MULTIPAN_1));`
 * to enable multipan interface 0 and 1.
 *
 * @param[in]  mask  The multipan interface bit mask.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_set_multipan_enable(uint8_t mask);

/**
 * @brief  Add a pending address to the multipan table for the specified interface.
 *
 * @note   This API should be called only when the IEEE 802.15.4 subsystem is enabled.
 *
 * @param  inf_index  Index of the interface.
 * @param  addr       Pointer to the address to add.
 * @param  is_short   True if the address is a short address, false if extended.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_multipan_add_pending_addr(esp_ieee802154_multipan_index_t inf_index, const uint8_t *addr, bool is_short);

/**
 * @brief  Remove a pending address from the multipan table for the specified interface.
 *
 * @note   This API should be called only when the IEEE 802.15.4 subsystem is enabled.
 *
 * @param  inf_index  Index of the interface.
 * @param  addr       Pointer to the address to remove.
 * @param  is_short   True if the address is a short address, false if extended.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_multipan_clear_pending_addr(esp_ieee802154_multipan_index_t inf_index, const uint8_t *addr, bool is_short);

/**
 * @brief  Reset the pending address table for the specified interface.
 *
 * @note   This API clears all pending addresses of the specified type (short or extended) for the interface.
 *
 * @param  inf_index  Index of the interface.
 * @param  is_short   True to reset short addresses, false to reset extended addresses.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_multipan_reset_pending_table(esp_ieee802154_multipan_index_t inf_index, bool is_short);

/**
 * @brief  Get the auto frame pending mode of the specified multipan interface.
 *
 * @note   This API returns the logical pending mode configured for a given multipan
 *         interface. The value is used by the ACK generation logic together with the
 *         per-interface pending address table.
 *
 * @param  inf_index  Index of the multipan interface.
 *
 * @return
 *      - Current pending mode of type refer to esp_ieee802154_pending_mode_t
 */
esp_ieee802154_pending_mode_t esp_ieee802154_multipan_get_pending_mode(esp_ieee802154_multipan_index_t inf_index);

/**
 * @brief  Set the auto frame pending mode of the specified multipan interface.
 *
 * @note   This API configures the logical pending mode for a given multipan interface.
 *         The configured mode is used by the ACK generation logic together with the
 *         per-interface pending address table.
 *
 * @param  inf_index      Index of the multipan interface.
 * @param  pending_mode   Pending mode to set for this interface.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_multipan_set_pending_mode(esp_ieee802154_multipan_index_t inf_index, esp_ieee802154_pending_mode_t pending_mode);

/**
 * @brief  Set the IEEE 802.15.4 Radio to sleep state for the specified multipan interface.
 *
 * @param[in]  idx  Index of the multipan interface.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_multipan_sleep(int8_t idx);

/**
 * @brief  Set the IEEE 802.15.4 Radio to receive state for the specified multipan interface.
 *
 * @param[in]  idx  Index of the multipan interface.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_FAIL on failure.
 */
esp_err_t esp_ieee802154_multipan_receive(int8_t idx);

/**
 * @brief  Set the RxOnWhenIdle mode for the specified multipan interface.
 *
 * Each interface owns one bit in an internal mask. The hardware RxOnWhenIdle
 * setting is enabled when any interface requests it, and is disabled only
 * when all interface bits are cleared.
 *
 * @param[in]  idx     Index of the multipan interface.
 * @param[in]  enable  True to enable RxOnWhenIdle for this interface.
 *
 * @return
 *      - ESP_OK on success.
 */
esp_err_t esp_ieee802154_multipan_set_rx_when_idle(int8_t idx, bool enable);

#ifdef __cplusplus
}
#endif
