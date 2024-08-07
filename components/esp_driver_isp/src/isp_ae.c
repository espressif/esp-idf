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
#include "driver/isp_ae.h"
#include "esp_private/isp_private.h"

static const char *TAG = "ISP_AE";

typedef struct isp_ae_controller_t {
    int                                id;
    _Atomic isp_fsm_t                  fsm;
    portMUX_TYPE                       spinlock;
    intr_handle_t                      intr_handle;
    int                                intr_priority;
    isp_proc_handle_t                  isp_proc;
    QueueHandle_t                      evt_que;
    int                                low_thresh;
    int                                high_thresh;
    esp_isp_ae_env_detector_evt_cbs_t  cbs;
    void                               *user_data;
} isp_ae_controller_t;

/*---------------------------------------------
                AE
----------------------------------------------*/
static esp_err_t s_isp_claim_ae_controller(isp_proc_handle_t isp_proc, isp_ae_ctlr_t ae_ctlr)
{
    assert(isp_proc && ae_ctlr);

    esp_err_t ret = ESP_ERR_NOT_FOUND;
    portENTER_CRITICAL(&isp_proc->spinlock);
    if (!isp_proc->ae_ctlr) {
        isp_proc->ae_ctlr = ae_ctlr;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&isp_proc->spinlock);

    return ret;
}

static void s_isp_declaim_ae_controller(isp_ae_ctlr_t ae_ctlr)
{
    if (ae_ctlr && ae_ctlr->isp_proc) {
        portENTER_CRITICAL(&ae_ctlr->isp_proc->spinlock);
        ae_ctlr->isp_proc->ae_ctlr = NULL;
        portEXIT_CRITICAL(&ae_ctlr->isp_proc->spinlock);
    }
}

static void s_isp_ae_free_controller(isp_ae_ctlr_t ae_ctlr)
{
    if (ae_ctlr) {
        if (ae_ctlr->intr_handle) {
            esp_intr_free(ae_ctlr->intr_handle);
        }
        if (ae_ctlr->evt_que) {
            vQueueDeleteWithCaps(ae_ctlr->evt_que);
        }
        free(ae_ctlr);
    }
}

