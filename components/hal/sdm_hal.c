/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for sigma delta modulator (common part)

#include "hal/sdm_ll.h"
#include "hal/sdm_hal.h"

void sdm_hal_init(sdm_hal_context_t *hal, int group_id)
{
    (void) group_id;
    hal->dev = &SDM;
}
