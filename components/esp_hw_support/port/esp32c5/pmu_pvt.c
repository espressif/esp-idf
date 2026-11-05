/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/pmu_struct.h"
#include "soc/pvt_reg.h"
#include "soc/pcr_reg.h"
#include "soc/pmu_reg.h"
#include "hal/pmu_hal.h"
#include "pmu_param.h"
#include "esp_rom_sys.h"
#include "esp_private/esp_pmu.h"
#include "soc/regi2c_dig_reg.h"
#include "regi2c_ctrl.h"
#include "soc/rtc.h"
#include "hal/efuse_hal.h"
#include "esp_hw_log.h"

static __attribute__((unused)) const char *TAG = "pmu_pvt";
static bool pvt_enable_flag = false;

#if CONFIG_ESP_ENABLE_PVT

static uint8_t get_lp_hp_gap(void)
{
    int8_t lp_hp_gap = 0;
    uint32_t blk_version = efuse_hal_blk_version();
    uint8_t lp_hp_gap_efuse = 0;
    if (blk_version >= 2) {
        lp_hp_gap_efuse = pmu_ll_get_dbias_vol_gap();
        bool gap_flag = lp_hp_gap_efuse >> 4;
        uint8_t gap_abs_value = lp_hp_gap_efuse & 0xf;
        if (gap_flag) {
            lp_hp_gap = -1 * gap_abs_value;
        } else {
            lp_hp_gap = gap_abs_value;
        }
        lp_hp_gap = lp_hp_gap - 8;
        if (lp_hp_gap < 0) {
            if (lp_hp_gap >= -15) {
                lp_hp_gap = 16 - lp_hp_gap;
            } else {
                // pvt offset value only has 4 bit
                lp_hp_gap = 31;
            }
        }
    }
    return lp_hp_gap;
}

static void set_pvt_hp_lp_gap(uint8_t value)
{
    bool flag = value >> 4;
    uint8_t abs_value = value & 0xf;

    SET_PERI_REG_BITS(PVT_DBIAS_CMD0_REG, PVT_DBIAS_CMD0_OFFSET_FLAG, flag, PVT_DBIAS_CMD0_OFFSET_FLAG_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CMD0_REG, PVT_DBIAS_CMD0_OFFSET_VALUE, abs_value, PVT_DBIAS_CMD0_OFFSET_VALUE_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CMD1_REG, PVT_DBIAS_CMD1_OFFSET_FLAG, flag, PVT_DBIAS_CMD1_OFFSET_FLAG_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CMD1_REG, PVT_DBIAS_CMD1_OFFSET_VALUE, abs_value, PVT_DBIAS_CMD1_OFFSET_VALUE_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CMD2_REG, PVT_DBIAS_CMD2_OFFSET_FLAG, flag, PVT_DBIAS_CMD2_OFFSET_FLAG_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CMD2_REG, PVT_DBIAS_CMD2_OFFSET_VALUE, abs_value, PVT_DBIAS_CMD2_OFFSET_VALUE_S);
}

/* 1. PMU. LP domain, once from pmu_init while DBIAS_SEL is still 1. */
void pvt_dig_dbias_init(void)
{
    SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);     // PVT gear <- register gear, OFFSET <- 0
    CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);   // Must clear @HP_CALI_DBIAS_DEFAULT
    CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL); // Hand over control of dbias to pvt
}

/* 2. PVT controller. Shared by auto dbias and charge pump, re-run after TOP/CPU power-down. */
static void pvt_ctrl_config(bool enable)
{
    if (enable) {
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_CLK_EN);
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_SITE2_CLK_EN);
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_EN);
        SET_PERI_REG_BITS(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM, PVT_CLK_DIV, PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_S); // PVT function clock divider number
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_SEL); // PLL_160M

        /* Soft reset after the clocks are running. Monitor does not reset itself on CPU power-up. */
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_RST_EN);
        CLEAR_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_RST_EN);

        SET_PERI_REG_MASK(PVT_CLK_CFG_REG, PVT_MONITOR_CLK_PVT_EN);
        SET_PERI_REG_MASK(PVT_COMB_PD_SITE2_UNIT0_VT1_CONF1_REG, PVT_MONITOR_EN_VT1_PD_SITE2_UNIT0); // Enable pvt clk , once open cannot be closed
        esp_rom_delay_us(10);
    } else {
        /* Leave PVT_MONITOR_EN set. Clearing it and setting it again can stick the VT reading at 0xFF. */
        CLEAR_PERI_REG_MASK(PVT_CLK_CFG_REG, PVT_MONITOR_CLK_PVT_EN);
        CLEAR_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_EN);
        CLEAR_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_SITE2_CLK_EN);
        CLEAR_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_CLK_EN);
    }
}

