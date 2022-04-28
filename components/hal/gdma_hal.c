/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/gdma_hal.h"
#include "hal/gdma_ll.h"

void gdma_hal_init(gdma_hal_context_t *hal, int group_id)
{
    hal->dev = GDMA_LL_GET_HW(group_id);
}
