/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "driver/isp_core.h"
#include "driver/isp_ccm.h"
#include "esp_private/isp_private.h"

static const char *TAG = "ISP_CCM";

/*---------------------------------------------------------------
                      CCM
---------------------------------------------------------------*/
esp_err_t esp_isp_ccm_configure(isp_proc_handle_t proc, const esp_isp_ccm_config_t *ccm_cfg)
{
    ESP_RETURN_ON_FALSE(proc && ccm_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool ret = true;
    bool valid = false;
    portENTER_CRITICAL(&proc->spinlock);
    isp_ll_ccm_set_clk_ctrl_mode(proc->hal.hw, ISP_LL_PIPELINE_CLK_CTRL_AUTO);
    ret = isp_hal_ccm_set_matrix(&proc->hal, ccm_cfg->saturation, ccm_cfg->matrix);
    valid = isp_ll_shadow_update_ccm(proc->hal.hw);
    portEXIT_CRITICAL(&proc->spinlock);
    ESP_RETURN_ON_FALSE(ret, ESP_ERR_INVALID_ARG, TAG, "invalid argument: ccm matrix contain NaN or out of range");
    ESP_RETURN_ON_FALSE(valid, ESP_ERR_INVALID_STATE, TAG, "failed to update ccm shadow register");

    return ESP_OK;
}

esp_err_t esp_isp_ccm_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&proc->ccm_fsm, &expected_fsm, ISP_FSM_ENABLE), ESP_ERR_INVALID_STATE, TAG, "ccm is enabled already");

    portENTER_CRITICAL(&proc->spinlock);
    isp_ll_ccm_enable(proc->hal.hw, true);
    portEXIT_CRITICAL(&proc->spinlock);

    return ESP_OK;
}

esp_err_t esp_isp_ccm_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&proc->ccm_fsm, &expected_fsm, ISP_FSM_INIT), ESP_ERR_INVALID_STATE, TAG, "ccm isn't enabled yet");

    portENTER_CRITICAL(&proc->spinlock);
    isp_ll_ccm_enable(proc->hal.hw, false);
    portEXIT_CRITICAL(&proc->spinlock);

    return ESP_OK;
}
