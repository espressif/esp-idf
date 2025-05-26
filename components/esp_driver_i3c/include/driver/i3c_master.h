/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
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
    gpio_num_t sda_io_num;                               /*!< GPIO number of I3C SDA signal*/
    gpio_num_t scl_io_num;                               /*!< GPIO number of I3C SCL signal*/
    i3c_master_clock_source_t clock_source;              /*!< Clock source of I3C master bus */
    size_t trans_queue_depth;                            /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background, only valid in asynchronous transaction. (Typically max_device_num * per_transaction)*/
    int intr_priority;                                   /*!< I3C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    struct {
        uint32_t enable_async_trans : 1;                 /*!< Enable asynchronous transactions, allowing the master to perform other tasks while a transaction is in progress. Only works when DMA is enabled via i3c_master_bus_decorate_dma(). */
    } flags;                                             /*!< I3C master config flags */
} i3c_master_bus_config_t;

/**
 * @brief Configuration structure for I3C master bus DMA decorator
 *
 * This structure defines the DMA configuration parameters for the I3C master bus.
 */
typedef struct {
    size_t max_transfer_size;                            /*!< Maximum transfer size in one transaction, in bytes. This decides the number of DMA nodes */
    size_t dma_burst_size;                               /*!< DMA burst size, in bytes. If 0, driver will use default value (16 bytes) */
} i3c_master_dma_config_t;

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
 * @brief Enable DMA for I3C master bus (decorator pattern)
 *
 * This function enables DMA functionality for an existing I3C master bus. It follows the decorator
 * pattern to avoid linking DMA code when not needed, thus reducing binary size.
 *
 * @param[in] bus_handle  Handle to the I3C master bus to be decorated with DMA capability.
 * @param[in] dma_config  Pointer to the DMA configuration structure. If NULL, DMA will be disabled and fifo will be used.
 *
 * @return
 *      - ESP_OK: DMA decorator applied successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid bus handle or configuration.
 *      - ESP_ERR_INVALID_STATE: Bus is already decorated with DMA or bus is in use.
 *      - ESP_ERR_NO_MEM: Memory allocation failed for DMA resources.
 */
esp_err_t i3c_master_bus_decorate_dma(i3c_master_bus_handle_t bus_handle, const i3c_master_dma_config_t *dma_config);

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

#ifdef __cplusplus
}
#endif
