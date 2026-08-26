/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_eth_driver.h"
#include "esp_eth_sublayer.h"

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
#include "esp_private/esp_eth_sublayer_iodriver.h"
#endif

typedef struct esp_eth_sublayer_switch_s esp_eth_sublayer_switch_t;

/**
 * @brief RX demultiplexing of an integrated switch frame.
 *
 * Strips the switch tag (e.g. Tail Tag) and resolves the ingress port by invoking the driver demux
 * callback. The frame is not re-routed; the sublayer continues its normal RX pipeline afterwards.
 *
 * @param[in] sw The switch instance.
 * @param[in,out] buffer In: frame start including the switch tag; out: frame start with tag removed.
 * @param[in,out] length Frame length including the tag on input, tag removed on output.
 * @param[out] src_port Ingress port number (may be NULL). Only written when ESP_OK is returned.
 * @return ESP_OK on success, ESP_ERR_NOT_SUPPORTED when the switch has no demux callback (frame left
 *         unchanged, @p src_port not resolved), error code otherwise (the sublayer drops the frame).
 */
esp_err_t eth_switch_demux(esp_eth_sublayer_switch_t *sw, uint8_t **buffer, uint32_t *length, int32_t *src_port);

/**
 * @brief Resolves a switch port number to its Ethernet driver handle and L2 TAP iodriver handle.
 *
 * @param[in] sw The switch instance.
 * @param[in] port_num Ingress port number reported by the demux callback.
 * @param[out] eth_handle Port Ethernet driver handle (may be NULL).
 * @param[out] io_handle Port L2 TAP iodriver handle (may be NULL).
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if @p port_num is not registered.
 */
esp_err_t eth_switch_resolve_ingress_port(esp_eth_sublayer_switch_t *sw, int32_t port_num,
                                          esp_eth_handle_t *eth_handle, void **io_handle);

/**
 * @brief TX multiplexing of an integrated switch frame.
 *
 * Appends the switch tag (e.g. Tail Tag) by invoking the driver mux callback.
 *
 * @param[in] sw The switch instance.
 * @param[in,out] tx_bufs Buffer descriptor set comprising the frame.
 * @param port Destination port number.
 * @return ESP_OK on success, ESP_ERR_NOT_SUPPORTED when the switch has no mux callback (frame left
 *         unchanged), error code otherwise.
 */
esp_err_t eth_switch_mux(esp_eth_sublayer_switch_t *sw, esp_eth_sublayer_tx_bufs_t *tx_bufs, int32_t port);

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
/**
 * @brief Fill IO driver function pointers for a switch.
 *
 * Allows transmit via switch specific port.
 *
 * @param[in] sw The switch instance.
 * @param[in] io_handle The IO handle.
 * @param[out] io_fns Output; switch specific port iodriver handle.
 * @return ESP_OK on success, error code otherwise.
 */
esp_err_t eth_switch_get_iodriver_io_fns(esp_eth_sublayer_switch_t *sw, void *io_handle, esp_eth_iodriver_io_fns_t *io_fns);
#endif
