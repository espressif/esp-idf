/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "esp32s2/rom/efuse.h"


uint32_t efuse_hal_get_chip_revision(void)
{
    return efuse_ll_get_chip_revision();
}

/******************* eFuse control functions *************************/

void efuse_hal_set_timing(uint32_t apb_freq_hz)
{
    ets_efuse_set_timing(apb_freq_hz);
}

void efuse_hal_read(void)
{
    ets_efuse_read();
}

void efuse_hal_clear_program_registers(void)
{
    ets_efuse_clear_program_registers();
}

void efuse_hal_program(uint32_t block)
{
    ets_efuse_program(block);
}

void efuse_hal_rs_calculate(const void *data, void *rs_values)
{
    ets_efuse_rs_calculate(data, rs_values);
}

/******************* eFuse control functions *************************/
