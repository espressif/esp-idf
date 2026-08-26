/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "hal/efuse_ll.h"
#include "esp_attr.h"

IRAM_ATTR uint32_t efuse_hal_get_major_chip_version(void)
{
    return efuse_ll_get_chip_wafer_version_major();
}

IRAM_ATTR uint32_t efuse_hal_get_minor_chip_version(void)
{
    return efuse_ll_get_chip_wafer_version_minor();
}

/******************* eFuse control functions *************************/

void efuse_hal_set_timing(uint32_t apb_freq_hz)
{
    (void) apb_freq_hz;
}

void efuse_hal_read(void)
{
}

void efuse_hal_clear_program_registers(void)
{
}

void efuse_hal_program(uint32_t block)
{
    (void) block;
}

void efuse_hal_rs_calculate(const void *data, void *rs_values)
{
    (void) data;
    (void) rs_values;
}

/******************* eFuse control functions *************************/

bool efuse_hal_is_coding_error_in_block(unsigned block)
{
    (void) block;
    return false;
}
