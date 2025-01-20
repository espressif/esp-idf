/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "driver/isp_core.h"
#include "driver/isp_sharpen.h"
#include "soc/isp_periph.h"
#include "esp_private/isp_private.h"

static const char *TAG = "ISP_SHARPEN";

/*---------------------------------------------------------------
                      Sharpen
---------------------------------------------------------------*/
esp_err_t esp_isp_sharpen_configure(isp_proc_handle_t proc, const esp_isp_sharpen_config_t *config)
{
    ESP_RETURN_ON_FALSE_ISR(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    if (config) {
        bool valid_padding_setting = (!config->padding_line_tail_valid_end_pixel && !config->padding_line_tail_valid_start_pixel) || (config->padding_line_tail_valid_end_pixel > config->padding_line_tail_valid_start_pixel);
        ESP_RETURN_ON_FALSE_ISR(valid_padding_setting, ESP_ERR_INVALID_ARG, TAG, "wrong padding line tail valid pixel setting");

        isp_hal_sharpen_cfg_t sharpen_hal_cfg = {
            .h_freq_coeff = config->h_freq_coeff,
            .m_freq_coeff = config->m_freq_coeff,
            .h_thresh = config->h_thresh,
            .l_thresh = config->l_thresh,
            .padding_mode = config->padding_mode,
            .padding_data = config->padding_data,
            .padding_line_tail_valid_start_pixel = config->padding_line_tail_valid_start_pixel,
            .padding_line_tail_valid_end_pixel = config->padding_line_tail_valid_end_pixel,
        };
        memcpy(sharpen_hal_cfg.sharpen_template, config->sharpen_template, ISP_SHARPEN_TEMPLATE_X_NUMS * ISP_SHARPEN_TEMPLATE_X_NUMS * sizeof(uint8_t));
        isp_hal_sharpen_config(&(proc->hal), &sharpen_hal_cfg);
    } else {
        isp_hal_sharpen_config(&(proc->hal), NULL);
    }

    return ESP_OK;
}

esp_err_t esp_isp_sharpen_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->sharpen_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "sharpen is enabled already");

    isp_ll_sharp_clk_enable(proc->hal.hw, true);
    isp_ll_enable_intr(proc->hal.hw, ISP_LL_EVENT_SHARP_FRAME, true);
    isp_ll_sharp_enable(proc->hal.hw, true);
    proc->sharpen_fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_sharpen_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->sharpen_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "sharpen isn't enabled yet");

    isp_ll_sharp_enable(proc->hal.hw, false);
    isp_ll_enable_intr(proc->hal.hw, ISP_LL_EVENT_SHARP_FRAME, false);
    isp_ll_sharp_clk_enable(proc->hal.hw, false);
    proc->sharpen_fsm = ISP_FSM_INIT;

    return ESP_OK;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
bool IRAM_ATTR esp_isp_sharpen_isr(isp_proc_handle_t proc, uint32_t sharp_events)
{
    bool need_yield = false;
    esp_isp_sharpen_evt_data_t edata = {};

    if (sharp_events) {
        edata.high_freq_pixel_max = isp_ll_sharp_get_high_freq_pixel_max(proc->hal.hw);
    }

    if (sharp_events & ISP_LL_EVENT_SHARP_FRAME) {
        if (proc->cbs.on_sharpen_frame_done) {
            need_yield |= proc->cbs.on_sharpen_frame_done(proc, &edata, proc->user_data);
        }
    }

    return need_yield;
}
