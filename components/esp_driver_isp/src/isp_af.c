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
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "driver/isp.h"
#include "hal/isp_hal.h"
#include "hal/isp_ll.h"
#include "isp_internal.h"

#if CONFIG_ISP_ISR_IRAM_SAFE
#define ISP_AF_MEM_ALLOC_CAPS   (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ISP_AF_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT
#endif

static const char *TAG = "ISP_AF";

/*---------------------------------------------
                AF
----------------------------------------------*/
static esp_err_t s_isp_claim_af_controller(isp_proc_handle_t isp_proc, isp_af_controller_t *af_ctlr)
{
    assert(isp_proc && af_ctlr);

    bool found = false;
    portENTER_CRITICAL(&isp_proc->spinlock);
    for (int i = 0; i < SOC_ISP_AF_CTLR_NUMS; i++) {
        found = !isp_proc->af_ctlr[i];
        if (found) {
            isp_proc->af_ctlr[i] = af_ctlr;
            af_ctlr->id = i;

            break;
        }
    }
    portEXIT_CRITICAL(&isp_proc->spinlock);

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t s_isp_declaim_af_controller(isp_af_controller_t *af_ctlr)
{
    assert(af_ctlr && af_ctlr->isp_proc);

    portENTER_CRITICAL(&af_ctlr->isp_proc->spinlock);
    af_ctlr->isp_proc->af_ctlr[af_ctlr->id] = NULL;
    portEXIT_CRITICAL(&af_ctlr->isp_proc->spinlock);

    return ESP_OK;
}

esp_err_t esp_isp_new_af_controller(isp_proc_handle_t isp_proc, const esp_isp_af_config_t *af_config, isp_af_ctrlr_t *ret_hdl)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(isp_proc && af_config && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool rgb2yuv_en = isp_ll_is_rgb2yuv_enabled(isp_proc->hal.hw);
    bool demosaic_en = isp_ll_is_demosaic_enabled(isp_proc->hal.hw);
    ESP_RETURN_ON_FALSE(demosaic_en && rgb2yuv_en, ESP_ERR_INVALID_STATE, TAG, "RGB2YUV not enabled, please update the output_data_color_type");

    for (int i = 0; i < SOC_ISP_AF_WINDOW_NUMS; i++) {
        ESP_LOGV(TAG, "af_config->window[%d].top_left_x: %"PRId32, i, af_config->window[i].top_left_x);
        ESP_LOGV(TAG, "af_config->window[%d].bottom_right_x: %"PRId32, i, af_config->window[i].bottom_right_x);
        ESP_LOGV(TAG, "af_config->window[%d].bottom_right_y: %"PRId32, i, af_config->window[i].bottom_right_y);
        ESP_LOGV(TAG, "af_config->window[%d].top_left_y: %"PRId32, i, af_config->window[i].top_left_y);

        ESP_RETURN_ON_FALSE(((af_config->window[i].top_left_x < ISP_LL_AF_WINDOW_MAX_RANGE) &&
                             (af_config->window[i].bottom_right_x >= af_config->window[i].top_left_x) &&
                             (af_config->window[i].bottom_right_x < ISP_LL_AF_WINDOW_MAX_RANGE) &&
                             (af_config->window[i].top_left_y < ISP_LL_AF_WINDOW_MAX_RANGE) &&
                             (af_config->window[i].bottom_right_y >= af_config->window[i].top_left_y) &&
                             (af_config->window[i].bottom_right_y < ISP_LL_AF_WINDOW_MAX_RANGE)), ESP_ERR_INVALID_ARG, TAG, "invalid window");
    }
    ESP_RETURN_ON_FALSE(af_config->edge_thresh > 0, ESP_ERR_INVALID_ARG, TAG, "edge threshold should be larger than 0");

    isp_af_controller_t *af_ctlr = heap_caps_calloc(1, sizeof(isp_af_controller_t), ISP_AF_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(af_ctlr, ESP_ERR_NO_MEM, TAG, "no mem");

    //claim an AF controller
    ESP_GOTO_ON_ERROR(s_isp_claim_af_controller(isp_proc, af_ctlr), err, TAG, "no available controller");

    af_ctlr->fsm = ISP_FSM_INIT;
    af_ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    af_ctlr->isp_proc = isp_proc;

    isp_ll_af_enable_auto_update(isp_proc->hal.hw, false);
    isp_ll_af_enable(isp_proc->hal.hw, false);

    for (int i = 0; i < SOC_ISP_AF_WINDOW_NUMS; i++) {
        isp_hal_af_window_config(&isp_proc->hal, i, &af_config->window[i]);
    }

    isp_ll_af_set_edge_thresh_mode(isp_proc->hal.hw, ISP_LL_AF_EDGE_MONITOR_MODE_MANUAL);
    isp_ll_af_set_edge_thresh(isp_proc->hal.hw, af_config->edge_thresh);
    isp_ll_clear_intr(isp_proc->hal.hw, ISP_LL_EVENT_AF_MASK);

    *ret_hdl = af_ctlr;

    return ESP_OK;

err:
    free(af_ctlr);

    return ret;
}

esp_err_t esp_isp_del_af_controller(isp_af_ctrlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_ERROR(s_isp_declaim_af_controller(af_ctlr), TAG, "controller isn't in use");
    ESP_RETURN_ON_FALSE(af_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");

    free(af_ctlr);

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_enable(isp_af_ctrlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(af_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");

    isp_ll_af_clk_enable(af_ctlr->isp_proc->hal.hw, true);
    isp_ll_af_enable(af_ctlr->isp_proc->hal.hw, true);
    af_ctlr->fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_disable(isp_af_ctrlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(af_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    isp_ll_af_clk_enable(af_ctlr->isp_proc->hal.hw, false);
    isp_ll_af_enable(af_ctlr->isp_proc->hal.hw, false);
    af_ctlr->fsm = ISP_FSM_INIT;

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_get_oneshot_result(isp_af_ctrlr_t af_ctlr, isp_af_result_t *out_res)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctlr && out_res, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(af_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    isp_hal_af_get_oneshot_result(&af_ctlr->isp_proc->hal, out_res);

    return ESP_OK;
}

/*---------------------------------------------
                AF Env Monitor
----------------------------------------------*/
esp_err_t esp_isp_af_controller_set_env_detector(isp_af_ctrlr_t af_ctrlr, const esp_isp_af_env_config_t *env_config)
{
    ESP_RETURN_ON_FALSE(af_ctrlr && env_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(af_ctrlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "invalid fsm, should be called when in init state");

    af_ctrlr->config.interval = env_config->interval;

    isp_ll_af_env_monitor_set_period(af_ctrlr->isp_proc->hal.hw, 0);
    isp_ll_clear_intr(af_ctrlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_ENV);

    isp_ll_af_env_monitor_set_mode(af_ctrlr->isp_proc->hal.hw, ISP_LL_AF_ENV_MONITOR_MODE_ABS);
    isp_ll_af_env_monitor_set_period(af_ctrlr->isp_proc->hal.hw, af_ctrlr->config.interval);
    isp_ll_enable_intr(af_ctrlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_ENV, true);

    return ESP_OK;
}

esp_err_t esp_isp_af_env_detector_register_event_callbacks(isp_af_ctrlr_t af_ctrlr, const esp_isp_af_env_detector_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(af_ctrlr && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(af_ctrlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "detector isn't in the init state");

#if CONFIG_ISP_ISR_IRAM_SAFE
    if (cbs->on_env_change) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_env_change), ESP_ERR_INVALID_ARG, TAG, "on_env_change callback not in IRAM");
    }
#endif

    ESP_RETURN_ON_ERROR(esp_isp_register_isr(af_ctrlr->isp_proc, ISP_SUBMODULE_AF), TAG, "fail to register ISR");

    af_ctrlr->cbs.on_env_change = cbs->on_env_change;
    af_ctrlr->user_data = user_data;

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_set_env_detector_threshold(isp_af_ctrlr_t af_ctrlr, int definition_thresh, int luminance_thresh)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "detector isn't in enable state");

    isp_ll_af_env_monitor_set_thresh(af_ctrlr->isp_proc->hal.hw, definition_thresh, luminance_thresh);

    return ESP_OK;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
static bool IRAM_ATTR s_af_env_isr(isp_af_ctrlr_t af_ctrlr)
{
    bool need_yield = false;

    esp_isp_af_env_detector_evt_data_t edata = {};
    if (af_ctrlr->cbs.on_env_change(af_ctrlr, &edata, af_ctrlr->user_data)) {
        need_yield |= true;
    }

    return need_yield;
}

bool IRAM_ATTR esp_isp_af_isr(isp_proc_handle_t proc, uint32_t af_events)
{
    /**
     * HW events are cleared in the ISP ISR dispatcher.
     * We only deal with HW events
     */

    bool need_yield = false;

    if (af_events & ISP_LL_EVENT_AF_ENV) {
        /**
         * Now only one detector.
         * Should decide a detector instance according to the hw event.
         */
        isp_af_ctrlr_t af_ctrlr = proc->af_ctlr[0];

        need_yield |= s_af_env_isr(af_ctrlr);
    }

    return need_yield;
}