esp_err_t esp_isp_new_ae_controller(isp_proc_handle_t isp_proc, const esp_isp_ae_config_t *ae_config, isp_ae_ctlr_t *ret_hdl)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(isp_proc && ae_config && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    ESP_RETURN_ON_FALSE(((ae_config->window.top_left.x < ISP_LL_AE_WINDOW_MAX_RANGE) &&
                         (ae_config->window.btm_right.x >= ae_config->window.top_left.x) &&
                         (ae_config->window.btm_right.x < ISP_LL_AE_WINDOW_MAX_RANGE) &&
                         (ae_config->window.top_left.y < ISP_LL_AE_WINDOW_MAX_RANGE) &&
                         (ae_config->window.btm_right.y >= ae_config->window.top_left.y) &&
                         (ae_config->window.btm_right.y < ISP_LL_AE_WINDOW_MAX_RANGE)), ESP_ERR_INVALID_ARG, TAG, "invalid window");

    isp_ae_ctlr_t ae_ctlr = heap_caps_calloc(1, sizeof(isp_ae_controller_t), ISP_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(ae_ctlr, ESP_ERR_NO_MEM, TAG, "no mem");
    ae_ctlr->evt_que = xQueueCreateWithCaps(1, sizeof(isp_ae_result_t), ISP_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(ae_ctlr->evt_que, ESP_ERR_NO_MEM, err1, TAG, "no mem for ae event queue");

    atomic_init(&ae_ctlr->fsm, ISP_FSM_INIT);

    ae_ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    ae_ctlr->isp_proc = isp_proc;

    //claim an AE controller
    ESP_GOTO_ON_ERROR(s_isp_claim_ae_controller(isp_proc, ae_ctlr), err1, TAG, "no available controller");

    // Register the AE ISR
    int intr_priority = (ae_config->intr_priority > 0 && ae_config->intr_priority <= 3) ? BIT(ae_config->intr_priority) : ESP_INTR_FLAG_LOWMED;
    ESP_GOTO_ON_ERROR(intr_priority != isp_proc->intr_priority, err2, TAG, "intr_priority error");
    ESP_GOTO_ON_ERROR(esp_isp_register_isr(ae_ctlr->isp_proc, ISP_SUBMODULE_AE), err2, TAG, "fail to register ISR");

    isp_ll_ae_set_sample_point(isp_proc->hal.hw, ae_config->sample_point);
    isp_ll_ae_enable(isp_proc->hal.hw, false);
    isp_hal_ae_window_config(&isp_proc->hal, &ae_config->window);

    isp_ll_clear_intr(isp_proc->hal.hw, ISP_LL_EVENT_AE_MASK);

    *ret_hdl = ae_ctlr;

    return ESP_OK;
err2:
    s_isp_declaim_ae_controller(ae_ctlr);
err1:
    s_isp_ae_free_controller(ae_ctlr);

    return ret;
}

esp_err_t esp_isp_del_ae_controller(isp_ae_ctlr_t ae_ctlr)
{
    ESP_RETURN_ON_FALSE(ae_ctlr && ae_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(ae_ctlr->isp_proc->ae_ctlr == ae_ctlr, ESP_ERR_INVALID_ARG, TAG, "controller isn't in use");
    ESP_RETURN_ON_FALSE(atomic_load(&ae_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

    // Deregister the AE ISR
    ESP_RETURN_ON_FALSE(esp_isp_deregister_isr(ae_ctlr->isp_proc, ISP_SUBMODULE_AE) == ESP_OK, ESP_FAIL, TAG, "fail to deregister ISR");

    s_isp_declaim_ae_controller(ae_ctlr);
    s_isp_ae_free_controller(ae_ctlr);

    return ESP_OK;
}

esp_err_t esp_isp_ae_controller_enable(isp_ae_ctlr_t ae_ctlr)
{
    ESP_RETURN_ON_FALSE(ae_ctlr && ae_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&ae_ctlr->fsm, &expected_fsm, ISP_FSM_ENABLE),
                        ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

    isp_ll_ae_clk_enable(ae_ctlr->isp_proc->hal.hw, true);
    isp_ll_enable_intr(ae_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AE_MASK, true);
    isp_ll_ae_enable(ae_ctlr->isp_proc->hal.hw, true);

    return ESP_OK;
}

esp_err_t esp_isp_ae_controller_disable(isp_ae_ctlr_t ae_ctlr)
{
    ESP_RETURN_ON_FALSE(ae_ctlr && ae_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&ae_ctlr->fsm, &expected_fsm, ISP_FSM_INIT),
                        ESP_ERR_INVALID_STATE, TAG, "controller not in enable state");

    isp_ll_ae_clk_enable(ae_ctlr->isp_proc->hal.hw, false);
    isp_ll_enable_intr(ae_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AE_MASK, false);
    isp_ll_ae_enable(ae_ctlr->isp_proc->hal.hw, false);
    esp_intr_disable(ae_ctlr->intr_handle);

    return ESP_OK;
}

esp_err_t esp_isp_ae_controller_get_oneshot_statistics(isp_ae_ctlr_t ae_ctlr, int timeout_ms, isp_ae_result_t *out_res)
{
    ESP_RETURN_ON_FALSE_ISR(ae_ctlr && (out_res || timeout_ms == 0), ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    esp_err_t ret = ESP_OK;
    TickType_t ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);

    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&ae_ctlr->fsm, &expected_fsm, ISP_FSM_ONESHOT), ESP_ERR_INVALID_STATE, TAG, "controller isn't enabled or continuous statistics has started");

    // Reset the queue in case receiving the legacy data in the queue
    xQueueReset(ae_ctlr->evt_que);

    // Disable the env detector when manual statistics.
    // Otherwise, the env detector results may overwrite the manual statistics results when the statistics results are not read out in time
    isp_ll_ae_env_detector_set_thresh(ae_ctlr->isp_proc->hal.hw, 0, 0);
    // Trigger the AE statistics manually
    isp_ll_ae_manual_update(ae_ctlr->isp_proc->hal.hw);
    // Wait the statistics to finish and receive the result from the queue
    if ((ticks > 0) && xQueueReceive(ae_ctlr->evt_que, out_res, ticks) != pdTRUE) {
        ret = ESP_ERR_TIMEOUT;
    }
    // Re-enable the env detector after manual statistics.
    isp_ll_ae_env_detector_set_thresh(ae_ctlr->isp_proc->hal.hw, ae_ctlr->low_thresh, ae_ctlr->high_thresh);

    atomic_store(&ae_ctlr->fsm, ISP_FSM_ENABLE);
    return ret;
}

esp_err_t esp_isp_ae_controller_start_continuous_statistics(isp_ae_ctlr_t ae_ctlr)
{
    ESP_RETURN_ON_FALSE_ISR(ae_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&ae_ctlr->fsm, &expected_fsm, ISP_FSM_CONTINUOUS), ESP_ERR_INVALID_STATE, TAG, "controller is not enabled yet");
    isp_ll_ae_manual_update(ae_ctlr->isp_proc->hal.hw);

    return ESP_OK;
}

esp_err_t esp_isp_ae_controller_stop_continuous_statistics(isp_ae_ctlr_t ae_ctlr)
{
    ESP_RETURN_ON_FALSE_ISR(ae_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    isp_fsm_t expected_fsm = ISP_FSM_CONTINUOUS;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&ae_ctlr->fsm, &expected_fsm, ISP_FSM_ENABLE),
                            ESP_ERR_INVALID_STATE, TAG, "controller is not running");
    return ESP_OK;
}

