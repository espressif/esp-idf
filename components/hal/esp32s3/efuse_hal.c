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
#include "esp32s3/rom/efuse.h"

uint32_t efuse_hal_get_chip_revision(void)
{
    return efuse_ll_get_chip_revision();
}

/******************* eFuse control functions *************************/

void efuse_hal_set_timing(uint32_t apb_freq_hz)
{
    (void) apb_freq_hz;
    efuse_ll_set_pwr_off_num(0x190);
}

void efuse_hal_read(void)
{
    efuse_hal_set_timing(0);

    efuse_ll_set_conf_read_op_code();
    efuse_ll_set_read_cmd();

    while (efuse_ll_get_read_cmd() != 0) { }
    /*Due to a hardware error, we have to read READ_CMD again to make sure the efuse clock is normal*/
    while (efuse_ll_get_read_cmd() != 0) { }
}

void efuse_hal_clear_program_registers(void)
{
    ets_efuse_clear_program_registers();
}

void efuse_hal_program(uint32_t block)
{
    efuse_hal_set_timing(0);

    efuse_ll_set_conf_write_op_code();
    efuse_ll_set_pgm_cmd(block);

    while (efuse_ll_get_pgm_cmd() != 0) { }

    efuse_hal_clear_program_registers();
    efuse_hal_read();
}

void efuse_hal_rs_calculate(const void *data, void *rs_values)
{
    ets_efuse_rs_calculate(data, rs_values);
}

/******************* eFuse control functions *************************/
