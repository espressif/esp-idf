/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/ppa_hal.h"
#include "hal/ppa_ll.h"

void ppa_hal_init(ppa_hal_context_t *hal)
{
    hal->dev = PPA_LL_GET_HW;
}

void ppa_hal_deinit(ppa_hal_context_t *hal)
{
    hal->dev = NULL;
}
