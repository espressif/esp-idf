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
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_clk_tree.h"
#include "driver/isp_core.h"
#include "driver/isp_bf.h"
#include "esp_private/isp_private.h"

static const char *TAG = "ISP_BF";

/*---------------------------------------------------------------
                      BF
---------------------------------------------------------------*/
esp_err_t esp_isp_bf_configure(isp_proc_handle_t proc, const esp_isp_bf_config_t *config)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->bf_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "bf is enabled already");

    if (config) {
        bool valid_padding_setting = (!config->padding_line_tail_valid_end_pixel && !config->padding_line_tail_valid_start_pixel) || (config->padding_line_tail_valid_end_pixel > config->padding_line_tail_valid_start_pixel);
        ESP_RETURN_ON_FALSE(valid_padding_setting, ESP_ERR_INVALID_ARG, TAG, "wrong padding line tail valid pixel setting");

        isp_hal_bf_cfg_t bf_hal_cfg = {
            .denoising_level = config->denoising_level,
            .padding_mode = config->padding_mode,
            .padding_data = config->padding_data,
            .padding_line_tail_valid_start_pixel = config->padding_line_tail_valid_start_pixel,
            .padding_line_tail_valid_end_pixel = config->padding_line_tail_valid_end_pixel,
        };
        memcpy(bf_hal_cfg.bf_template, config->bf_template, ISP_BF_TEMPLATE_X_NUMS * ISP_BF_TEMPLATE_X_NUMS * sizeof(uint8_t));
        isp_hal_bf_config(&(proc->hal), &bf_hal_cfg);
    } else {
        isp_hal_bf_config(&(proc->hal), NULL);
    }

    return ESP_OK;
}

esp_err_t esp_isp_bf_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->bf_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "bf is enabled already");

    isp_ll_bf_clk_enable(proc->hal.hw, true);
    isp_ll_bf_enable(proc->hal.hw, true);
    proc->bf_fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_bf_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->bf_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "bf isn't enabled yet");

    isp_ll_bf_enable(proc->hal.hw, false);
    isp_ll_bf_clk_enable(proc->hal.hw, false);
    proc->bf_fsm = ISP_FSM_INIT;

    return ESP_OK;
}
