/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "hal/isp_hal.h"
#include "hal/isp_ll.h"

/**
 * ISP HAL layer
 */
void isp_hal_init(isp_hal_context_t *hal, int isp_id)
{
    //ISP hardware instance
    hal->hw = ISP_LL_GET_HW(isp_id);
    isp_ll_init(hal->hw);
}


/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
void isp_hal_af_window_config(const isp_hal_context_t *hal, int window_id, const isp_af_window_t *window)
{
    isp_ll_af_set_window_range(hal->hw, window_id, window->top_left_x, window->top_left_y, window->bottom_right_x, window->bottom_right_y);
}

/*---------------------------------------------------------------
                      INTR, put in iram
---------------------------------------------------------------*/
uint32_t isp_hal_check_clear_intr_event(const isp_hal_context_t *hal, uint32_t mask)
{
    uint32_t triggered_events = isp_ll_get_intr_status(hal->hw) & mask;

    if (triggered_events) {
        isp_ll_clear_intr(hal->hw, triggered_events);
    }

    return triggered_events;
}
