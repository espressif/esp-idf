/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for LEDC (common part)

#include "hal/ledc_hal.h"
#include "esp_rom_sys.h"

void ledc_hal_init(ledc_hal_context_t *hal, ledc_mode_t speed_mode)
{
    //Get hardware instance.
    hal->dev = LEDC_LL_GET_HW();
    hal->speed_mode = speed_mode;
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
            ledc_slow_clk_sel_t slow_clk;
            ledc_hal_get_slow_clk_sel(hal, &slow_clk);
            driver_clk = (ledc_clk_cfg_t)slow_clk;
        }
#if SOC_LEDC_SUPPORT_HS_MODE
        else {
            driver_clk = LEDC_USE_APB_CLK;
        }
#endif
    }

    *clk_cfg = driver_clk;
}

#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
void ledc_hal_get_fade_param(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t range, uint32_t *dir, uint32_t *cycle, uint32_t *scale, uint32_t *step)
{
    ledc_ll_get_fade_param_range(hal->dev, hal->speed_mode, channel_num, range, dir, cycle, scale, step);
}
#endif
