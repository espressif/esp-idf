/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for PCNT (common part)

#include "hal/pcnt_hal.h"
#include "hal/pcnt_ll.h"

void pcnt_hal_init(pcnt_hal_context_t *hal, int pcnt_num)
{
    //Get hardware instance.
    hal->dev = PCNT_LL_GET_HW(pcnt_num);
}
