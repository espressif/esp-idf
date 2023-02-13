/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/analog_cmpr_ll.h"
#include "hal/analog_cmpr_hal.h"

void analog_cmpr_hal_init(analog_cmpr_hal_context_t *hal)
{
    hal->dev = ANALOG_CMPR_LL_GET_HW();
}
