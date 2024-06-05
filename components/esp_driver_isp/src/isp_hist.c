/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "driver/isp_hist.h"
#include "esp_private/isp_private.h"

typedef struct isp_hist_controller_t {
    isp_fsm_t                          fsm;
    portMUX_TYPE                       spinlock;
    intr_handle_t                      intr_handle;
    int                                intr_priority;
    isp_proc_handle_t                  isp_proc;
    QueueHandle_t                      evt_que;
    esp_isp_hist_cbs_t                 cbs;
    void                               *user_data;
} isp_hist_controller_t;

static const char *TAG = "ISP_hist";

static void s_isp_hist_default_isr(void *arg);

/*---------------------------------------------
                hist
----------------------------------------------*/
static esp_err_t s_isp_claim_hist_controller(isp_proc_handle_t isp_proc, isp_hist_ctlr_t hist_ctlr)
{
    assert(isp_proc && hist_ctlr);

    bool found = false;
    portENTER_CRITICAL(&isp_proc->spinlock);
    if (!isp_proc->hist_ctlr) {
        isp_proc->hist_ctlr = hist_ctlr;
        found = true;
    }
    portEXIT_CRITICAL(&isp_proc->spinlock);

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static void s_isp_declaim_hist_controller(isp_hist_ctlr_t hist_ctlr)
{
    if (hist_ctlr && hist_ctlr->isp_proc) {
        portENTER_CRITICAL(&hist_ctlr->isp_proc->spinlock);
        hist_ctlr->isp_proc->hist_ctlr = NULL;
        portEXIT_CRITICAL(&hist_ctlr->isp_proc->spinlock);
    }
}

static void s_isp_hist_free_controller(isp_hist_ctlr_t hist_ctlr)
{
    if (hist_ctlr) {
        if (hist_ctlr->intr_handle) {
            esp_intr_free(hist_ctlr->intr_handle);
        }
        if (hist_ctlr->evt_que) {
            vQueueDelete(hist_ctlr->evt_que);
        }
        free(hist_ctlr);
    }
}

esp_err_t esp_isp_new_hist_controller(isp_proc_handle_t isp_proc, const esp_isp_hist_config_t *hist_cfg, isp_hist_ctlr_t *ret_hdl)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(isp_proc && hist_cfg && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    isp_hist_ctlr_t hist_ctlr = heap_caps_calloc(1, sizeof(isp_hist_controller_t), ISP_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(hist_ctlr, ESP_ERR_NO_MEM, TAG, "no mem for hist controller");
    hist_ctlr->evt_que = xQueueCreateWithCaps(1, sizeof(isp_hist_result_t), ISP_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(hist_ctlr->evt_que, ESP_ERR_NO_MEM, err1, TAG, "no mem for hist event queue");
    hist_ctlr->fsm = ISP_FSM_INIT;
    hist_ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    hist_ctlr->isp_proc = isp_proc;

    // Claim an hist controller
    ESP_GOTO_ON_ERROR(s_isp_claim_hist_controller(isp_proc, hist_ctlr), err1, TAG, "no available controller");
    // Register the hist ISR
    uint32_t intr_st_reg_addr = isp_ll_get_intr_status_reg_addr(isp_proc->hal.hw);
    int intr_priority = hist_cfg->intr_priority > 0 && hist_cfg->intr_priority <= 7 ? BIT(hist_cfg->intr_priority) : ESP_INTR_FLAG_LOWMED;
    ESP_GOTO_ON_ERROR(esp_intr_alloc_intrstatus(isp_hw_info.instances[isp_proc->proc_id].irq, ISP_INTR_ALLOC_FLAGS | intr_priority, intr_st_reg_addr, ISP_LL_EVENT_HIST_MASK,
                                                s_isp_hist_default_isr, hist_ctlr, &hist_ctlr->intr_handle), err2, TAG, "allocate interrupt failed");

    // Configure the hardware
    isp_ll_hist_set_mode(isp_proc->hal.hw, hist_cfg->hist_mode);
    isp_ll_hist_set_x_offset(isp_proc->hal.hw, hist_cfg->first_window_x_offs);
    isp_ll_hist_set_y_offset(isp_proc->hal.hw, hist_cfg->first_window_y_offs);
    isp_ll_hist_set_window_x_size(isp_proc->hal.hw, hist_cfg->window_x_size);
    isp_ll_hist_set_window_y_size(isp_proc->hal.hw, hist_cfg->window_y_size);
    isp_ll_hist_set_subwindow_weight(isp_proc->hal.hw, hist_cfg->hist_windows_weight);
    isp_ll_hist_set_segment_threshold(isp_proc->hal.hw, hist_cfg->hist_segment_threshold);
    if (hist_cfg->hist_mode == ISP_HIST_RGB) {
        isp_ll_hist_set_rgb_coefficient(isp_proc->hal.hw, &hist_cfg->rgb_coefficient);
    }

    *ret_hdl = hist_ctlr;

    return ESP_OK;

err2:
    s_isp_declaim_hist_controller(hist_ctlr);
err1:
    s_isp_hist_free_controller(hist_ctlr);

    return ret;
}

esp_err_t esp_isp_del_hist_controller(isp_hist_ctlr_t hist_ctlr)
{
    ESP_RETURN_ON_FALSE(hist_ctlr && hist_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(hist_ctlr->isp_proc->hist_ctlr == hist_ctlr, ESP_ERR_INVALID_ARG, TAG, "controller isn't in use");
    ESP_RETURN_ON_FALSE(hist_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");
    s_isp_declaim_hist_controller(hist_ctlr);

    s_isp_hist_free_controller(hist_ctlr);

    return ESP_OK;
}

esp_err_t esp_isp_hist_controller_enable(isp_hist_ctlr_t hist_ctlr)
{
    ESP_RETURN_ON_FALSE(hist_ctlr && hist_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(hist_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller isn't in init state");
    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_ERROR(esp_intr_enable(hist_ctlr->intr_handle), err, TAG, "failed to enable the HIST interrupt");
    isp_ll_hist_clk_enable(hist_ctlr->isp_proc->hal.hw, true);
    isp_ll_enable_intr(hist_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_HIST_MASK, true);
    hist_ctlr->fsm = ISP_FSM_ENABLE;

err:
    hist_ctlr->fsm = ISP_FSM_INIT;
    return ret;
}

esp_err_t esp_isp_hist_controller_disable(isp_hist_ctlr_t hist_ctlr)
{
    ESP_RETURN_ON_FALSE(hist_ctlr && hist_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(hist_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    isp_ll_enable_intr(hist_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_HIST_MASK, false);
    isp_ll_hist_clk_enable(hist_ctlr->isp_proc->hal.hw, false);
    esp_intr_disable(hist_ctlr->intr_handle);
    hist_ctlr->fsm = ISP_FSM_INIT;

    return ESP_OK;
}

esp_err_t esp_isp_hist_controller_reconfig(isp_hist_ctlr_t hist_ctlr, const esp_isp_hist_config_t *hist_cfg)
{
    ESP_RETURN_ON_FALSE(hist_ctlr && hist_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    int intr_priority = hist_cfg->intr_priority > 0 && hist_cfg->intr_priority <= 3 ? BIT(hist_cfg->intr_priority) : ESP_INTR_FLAG_LOWMED;
    ESP_RETURN_ON_FALSE(intr_priority == hist_ctlr->intr_priority, ESP_ERR_INVALID_ARG, TAG, "can't change interrupt priority after initialized");

    // Configure the hardware
    isp_ll_hist_set_mode(hist_ctlr->isp_proc->hal.hw, hist_cfg->hist_mode);
    isp_ll_hist_set_x_offset(hist_ctlr->isp_proc->hal.hw, hist_cfg->first_window_x_offs);
    isp_ll_hist_set_y_offset(hist_ctlr->isp_proc->hal.hw, hist_cfg->first_window_y_offs);
    isp_ll_hist_set_window_x_size(hist_ctlr->isp_proc->hal.hw, hist_cfg->window_x_size);
    isp_ll_hist_set_window_y_size(hist_ctlr->isp_proc->hal.hw, hist_cfg->window_y_size);
    isp_ll_hist_set_subwindow_weight(hist_ctlr->isp_proc->hal.hw, hist_cfg->hist_windows_weight);
    isp_ll_hist_set_segment_threshold(hist_ctlr->isp_proc->hal.hw, hist_cfg->hist_segment_threshold);
    if (hist_cfg->hist_mode == ISP_HIST_RGB) {
        isp_ll_hist_set_rgb_coefficient(hist_ctlr->isp_proc->hal.hw, &hist_cfg->rgb_coefficient);
    }

    return ESP_OK;
}

esp_err_t esp_isp_hist_controller_get_oneshot_statistics(isp_hist_ctlr_t hist_ctlr, int timeout_ms, isp_hist_result_t *out_res)
{
    ESP_RETURN_ON_FALSE_ISR(hist_ctlr && (out_res || timeout_ms == 0), ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(hist_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    esp_err_t ret = ESP_OK;
    TickType_t ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    // Reset the queue in case receiving the legacy data in the queue
    xQueueReset(hist_ctlr->evt_que);
    // Start the histogram reference statistics and waiting it done
    isp_ll_hist_enable(hist_ctlr->isp_proc->hal.hw, true);
    // Wait the statistics to finish and receive the result from the queue
    if ((ticks > 0) && xQueueReceive(hist_ctlr->evt_que, out_res, ticks) != pdTRUE) {
        ret = ESP_ERR_TIMEOUT;
    }
    // Stop the histogram reference statistics
    isp_ll_hist_enable(hist_ctlr->isp_proc->hal.hw, false);

    return ret;
}

esp_err_t esp_isp_hist_controller_start_continuous_statistics(isp_hist_ctlr_t hist_ctlr)
{
    ESP_RETURN_ON_FALSE_ISR(hist_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(hist_ctlr->fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller isn't in enable state");

    hist_ctlr->fsm = ISP_FSM_START;
    isp_ll_hist_enable(hist_ctlr->isp_proc->hal.hw, true);

    return ESP_OK;
}

esp_err_t esp_isp_hist_controller_stop_continuous_statistics(isp_hist_ctlr_t hist_ctlr)
{
    ESP_RETURN_ON_FALSE_ISR(hist_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(hist_ctlr->fsm == ISP_FSM_START, ESP_ERR_INVALID_STATE, TAG, "controller isn't in continuous state");

    isp_ll_hist_enable(hist_ctlr->isp_proc->hal.hw, false);
    hist_ctlr->fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
static void IRAM_ATTR s_isp_hist_default_isr(void *arg)
{
    isp_hist_ctlr_t hist_ctlr = (isp_hist_ctlr_t)arg;
    isp_proc_handle_t proc = hist_ctlr->isp_proc;

    uint32_t hist_events = isp_hal_check_clear_intr_event(&proc->hal, ISP_LL_EVENT_HIST_MASK);

    bool need_yield = false;

    if (hist_events & ISP_LL_EVENT_HIST_MASK) {
        isp_hist_ctlr_t hist_ctlr = proc->hist_ctlr;
        uint32_t hist_value[ISP_HIST_SEGMENT_NUMS];
        isp_ll_hist_get_histogram_value(proc->hal.hw, hist_value);
        // Get the statistics result
        esp_isp_hist_evt_data_t edata = {};
        for (int i = 0; i < ISP_HIST_SEGMENT_NUMS; i++) {
            edata.hist_result.hist_value[i] = hist_value[i];
        }
        // Invoke the callback if the callback is registered
        if (hist_ctlr->cbs.on_statistics_done) {
            need_yield |= hist_ctlr->cbs.on_statistics_done(hist_ctlr, &edata, hist_ctlr->user_data);
        }
        BaseType_t high_task_awake = false;
        // Send the event data to the queue, overwrite the legacy one if exist
        xQueueOverwriteFromISR(hist_ctlr->evt_que, &edata.hist_result, &high_task_awake);
        need_yield |= high_task_awake == pdTRUE;
        /* If started continuous sampling, then trigger the next hist sample */
        if (hist_ctlr->fsm == ISP_FSM_START) {
            isp_ll_hist_enable(hist_ctlr->isp_proc->hal.hw, false);
            isp_ll_hist_enable(hist_ctlr->isp_proc->hal.hw, true);
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t esp_isp_hist_register_event_callbacks(isp_hist_ctlr_t hist_ctlr, const esp_isp_hist_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(hist_ctlr && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(hist_ctlr->fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "detector isn't in the init state");
#if CONFIG_ISP_ISR_IRAM_SAFE
    if (cbs->on_statistics_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_env_change), ESP_ERR_INVALID_ARG, TAG, "on_env_change callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    hist_ctlr->cbs.on_statistics_done = cbs->on_statistics_done;
    hist_ctlr->user_data = user_data;

    return ESP_OK;
}
