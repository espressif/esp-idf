/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
    /* write memory address to register */
    REG_SET_FIELD(SYSCON_RETENTION_CTRL_REG, SYSCON_RETENTION_LINK_ADDR, (uint32_t)addr);
    /* Enable clock */
    REG_SET_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_CLK8M_EN);
    /* Enable retention when cpu sleep enable */
    REG_SET_BIT(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_EN);
}

static inline void rtc_cntl_ll_disable_cpu_retention(void)
{
    REG_CLR_BIT(RTC_CNTL_RETENTION_CTRL_REG, RTC_CNTL_RETENTION_EN);
}

#ifdef __cplusplus
}
#endif
