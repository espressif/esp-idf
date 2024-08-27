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
#include "driver/isp_demosaic.h"
#include "soc/isp_periph.h"
#include "esp_private/isp_private.h"

static const char *TAG = "ISP_DEMOSAIC";

/*---------------------------------------------------------------
                      Demosaic
---------------------------------------------------------------*/
esp_err_t esp_isp_demosaic_configure(isp_proc_handle_t proc, const esp_isp_demosaic_config_t *config)
{
    ESP_RETURN_ON_FALSE_ISR(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    if (config) {
        bool valid_padding_setting = (!config->padding_line_tail_valid_end_pixel && !config->padding_line_tail_valid_start_pixel) || (config->padding_line_tail_valid_end_pixel > config->padding_line_tail_valid_start_pixel);
        ESP_RETURN_ON_FALSE_ISR(valid_padding_setting, ESP_ERR_INVALID_ARG, TAG, "wrong padding line tail valid pixel setting");

        isp_hal_demosaic_cfg_t demosaic_hal_cfg = {
            .grad_ratio = config->grad_ratio,
            .padding_mode = config->padding_mode,
            .padding_data = config->padding_data,
            .padding_line_tail_valid_start_pixel = config->padding_line_tail_valid_start_pixel,
            .padding_line_tail_valid_end_pixel = config->padding_line_tail_valid_end_pixel,
        };
        isp_hal_demosaic_config(&(proc->hal), &demosaic_hal_cfg);
    } else {
        isp_hal_demosaic_config(&(proc->hal), NULL);
    }

    return ESP_OK;
}

esp_err_t esp_isp_demosaic_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->demosaic_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "demosaic is enabled already");

    isp_ll_demosaic_enable(proc->hal.hw, true);
    proc->demosaic_fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_demosaic_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->demosaic_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "demosaic isn't enabled yet");

    isp_ll_demosaic_enable(proc->hal.hw, false);
    proc->demosaic_fsm = ISP_FSM_INIT;

    return ESP_OK;
}
