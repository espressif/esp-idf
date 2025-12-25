/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/clk_tree_ll.h"
#include "esp_rom_sys.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void rtc_timer_ll_set_wakeup_timer(uint64_t t)
{
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);
}

FORCE_INLINE_ATTR uint64_t rtc_timer_ll_get_rtc_time(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_UPDATE);
    int attempts = 1000;
    while (GET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_VALID) == 0) {
        esp_rom_delay_us(1);
        if (attempts) {
            if (--attempts == 0 && clk_ll_xtal32k_digi_is_enabled()) {
                esp_rom_printf("32KHz xtal has been stopped\n");
            }
        }
    }
    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG, RTC_CNTL_TIME_VALID_INT_CLR);
    uint64_t t = READ_PERI_REG(RTC_CNTL_TIME0_REG);
    t |= ((uint64_t) READ_PERI_REG(RTC_CNTL_TIME1_REG)) << 32;
    return t;
}

#ifdef __cplusplus
}
#endif
