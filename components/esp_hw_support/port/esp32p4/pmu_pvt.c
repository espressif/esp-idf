/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/pmu_reg.h"
#include "hal/pmu_hal.h"
#include "pmu_param.h"
#include "esp_rom_sys.h"
#include "esp_private/esp_pmu.h"
#include "soc/regi2c_dig_reg.h"
#include "regi2c_ctrl.h"
#include "soc/rtc.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
#include "esp_hw_log.h"

ESP_HW_LOG_ATTR_TAG(TAG, "pmu_pvt");

#if CONFIG_ESP_ENABLE_PVT

static uint8_t get_lp_hp_gap(void)
{
    int8_t lp_hp_gap = 0;
    uint32_t blk_version = efuse_hal_blk_version();
    uint8_t lp_hp_gap_efuse = 0;
    if (blk_version >= 2 && blk_version != 100) {
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

static uint32_t pvt_get_dcmvset(void)
{
    return GET_PERI_REG_BITS2(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_DBIAS_VOL_V, PMU_HP_DBIAS_VOL_S);
}

static uint32_t pvt_get_lp_dbias(void)
{
    return GET_PERI_REG_BITS2(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_LP_DBIAS_VOL_V, PMU_LP_DBIAS_VOL_S);
}

void pvt_auto_dbias_init(void)
{
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 2 && blk_version != 100) {
        SET_PERI_REG_MASK(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_160M_CLK_EN);
        SET_PERI_REG_MASK(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_PVT_SYS_CLK_EN);
        /*config for dbias func*/
        CLEAR_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);
        esp_rom_delay_us(100);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL0_SEL, PVT_CHANNEL0_SEL, PVT_DBIAS_CHANNEL0_SEL_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL1_SEL, PVT_CHANNEL1_SEL, PVT_DBIAS_CHANNEL1_SEL_S);  // Select monitor cell, which used to monitor PVT situation
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL0_SEL_REG, PVT_DBIAS_CHANNEL0_CFG, PVT_CHANNEL0_CFG, PVT_DBIAS_CHANNEL0_CFG_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL1_SEL_REG, PVT_DBIAS_CHANNEL1_CFG, PVT_CHANNEL1_CFG, PVT_DBIAS_CHANNEL1_CFG_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL2_SEL_REG, PVT_DBIAS_CHANNEL2_CFG, PVT_CHANNEL2_CFG, PVT_DBIAS_CHANNEL2_CFG_S);  // Configure filter threshold for avoiding auto-dbias overly sensitive regulation
        SET_PERI_REG_BITS(PVT_DBIAS_CMD0_REG, PVT_DBIAS_CMD0_PVT, PVT_CMD0, PVT_DBIAS_CMD0_PVT_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CMD1_REG, PVT_DBIAS_CMD1_PVT, PVT_CMD1, PVT_DBIAS_CMD1_PVT_S);
        SET_PERI_REG_BITS(PVT_DBIAS_CMD2_REG, PVT_DBIAS_CMD2_PVT, PVT_CMD2, PVT_DBIAS_CMD2_PVT_S);                          // Configure auto-dbias adjust property, such as adjusting step
        SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);                                             // Start calibration @HP_CALI_DBIAS_DEFAULT
        SET_PERI_REG_BITS(PVT_DBIAS_TIMER_REG, PVT_TIMER_TARGET, PVT_TARGET, PVT_TIMER_TARGET_S);                           // Configure auto-dbias voltage regulation cycle

        SET_PERI_REG_BITS(HP_SYS_CLKRST_PERI_CLK_CTRL24_REG, HP_SYS_CLKRST_REG_PVT_CLK_DIV_NUM, PVT_CLK_DIV, HP_SYS_CLKRST_REG_PVT_CLK_DIV_NUM_S);                       // PVT function clock divider number
        SET_PERI_REG_BITS(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP_CLK_DIV_NUM, PVT_CLK_DIV, HP_SYS_CLKRST_REG_PVT_PERI_GROUP_CLK_DIV_NUM_S); // PVT function clock divider number
        SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL24_REG, HP_SYS_CLKRST_REG_PVT_CLK_EN);
        SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP1_CLK_EN);
        SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP2_CLK_EN);
        SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP3_CLK_EN);
        SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP4_CLK_EN);

        /*config for pvt cell: unit0; site3; vt1*/
        SET_PERI_REG_MASK(PVT_CLK_CFG_REG, PVT_MONITOR_CLK_PVT_EN); // Once enable cannot be closed
        esp_rom_delay_us(100);
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE3_UNIT0_VT1_CONF2_REG, PVT_MONITOR_EDG_MOD_VT1_PD_SITE3_UNIT0, PVT_EDG_MODE, PVT_MONITOR_EDG_MOD_VT1_PD_SITE3_UNIT0_S);  // Select edge_mode
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE3_UNIT0_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE3_UNIT0, PVT_DELAY_NUM_HIGH, PVT_DELAY_LIMIT_VT1_PD_SITE3_UNIT0_S);    // The threshold for determining whether the voltage is too high
        SET_PERI_REG_BITS(PVT_COMB_PD_SITE3_UNIT1_VT1_CONF1_REG, PVT_DELAY_LIMIT_VT1_PD_SITE3_UNIT1, PVT_DELAY_NUM_LOW, PVT_DELAY_LIMIT_VT1_PD_SITE3_UNIT1_S);     // The threshold for determining whether the voltage is too low

        /*config lp offset for pvt func*/
        uint8_t lp_hp_gap = get_lp_hp_gap();
        set_pvt_hp_lp_gap(lp_hp_gap);
    } else {
        ESP_HW_LOGW(TAG, "blk_version is less than 2, pvt auto dbias init not supported in efuse.");
    }
}

