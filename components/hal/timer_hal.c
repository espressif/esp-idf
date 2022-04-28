/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/timer_hal.h"
#include "hal/timer_ll.h"

void timer_hal_init(timer_hal_context_t *hal, uint32_t group_num, uint32_t timer_num)
{
    hal->dev = TIMER_LL_GET_HW(group_num);
    hal->timer_id = timer_num;
}
