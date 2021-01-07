// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for RTC IO (common part)

#include "hal/rtc_io_hal.h"
#include "soc/soc_caps.h"

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

void rtcio_hal_set_direction(int rtcio_num, rtc_gpio_mode_t mode)
{
    switch (mode) {
    case RTC_GPIO_MODE_INPUT_ONLY:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_OUTPUT_NORMAL);
        rtcio_ll_output_disable(rtcio_num);
        rtcio_ll_input_enable(rtcio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_ONLY:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_OUTPUT_NORMAL);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_disable(rtcio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_OUTPUT_NORMAL);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_enable(rtcio_num);
        break;
    case RTC_GPIO_MODE_DISABLED:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_OUTPUT_NORMAL);
        rtcio_ll_output_disable(rtcio_num);
        rtcio_ll_input_disable(rtcio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_OD:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_OUTPUT_OD);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_disable(rtcio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT_OD:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_OUTPUT_OD);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_enable(rtcio_num);
        break;
    default:
        break;
    }
}

void rtcio_hal_isolate(int rtcio_num)
{
    rtcio_ll_pullup_disable(rtcio_num);
    rtcio_ll_pulldown_disable(rtcio_num);
    rtcio_ll_output_disable(rtcio_num);
    rtcio_ll_input_disable(rtcio_num);
    rtcio_ll_force_hold_enable(rtcio_num);
}

void rtcio_hal_set_direction_in_sleep(int rtcio_num, rtc_gpio_mode_t mode)
{
    switch (mode) {
    case RTC_GPIO_MODE_INPUT_ONLY:
        rtcio_ll_in_sleep_enable_input(rtcio_num);
        rtcio_ll_in_sleep_disable_output(rtcio_num);
        rtcio_ll_enable_sleep_setting(rtcio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_ONLY:
        rtcio_ll_enable_output_in_sleep(rtcio_num);
        rtcio_ll_in_sleep_disable_input(rtcio_num);
        rtcio_ll_enable_sleep_setting(rtcio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT:
        rtcio_ll_in_sleep_enable_input(rtcio_num);
        rtcio_ll_enable_output_in_sleep(rtcio_num);
        rtcio_ll_enable_sleep_setting(rtcio_num);
        break;
    case RTC_GPIO_MODE_DISABLED:
        rtcio_ll_in_sleep_disable_input(rtcio_num);
        rtcio_ll_in_sleep_disable_output(rtcio_num);
        rtcio_ll_disable_sleep_setting(rtcio_num);
        break;
    default:
        break;
    }
}

#endif
