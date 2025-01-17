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
#include "driver/isp_color.h"
#include "soc/isp_periph.h"
#include "esp_private/isp_private.h"

static const char *TAG = "ISP_COLOR";

/*---------------------------------------------------------------
                      Color
---------------------------------------------------------------*/

esp_err_t esp_isp_color_configure(isp_proc_handle_t proc, const esp_isp_color_config_t *config)
{
    ESP_RETURN_ON_FALSE_ISR(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    if (config) {
        ESP_RETURN_ON_FALSE_ISR(((config->color_contrast.val <= ISP_LL_COLOR_CONTRAST_MAX) &&
                                 (config->color_saturation.val <= ISP_LL_COLOR_SATURATION_MAX) &&
                                 (config->color_hue <= ISP_LL_COLOR_HUE_MAX) &&
                                 (config->color_brightness >= ISP_LL_COLOR_BRIGNTNESS_MIN) &&
                                 (config->color_brightness <= ISP_LL_COLOR_BRIGNTNESS_MAX)), ESP_ERR_INVALID_ARG, TAG, "invalid color config");
        isp_hal_color_cfg_t color_hal_cfg = {
            .color_contrast = config->color_contrast,
            .color_saturation = config->color_saturation,
            .color_hue =  config->color_hue,
            .color_brightness = config->color_brightness,
        };
        isp_hal_color_config(&(proc->hal), &color_hal_cfg);
    } else {
        isp_hal_color_config(&(proc->hal), NULL);
    }

    return ESP_OK;
}

esp_err_t esp_isp_color_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->color_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "color is enabled already");

    isp_ll_color_clk_enable(proc->hal.hw, true);
    isp_ll_color_enable(proc->hal.hw, true);
    proc->color_fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_color_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->color_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "color isn't enabled yet");

    isp_ll_color_enable(proc->hal.hw, false);
    isp_ll_color_clk_enable(proc->hal.hw, false);
    proc->color_fsm = ISP_FSM_INIT;

    return ESP_OK;
}
