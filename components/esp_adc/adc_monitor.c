/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "adc_continuous_internal.h"
#include "soc/periph_defs.h"
#include "esp_adc/adc_monitor.h"

static const char *MNTOR_TAG = "adc_monitor";

/**
 * @brief context for adc continuous driver
 */
typedef struct adc_monitor_platform_t {
    adc_continuous_ctx_t    *continuous_ctx;     // ADC continuous driver context
    intr_handle_t           monitor_intr_handle; // monitor intr handler
    portMUX_TYPE            monitor_spinlock;    // spinlock
} adc_monitor_platform_t;

// Global context of adc monitor, other member will be lazy loaded
static adc_monitor_platform_t s_adc_monitor_platform = {.monitor_spinlock = portMUX_INITIALIZER_UNLOCKED};

#if CONFIG_IDF_TARGET_ESP32S2
// Monitor unit index need equal to ADC unit index on ESP32S2
static atomic_bool s_adc_monitor_claimed[SOC_ADC_DIGI_MONITOR_NUM] = {};
static esp_err_t s_adc_monitor_claim(adc_continuous_handle_t handle, adc_monitor_t *monitor_ctx, adc_unit_t unit)
{
    assert(handle && monitor_ctx);
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    bool false_var = false;
    if (atomic_compare_exchange_strong(&s_adc_monitor_claimed[unit], &false_var, true)) {
        monitor_ctx->monitor_id = unit;
        handle->adc_monitor[unit] = monitor_ctx;
        ret = ESP_OK;
    } else {
        ESP_LOGE(MNTOR_TAG, "monitor %d already in use", (int)unit);
    }
    return ret;
}

static esp_err_t s_adc_monitor_release(adc_monitor_t *monitor_ctx)
{
    assert(monitor_ctx);
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    bool true_var = true;
    if (atomic_compare_exchange_strong(&s_adc_monitor_claimed[monitor_ctx->monitor_id], &true_var, false)) {
        s_adc_monitor_platform.continuous_ctx->adc_monitor[monitor_ctx->monitor_id] = NULL;
        ret = ESP_OK;
    }
    return ret;
}

#else
static esp_err_t s_adc_monitor_claim(adc_continuous_handle_t handle, adc_monitor_t *monitor_ctx, adc_unit_t unit)
{
    (void)unit;
    assert(handle && monitor_ctx);
    portENTER_CRITICAL(&s_adc_monitor_platform.monitor_spinlock);
    for (int i = 0; i < SOC_ADC_DIGI_MONITOR_NUM; i++) {
        if (!handle->adc_monitor[i]) {
            monitor_ctx->monitor_id = i;
            handle->adc_monitor[i] = monitor_ctx;
            portEXIT_CRITICAL(&s_adc_monitor_platform.monitor_spinlock);
            return ESP_OK;
        }
    }
    portEXIT_CRITICAL(&s_adc_monitor_platform.monitor_spinlock);
    ESP_LOGE(MNTOR_TAG, "no free monitor");
    return ESP_ERR_NOT_FOUND;
}

static esp_err_t s_adc_monitor_release(adc_monitor_t *monitor_ctx)
{
    assert(monitor_ctx);
    portENTER_CRITICAL(&s_adc_monitor_platform.monitor_spinlock);
    s_adc_monitor_platform.continuous_ctx->adc_monitor[monitor_ctx->monitor_id] = NULL;
    portEXIT_CRITICAL(&s_adc_monitor_platform.monitor_spinlock);

    return ESP_OK;
}
#endif

