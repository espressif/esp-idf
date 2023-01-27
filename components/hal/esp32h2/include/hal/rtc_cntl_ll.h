/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/rtc.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void rtc_cntl_ll_set_wakeup_timer(uint64_t t)
{
    // ESP32H2-TODO: IDF-6401
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_gpio_get_wakeup_pins(void)
{
    return 0;
    // ESP32H2-TODO: IDF-6401
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_gpio_get_wakeup_status(void)
{
    // ESP32H2-TODO: IDF-6401
    return 0;
}

FORCE_INLINE_ATTR void rtc_cntl_ll_gpio_clear_wakeup_status(void)
{
    // ESP32H2-TODO: IDF-6401
}

FORCE_INLINE_ATTR void rtc_cntl_ll_gpio_set_wakeup_pins(void)
{
    // ESP32H2-TODO: IDF-5718
}

FORCE_INLINE_ATTR void rtc_cntl_ll_gpio_clear_wakeup_pins(void)
{
    // ESP32H2-TODO: IDF-5718
}

FORCE_INLINE_ATTR void rtc_cntl_ll_set_cpu_retention_link_addr(uint32_t addr)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_enable_cpu_retention_clock(void)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_enable_cpu_retention(void)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_disable_cpu_retention(void)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_sleep_enable(void)
{
    // TODO: IDF-6572
}

FORCE_INLINE_ATTR uint64_t rtc_cntl_ll_get_rtc_time(void)
{
    // TODO: IDF-6572
    return 0;
}

FORCE_INLINE_ATTR uint64_t rtc_cntl_ll_time_to_count(uint64_t time_in_us)
{
    // TODO: IDF-6572
    return 0;
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_get_wakeup_cause(void)
{
    // TODO: IDF-6572
    return 0;
}

#ifdef __cplusplus
}
#endif
