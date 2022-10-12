/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/lp_aon_reg.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void rtc_cntl_ll_set_wakeup_timer(uint64_t t)
{
    // TODO: IDF-5645
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_gpio_get_wakeup_status(void)
{
    // TODO: IDF-5645
    return 0;
}

FORCE_INLINE_ATTR void rtc_cntl_ll_gpio_clear_wakeup_status(void)
{
    // TODO: IDF-5645
}

FORCE_INLINE_ATTR void rtc_cntl_ll_set_cpu_retention_link_addr(uint32_t addr)
{
    // TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_enable_cpu_retention_clock(void)
{
    // TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_enable_cpu_retention(void)
{
    // TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_disable_cpu_retention(void)
{
    // TODO: IDF-5718 has removed the retention feature
}

FORCE_INLINE_ATTR void rtc_cntl_ll_reset_system(void)
{
    REG_SET_BIT(LP_AON_SYS_CFG_REG, LP_AON_HPSYS_SW_RESET);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_reset_cpu(int cpu_no)
{
    REG_SET_BIT(LP_AON_CPUCORE0_CFG_REG, LP_AON_CPU_CORE0_SW_RESET);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_sleep_enable(void)
{
    // TODO: IDF-6064
}

FORCE_INLINE_ATTR uint64_t rtc_cntl_ll_get_rtc_time(void)
{
    // TODO: IDF-6064
    return 0;
}

FORCE_INLINE_ATTR uint64_t rtc_cntl_ll_time_to_count(uint64_t time_in_us)
{
    // TODO: IDF-6064
    return 0;
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_get_wakeup_cause(void)
{
    // TODO: IDF-6064
    return 0;
}

#ifdef __cplusplus
}
#endif
