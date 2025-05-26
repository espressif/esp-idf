/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "hal/gpio_types.h"
#include "driver/i3c_master_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for I3C master bus
 *
 * This structure defines the configuration parameters for the I3C master bus,
 * including GPIO pins, clock source, queue depth, interrupt priority, and other options.
 */
typedef struct {
    gpio_num_t sda_io_num;                               /*!< GPIO number of I3C SDA signal, pulled-up internally */
    gpio_num_t scl_io_num;                               /*!< GPIO number of I3C SCL signal, pulled-up internally */
    i3c_master_clock_source_t clock_source;              /*!< Clock source of I3C master bus */
    size_t trans_queue_depth;                            /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background, only valid in asynchronous transaction. (Typically max_device_num * per_transaction)*/
    size_t max_transfer_size;                            /*!< Maximum transfer size in one transaction, in bytes. This decides the number of DMA nodes will be used for each transaction */
    size_t dma_burst_size;                               /*!< DMA burst size, in bytes */
    int intr_priority;                                   /*!< I3C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    struct {
        uint32_t enable_internal_pullup : 1;             /*!< Enable internal pullups. Note: This is not strong enough to pullup buses under high-speed frequency. Recommend proper external pull-up if possible */
        uint32_t enable_internal_opendrain : 1;          /*!< Pull-Push mode on I3C SCL/SDA pins by default. Set true if open-drain mode is needed */
        uint32_t use_dma : 1;                            /*!< Use dma transaction. Set to 1 to enable the use of DMA for handling data transfers, which can improve performance for large transactions. */
        uint32_t enable_async_trans : 1;                 /*!< Enable asynchronous transactions, allowing the master to perform other tasks while a transaction is in progress. Only works when `use_dma` is set as true. */
    } flags;                                             /*!< I3C master config flags */
} i3c_master_bus_config_t;

/**
 * @brief Create a new I3C master bus.
 *
 * This function initializes a new I3C master bus with the provided configuration.
 *
 * @param[in] bus_config      Pointer to the I3C master bus configuration structure.
 * @param[out] ret_bus_handle Pointer to the location where the handle of the newly created bus will be stored.
 *
 * @return
 *      - ESP_OK: Bus created successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid configuration or parameters.
 *      - ESP_ERR_NO_MEM: Memory allocation failed.
 */
esp_err_t i3c_new_master_bus(const i3c_master_bus_config_t *bus_config, i3c_master_bus_handle_t *ret_bus_handle);

/**
 * @brief Deletes an I3C master bus and releases associated resources.
 *
 * This function deinitializes and deletes the specified I3C master bus instance. It ensures
 * that all resources allocated for the bus are properly released. The caller must ensure
 * that no active operations are ongoing on the bus before invoking this function.
 *
 * @param bus_handle Handle to the I3C master bus to be deleted. This handle must have been
 *                   previously obtained from a successful bus initialization.
 *
 * @return
 *  - `ESP_OK`: The bus was successfully deleted.
 *  - `ESP_ERR_INVALID_ARG`: The provided `bus_handle` is invalid (e.g., null or uninitialized).
 */
esp_err_t i3c_del_master_bus(i3c_master_bus_handle_t bus_handle);

/**
 * @brief Wait for all pending I3C transactions done
 *
 * @param[in] bus_handle I3C bus handle
 * @param[in] timeout_ms Wait timeout, in ms. Specially, -1 means to wait forever.
 * @return
 *      - ESP_OK: Flush transactions successfully
 *      - ESP_ERR_INVALID_ARG: Flush transactions failed because of invalid argument
 *      - ESP_ERR_TIMEOUT: Flush transactions failed because of timeout
 */
esp_err_t i3c_master_bus_wait_all_done(i3c_master_bus_handle_t bus_handle, int timeout_ms);

/**
 * @brief Toggle between synchronous and asynchronous transfer modes for the I3C master bus.
 *
 * This function allows switching the I3C master bus between synchronous and asynchronous
 * transfer modes. It ensures that the necessary preconditions are met before making the switch:
 * - If switching to asynchronous mode, it requires that the async memory has been initialized.
 * - If switching to synchronous mode, it verifies that no pending transactions are in the queue.
 *
 * @param[in] bus_handle Handle to the I3C master bus.
 * @param[in] async_mode Boolean value indicating the target mode:
 *                       - `true` for asynchronous mode.
 *                       - `false` for synchronous mode.
 *
 * @return
 *      - ESP_OK: The mode was successfully toggled.
 *      - ESP_ERR_INVALID_ARG: Invalid input arguments.
 *      - ESP_ERR_INVALID_STATE: Cannot switch to synchronous mode due to pending transactions.
 */
esp_err_t i3c_master_toggle_transfer_async_mode(i3c_master_bus_handle_t bus_handle, bool async_mode);

#ifdef __cplusplus
}
#endif
