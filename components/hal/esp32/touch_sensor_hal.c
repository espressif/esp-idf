/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for Touch sensor (common part)

#include "hal/touch_sensor_hal.h"
#include "hal/touch_sensor_types.h"

void touch_hal_init(void)
{
    touch_ll_stop_fsm();
    touch_ll_intr_disable();
    touch_ll_intr_clear();
    touch_ll_clear_channel_mask(TOUCH_PAD_BIT_MASK_ALL);
    touch_ll_clear_group_mask(TOUCH_PAD_BIT_MASK_ALL, TOUCH_PAD_BIT_MASK_ALL);
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
    } else {
        *pad_num = (touch_pad_t)(__builtin_ffs(touch_mask) - 1);
    }
}
