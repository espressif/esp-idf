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
