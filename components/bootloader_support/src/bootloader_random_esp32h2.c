/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp_log.h"
#include "bootloader_random.h"
#include "hal/regi2c_ctrl_ll.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"

void bootloader_random_enable(void)
{
    adc_ll_reset_register();
    adc_ll_enable_bus_clock(true);
    adc_ll_enable_func_clock(true);
    adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_XTAL);
    adc_ll_digi_controller_clk_div(0, 0, 0);

    // some ADC sensor registers are in power group PERIF_I2C and need to be enabled via PMU
    regi2c_ctrl_ll_i2c_periph_enable();
    // enable analog i2c master clock for RNG runtime
    ANALOG_CLOCK_ENABLE();

    adc_ll_set_dtest_param(0);
    adc_ll_set_ent_param(1);
    adc_ll_enable_tout_bus(ADC_UNIT_1, true);
    adc_ll_set_calibration_param(ADC_UNIT_1, 0x866);
    adc_ll_set_calibration_param(ADC_UNIT_2, 0x866);

    adc_digi_pattern_config_t pattern_config = {};
    pattern_config.atten = ADC_ATTEN_DB_2_5;
    adc_ll_digi_set_pattern_table(ADC_UNIT_1, 0, pattern_config);
    pattern_config.unit = ADC_UNIT_2;
    pattern_config.atten = ADC_ATTEN_DB_2_5;
    pattern_config.channel = ADC_CHANNEL_1;
    adc_ll_digi_set_pattern_table(ADC_UNIT_2, 1, pattern_config);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_1, 1);

    adc_ll_digi_set_clk_div(15);
    adc_ll_digi_set_trigger_interval(200);
    adc_ll_digi_trigger_enable();
}

void bootloader_random_disable(void)
{
    adc_ll_digi_trigger_disable();
    adc_ll_digi_reset_pattern_table();
    adc_ll_set_calibration_param(ADC_UNIT_1, 0x0);
    adc_ll_set_calibration_param(ADC_UNIT_2, 0x0);
    adc_ll_set_dtest_param(0);
    adc_ll_set_ent_param(0);
    adc_ll_enable_tout_bus(ADC_UNIT_1, false);

    // disable analog i2c master clock
    ANALOG_CLOCK_DISABLE();
    adc_ll_digi_controller_clk_div(4, 0, 0);
    adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_XTAL);
}
