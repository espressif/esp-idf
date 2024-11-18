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

//Not supported but common bootloader calls the function. Do nothing
void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    (void)enable;
}

void bootloader_power_glitch_reset_config(bool enable, uint8_t dref)
{
    assert(dref < 8);
    REG_SET_FIELD(LP_ANA_FIB_ENABLE_REG, LP_ANA_ANA_FIB_PWR_GLITCH_ENA, 0);
    if (enable) {
        SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_PERIF_I2C_RSTB);
        SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, POWER_GLITCH_DREF_VDET_PERIF, dref);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, POWER_GLITCH_DREF_VDET_VDDPST, dref);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, POWER_GLITCH_DREF_VDET_XTAL, dref);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, POWER_GLITCH_DREF_VDET_PLL, dref);
        REG_SET_FIELD(LP_ANA_CK_GLITCH_CNTL_REG, LP_ANA_PWR_GLITCH_RESET_ENA, 0xf);
    } else {
        REG_SET_FIELD(LP_ANA_CK_GLITCH_CNTL_REG, LP_ANA_PWR_GLITCH_RESET_ENA, 0);
    }
}
