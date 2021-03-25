// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for Touch sensor (common part)

#include "hal/touch_sensor_hal.h"
#include "hal/touch_sensor_types.h"

void touch_hal_init(void)
{
    touch_ll_stop_fsm();
    touch_ll_intr_disable();
    touch_ll_intr_clear();
    touch_ll_clear_channel_mask(SOC_TOUCH_SENSOR_BIT_MASK_MAX);
    touch_ll_clear_group_mask(SOC_TOUCH_SENSOR_BIT_MASK_MAX, SOC_TOUCH_SENSOR_BIT_MASK_MAX);
    touch_ll_set_trigger_mode(TOUCH_TRIGGER_MODE_DEFAULT);
    touch_ll_set_trigger_source(TOUCH_TRIGGER_SOURCE_DEFAULT);
    touch_ll_clear_trigger_status_mask();
    touch_ll_set_meas_time(TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    touch_ll_set_sleep_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT);
    touch_ll_set_fsm_mode(TOUCH_FSM_MODE_DEFAULT);
    touch_ll_start_fsm();
}

void touch_hal_deinit(void)
{
    touch_ll_stop_fsm();
    touch_ll_clear_trigger_status_mask();
    touch_ll_intr_disable();
}

void touch_hal_get_wakeup_status(touch_pad_t *pad_num)
{
    uint32_t touch_mask = 0;
    touch_ll_read_trigger_status_mask(&touch_mask);
    if (touch_mask == 0) {
        *pad_num = -1;
    }
    *pad_num = (touch_pad_t)(__builtin_ffs(touch_mask) - 1);
}