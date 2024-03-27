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
#include "rom/efuse.h"
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
    uint32_t tsup_a;
    uint32_t tpgm;
    uint32_t thp_a;
    uint32_t tpgm_inact;
    uint32_t clk_div;
    uint32_t power_on;
    uint32_t power_off;
    uint32_t tsur_a;
    uint32_t trd;
    uint32_t thr_a;
    if (apb_freq_hz == 80000000) {
        tsup_a = 0x2;
        tpgm = 0x320;
        thp_a = 0x2;
        tpgm_inact = 0x4;
        clk_div = 0xA0;
        power_on = 0xA200;
	    power_off = 0x100;
        tsur_a = 0x2;
        trd = 0x4;
        thr_a = 0x2;
    } else if (apb_freq_hz == 40000000) {
        tsup_a = 0x1;
        tpgm = 0x190;
        thp_a = 0x1;
        tpgm_inact = 0x2;
        clk_div = 0x50;
        power_on = 0x5100;
        power_off = 0x80;
        tsur_a = 0x1;
        trd = 0x2;
        thr_a = 0x1;
    } else { // 20000000 or 5000000 or 10000000
        tsup_a = 0x1;
        tpgm = 0xC8;
        thp_a = 0x1;
        tpgm_inact = 0x1;
        clk_div = 0x28;
        power_on = 0x2880;
        power_off = 0x40;
        tsur_a = 0x1;
        trd = 0x1;
        thr_a = 0x1;
    }
    REG_SET_FIELD(EFUSE_WR_TIM_CONF1_REG, EFUSE_TSUP_A, tsup_a);
    REG_SET_FIELD(EFUSE_WR_TIM_CONF0_REG, EFUSE_TPGM, tpgm);
    REG_SET_FIELD(EFUSE_WR_TIM_CONF0_REG, EFUSE_THP_A, thp_a);
    REG_SET_FIELD(EFUSE_WR_TIM_CONF0_REG, EFUSE_TPGM_INACTIVE, tpgm_inact);
    REG_SET_FIELD(EFUSE_DAC_CONF_REG, EFUSE_DAC_CLK_DIV, clk_div);
    REG_SET_FIELD(EFUSE_WR_TIM_CONF1_REG, EFUSE_PWR_ON_NUM, power_on);
    REG_SET_FIELD(EFUSE_WR_TIM_CONF2_REG, EFUSE_PWR_OFF_NUM, power_off);
    REG_SET_FIELD(EFUSE_RD_TIM_CONF_REG, EFUSE_TSUR_A, tsur_a);
    REG_SET_FIELD(EFUSE_RD_TIM_CONF_REG, EFUSE_TRD, trd);
    REG_SET_FIELD(EFUSE_RD_TIM_CONF_REG, EFUSE_THR_A, thr_a);
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
