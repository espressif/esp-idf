/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "driver/isp_core.h"
#include "driver/isp_blc.h"
#include "esp_private/isp_private.h"
#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"

/*---------------------------------------------------------------
                      BLC
---------------------------------------------------------------*/

static const char *TAG = "ISP_BLC";

esp_err_t esp_isp_blc_configure(isp_proc_handle_t isp_proc, const esp_isp_blc_config_t *config)
{
#if CONFIG_IDF_TARGET_ESP32P4
    unsigned chip_version = efuse_hal_chip_revision();
    if (!ESP_CHIP_REV_ABOVE(chip_version, 300)) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "BLC is not supported on ESP32P4 chips prior than v3.0");
    }
#endif

    ESP_RETURN_ON_FALSE(isp_proc && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(isp_proc->blc_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "blc is enabled already");
    ESP_RETURN_ON_FALSE(config->window.top_left.x < config->window.btm_right.x, ESP_ERR_INVALID_ARG, TAG, "invalid window x coordinates");
    ESP_RETURN_ON_FALSE(config->window.top_left.y < config->window.btm_right.y, ESP_ERR_INVALID_ARG, TAG, "invalid window y coordinates");
    ESP_RETURN_ON_FALSE(config->window.btm_right.x <= isp_proc->h_res, ESP_ERR_INVALID_ARG, TAG, "window exceeds horizontal resolution");
    ESP_RETURN_ON_FALSE(config->window.btm_right.y <= isp_proc->v_res, ESP_ERR_INVALID_ARG, TAG, "window exceeds vertical resolution");

    // Configure clock control mode
    isp_ll_blc_set_clk_ctrl_mode(isp_proc->hal.hw, ISP_LL_PIPELINE_CLK_CTRL_AUTO);

    // Configure sampling window
    isp_ll_blc_set_window(isp_proc->hal.hw,
                          config->window.top_left.x, config->window.top_left.y,
                          config->window.btm_right.x, config->window.btm_right.y);

    if (config->filter_enable) {
        // Configure threshold values for each channel
        isp_ll_blc_set_filter_threshold(isp_proc->hal.hw, config->filter_threshold.top_left_chan_thresh, config->filter_threshold.top_right_chan_thresh, config->filter_threshold.bottom_left_chan_thresh, config->filter_threshold.bottom_right_chan_thresh);
        isp_ll_blc_enable_filter(isp_proc->hal.hw, true);
    } else {
        isp_ll_blc_enable_filter(isp_proc->hal.hw, false);
    }

    // Configure stretch enable for each channel
    isp_ll_blc_enable_stretch(isp_proc->hal.hw, config->stretch.top_left_chan_stretch_en, config->stretch.top_right_chan_stretch_en, config->stretch.bottom_left_chan_stretch_en, config->stretch.bottom_right_chan_stretch_en);

    return ESP_OK;
}

esp_err_t esp_isp_blc_enable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->blc_fsm, &expected_fsm, ISP_FSM_ENABLE), ESP_ERR_INVALID_STATE, TAG, "blc is enabled already");

    // Enable BLC module
    isp_ll_blc_enable(isp_proc->hal.hw, true);

    ESP_LOGD(TAG, "BLC enabled");
    return ESP_OK;
}

esp_err_t esp_isp_blc_set_correction_offset(isp_proc_handle_t isp_proc, esp_isp_blc_offset_t *offset)
{
    ESP_RETURN_ON_FALSE(isp_proc && offset, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&isp_proc->blc_fsm) == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "blc isn't enabled yet");

    // Set correction offset for each channel
    isp_ll_blc_set_correction_offset(isp_proc->hal.hw, offset->top_left_chan_offset, offset->top_right_chan_offset, offset->bottom_left_chan_offset, offset->bottom_right_chan_offset);

    ESP_LOGD(TAG, "BLC correction offset set: TL=%"PRIu32", TR=%"PRIu32", BL=%"PRIu32", BR=%"PRIu32,
             offset->top_left_chan_offset, offset->top_right_chan_offset,
             offset->bottom_left_chan_offset, offset->bottom_right_chan_offset);

    return ESP_OK;
}

esp_err_t esp_isp_blc_disable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->blc_fsm, &expected_fsm, ISP_FSM_INIT), ESP_ERR_INVALID_STATE, TAG, "blc isn't enabled yet");

    // Disable BLC module
    isp_ll_blc_enable(isp_proc->hal.hw, false);

    ESP_LOGD(TAG, "BLC disabled");
    return ESP_OK;
}
