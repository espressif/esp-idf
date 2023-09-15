/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for RTC IO (common part)

#include "hal/rtc_io_hal.h"
#include "soc/soc_caps.h"

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

void rtcio_hal_set_direction(int rtcio_num, rtc_gpio_mode_t mode)
{
    switch (mode) {
    case RTC_GPIO_MODE_INPUT_ONLY:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_LL_OUTPUT_NORMAL);
        rtcio_ll_output_disable(rtcio_num);
        rtcio_ll_input_enable(rtcio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_ONLY:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_LL_OUTPUT_NORMAL);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_disable(rtcio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_LL_OUTPUT_NORMAL);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_enable(rtcio_num);
        break;
    case RTC_GPIO_MODE_DISABLED:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_LL_OUTPUT_NORMAL);
        rtcio_ll_output_disable(rtcio_num);
        rtcio_ll_input_disable(rtcio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_OD:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_LL_OUTPUT_OD);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_disable(rtcio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT_OD:
        rtcio_ll_output_mode_set(rtcio_num, RTCIO_LL_OUTPUT_OD);
        rtcio_ll_output_enable(rtcio_num);
        rtcio_ll_input_enable(rtcio_num);
        break;
    default:
        break;
    }
}

void rtcio_hal_set_direction_in_sleep(int rtcio_num, rtc_gpio_mode_t mode)
{
    switch (mode) {
    case RTC_GPIO_MODE_INPUT_ONLY:
        rtcio_ll_enable_input_in_sleep(rtcio_num);
        rtcio_ll_disable_output_in_sleep(rtcio_num);
        rtcio_ll_enable_sleep_setting(rtcio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_ONLY:
        rtcio_ll_enable_output_in_sleep(rtcio_num);
        rtcio_ll_disable_input_in_sleep(rtcio_num);
        rtcio_ll_enable_sleep_setting(rtcio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT:
        rtcio_ll_enable_input_in_sleep(rtcio_num);
        rtcio_ll_enable_output_in_sleep(rtcio_num);
        rtcio_ll_enable_sleep_setting(rtcio_num);
        break;
    case RTC_GPIO_MODE_DISABLED:
        rtcio_ll_disable_input_in_sleep(rtcio_num);
        rtcio_ll_disable_output_in_sleep(rtcio_num);
        rtcio_ll_disable_sleep_setting(rtcio_num);
        break;
    default:
        break;
    }
}

#if SOC_RTCIO_HOLD_SUPPORTED
void rtcio_hal_isolate(int rtcio_num)
{
    rtcio_ll_pullup_disable(rtcio_num);
    rtcio_ll_pulldown_disable(rtcio_num);
    rtcio_ll_output_disable(rtcio_num);
    rtcio_ll_input_disable(rtcio_num);
    rtcio_ll_force_hold_enable(rtcio_num);
}
#endif

#endif //SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
