/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_private/regi2c_ctrl.h"
#include "hal/adc_ll.h"
#include "hal/temperature_sensor_ll.h"
#include "esp_private/sar_periph_ctrl.h"

#define ADC_RNG_CLKM_DIV_NUM            15
#define ADC_RNG_CLKM_DIV_B              0
#define ADC_RNG_CLKM_DIV_A              0

void bootloader_random_enable(void)
{
    /* RNG module is always clock enabled */
    REG_SET_FIELD(RTC_CNTL_SENSOR_CTRL_REG, RTC_CNTL_FORCE_XPD_SAR, 0x3);
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_PU_M);

#ifndef BOOTLOADER_BUILD
    sar_periph_ctrl_adc_reset();
#else
    tsens_ll_reg_values_t saved_tsens_regs = {};
    tsens_ll_backup_registers(&saved_tsens_regs);
    _adc_ll_reset_register();
    // Restore temperature sensor related register values after ADC reset
    _temperature_sensor_ll_reset_module();
    tsens_ll_restore_registers(&saved_tsens_regs);
#endif

    _adc_ll_enable_bus_clock(true);
    adc_ll_enable_func_clock(true);
    adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_APB);
    adc_ll_digi_controller_clk_div(ADC_RNG_CLKM_DIV_NUM, ADC_RNG_CLKM_DIV_B, ADC_RNG_CLKM_DIV_A);
    adc_ll_digi_set_power_manage(ADC_LL_POWER_SW_ON);
    adc_ll_digi_set_fsm_time(ADC_LL_FSM_RSTB_WAIT_DEFAULT, ADC_LL_FSM_START_WAIT_DEFAULT,
        ADC_LL_FSM_STANDBY_WAIT_DEFAULT);

#ifndef BOOTLOADER_BUILD
    regi2c_saradc_enable();
#else
    regi2c_ctrl_ll_i2c_sar_periph_enable();
#endif
    // enable analog i2c master clock for RNG runtime
    ANALOG_CLOCK_ENABLE();
    adc_ll_regi2c_init();

    adc_digi_pattern_config_t pattern_config = {};
    pattern_config.unit = ADC_UNIT_2;
    pattern_config.atten = ADC_ATTEN_DB_12;
    pattern_config.channel = ADC_CHANNEL_1;     //Use reserved channel 10 to get internal voltage
    adc_ll_digi_set_pattern_table(ADC_UNIT_2, 0, pattern_config);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_2, 1);

    adc_ll_digi_dma_enable();

    adc_ll_digi_set_clk_div(1);
    adc_ll_digi_set_trigger_interval(100);
    adc_ll_digi_trigger_enable();
}

void bootloader_random_disable(void)
{
    _adc_ll_enable_bus_clock(false);
    adc_ll_digi_trigger_disable();
    adc_ll_digi_reset_pattern_table();
    adc_ll_regi2c_adc_deinit();
#ifndef BOOTLOADER_BUILD
    regi2c_saradc_disable();
#endif
    // disable analog i2c master clock
    ANALOG_CLOCK_DISABLE();
}
