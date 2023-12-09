/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stddef.h>
#include "hal/parlio_hal.h"
#include "hal/parlio_ll.h"

void parlio_hal_init(parlio_hal_context_t *hal)
{
    hal->regs = &PARL_IO;
}

void parlio_hal_deinit(parlio_hal_context_t *hal)
{
    hal->regs = NULL;
}
