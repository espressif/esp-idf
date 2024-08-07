/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "driver/isp_af.h"
#include "esp_private/isp_private.h"

static const char *TAG = "ISP_AF";

typedef struct isp_af_controller_t {
    int                                id;
    _Atomic isp_fsm_t                  fsm;
    portMUX_TYPE                       spinlock;
    intr_handle_t                      intr_handle;
    isp_proc_handle_t                  isp_proc;
    QueueHandle_t                      evt_que;
    esp_isp_af_env_config_t            config;
    esp_isp_af_env_detector_evt_cbs_t  cbs;
    void                               *user_data;
} isp_af_controller_t;

/*---------------------------------------------
                AF
----------------------------------------------*/
static esp_err_t s_isp_claim_af_controller(isp_proc_handle_t isp_proc, isp_af_ctlr_t af_ctlr)
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

static void s_isp_declaim_af_controller(isp_af_ctlr_t af_ctlr)
{
    assert(af_ctlr && af_ctlr->isp_proc);

    portENTER_CRITICAL(&af_ctlr->isp_proc->spinlock);
    af_ctlr->isp_proc->af_ctlr[af_ctlr->id] = NULL;
    portEXIT_CRITICAL(&af_ctlr->isp_proc->spinlock);
}

static void s_isp_af_free_controller(isp_af_ctlr_t af_ctlr)
{
    if (af_ctlr) {
        if (af_ctlr->intr_handle) {
            esp_intr_free(af_ctlr->intr_handle);
        }
        if (af_ctlr->evt_que) {
            vQueueDeleteWithCaps(af_ctlr->evt_que);
        }
        free(af_ctlr);
    }
}

