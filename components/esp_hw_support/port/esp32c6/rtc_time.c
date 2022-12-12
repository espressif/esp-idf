/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp32c6/rom/ets_sys.h"
#include "soc/rtc.h"
// #include "soc/rtc_cntl_reg.h"
#include "hal/clk_tree_ll.h"
#include "soc/timer_group_reg.h"
#include "esp_rom_sys.h"

/* Calibration of RTC_SLOW_CLK is performed using a special feature of TIMG0.
 * This feature counts the number of XTAL clock cycles within a given number of
 * RTC_SLOW_CLK cycles.
 *
 * Slow clock calibration feature has two modes of operation: one-off and cycling.
 * In cycling mode (which is enabled by default on SoC reset), counting of XTAL
 * cycles within RTC_SLOW_CLK cycle is done continuously. Cycling mode is enabled
 * using TIMG_RTC_CALI_START_CYCLING bit. In one-off mode counting is performed
 * once, and TIMG_RTC_CALI_RDY bit is set when counting is done. One-off mode is
 * enabled using TIMG_RTC_CALI_START bit.
 */

/**
 * @brief Clock calibration function used by rtc_clk_cal and rtc_clk_cal_ratio
 * @param cal_clk which clock to calibrate
 * @param slowclk_cycles number of slow clock cycles to count
 * @return number of XTAL clock cycles within the given number of slow clock cycles
 */
// TODO: IDF-5645
static const char *TAG = "rtc_time";

uint32_t rtc_clk_cal_internal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_clk_cal_internal() has not been implemented yet");
    return 0;
}

uint32_t rtc_clk_cal_ratio(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_clk_cal_ratio() has not been implemented yet");
    return 0;
}

uint32_t rtc_clk_cal(rtc_cal_sel_t cal_clk, uint32_t slowclk_cycles)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_clk_cal() has not been implemented yet");
    return 0;
}

uint64_t rtc_time_us_to_slowclk(uint64_t time_in_us, uint32_t period)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_time_us_to_slowclk() has not been implemented yet");
    return 0;
}

uint64_t rtc_time_slowclk_to_us(uint64_t rtc_cycles, uint32_t period)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_time_slowclk_to_us() has not been implemented yet");
    return 0;
}

uint64_t rtc_time_get(void)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_time_get() has not been implemented yet");
    return 0;
}

uint64_t rtc_light_slp_time_get(void)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_light_slp_time_get() has not been implemented yet");
    return 0;
}

uint64_t rtc_deep_slp_time_get(void)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_deep_slp_time_get() has not been implemented yet");
    return 0;
}

void rtc_clk_wait_for_slow_cycle(void) //This function may not by useful any more
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_clk_wait_for_slow_cycle() has not been implemented yet");
}

uint32_t rtc_clk_freq_cal(uint32_t cal_val)
{
    // TODO: IDF-5645
    ESP_EARLY_LOGW(TAG, "rtc_clk_freq_cal() has not been implemented yet");
    return 0;
}
