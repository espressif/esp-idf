// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_attr.h"
#include "hal/timer_hal.h"

void timer_hal_init(timer_hal_context_t *hal, timer_group_t group_num, timer_idx_t timer_num)
{
    hal->dev = TIMER_LL_GET_HW(group_num);
    hal->idx = timer_num;
}

void timer_hal_get_status_reg_mask_bit(timer_hal_context_t *hal, uint32_t *status_reg, uint32_t *mask_bit)
{
    *status_reg = timer_ll_get_intr_status_reg(hal->dev);
    *mask_bit = timer_ll_get_intr_mask_bit(hal->dev, hal->idx);
}

void timer_hal_reset_periph(timer_hal_context_t *hal)
{
    timer_ll_intr_disable(hal->dev, hal->idx);
    timer_ll_set_counter_enable(hal->dev, hal->idx, TIMER_PAUSE);
    timer_ll_set_counter_value(hal->dev, hal->idx, 0ULL);
}
