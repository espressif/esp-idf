/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "hal/regi2c_ctrl_ll.h"
#include "hal/adc_ll.h"
#include "hal/adc_types.h"
#include "esp_private/regi2c_ctrl.h"
#include "hal/temperature_sensor_ll.h"
#include "esp_private/sar_periph_ctrl.h"

#define I2C_SAR_ADC_INIT_CODE_VAL       2150
#define ADC_RNG_CLKM_DIV_NUM            0
#define ADC_RNG_CLKM_DIV_B              0
#define ADC_RNG_CLKM_DIV_A              0

void bootloader_random_enable(void)
{
#ifndef BOOTLOADER_BUILD
    sar_periph_ctrl_adc_reset();
#else
    tsens_ll_reg_values_t saved_tsens_regs = {};
    tsens_ll_backup_registers(&saved_tsens_regs);
    adc_ll_reset_register();
    // Restore temperature sensor related register values after ADC reset
    temperature_sensor_ll_reset_module();
    tsens_ll_restore_registers(&saved_tsens_regs);
#endif

    adc_ll_enable_bus_clock(true);
    adc_ll_enable_func_clock(true);
    adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_XTAL);
    adc_ll_digi_controller_clk_div(ADC_RNG_CLKM_DIV_NUM, ADC_RNG_CLKM_DIV_B, ADC_RNG_CLKM_DIV_A);

    // some ADC sensor registers are in power group PERIF_I2C and need to be enabled via PMU
#ifndef BOOTLOADER_BUILD
    regi2c_saradc_enable();
#else
    regi2c_ctrl_ll_i2c_sar_periph_enable();
#endif

    // enable analog i2c master clock for RNG runtime
    ANALOG_CLOCK_ENABLE();

    adc_ll_regi2c_init();
    adc_ll_set_calibration_param(ADC_UNIT_1, I2C_SAR_ADC_INIT_CODE_VAL);
    adc_ll_set_calibration_param(ADC_UNIT_2, I2C_SAR_ADC_INIT_CODE_VAL);

    adc_digi_pattern_config_t pattern_config = {};
    pattern_config.unit = ADC_UNIT_2;
    pattern_config.atten = ADC_ATTEN_DB_2_5;
    pattern_config.channel = ADC_CHANNEL_1;     //Use reserved channel to get internal voltage
    adc_ll_digi_set_pattern_table(ADC_UNIT_1, 0, pattern_config);
    pattern_config.unit = ADC_UNIT_2;
    pattern_config.atten = ADC_ATTEN_DB_2_5;
    pattern_config.channel = ADC_CHANNEL_1;     //Use reserved ADC2 and reserved channel to get internal voltage
    adc_ll_digi_set_pattern_table(ADC_UNIT_2, 1, pattern_config);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_2, 2);

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
    adc_ll_regi2c_adc_deinit();
#ifndef BOOTLOADER_BUILD
    regi2c_saradc_disable();
#endif

    // disable analog i2c master clock
    ANALOG_CLOCK_DISABLE();
    adc_ll_digi_controller_clk_div(4, 0, 0);
    adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_XTAL);
}
