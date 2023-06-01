/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal/i2c_types.h"
#include "esp_err.h"

/**
 * @brief Read from I2C device
 *
 * @note The LP I2C must have been initialized from the HP core using the lp_core_i2c_master_init() API
 * before invoking this API.
 *
 * @param lp_i2c_num        LP I2C port number
 * @param device_addr       I2C device address (7-bit)
 * @param data_rd           Buffer to hold data to be read
 * @param size              Size of data to be read in bytes
 * @param ticks_to_wait     Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t    ESP_OK when successful
 *
 * @note the LP I2C does not support 10-bit I2C device addresses.
 * @note the LP I2C port number is ignored at the moment.
 */
esp_err_t lp_core_i2c_master_read_from_device(i2c_port_t lp_i2c_num, uint16_t device_addr,
                                            uint8_t *data_rd, size_t size,
                                            int32_t ticks_to_wait);

/**
 * @brief Write to I2C device
 *
 * @note The LP I2C must have been initialized from the HP core using the lp_core_i2c_master_init() API
 * before invoking this API.
 *
 * @param lp_i2c_num        LP I2C port number
 * @param device_addr       I2C device address (7-bit)
 * @param data_wr           Buffer which holds the data to be written
 * @param size              Size of data to be written in bytes
 * @param ticks_to_wait     Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t    ESP_OK when successful
 *
 * @note the LP I2C does not support 10-bit I2C device addresses.
 * @note the LP I2C port number is ignored at the moment.
 */
esp_err_t lp_core_i2c_master_write_to_device(i2c_port_t lp_i2c_num, uint16_t device_addr,
                                            const uint8_t *data_wr, size_t size,
                                            int32_t ticks_to_wait);

/**
 * @brief Write to and then read from an I2C device in a single transaction
 *
 * @note The LP I2C must have been initialized from the HP core using the lp_core_i2c_master_init() API
 * before invoking this API.
 *
 * @param lp_i2c_num        LP I2C port number
 * @param device_addr       I2C device address (7-bit)
 * @param data_wr           Buffer which holds the data to be written
 * @param write_size        Size of data to be written in bytes
 * @param data_rd           Buffer to hold data to be read
 * @param read_size         Size of data to be read in bytes
 * @param ticks_to_wait     Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t    ESP_OK when successful
 *
 * @note the LP I2C does not support 10-bit I2C device addresses.
 * @note the LP I2C port number is ignored at the moment.
 */
esp_err_t lp_core_i2c_master_write_read_device(i2c_port_t lp_i2c_num, uint16_t device_addr,
                                            const uint8_t *data_wr, size_t write_size,
                                            uint8_t *data_rd, size_t read_size,
                                            int32_t ticks_to_wait);

/**
 * @brief Enable or disable ACK checking by the LP_I2C controller during write operations
 *
 * When ACK checking is enabled, the hardware will check the ACK/NACK level received during write
 * operations against the expected ACK/NACK level. If the received ACK/NACK level does not match the
 * expected ACK/NACK level then the hardware will generate the I2C_NACK_INT and a STOP condition
 * will be generated to stop the data transfer.
 *
 * @note ACK checking is enabled by default
 *
 * @param lp_i2c_num    LP I2C port number
 * @param ack_check_en  true: enable ACK check
 *                      false: disable ACK check
 *
 * @note the LP I2C port number is ignored at the moment.
 */
void lp_core_i2c_master_set_ack_check_en(i2c_port_t lp_i2c_num, bool ack_check_en);

#ifdef __cplusplus
}
#endif