void pvt_func_enable(bool enable)
{
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 2 && blk_version != 100){

        if (enable) {
            SET_PERI_REG_MASK(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_160M_CLK_EN);
            SET_PERI_REG_MASK(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_PVT_SYS_CLK_EN);
            SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL24_REG, HP_SYS_CLKRST_REG_PVT_CLK_EN);
            SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP1_CLK_EN);
            SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP2_CLK_EN);
            SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP3_CLK_EN);
            SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_PVT_PERI_GROUP4_CLK_EN);
            SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);                        // Start calibration @HP_CALI_DBIAS_DEFAULT
            SET_PERI_REG_MASK(PVT_CLK_CFG_REG, PVT_MONITOR_CLK_PVT_EN);                                    // Once enable cannot be closed
            SET_PERI_REG_MASK(PVT_COMB_PD_SITE3_UNIT0_VT1_CONF1_REG, PVT_MONITOR_EN_VT1_PD_SITE3_UNIT0);   // Enable pvt clk
            esp_rom_delay_us(10);
            CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL);            // Hand over control of dbias to pvt
            CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);                      // Must clear @HP_CALI_DBIAS_DEFAULT
            SET_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);                                          // Enable auto dbias
        } else {
            uint32_t pvt_dcmvset = pvt_get_dcmvset();
            uint32_t pvt_lpdbias = pvt_get_lp_dbias();                                                    // Update pvt_cali_dbias
            SET_PERI_REG_BITS(PMU_HP_ACTIVE_BIAS_REG, PMU_HP_ACTIVE_DCM_VSET, pvt_dcmvset, PMU_HP_ACTIVE_DCM_VSET_S);
            SET_PERI_REG_BITS(PMU_HP_SLEEP_LP_REGULATOR0_REG, PMU_HP_SLEEP_LP_REGULATOR_DBIAS, pvt_lpdbias, PMU_HP_SLEEP_LP_REGULATOR_DBIAS_S);
            SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL);              // Hand over control of dbias to pmu
            CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL24_REG, HP_SYS_CLKRST_REG_PVT_CLK_EN);
        }
    } else {
        ESP_HW_LOGD(TAG, "blk_version is less than 2, pvt function not supported in efuse.");
    }
}

#endif