/*---------------------------------------------
                AE Env Detector
----------------------------------------------*/
esp_err_t esp_isp_ae_controller_set_env_detector(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_config_t *env_config)
{
    ESP_RETURN_ON_FALSE(ae_ctlr && env_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(env_config->interval > 0, ESP_ERR_INVALID_STATE, TAG, "invalid interval, should be greater than 0");
    ESP_RETURN_ON_FALSE(atomic_load(&ae_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

    isp_ll_clear_intr(ae_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AE_ENV);
    isp_ll_ae_env_detector_set_period(ae_ctlr->isp_proc->hal.hw, env_config->interval);
    isp_ll_enable_intr(ae_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AE_ENV, true);

    return ESP_OK;
}

esp_err_t esp_isp_ae_env_detector_register_event_callbacks(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_detector_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(ae_ctlr && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(atomic_load(&ae_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

#if CONFIG_ISP_ISR_IRAM_SAEE
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
    ae_ctlr->cbs.on_env_statistics_done = cbs->on_env_statistics_done;
    ae_ctlr->cbs.on_env_change = cbs->on_env_change;
    ae_ctlr->user_data = user_data;

    return ESP_OK;
}

esp_err_t esp_isp_ae_controller_set_env_detector_threshold(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_thresh_t *env_thresh)
{
    ESP_RETURN_ON_FALSE_ISR(ae_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR((env_thresh->low_thresh != 0 && env_thresh->high_thresh != 0) && (env_thresh->low_thresh <= env_thresh->high_thresh), ESP_ERR_INVALID_STATE, TAG, "invalid AE env detector thresh");
    ESP_RETURN_ON_FALSE(atomic_load(&ae_ctlr->fsm) == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller not in enable state");

    ae_ctlr->low_thresh = env_thresh->low_thresh;
    ae_ctlr->high_thresh = env_thresh->high_thresh;

    isp_ll_ae_env_detector_set_thresh(ae_ctlr->isp_proc->hal.hw, ae_ctlr->low_thresh, ae_ctlr->high_thresh);

    return ESP_OK;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
bool IRAM_ATTR esp_isp_ae_isr(isp_proc_handle_t proc, uint32_t ae_events)
{
    isp_ae_ctlr_t ae_ctlr = proc->ae_ctlr;

    bool need_yield = false;
    esp_isp_ae_env_detector_evt_data_t edata = {};

    if (ae_events) {
        // Get the statistics result
        int block_id = 0;
        for (int i = 0; i < SOC_ISP_AE_BLOCK_X_NUMS; i++) {
            for (int j = 0; j < SOC_ISP_AE_BLOCK_Y_NUMS; j++) {
                edata.ae_result.luminance[i][j] = isp_ll_ae_get_block_mean_lum(proc->hal.hw, block_id);
                block_id++;
            }
        }
        BaseType_t high_task_awake = false;
        // Send the event data to the queue, overwrite the legacy one if exist
        xQueueOverwriteFromISR(ae_ctlr->evt_que, &edata.ae_result, &high_task_awake);
        // Invoke the callback if the callback is registered
        need_yield |= high_task_awake == pdTRUE;

        /* If started continuous sampling, then trigger the next AE sample */
        if (atomic_load(&ae_ctlr->fsm) == ISP_FSM_CONTINUOUS) {
            isp_ll_ae_manual_update(ae_ctlr->isp_proc->hal.hw);
        }
    }

    /**
     * Deal with the interrupts.
     * Now only one detector.
     * Should decide a detector instance according to the hw event.
     */
    if (ae_events & ISP_LL_EVENT_AE_FDONE) {
        if (ae_ctlr->cbs.on_env_statistics_done) {
            need_yield |= ae_ctlr->cbs.on_env_statistics_done(ae_ctlr, &edata, ae_ctlr->user_data);
        }
    }
    if (ae_events & ISP_LL_EVENT_AE_ENV) {
        if (ae_ctlr->cbs.on_env_change) {
            need_yield |= ae_ctlr->cbs.on_env_change(ae_ctlr, &edata, ae_ctlr->user_data);
        }
    }
    return need_yield;
}
