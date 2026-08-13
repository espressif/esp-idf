/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_eth_driver.h"
#include "esp_eth_sublayer.h"
#include "esp_eth_sublayer_vlan.h"
#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
#include "esp_eth_sublayer_switch.h"
#endif

/**
 * @brief Find a VLAN entry by VID in the sublayer's child list.
 *
 * @param sub The sublayer handle.
 * @param vlan_id The VLAN ID to find.
 * @return The VLAN child if found, NULL otherwise.
 */
esp_eth_sublayer_vlan_t *eth_sublayer_find_vlan_by_vid(esp_eth_sublayer_handle_t sub, uint16_t vlan_id);

/**
 * @brief Insert a VLAN child into the sublayer's child list.
 */
esp_err_t eth_sublayer_insert_vlan(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_vlan_t *vlan);

/**
 * @brief Remove a VLAN child from the sublayer's child list.
 *
 * @param sub The sublayer handle.
 * @param vlan The VLAN child to remove.
 * @return ESP_OK if found and removed, ESP_ERR_NOT_FOUND otherwise.
 */
esp_err_t eth_sublayer_remove_vlan(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_vlan_t *vlan);

/**
 * @brief Get the MAC address from the underlying Ethernet driver.
 *
 * @param sublayer The sublayer handle.
 * @param mac_addr The MAC address to get.
 * @return ESP_OK if the MAC address is got, ESP_ERR_INVALID_ARG if the sublayer is NULL, ESP_ERR_INVALID_ARG if the MAC address is NULL.
 */
 esp_err_t eth_sublayer_get_mac_addr(esp_eth_sublayer_handle_t sublayer, uint8_t *mac_addr);

/**
 * @brief Set the MAC filter through the sublayer.
 *
 * @param sub The sublayer handle.
 * @param eth_mac The MAC address.
 * @param mac_len The length of the MAC address (must be 6).
 * @param add Whether to add or remove the MAC address.
 * @return ESP_OK if the MAC filter is set, ESP_ERR_INVALID_ARG if the sublayer is NULL, ESP_ERR_INVALID_ARG if the MAC address is NULL, ESP_ERR_INVALID_ARG if the MAC address length is not 6, ESP_ERR_INVALID_ARG if the add flag is not true or false.
 */
 esp_err_t eth_sublayer_set_mac_filter(esp_eth_sublayer_handle_t sub, const uint8_t *eth_mac, size_t mac_len, bool add);

/**
 * @brief Transmit a frame through the sublayer.
 *
 * @param sub The sublayer handle.
 * @param tx_bufs The buffer descriptor set.
 * @param eb The extended buffer (may hold additional metadata like time stamp).
 * @param port The port number.
 * @return ESP_OK if the frame is transmitted, ESP_ERR_INVALID_ARG if the sublayer is NULL, ESP_ERR_INVALID_ARG if the buffer descriptors are NULL, ESP_ERR_INVALID_ARG if the event base is NULL.
 */
esp_err_t eth_sublayer_transmit(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_tx_bufs_t *tx_bufs, void *eb, int32_t port);


/**
 * @brief Free a buffer from the sublayer.
 *
 * @param sub The sublayer handle.
 * @param buffer The buffer to free.
 * @return void
 */
void eth_sublayer_buf_free(esp_eth_sublayer_handle_t sub, void *buffer);

#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
/**
 * @brief Attach the single integrated switch instance to the sublayer.
 *
 * @param sub The sublayer handle.
 * @param sw The switch instance.
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if a switch is already attached, ESP_ERR_INVALID_ARG on bad args.
 */
esp_err_t eth_sublayer_set_switch(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_switch_t *sw);

/**
 * @brief Get the single integrated switch instance attached to the sublayer.
 *
 * @param sub The sublayer handle.
 * @return The switch instance if found, NULL otherwise.
 */
esp_eth_sublayer_switch_t *eth_sublayer_get_switch(esp_eth_sublayer_handle_t sub);

/**
 * @brief Detach the integrated switch instance from the sublayer.
 *
 * @param sub The sublayer handle.
 * @param sw The switch instance.
 * @return ESP_OK if detached, ESP_ERR_NOT_FOUND if not attached to this sublayer, ESP_ERR_INVALID_ARG on bad args.
 */
esp_err_t eth_sublayer_remove_switch(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_switch_t *sw);
#endif // CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
