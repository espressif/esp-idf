/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/rtc_periph.h"
#include "soc/sens_reg.h"
#include "soc/dport_reg.h"
#include "soc/syscon_reg.h"
#include "soc/i2s_periph.h"
#include "esp_log.h"
#include "soc/io_mux_reg.h"
#include "soc/apb_saradc_reg.h"
#include "esp_private/regi2c_ctrl.h"
#include "hal/adc_ll.h"

#ifndef BOOTLOADER_BUILD
#include "esp_private/periph_ctrl.h"
#endif

void bootloader_random_enable(void)
{
    /* Ensure the Wifi clock for RNG modiule is enabled following a soft reset.  This should always be the case already
       (this clock is never disabled while the CPU is running), this is a "belt and braces" type check.
     */
#ifdef BOOTLOADER_BUILD
    DPORT_SET_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_RNG_EN);
#else
    periph_module_enable(PERIPH_RNG_MODULE);
#endif // BOOTLOADER_BUILD

    // Enable 8M clock source for RNG (this is actually enough to produce strong random results,
    // but enabling the SAR ADC as well adds some insurance.)
    REG_SET_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN);

    // Enable SAR ADC to read a disconnected input for additional entropy
    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN0_REG,DPORT_APB_SARADC_CLK_EN);

    REG_SET_FIELD(APB_SARADC_APB_ADC_CLKM_CONF_REG, APB_SARADC_CLK_SEL, 2);

    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PD_M);
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PU_M);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, BIT(18));
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, BIT(16));

    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_DREF_ADDR, 0x4);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_DREF_ADDR, 0x4);

    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENCAL_REF_ADDR, 1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);

    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SAR1_PATT_LEN, 0);
    WRITE_PERI_REG(APB_SARADC_SAR1_PATT_TAB1_REG,0xafffffff);    // set adc1 channel & bitwidth & atten

    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SAR2_PATT_LEN, 0);
    WRITE_PERI_REG(APB_SARADC_SAR2_PATT_TAB1_REG,0xafffffff); //set adc2 channel & bitwidth & atten

    SET_PERI_REG_MASK(SENS_SAR_MEAS1_MUX_REG,SENS_SAR1_DIG_FORCE);

    REG_SET_FIELD(APB_SARADC_CTRL_REG,APB_SARADC_WORK_MODE, 1);

    CLEAR_PERI_REG_MASK(APB_SARADC_CTRL2_REG,APB_SARADC_MEAS_NUM_LIMIT);

    REG_SET_FIELD(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 3);

    SET_PERI_REG_MASK(APB_SARADC_CTRL2_REG,APB_SARADC_TIMER_SEL);

    REG_SET_FIELD(APB_SARADC_CTRL2_REG, APB_SARADC_TIMER_TARGET, 100);

    CLEAR_PERI_REG_MASK(APB_SARADC_CTRL_REG,APB_SARADC_START_FORCE);

    SET_PERI_REG_MASK(APB_SARADC_CTRL2_REG,APB_SARADC_TIMER_EN);
}

void bootloader_random_disable(void)
{
    /* Restore internal I2C bus state */
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_DREF_ADDR, 0x1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_DREF_ADDR, 0x1);

    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENCAL_REF_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);

    /* Restore SARADC to default mode */
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS1_MUX_REG, SENS_SAR1_DIG_FORCE);
    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN0_REG, DPORT_APB_SARADC_CLK_EN);
    SET_PERI_REG_BITS(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    CLEAR_PERI_REG_MASK(APB_SARADC_CTRL2_REG, APB_SARADC_TIMER_EN);

    /* Note: the 8M CLK entropy source continues running even after this function is called,
       but as mentioned above it's better to enable Wi-Fi or BT or call bootloader_random_enable()
       in order to get a secondary entropy source.
    */
}
