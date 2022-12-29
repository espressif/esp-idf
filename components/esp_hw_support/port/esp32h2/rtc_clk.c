/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp32h2/rom/ets_sys.h"
#include "esp32h2/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_hw_log.h"
#include "esp_rom_sys.h"
#include "hal/usb_serial_jtag_ll.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "soc/lp_clkrst_reg.h"


static const char *TAG = "rtc_clk";

soc_rtc_slow_clk_src_t rtc_clk_slow_src_get(void)
{
    // ESP32H2-TODO: IDF-6254
    return REG_GET_FIELD(LP_CLKRST_LP_CLK_CONF_REG, LP_CLKRST_SLOW_CLK_SEL);
}

uint32_t rtc_clk_slow_freq_get_hz(void)
{
    // ESP32H2-TODO: IDF-6254
    switch (rtc_clk_slow_freq_get()) {
        case RTC_SLOW_FREQ_RTC: return RTC_SLOW_CLK_FREQ_150K;
        case RTC_SLOW_FREQ_32K_XTAL: return RTC_SLOW_CLK_FREQ_32K;
        case RTC_SLOW_FREQ_8MD256: return RTC_SLOW_CLK_FREQ_8MD256;
        default: return 0;
    }
    return 0;
}

void rtc_clk_cpu_freq_set_xtal(void)
{
    // ESP32H2-TODO: IDF-6254
}

rtc_xtal_freq_t rtc_clk_xtal_freq_get(void)
{
    ESP_EARLY_LOGW(TAG, "rtc_clk_xtal_freq_get() has not benn implemented yet");
    // ESP32H2-TODO: IDF-6254
    return 32;
}

void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq)
{
    // ESP32H2-TODO: IDF-6254
}

void rtc_clk_apb_freq_update(uint32_t apb_freq)
{
    // ESP32H2-TODO: IDF-6254
}

uint32_t rtc_clk_apb_freq_get(void)
{
    ESP_EARLY_LOGW(TAG, "rtc_clk_apb_freq_get() has not benn implemented yet");
    // ESP32H2-TODO: IDF-6254
    return 0;
}
