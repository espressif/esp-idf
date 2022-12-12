/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/etm_hal.h"
#include "hal/etm_ll.h"

void etm_hal_init(etm_hal_context_t *hal)
{
    hal->regs = &SOC_ETM;
}

void etm_hal_deinit(etm_hal_context_t *hal)
{
    hal->regs = NULL;
}
