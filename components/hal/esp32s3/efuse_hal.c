/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_attr.h"

#define ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block) ((error_reg) & (0x0F << (4 * (block))))


//The wafer_major and MSB of wafer_minor fields was allocated to other purposes when block version is v1.1.
//Luckily only chip v0.0 have this kind of block version and efuse usage.
//This workaround fixes the issue.
__attribute__((always_inline))
static inline bool is_eco0(uint32_t minor_raw)
{
    return ((minor_raw & 0x7) == 0 &&
            efuse_ll_get_blk_version_major() == 1 && efuse_ll_get_blk_version_minor() == 1);
}

IRAM_ATTR uint32_t efuse_hal_get_major_chip_version(void)
{
    uint32_t minor_raw = efuse_ll_get_chip_wafer_version_minor();

    if (is_eco0(minor_raw)) {
        return 0;
    }
    return efuse_ll_get_chip_wafer_version_major();
}

IRAM_ATTR uint32_t efuse_hal_get_minor_chip_version(void)
{
    uint32_t minor_raw = efuse_ll_get_chip_wafer_version_minor();

    if (is_eco0(minor_raw)) {
        return 0;
    }
    return minor_raw;
}

/******************* eFuse control functions *************************/

void efuse_hal_set_timing(uint32_t apb_freq_hz)
{
    (void) apb_freq_hz;
    efuse_ll_set_dac_num(0xFF);
    efuse_ll_set_dac_clk_div(0x28);
    efuse_ll_set_pwr_on_num(0x3000);
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

bool efuse_hal_is_coding_error_in_block(unsigned block)
{
    if (block == 0) {
        for (unsigned i = 0; i < 5; i++) {
            if (REG_READ(EFUSE_RD_REPEAT_ERR0_REG + i * 4)) {
                return true;
            }
        }
    } else if (block <= 10) {
        // EFUSE_RD_RS_ERR0_REG: (hi) BLOCK8, BLOCK7, BLOCK6, BLOCK5, BLOCK4, BLOCK3, BLOCK2, BLOCK1 (low)
        // EFUSE_RD_RS_ERR1_REG:                                                     BLOCK10, BLOCK9
        block--;
        uint32_t error_reg = REG_READ(EFUSE_RD_RS_ERR0_REG + (block / 8) * 4);
        return ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block % 8) != 0;
    }
    return false;
}
