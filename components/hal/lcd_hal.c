/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "hal/log.h"

void lcd_hal_init(lcd_hal_context_t *hal, int id)
{
    hal->dev = LCD_LL_GET_HW(id);
}

/**
 * @brief helper function, calculate the Greatest Common Divisor
 * @note gcd(a, b) = gcd(b, a % b)
 * @param a bigger value
 * @param b smaller value
 * @return result of gcd(a, b)
 */
__attribute__((always_inline))
static inline uint32_t _gcd(uint32_t a, uint32_t b)
{
    uint32_t c = a % b;
    while (c != 0) {
        a = b;
        b = c;
        c = a % b;
    }
    return b;
}

uint32_t lcd_hal_cal_pclk_freq(lcd_hal_context_t *hal, uint32_t src_freq_hz, uint32_t expect_pclk_freq_hz, int lcd_clk_flags)
{
    // lcd_clk = module_clock_src / (n + b / a)
    // pixel_clk = lcd_clk / mo
    uint32_t mo = src_freq_hz / expect_pclk_freq_hz / LCD_LL_CLK_FRAC_DIV_N_MAX + 1;
    if (mo == 1 && !(lcd_clk_flags & LCD_HAL_PCLK_FLAG_ALLOW_EQUAL_SYSCLK)) {
        mo = 2;
    }
    uint32_t n = src_freq_hz / expect_pclk_freq_hz / mo;
    uint32_t a = 0;
    uint32_t b = 0;
    // delta_hz / expect_pclk_freq_hz <==> b / a
    uint32_t delta_hz = src_freq_hz / mo - expect_pclk_freq_hz * n;
    // fractional divider
    if (delta_hz) {
        uint32_t gcd = _gcd(expect_pclk_freq_hz, delta_hz);
        a = expect_pclk_freq_hz / gcd;
        b = delta_hz / gcd;
        // normalize div_a and div_b
        uint32_t d = a / LCD_LL_CLK_FRAC_DIV_AB_MAX + 1;
        a /= d;
        b /= d;
    }

    HAL_EARLY_LOGD("lcd_hal", "n=%d,a=%d,b=%d,mo=%d", n, a, b, mo);

    lcd_ll_set_group_clock_coeff(hal->dev, n, a, b);
    lcd_ll_set_pixel_clock_prescale(hal->dev, mo);

    if (delta_hz) {
        return ((uint64_t)src_freq_hz * a) / (n * a + b) / mo;
    } else {
        return src_freq_hz / n / mo;
    }
}
