/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <assert.h>
#include "soc/soc.h"
#include "soc/lp_analog_peri_reg.h"
#include "soc/pmu_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"

void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    // TODO: IDF-9274
    (void)enable;
}

void bootloader_power_glitch_reset_config(bool enable)
{
    //only detect VDDPST POWER GLITCH
    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_PERIF_I2C_RSTB);
    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, POWER_GLITCH_XPD_VDET_PERIF, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, POWER_GLITCH_XPD_VDET_PLLBB, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, POWER_GLITCH_XPD_VDET_PLL, 0);

    REG_SET_FIELD(LP_ANA_FIB_ENABLE_REG, LP_ANA_ANA_FIB_PWR_GLITCH_ENA, 0);//default val for chip from ECO2
    if (enable) {
        REG_SET_FIELD(LP_ANA_POWER_GLITCH_CNTL_REG, LP_ANA_POWER_GLITCH_RESET_ENA, 0xf);//default val for chip from ECO2
    } else {
        REG_SET_FIELD(LP_ANA_POWER_GLITCH_CNTL_REG, LP_ANA_POWER_GLITCH_RESET_ENA, 0);
    }
}
