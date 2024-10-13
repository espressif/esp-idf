/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include <stdatomic.h>
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "driver/isp_awb.h"
#include "esp_private/isp_private.h"

typedef struct isp_awb_controller_t {
    _Atomic isp_fsm_t                  fsm;
    portMUX_TYPE                       spinlock;
    intr_handle_t                      intr_handle;
    isp_proc_handle_t                  isp_proc;
    QueueHandle_t                      evt_que;
    esp_isp_awb_cbs_t                  cbs;
    void                               *user_data;
} isp_awb_controller_t;

static const char *TAG = "ISP_AWB";

/*---------------------------------------------
                AWB
----------------------------------------------*/
static esp_err_t s_isp_claim_awb_controller(isp_proc_handle_t isp_proc, isp_awb_ctlr_t awb_ctlr)
{
    assert(isp_proc && awb_ctlr);

    esp_err_t ret = ESP_ERR_NOT_FOUND;
    portENTER_CRITICAL(&isp_proc->spinlock);
    if (!isp_proc->awb_ctlr) {
        isp_proc->awb_ctlr = awb_ctlr;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&isp_proc->spinlock);

    return ret;
}

static void s_isp_declaim_awb_controller(isp_awb_ctlr_t awb_ctlr)
{
    if (awb_ctlr && awb_ctlr->isp_proc) {
        portENTER_CRITICAL(&awb_ctlr->isp_proc->spinlock);
        awb_ctlr->isp_proc->awb_ctlr = NULL;
        portEXIT_CRITICAL(&awb_ctlr->isp_proc->spinlock);
    }
}

static void s_isp_awb_free_controller(isp_awb_ctlr_t awb_ctlr)
{
    if (awb_ctlr) {
        if (awb_ctlr->intr_handle) {
            esp_intr_free(awb_ctlr->intr_handle);
        }
        if (awb_ctlr->evt_que) {
            vQueueDeleteWithCaps(awb_ctlr->evt_que);
        }
        free(awb_ctlr);
    }
}

static esp_err_t s_esp_isp_awb_config_hardware(isp_proc_handle_t isp_proc, const esp_isp_awb_config_t *awb_cfg)
{
    isp_ll_awb_set_sample_point(isp_proc->hal.hw, awb_cfg->sample_point);
    ESP_RETURN_ON_FALSE(isp_hal_awb_set_window_range(&isp_proc->hal, &awb_cfg->window),
                        ESP_ERR_INVALID_ARG, TAG, "invalid window");
    isp_u32_range_t lum_range = awb_cfg->white_patch.luminance;
    ESP_RETURN_ON_FALSE(isp_hal_awb_set_luminance_range(&isp_proc->hal, lum_range.min, lum_range.max),
                        ESP_ERR_INVALID_ARG, TAG, "invalid luminance range");
    isp_float_range_t rg_range = awb_cfg->white_patch.red_green_ratio;
    ESP_RETURN_ON_FALSE(rg_range.min < rg_range.max && rg_range.min >= 0 &&
                        isp_hal_awb_set_rg_ratio_range(&isp_proc->hal, rg_range.min, rg_range.max),
                        ESP_ERR_INVALID_ARG, TAG, "invalid range of Red Green ratio");
    isp_float_range_t bg_range = awb_cfg->white_patch.blue_green_ratio;
    ESP_RETURN_ON_FALSE(bg_range.min < bg_range.max && bg_range.min >= 0 &&
                        isp_hal_awb_set_bg_ratio_range(&isp_proc->hal, bg_range.min, bg_range.max),
                        ESP_ERR_INVALID_ARG, TAG, "invalid range of Blue to Green ratio");
    return ESP_OK;
}

