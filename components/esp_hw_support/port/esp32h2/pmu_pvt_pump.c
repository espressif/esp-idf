/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/pvt_reg.h"
#include "soc/pmu_reg.h"
#include "soc/pcr_reg.h"
#include "pmu_param.h"
#include "esp_private/esp_pmu.h"
#include "soc/regi2c_pmu.h"
#include "soc/regi2c_bias.h"
#include "regi2c_ctrl.h"
#include "soc/rtc.h"

static __attribute__((unused)) const char *TAG = "pmu_pvt_pump";

void pvt_auto_dbias_init(void)
{
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_EN_I2C_RTC_DREG, 0);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_EN_I2C_DIG_DREG, 0);
    /*config for dbias func*/
    SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL0_SEL, PVT_CHANNEL0_SEL, PVT_DBIAS_CHANNEL0_SEL_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL_SEL0_REG, PVT_DBIAS_CHANNEL1_SEL, PVT_CHANNEL1_SEL, PVT_DBIAS_CHANNEL1_SEL_S);  // Select monitor cell ,which  used to monitor PVT situation
    SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL0_SEL_REG, PVT_DBIAS_CHANNEL0_CFG, PVT_CHANNEL0_CFG, PVT_DBIAS_CHANNEL0_CFG_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL1_SEL_REG, PVT_DBIAS_CHANNEL1_CFG, PVT_CHANNEL1_CFG, PVT_DBIAS_CHANNEL1_CFG_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CHANNEL2_SEL_REG, PVT_DBIAS_CHANNEL2_CFG, PVT_CHANNEL2_CFG, PVT_DBIAS_CHANNEL2_CFG_S);  // Configure filter threshold  for avoiding auto-dbias overly sensitive regulation
    SET_PERI_REG_BITS(PVT_DBIAS_CMD0_REG, PVT_DBIAS_CMD0, PVT_CMD0, PVT_DBIAS_CMD0_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CMD1_REG, PVT_DBIAS_CMD1, PVT_CMD1, PVT_DBIAS_CMD1_S);
    SET_PERI_REG_BITS(PVT_DBIAS_CMD2_REG, PVT_DBIAS_CMD2, PVT_CMD2, PVT_DBIAS_CMD2_S); // Configure auto-dbias adjust property, such as adjusting step
    SET_PERI_REG_BITS(PVT_DBIAS_TIMER_REG, PVT_TIMER_TARGET, PVT_TARGET, PVT_TIMER_TARGET_S);   // Configure auto-dbias  voltage regulation cycle

    SET_PERI_REG_BITS(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM, PVT_CLK_DIV, PCR_PVT_MONITOR_FUNC_CLK_DIV_NUM_S); //pvt function clock divider number

    /*config for pvt cell: unit0; site2; vt0*/
    SET_PERI_REG_MASK(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_SEL); //pvt function clock source select
    SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT0_VT0_CONF2_REG, PVT_MONITOR_EDG_MOD_VT0_PD_SITE2_UNIT0, PVT_EDG_MODE, PVT_MONITOR_EDG_MOD_VT0_PD_SITE2_UNIT0_S);  // Select edge_mode
    SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT0_VT0_CONF1_REG, PVT_DELAY_LIMIT_VT0_PD_SITE2_UNIT0, PVT_DELAY_NUM_HIGH, PVT_DELAY_LIMIT_VT0_PD_SITE2_UNIT0_S); // The threshold for determining whether the voltage is too high
    SET_PERI_REG_BITS(PVT_COMB_PD_SITE2_UNIT1_VT0_CONF1_REG, PVT_DELAY_LIMIT_VT0_PD_SITE2_UNIT1, PVT_DELAY_NUM_LOW, PVT_DELAY_LIMIT_VT0_PD_SITE2_UNIT1_S); // The threshold for determining whether the voltage is too low
}

void pvt_func_enable(bool enable)
{
    if (enable) {
        SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_DBIAS_INIT);
        CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL); // hand over control of dbias to pvt
        SET_PERI_REG_MASK(PVT_CLK_CFG_REG, PVT_MONITOR_CLK_PVT_EN);
        SET_PERI_REG_MASK(PVT_COMB_PD_SITE2_UNIT0_VT0_CONF1_REG, PVT_MONITOR_EN_VT0_PD_SITE2_UNIT0);    // enable pvt clk
        SET_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN);   // enable auto dbias
    } else {
        uint32_t pvt_cali_dbias = get_pvt_dbias(); // update pvt_cali_dbias
        SET_PERI_REG_BITS(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_ACTIVE_HP_REGULATOR_DBIAS_V, pvt_cali_dbias, PMU_HP_ACTIVE_HP_REGULATOR_DBIAS_S);
        SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL);   // hand over control of dbias to pmu
        CLEAR_PERI_REG_MASK(PVT_DBIAS_TIMER_REG, PVT_TIMER_EN); //disable auto dbias
        CLEAR_PERI_REG_MASK(PVT_COMB_PD_SITE2_UNIT0_VT0_CONF1_REG, PVT_MONITOR_EN_VT0_PD_SITE2_UNIT0);
        CLEAR_PERI_REG_MASK(PVT_CLK_CFG_REG, PVT_MONITOR_CLK_PVT_EN);
    }
}

void charge_pump_init(void)
{
    /*config for charge pump*/
    SET_PERI_REG_BITS(PVT_PMUP_CHANNEL_CFG_REG, PVT_PUMP_CHANNEL_CODE0, PVT_PUMP_CHANNEL_CODE, PVT_PUMP_CHANNEL_CODE0_S);   //Set channel code
    WRITE_PERI_REG(PVT_PMUP_BITMAP_LOW0_REG, PVT_PUMP_BITMAP);  // Select monitor cell for charge pump
    SET_PERI_REG_BITS(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_DRV0, PVT_PUMP_DRV, PVT_PUMP_DRV0_S); //Configure the charging intensity
}

void charge_pump_enable(bool enable)
{
    if (enable) {
        SET_PERI_REG_MASK(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_EN);   // enable charge pump
    } else {
        CLEAR_PERI_REG_MASK(PVT_PMUP_DRV_CFG_REG, PVT_PUMP_EN); //disable charge pump
    }
}

inline uint32_t get_pvt_dbias()
{
    return REG_GET_FIELD(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_DBIAS_VOL);
}
