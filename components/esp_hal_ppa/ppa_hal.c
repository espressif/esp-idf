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
    ppa_ll_mem_power_by_pmu(hal->dev);
    ppa_ll_mem_set_low_power_mode(hal->dev, PPA_LL_MEM_LP_MODE_SHUT_DOWN);
}

void ppa_hal_deinit(ppa_hal_context_t *hal)
{
    hal->dev = NULL;
}