esp_err_t esp_isp_new_awb_controller(isp_proc_handle_t isp_proc, const esp_isp_awb_config_t *awb_cfg, isp_awb_ctlr_t *ret_hdl)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(isp_proc && awb_cfg && ret_hdl, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    isp_awb_ctlr_t awb_ctlr = heap_caps_calloc(1, sizeof(isp_awb_controller_t), ISP_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(awb_ctlr, ESP_ERR_NO_MEM, TAG, "no mem for awb controller");
    awb_ctlr->evt_que = xQueueCreateWithCaps(1, sizeof(isp_awb_stat_result_t), ISP_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(awb_ctlr->evt_que, ESP_ERR_NO_MEM, err1, TAG, "no mem for awb event queue");
    atomic_init(&awb_ctlr->fsm, ISP_FSM_INIT);
    awb_ctlr->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    awb_ctlr->isp_proc = isp_proc;

    // Claim an AWB controller
    ESP_GOTO_ON_ERROR(s_isp_claim_awb_controller(isp_proc, awb_ctlr), err1, TAG, "no available controller");

    // Register the AWB ISR
    int intr_priority = (awb_cfg->intr_priority > 0 && awb_cfg->intr_priority <= 3) ? BIT(awb_cfg->intr_priority) : ESP_INTR_FLAG_LOWMED;
    ESP_GOTO_ON_ERROR(intr_priority != isp_proc->intr_priority, err2, TAG, "intr_priority error");
    ESP_GOTO_ON_ERROR(esp_isp_register_isr(awb_ctlr->isp_proc, ISP_SUBMODULE_AWB), err2, TAG, "fail to register ISR");

    // Configure the hardware
    isp_ll_awb_enable(isp_proc->hal.hw, false);
    isp_ll_awb_enable_algorithm_mode(isp_proc->hal.hw, true);
    ESP_GOTO_ON_ERROR(s_esp_isp_awb_config_hardware(isp_proc, awb_cfg), err2, TAG, "configure awb hardware failed");

    *ret_hdl = awb_ctlr;

    return ESP_OK;

err2:
    s_isp_declaim_awb_controller(awb_ctlr);
err1:
    s_isp_awb_free_controller(awb_ctlr);

    return ret;
}

esp_err_t esp_isp_del_awb_controller(isp_awb_ctlr_t awb_ctlr)
{
    ESP_RETURN_ON_FALSE(awb_ctlr && awb_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(awb_ctlr->isp_proc->awb_ctlr == awb_ctlr, ESP_ERR_INVALID_ARG, TAG, "controller isn't in use");
    ESP_RETURN_ON_FALSE(atomic_load(&awb_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

    ESP_RETURN_ON_FALSE(esp_isp_deregister_isr(awb_ctlr->isp_proc, ISP_SUBMODULE_AWB) == ESP_OK, ESP_FAIL, TAG, "fail to deregister ISR");
    s_isp_declaim_awb_controller(awb_ctlr);

    isp_ll_awb_enable_algorithm_mode(awb_ctlr->isp_proc->hal.hw, false);
    s_isp_awb_free_controller(awb_ctlr);

    return ESP_OK;
}

esp_err_t esp_isp_awb_controller_reconfig(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_config_t *awb_cfg)
{
    ESP_RETURN_ON_FALSE(awb_ctlr && awb_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&awb_ctlr->fsm) == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "controller not in enable state");

    return s_esp_isp_awb_config_hardware(awb_ctlr->isp_proc, awb_cfg);
}

esp_err_t esp_isp_awb_controller_enable(isp_awb_ctlr_t awb_ctlr)
{
    ESP_RETURN_ON_FALSE(awb_ctlr && awb_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&awb_ctlr->fsm, &expected_fsm, ISP_FSM_ENABLE),
                        ESP_ERR_INVALID_STATE, TAG, "controller not in init state");

    esp_err_t ret = ESP_OK;
    isp_ll_awb_clk_enable(awb_ctlr->isp_proc->hal.hw, true);
    isp_ll_enable_intr(awb_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AWB_MASK, true);

    return ret;
}

esp_err_t esp_isp_awb_controller_disable(isp_awb_ctlr_t awb_ctlr)
{
    ESP_RETURN_ON_FALSE(awb_ctlr && awb_ctlr->isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&awb_ctlr->fsm, &expected_fsm, ISP_FSM_INIT),
                        ESP_ERR_INVALID_STATE, TAG, "controller not in enable state");

    isp_ll_enable_intr(awb_ctlr->isp_proc->hal.hw, ISP_LL_EVENT_AWB_MASK, false);
    isp_ll_awb_clk_enable(awb_ctlr->isp_proc->hal.hw, false);
    esp_intr_disable(awb_ctlr->intr_handle);

    return ESP_OK;
}

esp_err_t esp_isp_awb_controller_get_oneshot_statistics(isp_awb_ctlr_t awb_ctlr, int timeout_ms, isp_awb_stat_result_t *out_res)
{
    ESP_RETURN_ON_FALSE(awb_ctlr && (out_res || timeout_ms == 0), ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    esp_err_t ret = ESP_OK;
    TickType_t ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);

    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&awb_ctlr->fsm, &expected_fsm, ISP_FSM_ONESHOT), ESP_ERR_INVALID_STATE, TAG, "controller isn't enabled or continuous statistics has starte");

    // Reset the queue in case receiving the legacy data in the queue
    xQueueReset(awb_ctlr->evt_que);
    // Start the AWB white patch statistics and waiting it done
    isp_ll_awb_enable(awb_ctlr->isp_proc->hal.hw, true);
    // Wait the statistics to finish and receive the result from the queue
    if ((ticks > 0) && xQueueReceive(awb_ctlr->evt_que, out_res, ticks) != pdTRUE) {
        ret = ESP_ERR_TIMEOUT;
    }
    // Stop the AWB white patch statistics
    isp_ll_awb_enable(awb_ctlr->isp_proc->hal.hw, false);

    atomic_store(&awb_ctlr->fsm, ISP_FSM_ENABLE);
    return ret;
}

