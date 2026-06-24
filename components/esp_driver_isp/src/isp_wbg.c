/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "driver/isp_wbg.h"
#include "esp_private/isp_private.h"
#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"

/*---------------------------------------------------------------
                    WBG
---------------------------------------------------------------*/

static const char *TAG = "ISP_WBG";

esp_err_t esp_isp_wbg_configure(isp_proc_handle_t isp_proc, const esp_isp_wbg_config_t *config)
{
#if CONFIG_IDF_TARGET_ESP32P4
    unsigned chip_version = efuse_hal_chip_revision();
    if (!ESP_CHIP_REV_ABOVE(chip_version, 300)) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "WBG is not supported on ESP32P4 chips prior than v3.0");
    }
#endif

    ESP_RETURN_ON_FALSE(isp_proc && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    // Configure clock control mode
    isp_ll_awb_set_wb_gain_clk_ctrl_mode(isp_proc->hal.hw, ISP_LL_PIPELINE_CLK_CTRL_AUTO);

    bool valid = isp_ll_shadow_update_wbg(isp_proc->hal.hw, config->flags.update_once_configured);
    ESP_RETURN_ON_FALSE_ISR(valid, ESP_ERR_INVALID_STATE, TAG, "failed to update wbg shadow register");

    isp_proc->sub_module_flags.wbg_update_once_configured = config->flags.update_once_configured;

    return ESP_OK;
}

esp_err_t esp_isp_wbg_enable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->wbg_fsm, &expected_fsm, ISP_FSM_ENABLE), ESP_ERR_INVALID_STATE, TAG, "wbg is enabled already");

    // Enable WBG module
    isp_ll_awb_enable_wb_gain(isp_proc->hal.hw, true);

    ESP_LOGD(TAG, "WBG enabled");
    return ESP_OK;
}

esp_err_t esp_isp_wbg_set_wb_gain(isp_proc_handle_t isp_proc, isp_wbg_gain_t gain)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&isp_proc->wbg_fsm) == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "wbg isn't enabled yet");

    // Set WBG gain
    isp_ll_awb_set_wb_gain(isp_proc->hal.hw, gain);

    bool valid = isp_ll_shadow_update_wbg(isp_proc->hal.hw, isp_proc->sub_module_flags.wbg_update_once_configured);
    ESP_RETURN_ON_FALSE_ISR(valid, ESP_ERR_INVALID_STATE, TAG, "failed to update wbg shadow register");

    return ESP_OK;
}

esp_err_t esp_isp_wbg_disable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->wbg_fsm, &expected_fsm, ISP_FSM_INIT), ESP_ERR_INVALID_STATE, TAG, "wbg isn't enabled yet");

    // Disable WBG module
    isp_ll_awb_enable_wb_gain(isp_proc->hal.hw, false);

    ESP_LOGD(TAG, "WBG disabled");
    return ESP_OK;
}
