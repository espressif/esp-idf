/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
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

#include "esp_rom_sys.h"
#include "hal/clk_tree_ll.h"
#include "hal/sar_ctrl_ll.h"

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
    clk_ll_rc_fast_digi_enable();

    // Enable SAR ADC to read a disconnected input for additional entropy
    _adc_ll_reset_register();
    _adc_ll_enable_bus_clock(true);

    adc_ll_digi_clk_sel(ADC_DIGI_CLK_SRC_APB);
    adc_ll_calibration_prepare(ADC_UNIT_1, false);
    adc_ll_calibration_prepare(ADC_UNIT_2, false);
    adc_ll_calibration_init(ADC_UNIT_1);
    adc_ll_calibration_init(ADC_UNIT_2);

#ifndef BOOTLOADER_BUILD
    regi2c_saradc_enable();
#else
    regi2c_ctrl_ll_i2c_sar_periph_enable();
#endif
    // enable analog i2c master clock for RNG runtime
    ANALOG_CLOCK_ENABLE();

    adc_ll_regi2c_init();
    sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_ON);

    adc_digi_pattern_config_t pattern_config = {};
    pattern_config.unit = ADC_UNIT_1;
    pattern_config.atten = ADC_ATTEN_DB_12;
    pattern_config.channel = ADC_CHANNEL_10;     //Use reserved channel 10 to get internal voltage
    adc_ll_digi_set_pattern_table(ADC_UNIT_1, 0, pattern_config);
    pattern_config.unit = ADC_UNIT_2;
    pattern_config.atten = ADC_ATTEN_DB_12;
    pattern_config.channel = ADC_CHANNEL_10;     //Use reserved channel 10 to get internal voltage
    adc_ll_digi_set_pattern_table(ADC_UNIT_2, 0, pattern_config);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_1, 1);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_2, 1);

    adc_ll_digi_set_convert_mode(ADC_LL_DIGI_CONV_BOTH_UNIT);

    adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_DIG);

    adc_ll_digi_set_clk_div(4);
    adc_ll_digi_set_trigger_interval(100);
    adc_ll_digi_trigger_enable();
}

void bootloader_random_disable(void)
{
    adc_ll_calibration_clear();
    _adc_ll_enable_bus_clock(false);
    adc_ll_digi_trigger_disable();
    adc_ll_digi_reset_pattern_table();

    adc_ll_regi2c_adc_deinit();
 #ifndef BOOTLOADER_BUILD
    regi2c_saradc_disable();
 #endif

    ANALOG_CLOCK_DISABLE();

    /* Note: the 8M CLK entropy source continues running even after this function is called,
       but as mentioned above it's better to enable Wi-Fi or BT or call bootloader_random_enable()
       in order to get a secondary entropy source.
    */
}
