/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sys/queue.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_netif_types.h"
#include "esp_eth_sublayer.h"

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
#include "esp_private/esp_eth_sublayer_iodriver.h"
#endif

typedef struct esp_eth_sublayer_vlan_s {
    esp_netif_driver_base_t base;
    uint16_t vlan_id;
    uint16_t tci_be;                                    /*!< Pre-computed big-endian TCI for TX path */
    esp_eth_sublayer_handle_t parent;
    SLIST_ENTRY(esp_eth_sublayer_vlan_s) next;    /*!< Managed exclusively by sublayer.c */
} esp_eth_sublayer_vlan_t;

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
/**
 * @brief Fill IO driver function pointers for a VLAN
 *
 * @param vlan_netif_driver VLAN netif driver handle (same pointer as esp_netif_attach(..., handle) / esp_netif_get_io_driver()).
 * @param io_fns            Output; iodriver handle transmit functions.
 */
esp_err_t eth_vlan_get_iodriver_io_fns(esp_eth_sublayer_vlan_t *vlan_netif_driver, esp_eth_iodriver_io_fns_t *io_fns);
#endif

/**
 * @brief Input a frame through the VLAN netif.
 *
 * @param vlan_netif_driver VLAN netif driver handle.
 * @param frame             Input; Ethernet frame start (may be offset from the allocation base).
 * @param length            Input; Ethernet frame length.
 * @param alloc_base        Original RX buffer allocation base (passed to esp_netif_receive for freeing).
 * @param info              Input; Additional information.
 * @return ESP_OK on success, error code otherwise.

 */
esp_err_t eth_vlan_input(esp_eth_sublayer_vlan_t *vlan_netif_driver, uint8_t *frame, uint32_t length,
                         void *alloc_base, void *info);

/**
 * @brief Extract VLAN ID from an Ethernet frame (or ESP_ETH_SUBLAYER_UNTAGGED_VID if untagged).
 */
uint16_t eth_vlan_get_ether_type(uint8_t *buffer);
