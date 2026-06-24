/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "hal/config.h"
#include "esp_attr.h"

#define ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block) ((error_reg) & (0x0F << (4 * (block))))

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
    efuse_ll_set_dac_num(0xFF);
    efuse_ll_set_dac_clk_div(0x28);
    efuse_ll_set_pwr_on_num(0x3000);
    efuse_ll_set_pwr_off_num(0x190);
    // for the XTAL = 40 MHz we use the default value = 200.
    // XTAL = 26 MHz the value = 130.
    efuse_ll_set_tpgm_inactive(HAL_CONFIG(XTAL_HINT_FREQ_MHZ) * 5);
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

bool efuse_hal_is_coding_error_in_block(unsigned block)
{
    if (block == 0) {
        return REG_READ(EFUSE_RD_REPEAT_ERR_REG) != 0;
    } else if (block <= 3) {
        // EFUSE_RD_RS_ERR_REG: (hi) ----, ----, ----, ----, ----, BLOCK3, BLOCK2, BLOCK1 (low)
        uint32_t error_reg = REG_READ(EFUSE_RD_RS_ERR_REG);
        return ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block - 1) != 0;
    }
    return false;
}
