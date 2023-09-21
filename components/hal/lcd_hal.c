/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "hal/log.h"
#include "hal/hal_utils.h"

void lcd_hal_init(lcd_hal_context_t *hal, int id)
{
    hal->dev = LCD_LL_GET_HW(id);
}

uint32_t lcd_hal_cal_pclk_freq(lcd_hal_context_t *hal, uint32_t src_freq_hz, uint32_t expect_pclk_freq_hz, int lcd_clk_flags)
{
    // lcd_clk = module_clock_src / (n + b / a)
    // pixel_clk = lcd_clk / mo
    uint32_t mo = src_freq_hz / expect_pclk_freq_hz / LCD_LL_CLK_FRAC_DIV_N_MAX + 1;
    if (mo == 1 && !(lcd_clk_flags & LCD_HAL_PCLK_FLAG_ALLOW_EQUAL_SYSCLK)) {
        mo = 2;
    }
    hal_utils_clk_info_t lcd_clk_info = {
        .src_freq_hz = src_freq_hz,
        .exp_freq_hz = expect_pclk_freq_hz * mo,
        .max_integ = LCD_LL_CLK_FRAC_DIV_N_MAX,
        .min_integ = 2,
        .max_fract = LCD_LL_CLK_FRAC_DIV_AB_MAX,
    };
    hal_utils_clk_div_t lcd_clk_div = {};
    uint32_t real_freq = hal_utils_calc_clk_div_frac_fast(&lcd_clk_info, &lcd_clk_div);
    HAL_EARLY_LOGD("lcd_hal", "n=%"PRIu32",a=%"PRIu32",b=%"PRIu32",mo=%"PRIu32, lcd_clk_div.integer, lcd_clk_div.denominator, lcd_clk_div.numerator, mo);

    lcd_ll_set_group_clock_coeff(hal->dev, lcd_clk_div.integer, lcd_clk_div.denominator, lcd_clk_div.numerator);
    lcd_ll_set_pixel_clock_prescale(hal->dev, mo);

    return real_freq / mo;
}
