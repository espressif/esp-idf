// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

/**
 * @file esp_deep_sleep.h
 * @brief legacy definitions of esp_deep_sleep APIs
 *
 * This file provides compatibility for applications using esp_deep_sleep_* APIs.
 * New applications should use functions defined in "esp_sleep.h" instead.
 * These functions and types will be deprecated at some point.
 */

#warning esp_deep_sleep.h will be deprecated in the next release. Use esp_sleep.h instead.

#include "esp_sleep.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef esp_sleep_pd_domain_t esp_deep_sleep_pd_domain_t;
typedef esp_sleep_pd_option_t esp_deep_sleep_pd_option_t;
typedef esp_sleep_ext1_wakeup_mode_t esp_ext1_wakeup_mode_t;
typedef esp_sleep_wakeup_cause_t esp_deep_sleep_wakeup_cause_t;

inline static esp_err_t esp_deep_sleep_enable_ulp_wakeup(void)
{
    return esp_sleep_enable_ulp_wakeup();
}

inline static esp_err_t esp_deep_sleep_enable_timer_wakeup(uint64_t time_in_us)
{
    return esp_sleep_enable_timer_wakeup(time_in_us);
}

inline static esp_err_t esp_deep_sleep_enable_touchpad_wakeup(void)
{
    return esp_sleep_enable_touchpad_wakeup();
}

inline static touch_pad_t esp_deep_sleep_get_touchpad_wakeup_status()
{
    return esp_sleep_get_touchpad_wakeup_status();
}

inline static esp_err_t esp_deep_sleep_enable_ext0_wakeup(gpio_num_t gpio_num, int level)
{
    return esp_sleep_enable_ext0_wakeup(gpio_num, level);
}

inline static esp_err_t esp_deep_sleep_enable_ext1_wakeup(uint64_t mask, esp_ext1_wakeup_mode_t mode)
{
    return esp_sleep_enable_ext1_wakeup(mask, mode);
}

inline static esp_err_t esp_deep_sleep_pd_config(
        esp_deep_sleep_pd_domain_t domain,
        esp_deep_sleep_pd_option_t option)
{
    return esp_sleep_pd_config(domain, option);
}

inline static esp_deep_sleep_wakeup_cause_t esp_deep_sleep_get_wakeup_cause()
{
    return esp_sleep_get_wakeup_cause();
}

#define ESP_DEEP_SLEEP_WAKEUP_UNDEFINED     ESP_SLEEP_WAKEUP_UNDEFINED
#define ESP_DEEP_SLEEP_WAKEUP_EXT0          ESP_SLEEP_WAKEUP_EXT0
#define ESP_DEEP_SLEEP_WAKEUP_EXT1          ESP_SLEEP_WAKEUP_EXT1
#define ESP_DEEP_SLEEP_WAKEUP_TIMER         ESP_SLEEP_WAKEUP_TIMER
#define ESP_DEEP_SLEEP_WAKEUP_TOUCHPAD      ESP_SLEEP_WAKEUP_TOUCHPAD
#define ESP_DEEP_SLEEP_WAKEUP_ULP           ESP_SLEEP_WAKEUP_ULP



#ifdef __cplusplus
}
#endif

