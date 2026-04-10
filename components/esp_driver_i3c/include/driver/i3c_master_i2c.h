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
 * @brief Configuration structure for I2C device on an I3C bus
 *
 * This structure defines the configuration parameters for an I2C device
 * when connected to an I3C bus. It includes the device address, clock speed,
 * and additional configuration flags.
 *
 * @note I3C bus can't handle I2C device which has CLOCK STRETCH feature.
 */
typedef struct {
    uint8_t device_address;                              /*!< Device address. Must be 7-bit in I3C-I2C mode */
    uint32_t scl_freq_hz;                                /*!< I2C Device SCL line frequency. */                                         /*!< I2C Devices config flags */
} i3c_device_i2c_config_t;

/**
 * @brief Group of I3C-I2C master callbacks, can be used to get status during transaction or doing other small things. But take care potential concurrency issues.
 * @note The callbacks are all running under ISR context
 * @note When CONFIG_I3C_MASTER_ISR_CACHE_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    i3c_master_i2c_callback_t on_trans_done;             /*!< I3C-I2C master transaction finish callback */
} i3c_master_i2c_event_callbacks_t;

/**
 * @brief Attach an I2C device to the I3C master bus.
 *
 * This function registers an I2C device for communication on the I3C master bus.
 *
 * @param[in] bus_handle  Handle to the I3C master bus.
 * @param[in] dev_config  Pointer to the configuration structure for the I2C device.
 * @param[out] ret_handle Pointer to the location where the handle of the attached I2C device will be stored.
 *
 * @return
 *      - ESP_OK: Device attached successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid parameters or device configuration.
 *      - ESP_ERR_NO_MEM: Memory allocation failed.
 */
esp_err_t i3c_master_bus_add_i2c_device(i3c_master_bus_handle_t bus_handle, const i3c_device_i2c_config_t *dev_config, i3c_master_i2c_device_handle_t *ret_handle);

/**
 * @brief Detach an I2C device from the I3C master bus.
 *
 * This function removes an I2C device that was previously attached to the I3C
 * master bus using `i3c_master_bus_add_i2c_device`. It releases any resources
 * associated with the device handle and invalidates the device handle.
 *
 * @param dev_handle Handle to the I2C device to be detached.
 *                   This handle is obtained from `i3c_master_bus_add_i2c_device`.
 *
 * @return
 *  - ESP_OK: Device successfully detached from the bus.
 *  - ESP_ERR_INVALID_ARG: The device handle is invalid or NULL.
 */
esp_err_t i3c_master_bus_rm_i2c_device(i3c_master_i2c_device_handle_t dev_handle);

/**
 * @brief Transmit data to an I2C device on the I3C master bus.
 *
 * Sends a block of data to the specified I2C device.
 *
 * @param[in] dev_handle      Handle to the I2C device.
 * @param[in] write_buffer    Pointer to the buffer containing data to transmit.
 * @param[in] write_size      Size of the data in bytes.
 * @param[in] xfer_timeout_ms Timeout for the operation in milliseconds. Note: -1 means wait forever.
 *
 * @note If `enable_async_trans` is set, this function operates in asynchronous transmission mode.
 * In this mode, the function returns immediately after being called, even if the transmission
 * is not yet completed. The completion status and any related events should be obtained
 * through the corresponding callback.
 *
 * @return
 *      - ESP_OK: Data transmitted successfully.
 *      - ESP_ERR_TIMEOUT: Transfer timed out.
 *      - ESP_ERR_INVALID_ARG: Invalid parameters.
 */
esp_err_t i3c_master_i2c_device_transmit(i3c_master_i2c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms);

/**
 * @brief Receive data from an I2C device on the I3C master bus.
 *
 * Reads a block of data from the specified I2C device.
 *
 * @param[in] dev_handle      Handle to the I2C device.
 * @param[out] read_buffer    Pointer to the buffer where received data will be stored.
 * @param[in] read_size       Number of bytes to read.
 * @param[in] xfer_timeout_ms Timeout for the operation in milliseconds. Note: -1 means wait forever.
 *
 * @note If `enable_async_trans` is set, this function operates in asynchronous transmission mode.
 * In this mode, the function returns immediately after being called, even if the transmission
 * is not yet completed. The completion status and any related events should be obtained
 * through the corresponding callback.
 *
 * @return
 *      - ESP_OK: Data received successfully.
 *      - ESP_ERR_TIMEOUT: Transfer timed out.
 *      - ESP_ERR_INVALID_ARG: Invalid parameters.
 */
esp_err_t i3c_master_i2c_device_receive(i3c_master_i2c_device_handle_t dev_handle, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Perform a transmit-then-receive operation with an I2C device on the I3C master bus.
 *
 * First sends data to the specified I2C device, then reads a block of data from it.
 *
 * @param[in] dev_handle      Handle to the I2C device.
 * @param[in] write_buffer    Pointer to the buffer containing data to transmit.
 * @param[in] write_size      Size of the data in bytes to transmit.
 * @param[out] read_buffer    Pointer to the buffer where received data will be stored.
 * @param[in] read_size       Number of bytes to read.
 * @param[in] xfer_timeout_ms Timeout for the operation in milliseconds. Note: -1 means wait forever.
 *
 * @note If `enable_async_trans` is set, this function operates in asynchronous transmission mode.
 * In this mode, the function returns immediately after being called, even if the transmission
 * is not yet completed. The completion status and any related events should be obtained
 * through the corresponding callback.
 *
 * @return
 *      - ESP_OK: Transmit and receive operations completed successfully.
 *      - ESP_ERR_TIMEOUT: Transfer timed out.
 *      - ESP_ERR_INVALID_ARG: Invalid parameters.
 */
esp_err_t i3c_master_i2c_device_transmit_receive(i3c_master_i2c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Register event callbacks for an I2C device operating on an I3C master bus
 *
 * This function allows the user to register callback functions for handling specific
 * events related to an I2C device on the I3C master bus. The registered callbacks
 * will be invoked during corresponding events, enabling custom handling of device-specific
 * behaviors.
 *
 * @param dev_handle Handle to the I2C device
 *                   This handle is obtained when the device is added to the I3C bus.
 *
 * @param cbs Pointer to a structure containing the event callback functions
 *            The `cbs` parameter should point to a valid `i3c_master_i2c_event_callbacks_t`
 *            structure that specifies the callbacks for various I2C-related events.
 *
 * @param user_data User-defined data to be passed to the callbacks
 *                  This pointer is passed to all callbacks as a parameter, allowing the user
 *                  to associate context or state with the callbacks.
 *
 * @return
 * - `ESP_OK`: The operation completed successfully.
 * - `ESP_ERR_INVALID_ARG`: One or more parameters are invalid.
 * - `ESP_ERR_NO_MEM`: Insufficient memory to register the callbacks.
 */
esp_err_t i3c_master_i2c_device_register_event_callbacks(i3c_master_i2c_device_handle_t dev_handle, const i3c_master_i2c_event_callbacks_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
