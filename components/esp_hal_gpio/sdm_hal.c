/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/sdm_ll.h"
#include "hal/sdm_hal.h"

void sdm_hal_init(sdm_hal_context_t *hal, const sdm_hal_init_config_t *config)
{
    hal->dev = SDM_LL_GET_HW(config->group_id);
    // enable clock
    // note that, this will enables all the channels' output, and channel can't be disable/enable separately
    sdm_ll_enable_clock(hal->dev, true);
}

void sdm_hal_deinit(sdm_hal_context_t *hal)
{
    if (hal && hal->dev) {
        // disable clock
        sdm_ll_enable_clock(hal->dev, false);
        hal->dev = NULL;
    }
}