/* 3. Auto dbias. Timer stays off until the channel and command registers are written. */
static void pvt_auto_dbias_config(bool enable)
{
    if (enable) {
        CLEAR_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT0_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT0, PVT_DELAY_NUM_HIGH, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT0_S); // The threshold for determining whether the voltage is too high
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT1_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT1, PVT_DELAY_NUM_LOW, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT1_S); // The threshold for determining whether the voltage is too low
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL0_SEL, PVT_CHANNEL0_SEL, PVT_DBIAS_CHANNEL0_SEL_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL1_SEL, PVT_CHANNEL1_SEL, PVT_DBIAS_CHANNEL1_SEL_S); // Select monitor cell , which used to monitor PVT situation
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL0_SEL_REG, PVT_DBIAS_CHANNEL0_CFG, PVT_CHANNEL0_CFG, PVT_DBIAS_CHANNEL0_CFG_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL1_SEL_REG, PVT_DBIAS_CHANNEL1_CFG, PVT_CHANNEL1_CFG, PVT_DBIAS_CHANNEL1_CFG_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL2_SEL_REG, PVT_DBIAS_CHANNEL2_CFG, PVT_CHANNEL2_CFG, PVT_DBIAS_CHANNEL2_CFG_S); // Configure filter threshold for avoiding auto-dbias overly sensitive regulation
        SET_PERI_REG_BITS(PVT_DBIAS_CMD0_REG, PVT_DBIAS_CMD0_PVT, PVT_CMD0, PVT_DBIAS_CMD0_PVT_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CMD1_REG, PVT_DBIAS_CMD1_PVT, PVT_CMD1, PVT_DBIAS_CMD1_PVT_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CMD2_REG, PVT_DBIAS_CMD2_PVT, PVT_CMD2, PVT_DBIAS_CMD2_PVT_S); // Configure auto-dbias adjust property, such as adjusting step
        SET_PERI_REG_BITS(PVT_DBIAS_TIMER_REG, PVT_TIMER_TARGET, PVT_TARGET, PVT_TIMER_TARGET_S); // Configure auto-dbias voltage regulation cycle
        set_pvt_hp_lp_gap(get_lp_hp_gap());
        esp_rom_delay_us(10);
        SET_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);
        esp_rom_delay_us(30);
    } else {
        CLEAR_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);
    }
}

/* 4. Charge pump. */
static void charge_pump_config(bool enable)
{
    if (enable) {
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT2_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT2, PVT_DELAY_NUM_PUMP, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT2_S); // The threshold for chargepump
        SET_PERI_REG_BITS(PVT_PMUP_CHANNEL_CFG_REG, PVT_PUMP_CHANNEL_CODE0, PVT_PUMP_CHANNEL_CODE, PVT_PUMP_CHANNEL_CODE0_S); // Set channel code
        WRITE_PERI_REG(PVT_PMUP_BITMAP_LOW0_REG, (1 << PVT_PUMP_BITMAP)); // Select monitor cell for charge pump
        SET_PERI_REG_BITS(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_DRV0, PVT_PUMP_DRV, PVT_PUMP_DRV0_S); // Configure the charging intensity
        esp_rom_delay_us(10);
        SET_PERI_REG_MASK(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_EN);
    } else {
        CLEAR_PERI_REG_MASK(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_EN);
    }
}

void pvt_auto_dbias_enable(bool enable, bool reconfig)
{
    (void) reconfig;
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 2) {
        if (enable) {
            if (pvt_enable_flag == true) {
                return;
            }
            pvt_ctrl_config(true);
            pvt_auto_dbias_config(true);
            charge_pump_config(true);
            pvt_enable_flag = true;
        } else {
            if (pvt_enable_flag == false) {
                return;
            }
            charge_pump_config(false);
            pvt_auto_dbias_config(false);
            pvt_ctrl_config(false);
            pvt_enable_flag = false;
        }
    }
}

#endif
