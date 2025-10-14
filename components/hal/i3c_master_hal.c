/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/i3c_master_hal.h"
#include "hal/i3c_master_ll.h"
#include "hal/i3c_master_types.h"

void i3c_master_hal_init(i3c_master_hal_context_t *hal, int i3c_port)
{
    if (hal->dev == NULL) {
        hal->dev = I3C_LL_GET_HW(i3c_port);
    }
}

void i3c_master_hal_deinit(i3c_master_hal_context_t *hal)
{
    hal->dev = NULL;
}
