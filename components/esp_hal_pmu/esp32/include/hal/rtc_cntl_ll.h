/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "esp_attr.h"
#include "soc/rtc_cntl_reg.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void rtc_cntl_ll_ext1_clear_wakeup_status(void)
{
    REG_SET_BIT(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_STATUS_CLR);
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_ext1_get_wakeup_status(void)
{
    return REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_STATUS_REG, RTC_CNTL_EXT_WAKEUP1_STATUS);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_ext1_set_wakeup_pins(uint32_t io_mask, uint32_t mode_mask)
{
    // The target only supports a unified trigger mode among all EXT1 wakeup IOs
    HAL_ASSERT((io_mask & mode_mask) == io_mask || (io_mask & mode_mask) == 0);
    REG_SET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL, io_mask);
    if ((io_mask & mode_mask) == io_mask) {
        SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
                          1, RTC_CNTL_EXT_WAKEUP1_LV_S);
    } else {
        SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
                          0, RTC_CNTL_EXT_WAKEUP1_LV_S);
    }
}

FORCE_INLINE_ATTR void rtc_cntl_ll_ext1_clear_wakeup_pins(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL_M);
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_ext1_get_wakeup_pins(void)
{
    return REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_ulp_wakeup_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_WAKEUP_FORCE_EN);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_ulp_int_clear(void)
{
    REG_SET_BIT(RTC_CNTL_INT_CLR_REG, RTC_CNTL_SAR_INT_CLR);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_timer2_set_touch_wait_cycle(uint32_t wait_cycle)
{
    REG_SET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT, wait_cycle);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_reset_system(void)
{
    REG_WRITE(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_reset_cpu(int cpu_no)
{
    uint32_t rtc_cntl_rst = (cpu_no == 0) ? RTC_CNTL_SW_PROCPU_RST : RTC_CNTL_SW_APPCPU_RST;
    REG_WRITE(RTC_CNTL_OPTIONS0_REG, rtc_cntl_rst);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_sleep_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_SLEEP_EN);
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_get_wakeup_cause(void)
{
    return REG_GET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_CAUSE);
}

#ifdef __cplusplus
}
#endif