esp_err_t esp_isp_new_af_controller(isp_proc_handle_t isp_proc, const esp_isp_af_config_t *af_config, isp_af_ctlr_t *ret_hdl)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(isp_proc && af_config && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool rgb2yuv_en = isp_ll_is_rgb2yuv_enabled(isp_proc->hal.hw);
    bool demosaic_en = isp_ll_is_demosaic_enabled(isp_proc->hal.hw);
    ESP_RETURN_ON_FALSE(demosaic_en && rgb2yuv_en, ESP_ERR_INVALID_STATE, TAG, "RGB2YUV not enabled, please update the output_data_color_type");

    for (int i = 0; i < SOC_ISP_AF_WINDOW_NUMS; i++) {
        ESP_LOGV(TAG, "af_config->window[%d].top_left.x: %"PRId32, i, af_config->window[i].top_left.x);
        ESP_LOGV(TAG, "af_config->window[%d].btm_right.x: %"PRId32, i, af_config->window[i].btm_right.x);
        ESP_LOGV(TAG, "af_config->window[%d].btm_right.y: %"PRId32, i, af_config->window[i].btm_right.y);
        ESP_LOGV(TAG, "af_config->window[%d].top_left.y: %"PRId32, i, af_config->window[i].top_left.y);

        ESP_RETURN_ON_FALSE(((af_config->window[i].top_left.x < ISP_LL_AF_WINDOW_MAX_RANGE) &&
                             (af_config->window[i].btm_right.x >= af_config->window[i].top_left.x) &&
                             (af_config->window[i].btm_right.x < ISP_LL_AF_WINDOW_MAX_RANGE) &&
                             (af_config->window[i].top_left.y < ISP_LL_AF_WINDOW_MAX_RANGE) &&
                             (af_config->window[i].btm_right.y >= af_config->window[i].top_left.y) &&
                             (af_config->window[i].btm_right.y < ISP_LL_AF_WINDOW_MAX_RANGE)), ESP_ERR_INVALID_ARG, TAG, "invalid window");
    }
    ESP_RETURN_ON_FALSE(af_config->edge_thresh > 0, ESP_ERR_INVALID_ARG, TAG, "edge threshold should be larger than 0");

    isp_af_ctlr_t af_ctlr = heap_caps_calloc(1, sizeof(isp_af_controller_t), ISP_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(af_ctlr, ESP_ERR_NO_MEM, TAG, "no mem");
    af_ctlr->evt_que = xQueueCreateWithCaps(1, sizeof(isp_af_result_t), ISP_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(af_ctlr->evt_que, ESP_ERR_NO_MEM, err1, TAG, "no mem for af event queue");
    atomic_init(&af_ctlr->fsm, ISP_FSM_INIT);
    af_ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    af_ctlr->isp_proc = isp_proc;

    //claim an AF controller
    ESP_GOTO_ON_ERROR(s_isp_claim_af_controller(isp_proc, af_ctlr), err1, TAG, "no available controller");

    // Register the AF ISR
    int intr_priority = (af_config->intr_priority > 0 && af_config->intr_priority <= 3) ? BIT(af_config->intr_priority) : ESP_INTR_FLAG_LOWMED;
    ESP_GOTO_ON_ERROR(intr_priority != isp_proc->intr_priority, err2, TAG, "intr_priority error");
    ESP_GOTO_ON_ERROR(esp_isp_register_isr(af_ctlr->isp_proc, ISP_SUBMODULE_AF), err2, TAG, "fail to register ISR");

    isp_ll_af_enable_auto_update(isp_proc->hal.hw, false);
    isp_ll_af_enable(isp_proc->hal.hw, false);

    for (int i = 0; i < SOC_ISP_AF_WINDOW_NUMS; i++) {
        isp_hal_af_window_config(&isp_proc->hal, i, &af_config->window[i]);
    }

    isp_ll_af_set_edge_thresh_mode(isp_proc->hal.hw, ISP_LL_AF_EDGE_DETECTOR_MODE_MANUAL);
    isp_ll_af_set_edge_thresh(isp_proc->hal.hw, af_config->edge_thresh);
    isp_ll_clear_intr(isp_proc->hal.hw, ISP_LL_EVENT_AF_MASK);

    *ret_hdl = af_ctlr;

    return ESP_OK;
err2:
    s_isp_declaim_af_controller(af_ctlr);
err1:
    s_isp_af_free_controller(af_ctlr);

    return ret;
}

esp_err_t esp_isp_del_af_controller(isp_af_ctlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&af_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");
    bool exist = false;
    for (int i = 0; i < SOC_ISP_AF_CTLR_NUMS; i++) {
        if (af_ctlr->isp_proc->af_ctlr[i] == af_ctlr) {
            exist = true;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(exist, ESP_ERR_INVALID_ARG, TAG, "controller isn't in use");

    // Deregister the AF ISR
    ESP_RETURN_ON_FALSE(esp_isp_deregister_isr(af_ctlr->isp_proc, ISP_SUBMODULE_AF) == ESP_OK, ESP_FAIL, TAG, "fail to deregister ISR");

    s_isp_declaim_af_controller(af_ctlr);
    s_isp_af_free_controller(af_ctlr);

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_enable(isp_af_ctlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&af_ctlr->fsm, &expected_fsm, ISP_FSM_ENABLE),
                        ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

    isp_ll_af_clk_enable(af_ctlr->isp_proc->hal.hw, true);
    isp_ll_enable_intr(af_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_MASK, true);
    isp_ll_af_enable(af_ctlr->isp_proc->hal.hw, true);

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_disable(isp_af_ctlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&af_ctlr->fsm, &expected_fsm, ISP_FSM_INIT),
                        ESP_ERR_INVALID_STATE, TAG, "controller not in enable state");

    isp_ll_af_clk_enable(af_ctlr->isp_proc->hal.hw, false);
    isp_ll_enable_intr(af_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_MASK, false);
    isp_ll_af_enable(af_ctlr->isp_proc->hal.hw, false);
    esp_intr_disable(af_ctlr->intr_handle);

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_get_oneshot_statistics(isp_af_ctlr_t af_ctlr, int timeout_ms, isp_af_result_t *out_res)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctlr && (out_res || timeout_ms == 0), ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    esp_err_t ret = ESP_OK;
    TickType_t ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&af_ctlr->fsm, &expected_fsm, ISP_FSM_ONESHOT), ESP_ERR_INVALID_STATE, TAG, "controller isn't enabled or continuous statistics has starte");

    // Reset the queue in case receiving the legacy data in the queue
    xQueueReset(af_ctlr->evt_que);
    // Trigger the AF statistics manually
    isp_ll_af_manual_update(af_ctlr->isp_proc->hal.hw);
    // Wait the statistics to finish and receive the result from the queue
    if ((ticks > 0) && xQueueReceive(af_ctlr->evt_que, out_res, ticks) != pdTRUE) {
        ret = ESP_ERR_TIMEOUT;
    }

    atomic_store(&af_ctlr->fsm, ISP_FSM_ENABLE);
    return ret;
}

esp_err_t esp_isp_af_controller_start_continuous_statistics(isp_af_ctlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&af_ctlr->fsm, &expected_fsm, ISP_FSM_CONTINUOUS), ESP_ERR_INVALID_STATE, TAG, "controller is not enabled yet");
    isp_ll_af_enable_auto_update(af_ctlr->isp_proc->hal.hw, true);

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_stop_continuous_statistics(isp_af_ctlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_CONTINUOUS;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&af_ctlr->fsm, &expected_fsm, ISP_FSM_ENABLE), ESP_ERR_INVALID_STATE, TAG, "controller is not enabled yet");
    isp_ll_af_enable_auto_update(af_ctlr->isp_proc->hal.hw, false);

    return ESP_OK;
}

