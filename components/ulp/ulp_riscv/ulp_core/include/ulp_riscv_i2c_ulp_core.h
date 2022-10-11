/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Set the I2C slave device address
 *
 * @param slave_addr    I2C slave address (7 bit)
 */
void ulp_riscv_i2c_master_set_slave_addr(uint8_t slave_addr);

/**
 * @brief Set the I2C slave device sub register address
 *
 * @param slave_reg_addr    I2C slave register address
 */
void ulp_riscv_i2c_master_set_slave_reg_addr(uint8_t slave_reg_addr);

/**
 * @brief Read from I2C slave device
 *
 * @note The I2C slave device address must be configured at least once before invoking this API.
 *
 * @param data_rd       Buffer to hold data to be read
 * @param size          Size of data to be read in bytes
 */
void ulp_riscv_i2c_master_read_from_device(uint8_t *data_rd, size_t size);

/**
 * @brief Write to I2C slave device
 *
 * @note The I2C slave device address must be configured at least once before invoking this API.
 *
 * @param data_wr       Buffer which holds the data to be written
 * @param size          Size of data to be written in bytes
 */
void ulp_riscv_i2c_master_write_to_device(uint8_t *data_wr, size_t size);

#ifdef __cplusplus
}
#endif
