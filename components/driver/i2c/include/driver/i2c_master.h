/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_types.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C master bus specific configurations
 */
typedef struct {
    i2c_port_num_t i2c_port;              /*!< I2C port number, `-1` for auto selecting */
    gpio_num_t sda_io_num;                /*!< GPIO number of I2C SDA signal, pulled-up internally */
    gpio_num_t scl_io_num;                /*!< GPIO number of I2C SCL signal, pulled-up internally */
    i2c_clock_source_t clk_source;        /*!< Clock source of I2C master bus, channels in the same group must use the same clock source */
    uint8_t glitch_ignore_cnt;            /*!< If the glitch period on the line is less than this value, it can be filtered out, typically value is 7 (unit: I2C module clock cycle)*/
    int intr_priority;                    /*!< I2C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    size_t trans_queue_depth;             /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background, only valid in asynchronous transaction. (Typically max_device_num * per_transaction)*/
    struct {
        uint32_t enable_internal_pullup:1;   /*!< Enable internal pullups. Note: This is not strong enough to pullup buses under high-speed frequency. Recommend proper external pull-up if possible */
    } flags;                              /*!< I2C master config flags */
} i2c_master_bus_config_t;

/**
 * @brief I2C device configuration
 */
typedef struct {
    i2c_addr_bit_len_t dev_addr_length;         /*!< Select the address length of the slave device. */
    uint16_t device_address;                    /*!< I2C device raw address. (The 7/10 bit address without read/write bit) */
    uint32_t scl_speed_hz;                      /*!< I2C SCL line frequency. */
} i2c_device_config_t;

/**
 * @brief Group of I2C master callbacks, can be used to get status during transaction or doing other small things. But take care potential concurrency issues.
 * @note The callbacks are all running under ISR context
 * @note When CONFIG_I2C_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    i2c_master_callback_t on_trans_done;  /*!< I2C master transaction finish callback */
} i2c_master_event_callbacks_t;

/**
 * @brief Allocate an I2C master bus
 *
 * @param[in] bus_config I2C master bus configuration.
 * @param[out] ret_bus_handle I2C bus handle
 * @return
 *      - ESP_OK: I2C master bus initialized successfully.
 *      - ESP_ERR_INVALID_ARG: I2C bus initialization failed because of invalid argument.
 *      - ESP_ERR_NO_MEM: Create I2C bus failed because of out of memory.
 *      - ESP_ERR_NOT_FOUND: No more free bus.
 */
esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_bus_handle);

/**
 * @brief Add I2C master BUS device.
 *
 * @param[in] bus_handle I2C bus handle.
 * @param[in] dev_config device config.
 * @param[out] ret_handle device handle.
 * @return
 *      - ESP_OK: Create I2C master device successfully.
 *      - ESP_ERR_INVALID_ARG: I2C bus initialization failed because of invalid argument.
 *      - ESP_ERR_NO_MEM: Create I2C bus failed because of out of memory.
 */
esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *ret_handle);

/**
 * @brief Deinitialize the I2C master bus and delete the handle.
 *
 * @param[in] bus_handle I2C bus handle.
 * @return
 *      - ESP_OK: Delete I2C bus success, otherwise, failed.
 *      - Otherwise: Some module delete failed.
 */
esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t bus_handle);

/**
 * @brief I2C master bus delete device
 *
 * @param handle i2c device handle
 * @return
 *      - ESP_OK: If device is successfully deleted.
 */
esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t handle);

/**
 * @brief Perform a write transaction on the I2C bus.
 *        The transaction will be undergoing until it finishes or it reaches
 *        the timeout provided.
 *
 * @note If a callback was registered with `i2c_master_register_event_callbacks`, the transaction will be asynchronous, and thus, this function will return directly, without blocking.
 *       You will get finish information from callback. Besides, data buffer should always be completely prepared when callback is registered, otherwise, the data will get corrupt.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] write_buffer Data bytes to send on the I2C bus.
 * @param[in] write_size Size, in bytes, of the write buffer.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 * @return
 *      - ESP_OK: I2C master transmit success
 *      - ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_transmit(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms);

/**
 * @brief Perform a write-read transaction on the I2C bus.
 *        The transaction will be undergoing until it finishes or it reaches
 *        the timeout provided.
 *
 * @note If a callback was registered with `i2c_master_register_event_callbacks`, the transaction will be asynchronous, and thus, this function will return directly, without blocking.
 *       You will get finish information from callback. Besides, data buffer should always be completely prepared when callback is registered, otherwise, the data will get corrupt.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] write_buffer Data bytes to send on the I2C bus.
 * @param[in] write_size Size, in bytes, of the write buffer.
 * @param[out] read_buffer Data bytes received from i2c bus.
 * @param[in] read_size Size, in bytes, of the read buffer.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 * @return
 *      - ESP_OK: I2C master transmit-receive success
 *      - ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_transmit_receive(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Perform a read transaction on the I2C bus.
 *        The transaction will be undergoing until it finishes or it reaches
 *        the timeout provided.
 *
 * @note If a callback was registered with `i2c_master_register_event_callbacks`, the transaction will be asynchronous, and thus, this function will return directly, without blocking.
 *       You will get finish information from callback. Besides, data buffer should always be completely prepared when callback is registered, otherwise, the data will get corrupt.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[out] read_buffer Data bytes received from i2c bus.
 * @param[in] read_size Size, in bytes, of the read buffer.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 * @return
 *      - ESP_OK: I2C master receive success
 *      - ESP_ERR_INVALID_ARG: I2C master receive parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_receive(i2c_master_dev_handle_t i2c_dev, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Probe I2C address, if address is correct and ACK is received, this function will return ESP_OK.
 *
 * @param[in] bus_handle I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] address I2C device address that you want to probe.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever (Not recommended in this function).
 * @return
 *      - ESP_OK: I2C device probe successfully
 *      - ESP_ERR_NOT_FOUND: I2C probe failed, doesn't find the device with specific address you gave.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_probe(i2c_master_bus_handle_t bus_handle, uint16_t address, int xfer_timeout_ms);

/**
 * @brief Register I2C transaction callbacks for a master device
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 * @note When CONFIG_I2C_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well. The `user_data` should also reside in SRAM.
 * @note If the callback is used for helping asynchronous transaction. On the same bus, only one device can be used for performing asynchronous operation.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set I2C transaction callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set I2C transaction callbacks failed because of invalid argument
 *      - ESP_FAIL: Set I2C transaction callbacks failed because of other error
 */
esp_err_t i2c_master_register_event_callbacks(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Reset the I2C master bus.
 *
 * @param bus_handle I2C bus handle.
 * @return
 *      - ESP_OK: Reset succeed.
 *      - ESP_ERR_INVALID_ARG: I2C master bus handle is not initialized.
 *      - Otherwise: Reset failed.
 */
esp_err_t i2c_master_bus_reset(i2c_master_bus_handle_t bus_handle);

/**
 * @brief Wait for all pending I2C transactions done
 *
 * @param[in] bus_handle I2C bus handle
 * @param[in] timeout_ms Wait timeout, in ms. Specially, -1 means to wait forever.
 * @return
 *      - ESP_OK: Flush transactions successfully
 *      - ESP_ERR_INVALID_ARG: Flush transactions failed because of invalid argument
 *      - ESP_ERR_TIMEOUT: Flush transactions failed because of timeout
 *      - ESP_FAIL: Flush transactions failed because of other error
 */
esp_err_t i2c_master_bus_wait_all_done(i2c_master_bus_handle_t bus_handle, int timeout_ms);

#ifdef __cplusplus
}
#endif
