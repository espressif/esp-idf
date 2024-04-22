/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "driver/isp_af.h"
#include "isp_af_scheme_sa.h"
#include "isp_af_scheme_interface.h"

#define ISP_AF_SCHEME_SA_DEFAULT_WINDOW_NUMS       3
#define ISP_AF_SCHEME_SA_ENV_THRESH_SEARCH_NUMS    30

static const char *TAG = "AF_SCHEME";

typedef struct {
    isp_af_ctrlr_t af_ctlr;
    int first_step_val;
    int first_approx_cycles;
    int second_step_val;
    int second_approx_cycles;

    isp_af_sa_scheme_sensor_info_t sensor_info;
    isp_af_sa_scheme_sensor_drv_t sensor_drv;
} af_scheme_context_t;

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t s_af_process(void *arg, int *out_definition_thresh, int *out_luminance_thresh);

/* ------------------------- Public API ------------------------------------- */
esp_err_t isp_af_create_sa_scheme(isp_af_ctrlr_t af_ctlr, const isp_af_sa_scheme_config_t *config, isp_af_scheme_handle_t *ret_scheme)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(af_ctlr && config && ret_scheme, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");

    isp_af_scheme_t *scheme = (isp_af_scheme_t *)heap_caps_calloc(1, sizeof(isp_af_scheme_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_NO_MEM, TAG, "no mem for scheme");

    af_scheme_context_t *ctx = (af_scheme_context_t *)heap_caps_calloc(1, sizeof(af_scheme_context_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(ctx, ESP_ERR_NO_MEM, err, TAG, "no mem scheme context");

    scheme->af_process = s_af_process;
    scheme->ctx = ctx;

    ctx->af_ctlr = af_ctlr;
    ctx->first_step_val = config->first_step_val;
    ctx->first_approx_cycles = config->first_approx_cycles;
    ctx->second_step_val = config->second_step_val;
    ctx->second_approx_cycles = config->second_approx_cycles;

    *ret_scheme = scheme;

    return ESP_OK;

err:
    free(scheme);

    return ret;
}

esp_err_t isp_af_delete_sa_scheme(isp_af_scheme_handle_t scheme)
{
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    free(scheme->ctx);
    scheme->ctx = NULL;

    free(scheme);
    scheme = NULL;

    return ESP_OK;
}

esp_err_t isp_af_sa_scheme_register_sensor_driver(isp_af_scheme_handle_t scheme, const isp_af_sa_scheme_sensor_drv_t *sensor_drv, const isp_af_sa_scheme_sensor_info_t *info)
{
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_INVALID_ARG, TAG, "invalid arg: null pointer");
    ESP_RETURN_ON_FALSE(scheme->ctx, ESP_ERR_INVALID_STATE, TAG, "no scheme created yet");

    af_scheme_context_t *ctx = scheme->ctx;
    ctx->sensor_drv.af_sensor_set_focus = sensor_drv->af_sensor_set_focus;
    ctx->sensor_info.focus_val_max = info->focus_val_max;

    return ESP_OK;
}

/* ------------------------ Interface Functions --------------------------- */
static esp_err_t s_af_process(void *arg, int *out_definition_thresh, int *out_luminance_thresh)
{
    //arg pointer is checked in the upper layer

    af_scheme_context_t *ctx = arg;
    ESP_RETURN_ON_FALSE(ctx->af_ctlr, ESP_ERR_INVALID_STATE, TAG, "no AF controller registered");
    ESP_RETURN_ON_FALSE(ctx->sensor_drv.af_sensor_set_focus, ESP_ERR_INVALID_STATE, TAG, "no sensor driver function `af_sensor_set_focus` registered");

    int af_sum = 0;
    int af_lum = 0;
    int af_sum_max = 0;

    int af_current_base = 0;
    int af_current = 0;
    int af_current_best = 0;

    int af_sum_env_th = 0;
    int af_lum_env_th = 0;
    int af_sum_tmp[ISP_AF_SCHEME_SA_ENV_THRESH_SEARCH_NUMS] = {0};
    int af_lum_tmp[ISP_AF_SCHEME_SA_ENV_THRESH_SEARCH_NUMS] = {0};

    int ref_x = ISP_AF_SCHEME_SA_ENV_THRESH_SEARCH_NUMS;
    int ref_x_fallback = ISP_AF_SCHEME_SA_ENV_THRESH_SEARCH_NUMS - 1;

    isp_af_result_t result = {};

    ESP_RETURN_ON_ERROR(ctx->sensor_drv.af_sensor_set_focus(0), TAG, "sensor set focus val fail");

    ESP_LOGV(TAG, "//----------- af start ----------//");

    // first search
    ESP_LOGV(TAG, "//----------- first search ----------//");
    af_sum_max = 0;
    af_current_base = 0;

    for (int x = 0; x <= ctx->first_approx_cycles; x++) {
        af_current = af_current_base + x * ctx->first_step_val;
        ESP_RETURN_ON_ERROR(ctx->sensor_drv.af_sensor_set_focus(af_current), TAG, "sensor set focus val fail");
        ESP_RETURN_ON_ERROR(esp_isp_af_controller_get_oneshot_result(ctx->af_ctlr, &result), TAG, "get AF result fail");
        af_sum = result.definition[0] + result.definition[1] + result.definition[2];
        if (af_sum > af_sum_max) {
            af_sum_max = af_sum;
            af_current_best = af_current;
        }

        ESP_LOGV(TAG, "af_sum: %d, af_current: %d.%d", af_sum, (int)af_current, (int)((int)(af_current * 1000) % 1000));
    }

    // second search
    ESP_LOGV(TAG, "//----------- second search ----------//");
    af_sum_max = 0;
    af_current_base = af_current_best + 10;

    if (af_current_base > ctx->sensor_info.focus_val_max) {
        af_current_base = ctx->sensor_info.focus_val_max;
    }

    for (int x = 0; x <= ctx->second_approx_cycles; x++) {
        af_current = af_current_base - x * ctx->second_step_val;
        if (af_current < 0) {
            af_current = 0;
        }

        ESP_RETURN_ON_ERROR(ctx->sensor_drv.af_sensor_set_focus(af_current), TAG, "sensor set focus val fail");
        ESP_RETURN_ON_ERROR(esp_isp_af_controller_get_oneshot_result(ctx->af_ctlr, &result), TAG, "get AF result fail");
        af_sum = result.definition[0] + result.definition[1] + result.definition[2];
        if (af_sum > af_sum_max) {
            af_sum_max = af_sum;
            af_current_best = af_current;
        }
        ESP_LOGV(TAG, "af_sum: %d, af_current: %d.%d", af_sum, (int)af_current, (int)((int)(af_current * 1000) % 1000));
    }

    // af done
    ESP_LOGV(TAG, "//----------- af done ----------//");
    ESP_LOGV(TAG, "af_sum_max: %d, af_current_best: %d.%d", af_sum_max, (int)af_current_best, (int)((int)(af_current_best * 1000) % 1000));
    ESP_RETURN_ON_ERROR(ctx->sensor_drv.af_sensor_set_focus(af_current_best), TAG, "sensor set focus val fail");

    // update env threshold
    ESP_LOGV(TAG, "//------- update env threshold -------//");

    bool use_fallback_th = true;
    for (int x = 0; x < ref_x; x++) {
        ESP_RETURN_ON_ERROR(esp_isp_af_controller_get_oneshot_result(ctx->af_ctlr, &result), TAG, "get AF result fail");
        af_sum_tmp[x] = result.definition[0] + result.definition[1] + result.definition[2];
        af_lum_tmp[x] = result.luminance[0] + result.luminance[1] + result.luminance[2];

        if ((x >= 1) && (abs(af_sum_tmp[x] - af_sum_max) < af_sum_max * 0.3) && (abs(af_sum_tmp[x - 1] - af_sum_max) < af_sum_max * 0.3)) {
            ref_x = x;
            use_fallback_th = false;
            break;
        }
    }

    if (use_fallback_th) {
        ref_x = ref_x_fallback;
    }

    af_sum = af_sum_tmp[ref_x];
    af_lum = af_lum_tmp[ref_x];
    af_sum_env_th = af_sum * 0.5;
    af_lum_env_th = af_lum * 0.05;
    *out_definition_thresh = af_sum_env_th;
    *out_luminance_thresh = af_lum_env_th;

    for (int x = 0; x < ref_x; x++) {
        ESP_LOGV(TAG, "af_sum[%d]: %d, af_lum[%d]: %d", x, af_sum_tmp[x], x, af_lum_tmp[x]);
    }
    ESP_LOGV(TAG, "//------- update af env threshold done -------//");
    ESP_LOGV(TAG, "af_sum: %d, af_sum_env_th: %d", af_sum, af_sum_env_th);
    ESP_LOGV(TAG, "af_lum: %d, af_lum_env_th: %d", af_lum, af_lum_env_th);
    ESP_LOGV(TAG, "//----------- af update done ----------//\n\n");

    return ESP_OK;
}
