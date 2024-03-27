/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for LEDC (common part, in iram)
// make these functions in a seperate file to make sure all LL functions are in the IRAM.

#include "hal/ledc_hal.h"
#include "hal/assert.h"
#include "esp_rom_sys.h"

void ledc_hal_ls_channel_update(ledc_hal_context_t *hal, ledc_channel_t channel_num)
{
    ledc_ll_ls_channel_update(hal->dev, hal->speed_mode, channel_num);
}

void ledc_hal_set_duty_start(ledc_hal_context_t *hal, ledc_channel_t channel_num, bool duty_start)
{
    ledc_ll_set_duty_start(hal->dev, hal->speed_mode, channel_num, duty_start);
}

void ledc_hal_set_duty_int_part(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_val)
{
    ledc_ll_set_duty_int_part(hal->dev, hal->speed_mode, channel_num, duty_val);
}

void ledc_hal_set_hpoint(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t hpoint_val)
{
    ledc_ll_set_hpoint(hal->dev, hal->speed_mode, channel_num, hpoint_val);
}

void ledc_hal_get_duty(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t *duty_val)
{
    ledc_ll_get_duty(hal->dev, hal->speed_mode, channel_num, duty_val);
}

void ledc_hal_set_fade_param(const ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t range, uint32_t dir, uint32_t cycle, uint32_t scale, uint32_t step)
{
#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
    HAL_ASSERT(range < SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX);
    ledc_ll_set_fade_param_range(hal->dev, hal->speed_mode, channel_num, range, dir, cycle, scale, step);
#else // !SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
    HAL_ASSERT(range == 0);
    ledc_ll_set_fade_param(hal->dev, hal->speed_mode, channel_num, dir, cycle, scale, step);
#endif // SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
}

#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED

void ledc_hal_set_range_number(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t range_num)
{
    ledc_ll_set_range_number(hal->dev, hal->speed_mode, channel_num, range_num);
}

void ledc_hal_get_range_number(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t *range_num)
{
    ledc_ll_get_range_number(hal->dev, hal->speed_mode, channel_num, range_num);
}
#endif //SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED

void ledc_hal_get_fade_end_intr_status(ledc_hal_context_t *hal, uint32_t *intr_status)
{
    ledc_ll_get_fade_end_intr_status(hal->dev, hal->speed_mode, intr_status);
}

void ledc_hal_clear_fade_end_intr_status(ledc_hal_context_t *hal, ledc_channel_t channel_num)
{
    ledc_ll_clear_fade_end_intr_status(hal->dev, hal->speed_mode, channel_num);
}
