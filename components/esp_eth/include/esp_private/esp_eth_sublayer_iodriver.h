/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
 * @file esp_private/esp_eth_sublayer_iodriver.h
 *
 * @brief Internal (inter-component) IO driver access contract.
 *
 * This header is NOT part of the stable public application API. It defines a generic virtual table used
 * between an IO driver provider (e.g. the Ethernet sublayer) and a consumer (e.g. L2 TAP, or any other
 * upper layer).
 */

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle identifying a single logical IO driver endpoint.
 *
 * An IO driver endpoint is a point at which L2 frames can be transmitted and received (e.g. an Ethernet
 * driver, a VLAN child, or an integrated switch port of the Ethernet sublayer). The concrete meaning of the
 * handle is defined by the provider that resolves it (see @ref esp_eth_iodriver_provider_base_t).
 */
typedef void *esp_eth_iodriver_handle;

/**
 * @brief Opaque handle identifying an IO driver provider.
 *
 * A provider knows how to resolve one or more @ref esp_eth_iodriver_handle values into a set of IO functions
 * (@ref esp_eth_iodriver_io_fns_t). The Ethernet sublayer is one such provider.
 */
typedef void *esp_eth_iodriver_provider_handle;

/**
 * @brief Set of IO functions for a single IO driver endpoint.
 *
 * This is a lightweight virtual table that decouples a consumer (e.g. L2 TAP, or any other upper layer) from
 * the concrete implementation of an IO driver endpoint. A provider fills this structure for a given
 * @ref esp_eth_iodriver_handle; the consumer then calls the function pointers passing back @c io_handle.
 *
 * @note This concept is intentionally independent of any particular consumer so it can be reused by layers
 *       other than L2 TAP.
 */
typedef struct {
    esp_eth_iodriver_handle io_handle; /*!< Handle passed back to the function pointers below */
    esp_err_t (*iodriver_transmit)(esp_eth_iodriver_handle io_handle, void *buf, size_t len);                 /*!< Transmit a single buffer */
    esp_err_t (*iodriver_transmit_wrap)(esp_eth_iodriver_handle io_handle, void *buf, size_t len, void *eb);  /*!< Transmit with a driver control/extended buffer (e.g. metadata) */
    void (*iodriver_free_rx_buffer)(esp_eth_iodriver_handle io_handle, void *buffer);                         /*!< Free an RX buffer previously received on this endpoint */
    esp_err_t (*iodriver_get_ll_driver)(esp_eth_iodriver_handle io_handle, void **ll_driver);                 /*!< Get the underlying low-level driver handle */
} esp_eth_iodriver_io_fns_t;

/**
 * @brief Base interface implemented by an IO driver provider.
 *
 * A provider is registered with a consumer (e.g. L2 TAP) by embedding this structure as the first member of
 * the provider object, so the provider handle can be cast to @c esp_eth_iodriver_provider_base_t *.
 */
typedef struct {
    /**
     * @brief Resolve an IO driver handle into its set of IO functions.
     *
     * @warning The consumer may call this callback with its internal lock held. The implementation
     *          must therefore be a simple lookup only: it must not log, allocate, block, or take any lock.
     *
     * @param[in]  provider_handle Provider handle (the object embedding this base).
     * @param[in]  io_handle       IO driver handle to resolve.
     * @param[out] io_fns          Filled with the IO functions for @p io_handle on success.
     * @return
     *      - ESP_OK if @p io_handle is served by this provider and @p io_fns was filled
     *      - ESP_ERR_NOT_FOUND if this provider does not serve @p io_handle
     *      - ESP_ERR_INVALID_ARG on invalid arguments
     */
    esp_err_t (*get_io_fns)(esp_eth_iodriver_provider_handle provider_handle, void *io_handle, esp_eth_iodriver_io_fns_t *io_fns);
} esp_eth_iodriver_provider_base_t;

#ifdef __cplusplus
}
#endif
