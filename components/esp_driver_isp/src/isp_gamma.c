/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "driver/isp_core.h"
#include "driver/isp_gamma.h"
#include "esp_private/isp_private.h"
#include "hal/isp_types.h"

static const char *TAG = "ISP_GAMMA";

/*---------------------------------------------------------------
                      Gamma Correction
---------------------------------------------------------------*/
// A curve of y = x indicates no gamma curve correction involved
static inline uint32_t s_gamma_linear_curve(uint32_t x)
{
    return x;
}

esp_err_t esp_isp_gamma_configure(isp_proc_handle_t proc, color_component_t component, const isp_gamma_curve_points_t *pts)
{
    ESP_RETURN_ON_FALSE_ISR(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(component < COLOR_COMPONENT_INVALID, ESP_ERR_INVALID_ARG, TAG, "invalid argument: component");

    if (!pts) {
        isp_gamma_curve_points_t linear_pts = {};
        esp_isp_gamma_fill_curve_points(s_gamma_linear_curve, &linear_pts);
        isp_ll_gamma_set_correction_curve(proc->hal.hw, component, &linear_pts);
        return ESP_OK;
    }

    // Check the validation of x values
    uint32_t x_prev = 0;
    for (int i = 0; i < ISP_GAMMA_CURVE_POINTS_NUM; i++) {
        uint32_t x_i = pts->pt[i].x;
        if (i == ISP_GAMMA_CURVE_POINTS_NUM - 1) {
            ESP_RETURN_ON_FALSE_ISR(x_i == 255, ESP_ERR_INVALID_ARG, TAG, "invalid argument: pts->pt[ISP_GAMMA_CURVE_POINTS_NUM - 1].x != 255");
            x_i = 256;
        }
        uint32_t x_delta = x_i - x_prev;
        ESP_RETURN_ON_FALSE_ISR((x_i > x_prev) && ((x_delta & (x_delta - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "invalid argument: pts->pt[%d].x", i);
        x_prev = x_i;
    }

    isp_ll_gamma_set_correction_curve(proc->hal.hw, component, pts);

    return ESP_OK;
}

esp_err_t esp_isp_gamma_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    portENTER_CRITICAL(&proc->spinlock);
    isp_ll_gamma_enable(proc->hal.hw, true);
    portEXIT_CRITICAL(&proc->spinlock);

    return ESP_OK;
}

esp_err_t esp_isp_gamma_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    portENTER_CRITICAL(&proc->spinlock);
    isp_ll_gamma_enable(proc->hal.hw, false);
    portEXIT_CRITICAL(&proc->spinlock);

    return ESP_OK;
}

esp_err_t esp_isp_gamma_fill_curve_points(uint32_t (* gamma_correction_operator)(uint32_t), isp_gamma_curve_points_t *pts)
{
    ESP_RETURN_ON_FALSE_ISR(gamma_correction_operator && pts, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    uint32_t x_i = 0;
    uint32_t y_i = 0;
    const uint32_t x_delta = 256 / ISP_GAMMA_CURVE_POINTS_NUM;
    for (int i = 0; i < ISP_GAMMA_CURVE_POINTS_NUM; i++) {
        x_i += x_delta;
        x_i = MIN(x_i, 255);
        y_i = gamma_correction_operator(x_i);
#if !CONFIG_ISP_CTRL_FUNC_IN_IRAM
        ESP_LOGD(TAG, "x[%d] = %"PRIu32", y[%d] = %"PRIu32"", i, x_i, i, y_i);
#endif
        ESP_RETURN_ON_FALSE_ISR(y_i < 256, ESP_ERR_INVALID_ARG, TAG, "invalid argument: gamma_correction_operator");
        pts->pt[i].x = x_i;
        pts->pt[i].y = y_i;
    }
    return ESP_OK;
}
