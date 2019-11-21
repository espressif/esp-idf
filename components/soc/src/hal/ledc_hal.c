// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The HAL layer for LEDC (common part)

#include "esp_attr.h"
#include "hal/ledc_hal.h"

void ledc_hal_init(ledc_hal_context_t *hal, ledc_mode_t speed_mode)
{
    //Get hardware instance.
    hal->dev = LEDC_LL_GET_HW();
    hal->speed_mode = speed_mode;
}

void ledc_hal_get_clk_cfg(ledc_hal_context_t *hal, ledc_timer_t timer_sel, ledc_clk_cfg_t *clk_cfg)
{
    ledc_clk_src_t clk_src = LEDC_APB_CLK;
    ledc_hal_get_clock_source(hal, timer_sel, &clk_src);
    if (clk_src == LEDC_REF_TICK) {
        *clk_cfg = LEDC_USE_REF_TICK;
    } else {
        *clk_cfg = LEDC_USE_APB_CLK;
        if (hal->speed_mode == LEDC_LOW_SPEED_MODE) {
            ledc_slow_clk_sel_t slow_clk = LEDC_SLOW_CLK_APB;
            ledc_hal_get_slow_clk_sel(hal, &slow_clk);
            if (slow_clk == LEDC_SLOW_CLK_RTC8M) {
                *clk_cfg = LEDC_USE_RTC8M_CLK;
#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
            } else if (slow_clk == LEDC_SLOW_CLK_XTAL) {
                *clk_cfg = LEDC_USE_XTAL_CLK;
#endif
            }
        }
    }
}

void ledc_hal_set_slow_clk(ledc_hal_context_t *hal, ledc_clk_cfg_t clk_cfg)
{
    // For low speed channels, if RTC_8MCLK is used as the source clock, the `slow_clk_sel` register should be cleared, otherwise it should be set.
    ledc_slow_clk_sel_t slow_clk_sel = LEDC_SLOW_CLK_APB;
#ifdef CONFIG_IDF_TARGET_ESP32
    slow_clk_sel = (clk_cfg == LEDC_USE_RTC8M_CLK) ? LEDC_SLOW_CLK_RTC8M : LEDC_SLOW_CLK_APB;
#elif defined CONFIG_IDF_TARGET_ESP32S2BETA
    slow_clk_sel = (clk_cfg == LEDC_USE_RTC8M_CLK) ? LEDC_SLOW_CLK_RTC8M :
                                       ((clk_cfg == LEDC_USE_XTAL_CLK) ? LEDC_SLOW_CLK_XTAL : LEDC_SLOW_CLK_APB);
#endif
    ledc_hal_set_slow_clk_sel(hal, slow_clk_sel);
}
