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
    portENTER_CRITICAL(&proc->spinlock);
    ret = isp_hal_ccm_set_matrix(&proc->hal, ccm_cfg->saturation, ccm_cfg->matrix);
    portEXIT_CRITICAL(&proc->spinlock);
    ESP_RETURN_ON_FALSE(ret, ESP_ERR_INVALID_ARG, TAG, "invalid argument: ccm matrix contain NaN or out of range");

    return ESP_OK;
}

esp_err_t esp_isp_ccm_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    portENTER_CRITICAL(&proc->spinlock);
    isp_ll_ccm_clk_enable(proc->hal.hw, true);
    isp_ll_ccm_enable(proc->hal.hw, true);
    portEXIT_CRITICAL(&proc->spinlock);

    return ESP_OK;
}

esp_err_t esp_isp_ccm_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    portENTER_CRITICAL(&proc->spinlock);
    isp_ll_ccm_enable(proc->hal.hw, false);
    isp_ll_ccm_clk_enable(proc->hal.hw, false);
    portEXIT_CRITICAL(&proc->spinlock);

    return ESP_OK;
}
