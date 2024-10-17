/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "driver/touch_sens.h"
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

// Touch version 3 supports multiple sample configurations
#define EXAMPLE_TOUCH_SAMPLE_CFG_NUM        TOUCH_SAMPLE_CFG_NUM  // Up to 'TOUCH_SAMPLE_CFG_NUM'
#define EXAMPLE_TOUCH_CHANNEL_NUM           4
#define EXAMPLE_TOUCH_CHAN_INIT_SCAN_TIMES  3

ESP_STATIC_ASSERT(EXAMPLE_TOUCH_SAMPLE_CFG_NUM <= TOUCH_SAMPLE_CFG_NUM, "sample configuration number exceed the supported number");
ESP_STATIC_ASSERT(EXAMPLE_TOUCH_CHANNEL_NUM <= (TOUCH_MAX_CHAN_ID - TOUCH_MIN_CHAN_ID + 1), "touch channel number exceed the max supported number ");

#if SOC_TOUCH_SENSOR_VERSION == 2
#define EXAMPLE_TOUCH_SAMPLE_CFG_DEFAULT()      {TOUCH_SENSOR_V2_DEFAULT_SAMPLE_CONFIG(500, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_2V2)}
#define EXAMPLE_TOUCH_CHAN_CFG_DEFAULT()        {  \
    .active_thresh = {2000},  \
    .charge_speed = TOUCH_CHARGE_SPEED_7,  \
    .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_LOW,  \
}
#elif SOC_TOUCH_SENSOR_VERSION == 3
#define EXAMPLE_TOUCH_SAMPLE_CFG(res, cap, coarse_freq_tune, fine_freq_tune) { \
    .div_num = 8, \
    .charge_times = 500, \
    .rc_filter_res = res, \
    .rc_filter_cap = cap, \
    .low_drv = fine_freq_tune, \
    .high_drv = coarse_freq_tune, \
    .bias_volt = 5, \
    .bypass_shield_output = false, \
}
#define EXAMPLE_TOUCH_SAMPLE_CFG_DEFAULT()      {EXAMPLE_TOUCH_SAMPLE_CFG(3, 29, 8, 3),\
                                                 EXAMPLE_TOUCH_SAMPLE_CFG(2, 88, 31, 7), \
                                                 EXAMPLE_TOUCH_SAMPLE_CFG(3, 10, 31, 7)}
#define EXAMPLE_TOUCH_CHAN_CFG_DEFAULT()        {  \
    .active_thresh = {1000, 2500, 5000},  \
}
#else
#error "Target not supported"
#endif

#ifdef __cplusplus
}
#endif
