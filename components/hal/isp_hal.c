/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <string.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "hal/isp_hal.h"
#include "hal/isp_ll.h"
#include "hal/isp_types.h"

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
                      BF
---------------------------------------------------------------*/
void isp_hal_bf_config(isp_hal_context_t *hal, isp_hal_bf_cfg_t *config)
{
    if (config) {
        isp_ll_bf_set_sigma(hal->hw, config->denoising_level);
        isp_ll_bf_set_padding_mode(hal->hw, config->padding_mode);
        isp_ll_bf_set_padding_data(hal->hw, config->padding_data);
        isp_ll_bf_set_padding_line_tail_valid_start_pixel(hal->hw, config->padding_line_tail_valid_start_pixel);
        isp_ll_bf_set_padding_line_tail_valid_end_pixel(hal->hw, config->padding_line_tail_valid_end_pixel);
        isp_ll_bf_set_template(hal->hw, config->bf_template);
    } else {
        isp_ll_bf_set_sigma(hal->hw, 0);
        isp_ll_bf_set_padding_mode(hal->hw, 0);
        isp_ll_bf_set_padding_data(hal->hw, 0);
        isp_ll_bf_set_padding_line_tail_valid_start_pixel(hal->hw, 0);
        isp_ll_bf_set_padding_line_tail_valid_end_pixel(hal->hw, 0);
        uint8_t default_template[SOC_ISP_BF_TEMPLATE_X_NUMS][SOC_ISP_BF_TEMPLATE_Y_NUMS] = {};
        memset(default_template, SOC_ISP_BF_TEMPLATE_X_NUMS, sizeof(default_template));
        isp_ll_bf_set_template(hal->hw, default_template);
    }
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
