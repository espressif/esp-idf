/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);
}

static inline void rtc_cntl_ll_ext1_clear_wakeup_status(void)
{
    REG_SET_BIT(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_STATUS_CLR);
}

static inline uint32_t rtc_cntl_ll_ext1_get_wakeup_status(void)
{
    return REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_STATUS_REG, RTC_CNTL_EXT_WAKEUP1_STATUS);
}

static inline void rtc_cntl_ll_ext1_set_wakeup_pins(uint32_t mask, int mode)
{
    REG_SET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL, mask);
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
            mode, RTC_CNTL_EXT_WAKEUP1_LV_S);
}

static inline void rtc_cntl_ll_ext1_clear_wakeup_pins(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL_M);
}

static inline uint32_t rtc_cntl_ll_ext1_get_wakeup_pins(void)
{
    return REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL);
}

static inline void rtc_cntl_ll_ulp_wakeup_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN);
}

static inline void rtc_cntl_ll_ulp_int_clear(void)
{
    REG_SET_BIT(RTC_CNTL_INT_CLR_REG, RTC_CNTL_SAR_INT_CLR);
}

static inline void rtc_cntl_ll_timer2_set_touch_wait_cycle(uint32_t wait_cycle)
{
    REG_SET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT, wait_cycle);
}

static inline void rtc_cntl_ll_reset_system(void)
{
    REG_WRITE(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
}

static inline void rtc_cntl_ll_reset_cpu(int cpu_no)
{
    uint32_t rtc_cntl_rst = (cpu_no == 0) ? RTC_CNTL_SW_PROCPU_RST : RTC_CNTL_SW_APPCPU_RST;
    REG_WRITE(RTC_CNTL_OPTIONS0_REG, rtc_cntl_rst);
}

#ifdef __cplusplus
}
#endif