/*---------------------------------------------
                AF Env Detector
----------------------------------------------*/
esp_err_t esp_isp_af_controller_set_env_detector(isp_af_ctlr_t af_ctlr, const esp_isp_af_env_config_t *env_config)
{
    ESP_RETURN_ON_FALSE(af_ctlr && env_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&af_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

    af_ctlr->config.interval = env_config->interval;

    isp_ll_af_env_detector_set_period(af_ctlr->isp_proc->hal.hw, 0);
    isp_ll_clear_intr(af_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_ENV);

    isp_ll_af_env_detector_set_mode(af_ctlr->isp_proc->hal.hw, ISP_LL_AF_ENV_DETECTOR_MODE_ABS);
    isp_ll_af_env_detector_set_period(af_ctlr->isp_proc->hal.hw, af_ctlr->config.interval);
    isp_ll_enable_intr(af_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_ENV, true);

    return ESP_OK;
}

esp_err_t esp_isp_af_env_detector_register_event_callbacks(isp_af_ctlr_t af_ctlr, const esp_isp_af_env_detector_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(af_ctlr && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(atomic_load(&af_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

#if CONFIG_ISP_ISR_IRAM_SAFE
    if (cbs->on_env_statistics_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_env_statistics_done), ESP_ERR_INVALID_ARG, TAG, "on_env_statistics_done callback not in IRAM");
    }
    if (cbs->on_env_change) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_env_change), ESP_ERR_INVALID_ARG, TAG, "on_env_change callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    af_ctlr->cbs.on_env_statistics_done = cbs->on_env_statistics_done;
    af_ctlr->cbs.on_env_change = cbs->on_env_change;
    af_ctlr->user_data = user_data;

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_set_env_detector_threshold(isp_af_ctlr_t af_ctlr, int definition_thresh, int luminance_thresh)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(atomic_load(&af_ctlr->fsm) == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller not in enable state");

    isp_ll_af_env_detector_set_thresh(af_ctlr->isp_proc->hal.hw, definition_thresh, luminance_thresh);

    return ESP_OK;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
bool IRAM_ATTR esp_isp_af_isr(isp_proc_handle_t proc, uint32_t af_events)
{
    isp_af_ctlr_t af_ctlr = proc->af_ctlr[0];

    bool need_yield = false;
    esp_isp_af_env_detector_evt_data_t edata = {};

    if (af_events) {
        // Get the statistics result
        for (int i = 0; i < SOC_ISP_AF_WINDOW_NUMS; i++) {
            edata.af_result.definition[i] = isp_ll_af_get_window_sum(proc->hal.hw, i);
            edata.af_result.luminance[i] = isp_ll_af_get_window_lum(proc->hal.hw, i);
        }
    }

    /**
     * Deal with the interrupts.
     * Now only one detector.
     * Should decide a detector instance according to the hw event.
     */
    if (af_events & ISP_LL_EVENT_AF_FDONE) {
        BaseType_t high_task_awake = false;
        // Send the event data to the queue, overwrite the legacy one if exist
        xQueueOverwriteFromISR(af_ctlr->evt_que, &edata.af_result, &high_task_awake);
        // Invoke the callback if the callback is registered
        need_yield |= high_task_awake == pdTRUE;
        if (af_ctlr->cbs.on_env_statistics_done) {
            need_yield |= af_ctlr->cbs.on_env_statistics_done(af_ctlr, &edata, af_ctlr->user_data);
        }
    }
    if (af_events & ISP_LL_EVENT_AF_ENV) {
        // Invoke the callback if the callback is registered
        if (af_ctlr->cbs.on_env_change) {
            need_yield |= af_ctlr->cbs.on_env_change(af_ctlr, &edata, af_ctlr->user_data);
        }
    }
    return need_yield;
}
