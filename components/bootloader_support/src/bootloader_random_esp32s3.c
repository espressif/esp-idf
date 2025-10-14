/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/syscon_reg.h"
#include "esp_private/regi2c_ctrl.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#include "hal/regi2c_ctrl_ll.h"

#define ADC_RNG_CLKM_DIV_NUM            3
#define ADC_RNG_CLKM_DIV_B              0
#define ADC_RNG_CLKM_DIV_A              0

void bootloader_random_enable(void)
{
    SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_RNG_EN);

    // Enable 8M clock source for RNG (this is actually enough to produce strong random results,
    // but enabling the SAR ADC as well adds some insurance.)
    REG_SET_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN);

    _adc_ll_reset_register();
    _adc_ll_enable_bus_clock(true);
    _adc_ll_enable_bus_clock(false);

    adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_APB);
    adc_ll_digi_controller_clk_div(ADC_RNG_CLKM_DIV_NUM, ADC_RNG_CLKM_DIV_B, ADC_RNG_CLKM_DIV_A);

#ifndef BOOTLOADER_BUILD
    regi2c_saradc_enable();
#else
    regi2c_ctrl_ll_i2c_sar_periph_enable();
#endif
    // enable analog i2c master clock for RNG runtime
    ANALOG_CLOCK_ENABLE();

    adc_ll_regi2c_init();

    adc_digi_pattern_config_t pattern_config = {};
    pattern_config.unit = ADC_UNIT_1;
    pattern_config.atten = ADC_ATTEN_DB_12;
    pattern_config.channel = ADC_CHANNEL_10;     //Use reserved channel to get internal voltage
    adc_ll_digi_set_pattern_table(ADC_UNIT_1, 0, pattern_config);
    pattern_config.unit = ADC_UNIT_2;
    pattern_config.atten = ADC_ATTEN_DB_12;
    pattern_config.channel = ADC_CHANNEL_10;     //Use reserved ADC2 and reserved channel to get internal voltage
    adc_ll_digi_set_pattern_table(ADC_UNIT_2, 1, pattern_config);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_1, 1);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_2, 1);

    adc_ll_digi_set_convert_mode(ADC_LL_DIGI_CONV_BOTH_UNIT);

    adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_DIG);
    adc_ll_disable_sleep_controller();
    adc_ll_set_arbiter_work_mode(ADC_ARB_MODE_LOOP);

    adc_ll_digi_set_clk_div(3);
    adc_ll_digi_set_trigger_interval(70);
    adc_ll_digi_trigger_enable();
}

void bootloader_random_disable(void)
{
   adc_ll_digi_trigger_disable();
   adc_ll_digi_reset_pattern_table();

   adc_ll_regi2c_adc_deinit();
#ifndef BOOTLOADER_BUILD
   regi2c_saradc_disable();
#endif

   // disable analog i2c master clock
   ANALOG_CLOCK_DISABLE();
   adc_ll_digi_controller_clk_div(4, 0, 0);
   adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_APB);

   adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_ULP);
}
