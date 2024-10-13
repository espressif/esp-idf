/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for LP I2S

#include "soc/soc.h"
#include "hal/lp_i2s_hal.h"
#include "hal/lp_i2s_ll.h"

void lp_i2s_hal_init(lp_i2s_hal_context_t *hal, int group_id)
{
    hal->dev = LP_I2S_LL_GET_HW(group_id);
}