static void IRAM_ATTR s_adc_digi_monitor_isr(void *args)
{
    bool need_yield = false;
    uint32_t intr_val = *((uint32_t *)adc_ll_digi_monitor_get_intr_status_addr());

    // clear all intr flags as have save intr status in `intr_val`
    adc_ll_digi_monitor_clear_intr();

    for (uint8_t i = 0; i < SOC_ADC_DIGI_MONITOR_NUM; i++) {
        adc_monitor_handle_t monitor_handle = s_adc_monitor_platform.continuous_ctx->adc_monitor[i];

        // check if high threshold alert
        if (intr_val & ADC_LL_GET_HIGH_THRES_MASK(i)) {
            assert(monitor_handle);
            assert(monitor_handle->monitor_id == i);

            if (monitor_handle->cbs.on_over_high_thresh) {
                adc_monitor_evt_data_t event_data = {};
                need_yield |= monitor_handle->cbs.on_over_high_thresh(monitor_handle, &event_data, monitor_handle->user_data);
            }
        }
        // check if low threshold alert
        if (intr_val & ADC_LL_GET_LOW_THRES_MASK(i)) {
            assert(monitor_handle);
            assert(monitor_handle->monitor_id == i);

            if (monitor_handle->cbs.on_below_low_thresh) {
                adc_monitor_evt_data_t event_data = {};
                need_yield |= monitor_handle->cbs.on_below_low_thresh(monitor_handle, &event_data, monitor_handle->user_data);
            }
        }
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t adc_monitor_intr_alloc(void)
{
    esp_err_t ret = ESP_OK;
    int intr_flags = ESP_INTR_FLAG_LOWMED;
#if CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE
    intr_flags |= ESP_INTR_FLAG_IRAM;
#endif

#if SOC_ADC_TEMPERATURE_SHARE_INTR
    intr_flags |= ESP_INTR_FLAG_SHARED;
    ret = esp_intr_alloc_intrstatus(ETS_APB_ADC_INTR_SOURCE, intr_flags,
                                    (uint32_t)adc_ll_digi_monitor_get_intr_status_addr(),
                                    ADC_LL_THRES_ALL_INTR_ST_M, s_adc_digi_monitor_isr, NULL, &s_adc_monitor_platform.monitor_intr_handle);
#else

    ret = esp_intr_alloc(ETS_APB_ADC_INTR_SOURCE, intr_flags, s_adc_digi_monitor_isr, NULL, &s_adc_monitor_platform.monitor_intr_handle);
#endif  //SOC_ADC_TEMPERATURE_SHARE_INTR
    return ret;
}

//-------------------------------------------PUBLIC APIs--------------------------------------------//
esp_err_t adc_new_continuous_monitor(adc_continuous_handle_t handle, const adc_monitor_config_t *monitor_cfg, adc_monitor_handle_t *ret_handle)
{
    esp_err_t ret;
    ESP_RETURN_ON_FALSE(handle && monitor_cfg && ret_handle, ESP_ERR_INVALID_ARG, MNTOR_TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(monitor_cfg->adc_unit < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, MNTOR_TAG, "invalid adc_unit");
    ESP_RETURN_ON_FALSE(handle->fsm == ADC_FSM_INIT, ESP_ERR_INVALID_STATE, MNTOR_TAG, "ADC continuous driver should be in init state");
#if CONFIG_IDF_TARGET_ESP32S2
    ESP_RETURN_ON_FALSE(!((monitor_cfg->h_threshold >= 0) && (monitor_cfg->l_threshold >= 0)), ESP_ERR_NOT_SUPPORTED, MNTOR_TAG, "ESP32S2 support only one threshold");
#endif

    // alloc handler memory
    adc_monitor_t *monitor_ctx = heap_caps_calloc(1, sizeof(adc_monitor_t), MALLOC_CAP_INTERNAL);
    ESP_RETURN_ON_FALSE(monitor_ctx, ESP_ERR_NO_MEM, MNTOR_TAG, "no mem");

    // alloc monitor hardware
    ESP_GOTO_ON_ERROR(s_adc_monitor_claim(handle, monitor_ctx, monitor_cfg->adc_unit), claim_err, MNTOR_TAG, "ADC monitor claim failed");
    memcpy(&monitor_ctx->config, monitor_cfg, sizeof(adc_monitor_config_t));
    s_adc_monitor_platform.continuous_ctx = handle;

    // alloc cpu intr
    portENTER_CRITICAL(&s_adc_monitor_platform.monitor_spinlock);
    bool alloc_intr = !s_adc_monitor_platform.monitor_intr_handle;
    portEXIT_CRITICAL(&s_adc_monitor_platform.monitor_spinlock);
    if (alloc_intr) {
        ESP_GOTO_ON_ERROR(adc_monitor_intr_alloc(), intr_err, MNTOR_TAG, "esp intr alloc failed");
    }

    *ret_handle = monitor_ctx;
    return ESP_OK;

intr_err:
    s_adc_monitor_release(monitor_ctx);
claim_err:
    free(monitor_ctx);
    return ret;
}

esp_err_t adc_continuous_monitor_register_event_callbacks(adc_monitor_handle_t monitor_handle, const adc_monitor_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(monitor_handle && cbs, ESP_ERR_INVALID_ARG, MNTOR_TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(monitor_handle->fsm == ADC_MONITOR_FSM_INIT, ESP_ERR_INVALID_STATE, MNTOR_TAG, "monitor should be in init state");
#if CONFIG_IDF_TARGET_ESP32S2
    ESP_RETURN_ON_FALSE(!(cbs->on_below_low_thresh && cbs->on_over_high_thresh), ESP_ERR_NOT_SUPPORTED, MNTOR_TAG, "ESP32S2 support only one threshold");
#endif

    // If iram_safe enabled, check if user_data and cbs is iram_safe
#if CONFIG_ADC_CONTINUOUS_ISR_IRAM_SAFE
    if (cbs->on_over_high_thresh) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_over_high_thresh), ESP_ERR_INVALID_ARG, MNTOR_TAG, "on_over_high_thresh func not in iram");
    }
    if (cbs->on_below_low_thresh) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_below_low_thresh), ESP_ERR_INVALID_ARG, MNTOR_TAG, "on_below_low_thresh func not in iram");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_dram(user_data) || esp_ptr_in_diram_dram(user_data), ESP_ERR_INVALID_ARG, MNTOR_TAG, "user_data not in iram");
    }
