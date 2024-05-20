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
#include "driver/isp_af.h"
#include "isp_internal.h"

static const char *TAG = "ISP_AF";

typedef struct isp_af_controller_t {
    int                                id;
    isp_fsm_t                          fsm;
    portMUX_TYPE                       spinlock;
    intr_handle_t                      intr_handle;
    isp_proc_handle_t                  isp_proc;
    QueueHandle_t                      evt_que;
    esp_isp_af_env_config_t            config;
    esp_isp_af_env_detector_evt_cbs_t  cbs;
    void                               *user_data;
} isp_af_controller_t;

static void s_isp_af_default_isr(void *arg);

/*---------------------------------------------
                AF
----------------------------------------------*/
static esp_err_t s_isp_claim_af_controller(isp_proc_handle_t isp_proc, isp_af_ctrlr_t af_ctlr)
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

static void s_isp_declaim_af_controller(isp_af_ctrlr_t af_ctlr)
{
    assert(af_ctlr && af_ctlr->isp_proc);

    portENTER_CRITICAL(&af_ctlr->isp_proc->spinlock);
    af_ctlr->isp_proc->af_ctlr[af_ctlr->id] = NULL;
    portEXIT_CRITICAL(&af_ctlr->isp_proc->spinlock);
}

static void s_isp_af_free_controller(isp_af_ctrlr_t af_ctlr)
{
    if (af_ctlr) {
        if (af_ctlr->intr_handle) {
            esp_intr_free(af_ctlr->intr_handle);
        }
        if (af_ctlr->evt_que) {
            vQueueDelete(af_ctlr->evt_que);
        }
        free(af_ctlr);
    }
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

    isp_af_ctrlr_t af_ctlr = heap_caps_calloc(1, sizeof(isp_af_controller_t), ISP_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(af_ctlr, ESP_ERR_NO_MEM, TAG, "no mem");
    af_ctlr->evt_que = xQueueCreateWithCaps(1, sizeof(isp_af_result_t), ISP_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(af_ctlr->evt_que, ESP_ERR_NO_MEM, err1, TAG, "no mem for af event queue");
    af_ctlr->fsm = ISP_FSM_INIT;
    af_ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    af_ctlr->isp_proc = isp_proc;

    //claim an AF controller
    ESP_GOTO_ON_ERROR(s_isp_claim_af_controller(isp_proc, af_ctlr), err1, TAG, "no available controller");

    // Register the AF ISR
    uint32_t intr_st_reg_addr = isp_ll_get_intr_status_reg_addr(isp_proc->hal.hw);
    int intr_priority = af_config->intr_priority > 0 && af_config->intr_priority <= 7 ? BIT(af_config->intr_priority) : ESP_INTR_FLAG_LOWMED;
    ESP_GOTO_ON_ERROR(esp_intr_alloc_intrstatus(isp_hw_info.instances[isp_proc->proc_id].irq, ISP_INTR_ALLOC_FLAGS | intr_priority, intr_st_reg_addr, ISP_LL_EVENT_AF_MASK,
                                                s_isp_af_default_isr, af_ctlr, &af_ctlr->intr_handle), err2, TAG, "allocate interrupt failed");

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
err2:
    s_isp_declaim_af_controller(af_ctlr);
err1:
    s_isp_af_free_controller(af_ctlr);

    return ret;
}

esp_err_t esp_isp_del_af_controller(isp_af_ctrlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(af_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");
    bool exist = false;
    for (int i = 0; i < SOC_ISP_AF_CTLR_NUMS; i++) {
        if (af_ctlr->isp_proc->af_ctlr[i] == af_ctlr) {
            exist = true;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(exist, ESP_ERR_INVALID_ARG, TAG, "controller isn't in use");
    s_isp_declaim_af_controller(af_ctlr);
    s_isp_af_free_controller(af_ctlr);

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_enable(isp_af_ctrlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(af_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");

    esp_intr_enable(af_ctlr->intr_handle);
    isp_ll_af_clk_enable(af_ctlr->isp_proc->hal.hw, true);
    isp_ll_enable_intr(af_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_MASK, true);
    isp_ll_af_enable(af_ctlr->isp_proc->hal.hw, true);
    af_ctlr->fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_disable(isp_af_ctrlr_t af_ctlr)
{
    ESP_RETURN_ON_FALSE(af_ctlr && af_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(af_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    isp_ll_af_clk_enable(af_ctlr->isp_proc->hal.hw, false);
    isp_ll_enable_intr(af_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AF_MASK, false);
    isp_ll_af_enable(af_ctlr->isp_proc->hal.hw, false);
    esp_intr_disable(af_ctlr->intr_handle);
    af_ctlr->fsm = ISP_FSM_INIT;

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_get_oneshot_statistics(isp_af_ctrlr_t af_ctrlr, int timeout_ms, isp_af_result_t *out_res)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr && (out_res || timeout_ms == 0), ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't enabled or continuous statistics has started");

    esp_err_t ret = ESP_OK;
    TickType_t ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    // Reset the queue in case receiving the legacy data in the queue
    xQueueReset(af_ctrlr->evt_que);
    // Trigger the AF statistics manually
    isp_ll_af_manual_update(af_ctrlr->isp_proc->hal.hw);
    // Wait the statistics to finish and receive the result from the queue
    if ((ticks > 0) && xQueueReceive(af_ctrlr->evt_que, out_res, ticks) != pdTRUE) {
        ret = ESP_ERR_TIMEOUT;
    }

    return ret;
}

esp_err_t esp_isp_af_controller_start_continuous_statistics(isp_af_ctrlr_t af_ctrlr)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    af_ctrlr->fsm = ISP_FSM_START;
    isp_ll_af_enable_auto_update(af_ctrlr->isp_proc->hal.hw, true);

    return ESP_OK;
}

esp_err_t esp_isp_af_controller_stop_continuous_statistics(isp_af_ctrlr_t af_ctrlr)
{
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(af_ctrlr->fsm == ISP_FSM_START, ESP_ERR_INVALID_STATE, TAG, "controller isn't in continuous state");

    isp_ll_af_enable_auto_update(af_ctrlr->isp_proc->hal.hw, false);
    af_ctrlr->fsm = ISP_FSM_ENABLE;

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
    af_ctrlr->cbs.on_env_statistics_done = cbs->on_env_statistics_done;
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
static void IRAM_ATTR s_isp_af_default_isr(void *arg)
{
    isp_af_ctrlr_t af_ctrlr = (isp_af_ctrlr_t)arg;
    isp_proc_handle_t proc = af_ctrlr->isp_proc;

    uint32_t af_events = isp_hal_check_clear_intr_event(&proc->hal, ISP_LL_EVENT_AF_MASK);

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
        xQueueOverwriteFromISR(af_ctrlr->evt_que, &edata.af_result, &high_task_awake);
        // Invoke the callback if the callback is registered
        need_yield |= high_task_awake == pdTRUE;
        if (af_ctrlr->cbs.on_env_statistics_done) {
            need_yield |= af_ctrlr->cbs.on_env_statistics_done(af_ctrlr, &edata, af_ctrlr->user_data);
        }
    }
    if (af_events & ISP_LL_EVENT_AF_ENV) {
        // Invoke the callback if the callback is registered
        if (af_ctrlr->cbs.on_env_change) {
            need_yield |= af_ctrlr->cbs.on_env_change(af_ctrlr, &edata, af_ctrlr->user_data);
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