esp_err_t esp_isp_awb_controller_start_continuous_statistics(isp_awb_ctlr_t awb_ctlr)
{
    ESP_RETURN_ON_FALSE(awb_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(awb_ctlr->cbs.on_statistics_done, ESP_ERR_INVALID_STATE, TAG, "invalid state: on_statistics_done callback not registered");

    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&awb_ctlr->fsm, &expected_fsm, ISP_FSM_CONTINUOUS), ESP_ERR_INVALID_STATE, TAG, "controller is not enabled yet");
    isp_ll_awb_enable(awb_ctlr->isp_proc->hal.hw, true);

    return ESP_OK;
}

esp_err_t esp_isp_awb_controller_stop_continuous_statistics(isp_awb_ctlr_t awb_ctlr)
{
    ESP_RETURN_ON_FALSE(awb_ctlr, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    isp_fsm_t expected_fsm = ISP_FSM_CONTINUOUS;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&awb_ctlr->fsm, &expected_fsm, ISP_FSM_ENABLE), ESP_ERR_INVALID_STATE, TAG, "controller is not enabled yet");
    isp_ll_awb_enable(awb_ctlr->isp_proc->hal.hw, false);

    return ESP_OK;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
bool IRAM_ATTR esp_isp_awb_isr(isp_proc_handle_t proc, uint32_t awb_events)
{
    bool need_yield = false;

    if (awb_events & ISP_LL_EVENT_AWB_FDONE) {
        isp_awb_ctlr_t awb_ctlr = proc->awb_ctlr;
        // Get the statistics result
        esp_isp_awb_evt_data_t edata = {
            .awb_result = {
                .white_patch_num = isp_ll_awb_get_white_patch_cnt(proc->hal.hw),
                .sum_r = isp_ll_awb_get_accumulated_r_value(proc->hal.hw),
                .sum_g = isp_ll_awb_get_accumulated_g_value(proc->hal.hw),
                .sum_b = isp_ll_awb_get_accumulated_b_value(proc->hal.hw),
            },
        };
        // Invoke the callback if the callback is registered
        if (awb_ctlr->cbs.on_statistics_done) {
            need_yield |= awb_ctlr->cbs.on_statistics_done(awb_ctlr, &edata, awb_ctlr->user_data);
        }
        BaseType_t high_task_awake = false;
        // Send the event data to the queue, overwrite the legacy one if exist
        xQueueOverwriteFromISR(awb_ctlr->evt_que, &edata.awb_result, &high_task_awake);
        need_yield |= high_task_awake == pdTRUE;
        /* If started continuous sampling, then trigger the next AWB sample */
        if (atomic_load(&awb_ctlr->fsm) == ISP_FSM_CONTINUOUS) {
            isp_ll_awb_enable(awb_ctlr->isp_proc->hal.hw, false);
            isp_ll_awb_enable(awb_ctlr->isp_proc->hal.hw, true);
        }
    }
    return need_yield;
}

esp_err_t esp_isp_awb_register_event_callbacks(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(awb_ctlr && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(atomic_load(&awb_ctlr->fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "controller not in init state");
#if CONFIG_ISP_ISR_IRAM_SAFE
    if (cbs->on_statistics_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_env_change), ESP_ERR_INVALID_ARG, TAG, "on_env_change callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    awb_ctlr->cbs.on_statistics_done = cbs->on_statistics_done;
    awb_ctlr->user_data = user_data;

    return ESP_OK;
}
