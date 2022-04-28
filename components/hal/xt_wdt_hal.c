/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "soc/soc_caps.h"

#include "hal/xt_wdt_hal.h"
#include "hal/xt_wdt_ll.h"
#include "hal/assert.h"

#define DIV_COMP_N_MAX 8

static uint32_t xt_wdt_hal_calculate(uint32_t rtc_clk_frequency_khz)
{
    uint32_t xtal32k_clk_factor = 0;
    uint8_t divisor_comps[DIV_COMP_N_MAX];

    /*  From the TRM:

        Define the frequency of RTC_CLK as f_rtc_clk (unit: kHz), and the eight divisor components as
        x0, x1, x2, x3, x4, x5, x6, and x7, respectively. S = x0 + x1 + x2 + x3 + x4 + x5 + x6 + x7.
        The following conditions should be fulfilled:
        S = f_rtc_clk * (4/32)
        M + 1 >= xn >= M(0 <= n <= 7)
        M = f_rtc_clk/32/2
        xn should be an integer. M and S are rounded up or down. Each divisor component (x0 ~x7) is 4-bit long, and
        corresponds to the value of RTC_CNTL_XTAL32K_CLK_FACTOR (32-bit) in order.
    */

    uint8_t M = ((rtc_clk_frequency_khz / 32) / 2);
    uint32_t S = ((4 * rtc_clk_frequency_khz) / 32);

    memset(divisor_comps, M, DIV_COMP_N_MAX);

    /* Calculate how far we are away from satisfying S = SUM(x_n) */
    uint8_t off = S - DIV_COMP_N_MAX * M;

    /* Offset should never be this big */
    HAL_ASSERT(off <= DIV_COMP_N_MAX);

    for (int i = 0; i < DIV_COMP_N_MAX; i++) {
        if (off) {
            divisor_comps[i]++;
            off--;
        }
        /* Sum up all divisors */
        xtal32k_clk_factor |=  (divisor_comps[i] << 4 * i);
    }

    return xtal32k_clk_factor;
}

void xt_wdt_hal_init(xt_wdt_hal_context_t *hal, const xt_wdt_hal_config_t *config)
{
    hal->dev = &RTCCNTL;

    xt_wdt_ll_enable(hal->dev, false);
    xt_wdt_ll_set_timeout(hal->dev, config->timeout);
}

uint32_t xt_wdt_hal_enable_backup_clk(xt_wdt_hal_context_t *hal, uint32_t rtc_clk_frequency_khz)
{
    uint32_t xtal32k_clk_factor = xt_wdt_hal_calculate(rtc_clk_frequency_khz);

    xt_wdt_ll_set_backup_clk_factor(hal->dev, xtal32k_clk_factor);
    xt_wdt_ll_auto_backup_enable(hal->dev, true);

    return xtal32k_clk_factor;
}

void xt_wdt_hal_enable(xt_wdt_hal_context_t *hal, bool enable)
{
    xt_wdt_ll_enable(hal->dev, enable);
    xt_wdt_ll_intr_enable(hal->dev, enable);
}
