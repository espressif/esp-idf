/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/isp_core.h"
#include "driver/isp_crop.h"
#include "esp_private/isp_private.h"
#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"

/*---------------------------------------------------------------
                      Crop
---------------------------------------------------------------*/

#define IS_EVEN(x) ((x) % 2 == 0)
#define IS_ODD(x) ((x) % 2 != 0)

static const char *TAG = "ISP_CROP";

esp_err_t esp_isp_crop_configure(isp_proc_handle_t isp_proc, const esp_isp_crop_config_t *config)
{
#if CONFIG_IDF_TARGET_ESP32P4
    unsigned chip_version = efuse_hal_chip_revision();
    if (!ESP_CHIP_REV_ABOVE(chip_version, 300)) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "Crop is not supported on ESP32P4 chips prior than v3.0");
    }
#endif
    uint32_t x_start = config->window.top_left.x;
    uint32_t x_end = config->window.btm_right.x;
    uint32_t y_start = config->window.top_left.y;
    uint32_t y_end = config->window.btm_right.y;

    ESP_RETURN_ON_FALSE(isp_proc && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(isp_proc->crop_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "crop is enabled already");
    ESP_RETURN_ON_FALSE(x_start < x_end, ESP_ERR_INVALID_ARG, TAG, "invalid window x coordinates");
    ESP_RETURN_ON_FALSE(y_start < y_end, ESP_ERR_INVALID_ARG, TAG, "invalid window y coordinates");
    ESP_RETURN_ON_FALSE(x_end <= isp_proc->h_res, ESP_ERR_INVALID_ARG, TAG, "window exceeds horizontal resolution");
    ESP_RETURN_ON_FALSE(y_end <= isp_proc->v_res, ESP_ERR_INVALID_ARG, TAG, "window exceeds vertical resolution");

    // Check the validity of x/y coordinates
    ESP_RETURN_ON_FALSE(IS_EVEN(x_start), ESP_ERR_INVALID_ARG, TAG, "x_start must be even");
    ESP_RETURN_ON_FALSE(IS_ODD(x_end), ESP_ERR_INVALID_ARG, TAG, "x_end must be odd");
    ESP_RETURN_ON_FALSE(IS_EVEN(y_start), ESP_ERR_INVALID_ARG, TAG, "y_start must be even");
    ESP_RETURN_ON_FALSE(IS_ODD(y_end), ESP_ERR_INVALID_ARG, TAG, "y_end must be odd");

    isp_ll_crop_set_clk_ctrl_mode(isp_proc->hal.hw, ISP_LL_PIPELINE_CLK_CTRL_AUTO);
    isp_ll_crop_set_window(isp_proc->hal.hw, x_start, x_end, y_start, y_end);

    return ESP_OK;
}

esp_err_t esp_isp_crop_enable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(isp_proc->crop_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "crop is enabled already");

    isp_ll_crop_enable(isp_proc->hal.hw, true);
    isp_proc->crop_fsm = ISP_FSM_ENABLE;

    ESP_LOGD(TAG, "Crop enabled");
    return ESP_OK;
}

esp_err_t esp_isp_crop_disable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(isp_proc->crop_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "crop isn't enabled yet");

    isp_ll_crop_enable(isp_proc->hal.hw, false);
    isp_proc->crop_fsm = ISP_FSM_INIT;

    ESP_LOGD(TAG, "Crop disabled");
    return ESP_OK;
}
