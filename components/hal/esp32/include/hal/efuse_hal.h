/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
 * @brief get rated frequency in MHz
 */
uint32_t efuse_hal_get_rated_freq_mhz(void);

/**
 * @brief set eFuse timings
 *
 * @param apb_freq_mhz APB frequency in MHz
 */
void efuse_hal_set_timing(uint32_t apb_freq_mhz);

/**
 * @brief trigger eFuse read operation
 */
void efuse_hal_read(void);

/**
 * @brief clear registers for programming eFuses
 */
void efuse_hal_clear_program_registers(void);

/**
 * @brief burn eFuses written in programming registers (all blocks at once)
 *
 * @param block not used
 */
void efuse_hal_program(uint32_t block);

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
