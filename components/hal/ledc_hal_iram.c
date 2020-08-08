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

// The HAL layer for LEDC (common part, in iram)
// make these functions in a seperate file to make sure all LL functions are in the IRAM.

#include "esp_attr.h"
#include "hal/ledc_hal.h"

void ledc_hal_ls_channel_update(ledc_hal_context_t *hal, ledc_channel_t channel_num)
{
    ledc_ll_ls_channel_update(hal->dev, hal->speed_mode, channel_num);
}

void ledc_hal_set_hpoint(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t hpoint_val)
{
    ledc_ll_set_hpoint(hal->dev, hal->speed_mode, channel_num, hpoint_val);
}

void ledc_hal_get_duty(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t *duty_val)
{
    ledc_ll_get_duty(hal->dev, hal->speed_mode, channel_num, duty_val);
}

void ledc_hal_set_duty_direction(ledc_hal_context_t *hal, ledc_channel_t channel_num, ledc_duty_direction_t duty_direction)
{
    ledc_ll_set_duty_direction(hal->dev, hal->speed_mode, channel_num, duty_direction);
}

void ledc_hal_set_duty_num(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_num)
{
    ledc_ll_set_duty_num(hal->dev, hal->speed_mode, channel_num, duty_num);
}

void ledc_hal_set_duty_cycle(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_cycle)
{
    ledc_ll_set_duty_cycle(hal->dev, hal->speed_mode, channel_num, duty_cycle);
}

void ledc_hal_set_duty_scale(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_scale)
{
    ledc_ll_set_duty_scale(hal->dev, hal->speed_mode, channel_num, duty_scale);
}

void ledc_hal_get_fade_end_intr_status(ledc_hal_context_t *hal, uint32_t *intr_status)
{
    ledc_ll_get_fade_end_intr_status(hal->dev, hal->speed_mode, intr_status);
}

void ledc_hal_clear_fade_end_intr_status(ledc_hal_context_t *hal, ledc_channel_t channel_num)
{
    ledc_ll_clear_fade_end_intr_status(hal->dev, hal->speed_mode, channel_num);
}