#endif

    memcpy(&monitor_handle->cbs, cbs, sizeof(adc_monitor_evt_cbs_t));
    monitor_handle->user_data = user_data;
    return ESP_OK;
}

esp_err_t adc_continuous_monitor_enable(adc_monitor_handle_t monitor_handle)
{
    ESP_RETURN_ON_FALSE_ISR(monitor_handle, ESP_ERR_INVALID_ARG, MNTOR_TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(monitor_handle->fsm == ADC_MONITOR_FSM_INIT, ESP_ERR_INVALID_STATE, MNTOR_TAG, "monitor should be in init state");

    // start monitor
    adc_ll_digi_monitor_user_start(monitor_handle->monitor_id, ((monitor_handle->config.h_threshold >= 0) || (monitor_handle->config.l_threshold >= 0)));
    monitor_handle->fsm = ADC_MONITOR_FSM_ENABLED;
    return ESP_OK;
}

esp_err_t adc_continuous_monitor_disable(adc_monitor_handle_t monitor_handle)
{
    ESP_RETURN_ON_FALSE_ISR(monitor_handle, ESP_ERR_INVALID_ARG, MNTOR_TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(monitor_handle->fsm == ADC_MONITOR_FSM_ENABLED, ESP_ERR_INVALID_STATE, MNTOR_TAG, "monitor not in running");

    // stop monitor
    adc_ll_digi_monitor_user_start(monitor_handle->monitor_id, false);
    monitor_handle->fsm = ADC_MONITOR_FSM_INIT;
    return ESP_OK;
}

esp_err_t adc_del_continuous_monitor(adc_monitor_handle_t monitor_handle)
{
    ESP_RETURN_ON_FALSE(monitor_handle, ESP_ERR_INVALID_ARG, MNTOR_TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE((monitor_handle->fsm == ADC_MONITOR_FSM_INIT) && (s_adc_monitor_platform.continuous_ctx->fsm == ADC_FSM_INIT), \
                        ESP_ERR_INVALID_STATE, MNTOR_TAG, "monitor and ADC continuous driver should all stopped");
    ESP_RETURN_ON_ERROR(s_adc_monitor_release(monitor_handle), MNTOR_TAG, "monitor not find or isn't in use");

    for (int i = 0; i < SOC_ADC_DIGI_MONITOR_NUM; i++) {
        if (s_adc_monitor_platform.continuous_ctx->adc_monitor[i]) {
            // If any other monitor not freed, then delete self and exit now. del_monitor is complete
            free(monitor_handle);
            return ESP_OK;
        }
    }

    // If no monitor is using, then release intr handle as well
    ESP_RETURN_ON_ERROR(esp_intr_free(s_adc_monitor_platform.monitor_intr_handle), MNTOR_TAG, "esp intr release failed\n");
    s_adc_monitor_platform.monitor_intr_handle = NULL;
    free(monitor_handle);
    return ESP_OK;
}
