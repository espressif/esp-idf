/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/rtc.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "soc/rtc_cntl_reg.h"


#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void rtc_cntl_ll_set_wakeup_timer(uint64_t t)
{
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_ext1_clear_wakeup_status(void)
{
    REG_SET_BIT(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_STATUS_CLR);
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_ext1_get_wakeup_status(void)
{
    return REG_GET_FIELD(RTC_CNTL_EXT_WAKEUP1_STATUS_REG, RTC_CNTL_EXT_WAKEUP1_STATUS);
}

FORCE_INLINE_ATTR void rtc_cntl_ll_ext1_set_wakeup_pins(uint32_t mask, int mode)
{
    REG_SET_FIELD(RTC_CNTL_EXT_WAKEUP1_REG, RTC_CNTL_EXT_WAKEUP1_SEL, mask);
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
            mode, RTC_CNTL_EXT_WAKEUP1_LV_S);
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

FORCE_INLINE_ATTR void rtc_cntl_ll_sleep_enable(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_SLEEP_EN);
}

FORCE_INLINE_ATTR uint64_t rtc_cntl_ll_get_rtc_time(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_UPDATE);
    int attempts = 1000;
    while (GET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_VALID) == 0) {
        esp_rom_delay_us(1);
        if (attempts) {
            if (--attempts == 0 && REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_DIG_XTAL32K_EN)) {
                esp_rom_printf("32KHz xtal has been stopped\n");
            }
        }
    }
    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG, RTC_CNTL_TIME_VALID_INT_CLR);
    uint64_t t = READ_PERI_REG(RTC_CNTL_TIME0_REG);
    t |= ((uint64_t) READ_PERI_REG(RTC_CNTL_TIME1_REG)) << 32;
    return t;
}

FORCE_INLINE_ATTR uint64_t rtc_cntl_ll_time_to_count(uint64_t time_in_us)
{
    uint32_t slow_clk_value = REG_READ(RTC_CNTL_STORE1_REG);
    return ((time_in_us * (1 << RTC_CLK_CAL_FRACT)) / slow_clk_value);
}

FORCE_INLINE_ATTR uint32_t rtc_cntl_ll_get_wakeup_cause(void)
{
    return REG_GET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_CAUSE);
}

#ifdef __cplusplus
}
#endif
