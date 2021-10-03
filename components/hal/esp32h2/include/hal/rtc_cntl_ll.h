// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/syscon_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void rtc_cntl_ll_set_wakeup_timer(uint64_t t)
{
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);

    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG, RTC_CNTL_MAIN_TIMER_INT_CLR_M);
    SET_PERI_REG_MASK(RTC_CNTL_SLP_TIMER1_REG, RTC_CNTL_MAIN_TIMER_ALARM_EN_M);
}

static inline uint32_t rtc_cntl_ll_gpio_get_wakeup_pins(void)
{
    return GET_PERI_REG_MASK(RTC_CNTL_GPIO_WAKEUP_REG, RTC_CNTL_GPIO_WAKEUP_STATUS);
}

static inline void rtc_cntl_ll_gpio_set_wakeup_pins(void)
{
    REG_CLR_BIT(RTC_CNTL_GPIO_WAKEUP_REG, RTC_CNTL_GPIO_WAKEUP_STATUS_CLR);
}

static inline void rtc_cntl_ll_gpio_clear_wakeup_pins(void)
{
    REG_SET_BIT(RTC_CNTL_GPIO_WAKEUP_REG, RTC_CNTL_GPIO_WAKEUP_STATUS_CLR);
}

static inline void rtc_cntl_ll_enable_cpu_retention(uint32_t addr)
{
    // ESP32H2-TODO: IDF-3383
}

static inline void rtc_cntl_ll_disable_cpu_retention(void)
{
    // ESP32H2-TODO: IDF-3383
}

#ifdef __cplusplus
}
#endif
