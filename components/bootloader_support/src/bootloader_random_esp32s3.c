/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "esp_log.h"
#include "soc/system_reg.h"
#include "soc/syscon_reg.h"
#include "soc/apb_saradc_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"

void bootloader_random_enable(void)
{
    SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_RNG_EN);

    // Enable 8M clock source for RNG (this is actually enough to produce strong random results,
    // but enabling the SAR ADC as well adds some insurance.)
    REG_SET_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN);

    /// Enable SAR ADC to read a disconnected input for additional entropy

    // Reset ADC clock
    SET_PERI_REG_MASK(SYSTEM_PERIP_CLK_EN0_REG, SYSTEM_APB_SARADC_CLK_EN);
    CLEAR_PERI_REG_MASK(SYSTEM_PERIP_CLK_EN0_REG, SYSTEM_APB_SARADC_CLK_EN);

    // Enable clock and select clock source for ADC digital controller
    REG_SET_FIELD(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLK_SEL, 2); //APB clock
    SET_PERI_REG_MASK(APB_SARADC_CTRL_REG, APB_SARADC_SAR_CLK_GATED);
    SET_PERI_REG_MASK(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLK_EN);

    // Read freq = apb_clk / (APB_SARADC_CLKM_DIV_NUM + 1) / TIMER_TARGET / 2
    // Internal ADC sample freq = apb_clk / (APB_SARADC_CLKM_DIV_NUM + 1) / (APB_SARADC_SAR_CLK_DIV + 1)
    // Read frequency has to be at least 35 times lower than the sampling frequency

    REG_SET_FIELD(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLKM_DIV_NUM, 3);
    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SAR_CLK_DIV, 3); // SAR clock divider has to be at least 2
    REG_SET_FIELD(APB_SARADC_CTRL2_REG, APB_SARADC_TIMER_TARGET, 70);

    CLEAR_PERI_REG_MASK(APB_SARADC_CTRL_REG, APB_SARADC_START_FORCE);
    REG_SET_FIELD(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 3);
    CLEAR_PERI_REG_MASK(APB_SARADC_CTRL2_REG, APB_SARADC_MEAS_NUM_LIMIT);
    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_WORK_MODE, 1);

    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SAR2_PATT_LEN, 0);
    WRITE_PERI_REG(APB_SARADC_SAR2_PATT_TAB1_REG,0xafffff); // Test internal voltage if the channel info is 0xa.
    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SAR1_PATT_LEN, 0);
    WRITE_PERI_REG(APB_SARADC_SAR1_PATT_TAB1_REG,0xafffff); // Test internal voltage if the channel info is 0xa.

    // Enable adc1 digital controller
    SET_PERI_REG_MASK(SENS_SAR_MEAS1_MUX_REG, SENS_SAR1_DIG_FORCE);

    // Set SARADC2 arbiter
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS2_MUX_REG, SENS_SAR2_RTC_FORCE);
    CLEAR_PERI_REG_MASK(APB_SARADC_APB_ADC_ARB_CTRL_REG, APB_SARADC_ADC_ARB_GRANT_FORCE);
    CLEAR_PERI_REG_MASK(APB_SARADC_APB_ADC_ARB_CTRL_REG, APB_SARADC_ADC_ARB_FIX_PRIORITY);

    // Disable ADC filter
    REG_SET_FIELD(APB_SARADC_FILTER_CTRL0_REG, APB_SARADC_FILTER_CHANNEL0, 0xD);
    REG_SET_FIELD(APB_SARADC_FILTER_CTRL0_REG, APB_SARADC_FILTER_CHANNEL1, 0xD);

    // Start ADC sample
    SET_PERI_REG_MASK(APB_SARADC_CTRL2_REG, APB_SARADC_TIMER_SEL);
    SET_PERI_REG_MASK(APB_SARADC_CTRL2_REG, APB_SARADC_TIMER_EN);

    /*Choose the appropriate internal voltage to measure*/
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENCAL_REF_ADDR, 1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 0);
}

//TODO: IDF-4714
void bootloader_random_disable(void)
{
    /* Restore internal I2C bus state */
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENCAL_REF_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 0);

    //Power off SAR ADC
    REG_SET_FIELD(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 0);
    //return to ADC RTC controller
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS1_MUX_REG, SENS_SAR1_DIG_FORCE);
    //Invalidate ADC digital trigger timer
    CLEAR_PERI_REG_MASK(APB_SARADC_CTRL2_REG, APB_SARADC_TIMER_EN);

    //Disable ADC digital part
    CLEAR_PERI_REG_MASK(SYSTEM_PERIP_CLK_EN0_REG, SYSTEM_APB_SARADC_CLK_EN);
    //Hold reset bit for ADC digital part
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN0_REG, SYSTEM_APB_SARADC_RST);

    /* Note: the 8M CLK entropy source continues running even after this function is called,
       but as mentioned above it's better to enable Wi-Fi or BT or call bootloader_random_enable()
       in order to get a secondary entropy source.
    */
}
