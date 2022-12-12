/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void rtc_cntl_ll_set_wakeup_timer(uint64_t t)
{
    // ESP32H2-TODO: IDF-6401
}

static inline uint32_t rtc_cntl_ll_gpio_get_wakeup_pins(void)
{
    return 0;
    // ESP32H2-TODO: IDF-6401
}

static inline uint32_t rtc_cntl_ll_gpio_get_wakeup_status(void)
{
    // ESP32H2-TODO: IDF-6401
    return 0;
}

static inline void rtc_cntl_ll_gpio_clear_wakeup_status(void)
{
    // ESP32H2-TODO: IDF-6401
}

static inline void rtc_cntl_ll_gpio_set_wakeup_pins(void)
{
    // ESP32H2-TODO: IDF-5718
}

static inline void rtc_cntl_ll_gpio_clear_wakeup_pins(void)
{
    // ESP32H2-TODO: IDF-5718
}

static inline void rtc_cntl_ll_set_cpu_retention_link_addr(uint32_t addr)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

static inline void rtc_cntl_ll_enable_cpu_retention_clock(void)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

static inline void rtc_cntl_ll_enable_cpu_retention(void)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

static inline void rtc_cntl_ll_disable_cpu_retention(void)
{
    // ESP32H2-TODO: IDF-5718 has removed the retention feature
}

#ifdef __cplusplus
}
#endif
