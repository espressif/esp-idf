/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#include "driver/isp_bf.h"
#include "driver/isp_lsc.h"
#include "esp_private/isp_private.h"

/*---------------------------------------------------------------
                      LSC
---------------------------------------------------------------*/
#define ISP_LSC_GET_GRIDS(res)    (((res) - 1) / 2 / ISP_LL_LSC_GRID_HEIGHT + 2)

static const char *TAG = "ISP_LSC";

esp_err_t esp_isp_lsc_allocate_gain_array(isp_proc_handle_t isp_proc, esp_isp_lsc_gain_array_t *gain_array, size_t *out_array_size_per_channel)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(isp_proc->lsc_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "lsc is enabled already");

    int num_grids_x_max = ISP_LSC_GET_GRIDS(ISP_LL_HSIZE_MAX);
    int num_grids_y_max = ISP_LSC_GET_GRIDS(ISP_LL_VSIZE_MAX);
    int num_grids_x = ISP_LSC_GET_GRIDS(isp_proc->h_res);
    int num_grids_y = ISP_LSC_GET_GRIDS(isp_proc->v_res);
    ESP_LOGD(TAG, "num_grids_x_max: %d, num_grids_x: %d, num_grids_y_max: %d, num_grids_y: %d", num_grids_x_max, num_grids_y_max, num_grids_x, num_grids_y);
    ESP_RETURN_ON_FALSE(num_grids_x <= num_grids_x_max && num_grids_y <= num_grids_y_max, ESP_ERR_INVALID_ARG, TAG, "invalid h_res or v_res");

    gain_array->gain_r = (isp_lsc_gain_t *)heap_caps_calloc(1, num_grids_x * num_grids_y * sizeof(isp_lsc_gain_t), ISP_MEM_ALLOC_CAPS);
    gain_array->gain_gr = (isp_lsc_gain_t *)heap_caps_calloc(1, num_grids_x * num_grids_y * sizeof(isp_lsc_gain_t), ISP_MEM_ALLOC_CAPS);
    gain_array->gain_gb = (isp_lsc_gain_t *)heap_caps_calloc(1, num_grids_x * num_grids_y * sizeof(isp_lsc_gain_t), ISP_MEM_ALLOC_CAPS);
    gain_array->gain_b = (isp_lsc_gain_t *)heap_caps_calloc(1, num_grids_x * num_grids_y * sizeof(isp_lsc_gain_t), ISP_MEM_ALLOC_CAPS);

    if (!gain_array->gain_r || !gain_array->gain_gr || !gain_array->gain_gb || !gain_array->gain_b) {
        ESP_LOGE(TAG, "no enough mem for gain arrays");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGD(TAG, "gain_array->gain_r: %p, gain_array->gain_gr: %p, gain_array->gain_gb: %p, gain_array->gain_b: %p", gain_array->gain_r, gain_array->gain_gr, gain_array->gain_gb, gain_array->gain_b);
    *out_array_size_per_channel = num_grids_x * num_grids_y;
    return ESP_OK;
}

esp_err_t esp_isp_lsc_configure(isp_proc_handle_t isp_proc, const esp_isp_lsc_config_t *config)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    int num_grids_x_max = ISP_LSC_GET_GRIDS(ISP_LL_HSIZE_MAX);
    int num_grids_y_max = ISP_LSC_GET_GRIDS(ISP_LL_VSIZE_MAX);
    int num_grids_x = ISP_LSC_GET_GRIDS(isp_proc->h_res);
    int num_grids_y = ISP_LSC_GET_GRIDS(isp_proc->v_res);
    ESP_LOGD(TAG, "num_grids_x_max: %d, num_grids_x: %d, num_grids_y_max: %d, num_grids_y: %d", num_grids_x_max, num_grids_y_max, num_grids_x, num_grids_y);
    ESP_RETURN_ON_FALSE(num_grids_x <= num_grids_x_max && num_grids_y <= num_grids_y_max, ESP_ERR_INVALID_ARG, TAG, "invalid h_res or v_res");
    ESP_RETURN_ON_FALSE(config->gain_array->gain_r && config->gain_array->gain_gr && config->gain_array->gain_gb && config->gain_array->gain_b, ESP_ERR_INVALID_ARG, TAG, "null pointer to gain arrays");

    isp_ll_lsc_set_xtablesize(isp_proc->hal.hw, num_grids_x);

    for (int y = 0; y < num_grids_y; y++) {
        for (int x = 0; x < num_grids_x; x++) {
            int i = y * num_grids_x + x;
            isp_ll_lut_set_wdata_r_gr(isp_proc->hal.hw, config->gain_array->gain_r[i], config->gain_array->gain_gr[i]);
            isp_ll_lut_set_cmd(isp_proc->hal.hw, true, false, i, ISP_LL_LUT_LSC);
            isp_ll_lut_set_wdata_gb_b(isp_proc->hal.hw, config->gain_array->gain_gb[i], config->gain_array->gain_b[i]);
            isp_ll_lut_set_cmd(isp_proc->hal.hw, true, true, i, ISP_LL_LUT_LSC);
        }
    }

    return ESP_OK;
}

esp_err_t esp_isp_lsc_enable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(isp_proc->lsc_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "lsc is enabled already");

    isp_ll_lsc_clk_enable(isp_proc->hal.hw, true);
    isp_ll_lsc_enable(isp_proc->hal.hw, true);
    isp_proc->lsc_fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_lsc_disable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(isp_proc->lsc_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "lsc isn't enabled yet");

    isp_ll_lsc_enable(isp_proc->hal.hw, false);
    isp_ll_lsc_clk_enable(isp_proc->hal.hw, false);
    isp_proc->lsc_fsm = ISP_FSM_INIT;

    return ESP_OK;
}
