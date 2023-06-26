/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/soc.h"
#include "soc/pcr_reg.h"
#include "soc/apb_saradc_reg.h"
#include "soc/pmu_reg.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"
#include "esp_log.h"

static const uint32_t SAR2_CHANNEL = 9;
static const uint32_t PATTERN_BIT_WIDTH = 6;
static const uint32_t SAR1_ATTEN = 1;
static const uint32_t SAR2_ATTEN = 1;

void bootloader_random_enable(void)
{
    // pull SAR ADC out of reset
    REG_SET_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_RST_EN);
    REG_CLR_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_RST_EN);

    // enable SAR ADC APB clock
    REG_SET_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_REG_CLK_EN);

    // enable ADC_CTRL_CLK (SAR ADC function clock)
    REG_SET_BIT(PCR_SARADC_CLKM_CONF_REG, PCR_SARADC_CLKM_EN);

    // select XTAL clock (40 MHz) source for ADC_CTRL_CLK
    REG_SET_FIELD(PCR_SARADC_CLKM_CONF_REG, PCR_SARADC_CLKM_SEL, 0);

    // set the clock divider for ADC_CTRL_CLK to default value (in case it has been changed)
    REG_SET_FIELD(PCR_SARADC_CLKM_CONF_REG, PCR_SARADC_CLKM_DIV_NUM, 0);

    // some ADC sensor registers are in power group PERIF_I2C and need to be enabled via PMU
    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_PERIF_I2C_RSTB);
    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);

    // Config ADC circuit (Analog part) with I2C(HOST ID 0x69) and chose internal voltage as sampling source
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR , 2);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR   , 1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC1_ENCAL_REF_ADDR, 1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC2_ENCAL_REF_ADDR, 1);

    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_HIGH_ADDR, 0x08);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_LOW_ADDR, 0x66);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_HIGH_ADDR, 0x08);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_LOW_ADDR, 0x66);

    // create patterns and set them in pattern table
    uint32_t pattern_one = (SAR2_CHANNEL << 2) | SAR2_ATTEN; // we want channel 9 with max attenuation
    uint32_t pattern_two = SAR1_ATTEN; // we want channel 0 with max attenuation, channel doesn't really matter here
    uint32_t pattern_table = 0 | (pattern_two << 3 * PATTERN_BIT_WIDTH) | pattern_one << 2 * PATTERN_BIT_WIDTH;
    REG_WRITE(APB_SARADC_SAR_PATT_TAB1_REG, pattern_table);

    // set pattern length to 2 (APB_SARADC_SAR_PATT_LEN counts from 0)
    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SARADC_SAR_PATT_LEN, 1);

    // Same as in C3
    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SARADC_SAR_CLK_DIV, 15);

    // set timer expiry (timer is ADC_CTRL_CLK)
    REG_SET_FIELD(APB_SARADC_CTRL2_REG, APB_SARADC_SARADC_TIMER_TARGET, 200);

    // enable timer
    REG_SET_BIT(APB_SARADC_CTRL2_REG, APB_SARADC_SARADC_TIMER_EN);
}

void bootloader_random_disable(void)
{
    // disable timer
    REG_CLR_BIT(APB_SARADC_CTRL2_REG, APB_SARADC_SARADC_TIMER_EN);

    // Write reset value of this register
    REG_WRITE(APB_SARADC_SAR_PATT_TAB1_REG, 0xFFFFFF);

    // Revert ADC I2C configuration and initial voltage source setting
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_HIGH_ADDR, 0x60);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_LOW_ADDR, 0x0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_HIGH_ADDR, 0x60);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_LOW_ADDR, 0x0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC1_ENCAL_REF_ADDR, 0);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC2_ENCAL_REF_ADDR, 0);

    // Revert PMU_RF_PWC_REG to it's initial value
    CLEAR_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_PERIF_I2C_RSTB);

    // disable ADC_CTRL_CLK (SAR ADC function clock)
    REG_WRITE(PCR_SARADC_CLKM_CONF_REG, 0x00404000);

    // Set PCR_SARADC_CONF_REG to initial state
    REG_WRITE(PCR_SARADC_CONF_REG, 0x5);
}
