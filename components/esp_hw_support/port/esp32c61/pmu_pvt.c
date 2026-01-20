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
#include "soc/rtc.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
#include "esp_hw_log.h"

static __attribute__((unused)) const char *TAG = "pmu_pvt";

#if CONFIG_ESP_ENABLE_PVT

static uint8_t get_lp_hp_gap(void)
{
    int8_t lp_hp_gap = 0;
    uint32_t blk_version = efuse_hal_blk_version();
    uint8_t lp_hp_gap_efuse = 0;
    if (blk_version >= 2) {
        lp_hp_gap_efuse = efuse_ll_get_dbias_vol_gap();
        bool gap_flag = lp_hp_gap_efuse >> 4;
        uint8_t gap_abs_value = lp_hp_gap_efuse & 0xf;
        if (gap_flag) {
            lp_hp_gap = -1 * gap_abs_value;
        } else {
            lp_hp_gap = gap_abs_value;
        }
        lp_hp_gap = lp_hp_gap - 8;
        assert((lp_hp_gap >= -15) && (lp_hp_gap <= 7));
        if (lp_hp_gap < 0 ) {
            lp_hp_gap = 16 - lp_hp_gap;
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

FORCE_INLINE_ATTR uint32_t get_pvt_hp_dbias(void)
{
    return GET_PERI_REG_BITS2(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_DBIAS_VOL_V, PMU_HP_DBIAS_VOL_S);
}

FORCE_INLINE_ATTR uint32_t get_pvt_lp_dbias(void)
{
    return GET_PERI_REG_BITS2(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_LP_DBIAS_VOL_V, PMU_LP_DBIAS_VOL_S);
}

void pvt_auto_dbias_init(void)
{
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 2) {
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_CLK_EN);
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_EN);
        /*config for dbias func*/
        CLEAR_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);
        esp_rom_delay_us(1);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL0_SEL, PVT_CHANNEL0_SEL, PVT_DBIAS_CHANNEL0_SEL_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL1_SEL, PVT_CHANNEL1_SEL, PVT_DBIAS_CHANNEL1_SEL_S);  // Select monitor cell ,which  used to monitor PVT situation
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL0_SEL_REG, PVT_DBIAS_CHANNEL0_CFG, PVT_CHANNEL0_CFG, PVT_DBIAS_CHANNEL0_CFG_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL1_SEL_REG, PVT_DBIAS_CHANNEL1_CFG, PVT_CHANNEL1_CFG, PVT_DBIAS_CHANNEL1_CFG_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL2_SEL_REG, PVT_DBIAS_CHANNEL2_CFG, PVT_CHANNEL2_CFG, PVT_DBIAS_CHANNEL2_CFG_S);  // Configure filter threshold  for avoiding auto-dbias overly sensitive regulation
        SET_PERI_REG_BITS(PVT_DBIAS_CMD0_REG, PVT_DBIAS_CMD0_PVT, PVT_CMD0, PVT_DBIAS_CMD0_PVT_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CMD1_REG, PVT_DBIAS_CMD1_PVT, PVT_CMD1, PVT_DBIAS_CMD1_PVT_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CMD2_REG, PVT_DBIAS_CMD2_PVT, PVT_CMD2, PVT_DBIAS_CMD2_PVT_S); // Configure auto-dbias adjust property, such as adjusting step
        SET_PERI_REG_BITS(PVT_DBIAS_TIMER_REG, PVT_TIMER_TARGET, PVT_TARGET, PVT_TIMER_TARGET_S);   // Configure auto-dbias  voltage regulation cycle

        SET_PERI_REG_BITS(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM, PVT_CLK_DIV, PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_S);//pvt function clock divider number

        /*config for pvt cell: unit0; site2; vt1*/
        SET_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_SEL); // choose pvt clk
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT0_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT0, PVT_DELAY_NUM_HIGH, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT0_S); // The threshold for determining whether the voltage is too high
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT1_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT1, PVT_DELAY_NUM_LOW, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT1_S); // The threshold for determining whether the voltage is too low
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT2_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT2, PVT_DELAY_NUM_PUMP, PVT_DELAY_LIMIT_VT1_PD_SITE2_UNIT2_S); // The threshold for chargepump

        /*config lp offset for pvt func*/
        uint8_t lp_hp_gap = get_lp_hp_gap();
        set_pvt_hp_lp_gap(lp_hp_gap);
    }
}

void IRAM_ATTR pvt_func_enable(bool enable)
{
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 2) {
        if (enable) {
            SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);     // start calibration @HP_CALI_DBIAS_DEFAULT
            SET_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_EN);
            SET_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_CLK_EN);
            SET_PERI_REG_MASK(PVT_CLK_CFG_REG, PVT_MONITOR_CLK_PVT_EN);                // once enable cannot be closed
            SET_PERI_REG_MASK(PVT_COMB_PD_SITE2_UNIT0_VT1_CONF1_REG, PVT_MONITOR_EN_VT1_PD_SITE2_UNIT0); // enable pvt clk
            esp_rom_delay_us(10);
            CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL); // hand over control of dbias to pvt
            CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);     // must clear @HP_CALI_DBIAS_DEFAULT
            SET_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);   // enable auto dbias
        } else {
            uint32_t pvt_hp_dbias = get_pvt_hp_dbias();
            uint32_t pvt_lp_dbias = get_pvt_lp_dbias(); // update pvt_cali_dbias
            SET_PERI_REG_BITS(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_ACTIVE_HP_REGULATOR_DBIAS, pvt_hp_dbias, PMU_HP_ACTIVE_HP_REGULATOR_DBIAS_S);
            SET_PERI_REG_BITS(PMU_HP_SLEEP_LP_REGULATOR0_REG, PMU_HP_SLEEP_LP_REGULATOR_DBIAS, pvt_lp_dbias, PMU_HP_SLEEP_LP_REGULATOR_DBIAS_S);
            CLEAR_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN); //disable auto dbias
            SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL);   // hand over control of dbias to pmu
            CLEAR_PERI_REG_MASK(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_CLK_EN);
            CLEAR_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_EN);
        }
    }
}

void charge_pump_init(void)
{
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 2) {
        /*config for charge pump*/
        SET_PERI_REG_BITS(PVT_PMUP_CHANNEL_CFG_REG, PVT_PUMP_CHANNEL_CODE0, PVT_PUMP_CHANNEL_CODE, PVT_PUMP_CHANNEL_CODE0_S);   //Set channel code
        WRITE_PERI_REG(PVT_PMUP_BITMAP_LOW0_REG, (1 << PVT_PUMP_BITMAP));  // Select monitor cell for charge pump
        SET_PERI_REG_BITS(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_DRV0, PVT_PUMP_DRV, PVT_PUMP_DRV0_S); //Configure the charging intensity
    }
}

void IRAM_ATTR charge_pump_enable(bool enable)
{
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 2) {
        if (enable) {
            SET_PERI_REG_MASK(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_EN);   // enable charge pump
        } else {
            CLEAR_PERI_REG_MASK(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_EN); //disable charge pump
        }
    }
}

#endif
