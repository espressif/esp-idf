/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include_next "hal/efuse_hal.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief get chip version
 */
uint32_t efuse_hal_get_chip_revision(void);

/**
 * @brief set eFuse timings
 *
 * @param apb_freq_hz APB frequency in Hz
 */
void efuse_hal_set_timing(uint32_t apb_freq_hz);

/**
 * @brief trigger eFuse read operation
 */
void efuse_hal_read(void);

/**
 * @brief clear registers for programming eFuses
 */
void efuse_hal_clear_program_registers(void);

/**
 * @brief burn eFuses written in programming registers (one block at once)
 *
 * @param block block number
 */
void efuse_hal_program(uint32_t block);

/**
 * @brief Calculate Reed-Solomon Encoding values for a block of efuse data.
 *
 * @param data Pointer to data buffer (length 32 bytes)
 * @param rs_values Pointer to write encoded data to (length 12 bytes)
 */
void efuse_hal_rs_calculate(const void *data, void *rs_values);

/**
 * @brief Checks coding error in a block
 *
 * @param block Index of efuse block
 *
 * @return True  - block has an error.
 *         False - no error.
 */
bool efuse_hal_is_coding_error_in_block(unsigned block);

#ifdef __cplusplus
}
#endif
