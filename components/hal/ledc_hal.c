/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for LEDC (common part)

#include "esp_attr.h"
#include "hal/ledc_hal.h"
#include "soc/soc_caps.h"
#include "hal/assert.h"

void ledc_hal_init(ledc_hal_context_t *hal, ledc_mode_t speed_mode)
{
    //Get hardware instance.
    hal->dev = LEDC_LL_GET_HW();
    hal->speed_mode = speed_mode;
}

static inline ledc_clk_cfg_t ledc_hal_get_slow_clock_helper(ledc_hal_context_t *hal)
{
    ledc_slow_clk_sel_t slow_clk;
    ledc_hal_get_slow_clk_sel(hal, &slow_clk);

    switch (slow_clk) {
#if SOC_LEDC_SUPPORT_APB_CLOCK
        case LEDC_SLOW_CLK_APB:
            return LEDC_USE_APB_CLK;
#endif
#if SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
        case LEDC_SLOW_CLK_PLL_DIV:
            return LEDC_USE_PLL_DIV_CLK;
#endif
        case LEDC_SLOW_CLK_RTC8M:
            return LEDC_USE_RTC8M_CLK;
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
        case LEDC_SLOW_CLK_XTAL:
            return LEDC_USE_XTAL_CLK;
#endif
        default:
            // Should never reach here
            HAL_ASSERT(false && "invalid slow clock source");
            return LEDC_AUTO_CLK;
    }
}

void ledc_hal_get_clk_cfg(ledc_hal_context_t *hal, ledc_timer_t timer_sel, ledc_clk_cfg_t *clk_cfg)
{
    /* Use the following variable to retrieve the clock source used by the LEDC
     * hardware controller. */
    ledc_clk_src_t clk_src;

    /* Clock configuration to return to the driver. */
    ledc_clk_cfg_t driver_clk = LEDC_AUTO_CLK;

    /* Get the timer-specific mux value. */
    ledc_hal_get_clock_source(hal, timer_sel, &clk_src);
#if SOC_LEDC_SUPPORT_REF_TICK
    if (clk_src == LEDC_REF_TICK) {
        driver_clk = LEDC_USE_REF_TICK;
    } else
#endif
    {
        /* If the timer-specific mux is not set to REF_TICK, it either means that:
        * - The controler is in fast mode, and thus using APB clock (driver_clk
        *   variable's default value)
        * - The controler is in slow mode and so, using a global clock,
        *   so we have to retrieve that clock here.
        */
        if (hal->speed_mode == LEDC_LOW_SPEED_MODE) {
            /* If the source clock used by LEDC hardware is not REF_TICK, it is
            * necessary to retrieve the global clock source used. */
            driver_clk = ledc_hal_get_slow_clock_helper(hal);
        }
#if SOC_LEDC_SUPPORT_HS_MODE
        else {
            driver_clk = LEDC_USE_APB_CLK;
        }
#endif
    }

    *clk_cfg = driver_clk;
}
