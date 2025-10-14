/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_types.h"
#include "hal/gpio_types.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C slave specific configurations
 */
typedef struct {
    i2c_port_num_t i2c_port;                 /*!< I2C port number, `-1` for auto selecting */
    gpio_num_t sda_io_num;                   /*!< SDA IO number used by I2C bus */
    gpio_num_t scl_io_num;                   /*!< SCL IO number used by I2C bus */
    i2c_clock_source_t clk_source;           /*!< Clock source of I2C bus. */
    uint32_t send_buf_depth;                 /*!< Depth of internal transfer ringbuffer */
    uint32_t receive_buf_depth;              /*!< Depth of receive internal software buffer */
    uint16_t slave_addr;                     /*!< I2C slave address */
    i2c_addr_bit_len_t addr_bit_len;         /*!< I2C slave address in bit length */
    int intr_priority;                       /*!< I2C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    struct {
        uint32_t allow_pd:    1;  /*!< If set, the driver will backup/restore the I2C registers before/after entering/exist sleep mode.
                                              By this approach, the system can power off I2C's power domain.
                                              This can save power, but at the expense of more RAM being consumed */
        uint32_t enable_internal_pullup: 1;  /*!< Enable internal pullups. Note: This is not strong enough to pullup buses under high-speed frequency. Recommend proper external pull-up if possible */
#if SOC_I2C_SLAVE_SUPPORT_BROADCAST
        uint32_t broadcast_en: 1;            /*!< I2C slave enable broadcast, able to respond to broadcast address */
#endif
    } flags;                                 /*!< I2C slave config flags */
} i2c_slave_config_t;

/**
 * @brief Group of I2C slave callbacks. Take care of potential concurrency issues.
 * @note The callbacks are all running under ISR context
 * @note When CONFIG_I2C_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    i2c_slave_request_callback_t on_request;         /*!< Callback for when a master requests data from the slave */
    i2c_slave_received_callback_t on_receive;         /*!< Callback for when the slave receives data from the master */
} i2c_slave_event_callbacks_t;

/**
 * @brief Write buffer to hardware fifo. If write length is larger than hardware fifo, then restore in software buffer.
 *
 * @param[in] i2c_slave I2C slave device handle that created by `i2c_new_slave_device`.
 * @param[in] data Buffer to write to slave fifo, can pickup by master.
 * @param[in] len In bytes, of `data` buffer.
 * @param[out] write_len In bytes, actually write length.
 * @param[in] timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 * @return
 *      - ESP_OK: I2C slave write success.
 *      - ESP_ERR_INVALID_ARG: I2C slave write parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the device is busy or hardware crash.
 */
esp_err_t i2c_slave_write(i2c_slave_dev_handle_t i2c_slave, const uint8_t *data, uint32_t len, uint32_t *write_len, int timeout_ms);

/**
 * @brief Initialize an I2C slave device
 *
 * @param[in] slave_config I2C slave device configurations
 * @param[out] ret_handle Return a generic I2C device handle
 * @return
 *      - ESP_OK: I2C slave device initialized successfully
 *      - ESP_ERR_INVALID_ARG: I2C device initialization failed because of invalid argument.
 *      - ESP_ERR_NO_MEM: Create I2C device failed because of out of memory.
 */
esp_err_t i2c_new_slave_device(const i2c_slave_config_t *slave_config, i2c_slave_dev_handle_t *ret_handle);

/**
 * @brief Set I2C slave event callbacks for I2C slave channel.
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 * @note When CONFIG_I2C_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well. The `user_data` should also reside in SRAM.
 *
 * @param[in] i2c_slave I2C slave device handle that created by `i2c_new_slave_device`.
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set I2C transaction callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set I2C transaction callbacks failed because of invalid argument
 *      - ESP_FAIL: Set I2C transaction callbacks failed because of other error
 */
esp_err_t i2c_slave_register_event_callbacks(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Deinitialize the I2C slave device
 *
 * @param[in] i2c_slave I2C slave device handle that created by `i2c_new_slave_device`.
 * @return
 *      - ESP_OK: Delete I2C device successfully.
 *      - ESP_ERR_INVALID_ARG: I2C device initialization failed because of invalid argument.
 */
esp_err_t i2c_del_slave_device(i2c_slave_dev_handle_t i2c_slave);

#ifdef __cplusplus
}
#endif
