/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "soc/clk_tree_defs.h"
#include "soc/touch_sensor_periph.h"
#include "driver/rtc_io.h"
#include "driver/touch_sens.h"

#if SOC_TOUCH_SENSOR_VERSION <= 2
#include "esp_private/rtc_ctrl.h"
#else
#include "soc/interrupts.h"
#include "esp_intr_alloc.h"
#endif

#if CONFIG_TOUCH_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_check.h"
#include "touch_sens_private.h"

#define TOUCH_CHANNEL_CHECK(num)        ESP_RETURN_ON_FALSE(num >= TOUCH_MIN_CHAN_ID && num <= TOUCH_MAX_CHAN_ID,  \
                                        ESP_ERR_INVALID_ARG, TAG, "The channel number is out of supported range");

static const char *TAG = "touch";

touch_sensor_handle_t g_touch = NULL;

static void touch_channel_pin_init(int id)
{
    gpio_num_t pin = touch_sensor_channel_io_map[id];
    rtc_gpio_init(pin);
    rtc_gpio_set_direction(pin, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(pin);
    rtc_gpio_pullup_dis(pin);
}

static void s_touch_free_resource(touch_sensor_handle_t sens_handle)
{
    if (!sens_handle) {
        return;
    }
    if (sens_handle->mutex) {
        vSemaphoreDeleteWithCaps(sens_handle->mutex);
        sens_handle->mutex = NULL;
    }
    free(g_touch);
    g_touch = NULL;
}

esp_err_t touch_sensor_new_controller(const touch_sensor_config_t *sens_cfg, touch_sensor_handle_t *ret_sens_handle)
{
#if CONFIG_TOUCH_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    TOUCH_NULL_POINTER_CHECK(sens_cfg);
    TOUCH_NULL_POINTER_CHECK(ret_sens_handle);
    ESP_RETURN_ON_FALSE(!g_touch, ESP_ERR_INVALID_STATE, TAG, "Touch sensor has been allocated");

    g_touch = (touch_sensor_handle_t)heap_caps_calloc(1, sizeof(struct touch_sensor_s), TOUCH_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(g_touch, ESP_ERR_NO_MEM, TAG, "No memory for touch sensor struct");

    g_touch->mutex = xSemaphoreCreateMutexWithCaps(TOUCH_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(g_touch->mutex, ESP_ERR_NO_MEM, err, TAG, "No memory for mutex semaphore");

    touch_priv_enable_module(true);
    ESP_GOTO_ON_ERROR(touch_priv_config_controller(g_touch, sens_cfg), err, TAG, "Failed to configure the touch controller");
#if SOC_TOUCH_SENSOR_VERSION <= 2
    ESP_GOTO_ON_ERROR(rtc_isr_register(touch_priv_default_intr_handler, NULL, TOUCH_LL_INTR_MASK_ALL, 0), err, TAG, "Failed to register interrupt handler");
#else
    ESP_GOTO_ON_ERROR(esp_intr_alloc(ETS_LP_TOUCH_INTR_SOURCE, TOUCH_INTR_ALLOC_FLAGS, touch_priv_default_intr_handler, NULL, &(g_touch->intr_handle)),
                      err, TAG, "Failed to register interrupt handler");
#endif
    *ret_sens_handle = g_touch;
    return ret;

err:
    touch_priv_enable_module(false);
    s_touch_free_resource(g_touch);
    return ret;
}

esp_err_t touch_sensor_del_controller(touch_sensor_handle_t sens_handle)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);
    ESP_RETURN_ON_FALSE(g_touch == sens_handle, ESP_ERR_INVALID_ARG, TAG, "The input touch sensor handle is unmatched");

    esp_err_t ret = ESP_OK;
    // Take the semaphore to make sure the touch has stopped
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Touch sensor has not disabled");
    FOR_EACH_TOUCH_CHANNEL(i) {
        ESP_GOTO_ON_FALSE(!sens_handle->ch[i], ESP_ERR_INVALID_STATE, err, TAG, "There are still some touch channels not deleted");
    }

    ESP_GOTO_ON_ERROR(touch_priv_deinit_controller(sens_handle), err, TAG, "Failed to deinitialize the controller");
#if SOC_TOUCH_SENSOR_VERSION <= 2
    ESP_GOTO_ON_ERROR(rtc_isr_deregister(touch_priv_default_intr_handler, NULL), err, TAG, "Failed to deregister the interrupt handler");
#else
    ESP_GOTO_ON_ERROR(esp_intr_free(sens_handle->intr_handle), err, TAG, "Failed to deregister the interrupt handler");
#endif
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_ll_intr_disable(TOUCH_LL_INTR_MASK_ALL);
    touch_ll_clear_active_channel_status();
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    touch_priv_enable_module(false);
    s_touch_free_resource(sens_handle);
err:
    if (g_touch && g_touch->mutex) {
        xSemaphoreGive(g_touch->mutex);
    }
    return ret;
}

esp_err_t touch_sensor_new_channel(touch_sensor_handle_t sens_handle, int chan_id,
                                   const touch_channel_config_t *chan_cfg,
                                   touch_channel_handle_t *ret_chan_handle)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);
    TOUCH_NULL_POINTER_CHECK(chan_cfg);
    TOUCH_NULL_POINTER_CHECK(ret_chan_handle);
    TOUCH_CHANNEL_CHECK(chan_id);
    ESP_RETURN_ON_FALSE(g_touch == sens_handle, ESP_ERR_INVALID_ARG, TAG, "The input touch sensor handle is unmatched");

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);

    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err2, TAG, "Please disable the touch sensor first");
    ESP_GOTO_ON_FALSE(!sens_handle->ch[chan_id], ESP_ERR_INVALID_STATE, err2, TAG, "The channel %d has been registered", chan_id);

    sens_handle->ch[chan_id] = (touch_channel_handle_t)heap_caps_calloc(1, sizeof(struct touch_channel_s), TOUCH_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(sens_handle->ch[chan_id], ESP_ERR_NO_MEM, err2, TAG, "No memory for touch channel");
    sens_handle->ch[chan_id]->id = chan_id;
    sens_handle->ch[chan_id]->base = sens_handle;
    sens_handle->ch[chan_id]->is_prox_chan = false;

    /* Init the channel */
    ESP_GOTO_ON_ERROR(touch_priv_config_channel(sens_handle->ch[chan_id], chan_cfg),
                      err1, TAG, "Failed to configure the touch channel %d", chan_id);
    touch_channel_pin_init(chan_id);
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
#if SOC_TOUCH_SENSOR_VERSION == 2
    touch_ll_reset_chan_benchmark(1 << chan_id);
#endif
    sens_handle->chan_mask |= 1 << chan_id;
    touch_ll_set_channel_mask(sens_handle->chan_mask);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    *ret_chan_handle = sens_handle->ch[chan_id];

    xSemaphoreGive(sens_handle->mutex);
    return ret;
err1:
    free(sens_handle->ch[chan_id]);
    sens_handle->ch[chan_id] = NULL;
err2:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_del_channel(touch_channel_handle_t chan_handle)
{
    TOUCH_NULL_POINTER_CHECK(chan_handle);

    esp_err_t ret = ESP_OK;
    touch_sensor_handle_t sens_handle = chan_handle->base;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);

    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");
#if SOC_TOUCH_SENSOR_VERSION == 2
    if (sens_handle->guard_chan == chan_handle || (BIT(chan_handle->id) & sens_handle->shield_chan_mask)) {
        ESP_GOTO_ON_ERROR(touch_sensor_config_waterproof(sens_handle, NULL), err, TAG, "Failed to disable waterproof on this channel");
    }
    if (sens_handle->sleep_chan == chan_handle) {
        ESP_GOTO_ON_ERROR(touch_sensor_config_sleep_channel(sens_handle, NULL), err, TAG, "Failed to disable sleep function on this channel");
    }
#endif
    int id = chan_handle->id;
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    sens_handle->chan_mask &= ~(1UL << id);
    touch_ll_set_channel_mask(sens_handle->chan_mask);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    free(g_touch->ch[id]);
    g_touch->ch[id] = NULL;
err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_reconfig_controller(touch_sensor_handle_t sens_handle, const touch_sensor_config_t *sens_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);
    TOUCH_NULL_POINTER_CHECK(sens_cfg);
    ESP_RETURN_ON_FALSE(sens_cfg->meas_interval_us >= 0, ESP_ERR_INVALID_ARG, TAG, "interval_us should be a positive value");

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    ESP_GOTO_ON_ERROR(touch_priv_config_controller(sens_handle, sens_cfg), err, TAG, "Configure touch controller failed");

err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_enable(touch_sensor_handle_t sens_handle)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);

    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Touch sensor has already enabled");
    ESP_GOTO_ON_FALSE(sens_handle->sample_cfg_num, ESP_ERR_INVALID_STATE, err, TAG, "No sample configuration was added to the touch controller");

    sens_handle->is_enabled = true;
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_ll_intr_clear(TOUCH_LL_INTR_MASK_ALL);
    touch_ll_intr_enable(TOUCH_LL_INTR_MASK_ALL);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
#if SOC_TOUCH_SUPPORT_PROX_SENSING
    /* Reset the cached data of proximity channel */
    FOR_EACH_TOUCH_CHANNEL(i) {
        if (sens_handle->ch[i] && sens_handle->ch[i]->is_prox_chan) {
            sens_handle->ch[i]->prox_cnt = 0;
            memset(sens_handle->ch[i]->prox_val, 0, sizeof(sens_handle->ch[i]->prox_val[0]) * TOUCH_SAMPLE_CFG_NUM);
        }
    }
#endif

err:
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_disable(touch_sensor_handle_t sens_handle)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Touch sensor has not enabled");

    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_ll_intr_disable(TOUCH_LL_INTR_MASK_ALL);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    sens_handle->is_enabled = false;

err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_reconfig_channel(touch_channel_handle_t chan_handle, const touch_channel_config_t *chan_cfg)
{
    TOUCH_NULL_POINTER_CHECK(chan_handle);
    TOUCH_NULL_POINTER_CHECK(chan_cfg);

    esp_err_t ret = ESP_OK;
    touch_sensor_handle_t sens_handle = chan_handle->base;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    ESP_GOTO_ON_ERROR(touch_priv_config_channel(chan_handle, chan_cfg), err, TAG, "Configure touch channel failed");

err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_start_continuous_scanning(touch_sensor_handle_t sens_handle)
{
    TOUCH_NULL_POINTER_CHECK_ISR(sens_handle);

    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_FALSE_ISR(sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please enable the touch sensor first");
    ESP_GOTO_ON_FALSE_ISR(!sens_handle->is_started, ESP_ERR_INVALID_STATE, err, TAG, "Continuous scanning has started already");

    TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
    sens_handle->is_started = true;
#if SOC_TOUCH_SENSOR_VERSION <= 2
    touch_ll_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_ll_start_fsm();
#else
    touch_ll_enable_fsm_timer(true);
    touch_ll_start_fsm_repeated_timer(false);
#endif
    TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);

err:
    return ret;
}

esp_err_t touch_sensor_stop_continuous_scanning(touch_sensor_handle_t sens_handle)
{
    TOUCH_NULL_POINTER_CHECK_ISR(sens_handle);

    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_FALSE_ISR(sens_handle->is_started, ESP_ERR_INVALID_STATE, err, TAG, "Continuous scanning not started yet");

    TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
#if SOC_TOUCH_SENSOR_VERSION <= 2
    touch_ll_stop_fsm();
    touch_ll_set_fsm_mode(TOUCH_FSM_MODE_SW);
#else
    touch_ll_stop_fsm_repeated_timer(false);
    touch_ll_enable_fsm_timer(false);
#endif
    sens_handle->is_started = false;
    TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);

err:
    return ret;
}

esp_err_t touch_sensor_trigger_oneshot_scanning(touch_sensor_handle_t sens_handle, int timeout_ms)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please enable the touch sensor first");
    ESP_GOTO_ON_FALSE(!sens_handle->is_started, ESP_ERR_INVALID_STATE, err, TAG, "Failed to trigger oneshot scanning because scanning has started");
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    sens_handle->is_started = true;
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    TickType_t ticks = 0;
    if (timeout_ms > 0) {
        ticks = pdMS_TO_TICKS(timeout_ms);
        if (!ticks) {
            ESP_LOGW(TAG, "The timeout is too small, use the minimum tick resolution as default: %"PRIu32" ms", portTICK_PERIOD_MS);
            ticks = 1;
        }
    }
    xSemaphoreTake(sens_handle->mutex, ticks);
    TickType_t end_tick = xTaskGetTickCount() + ticks;
    // TODO: extract the following implementation into version specific source file when supporting other targets
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_ll_enable_fsm_timer(false);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    FOR_EACH_TOUCH_CHANNEL(i) {
        if (sens_handle->ch[i]) {
            TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
            touch_ll_channel_sw_measure_mask(BIT(i));
            touch_ll_trigger_oneshot_measurement();
            TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
            while (!touch_ll_is_measure_done()) {
                if (g_touch->is_meas_timeout) {
                    g_touch->is_meas_timeout = false;
                    ESP_LOGW(TAG, "The measurement time on channel %d exceed the limitation", i);
                    break;
                }
                if (timeout_ms >= 0) {
                    ESP_GOTO_ON_FALSE(xTaskGetTickCount() <= end_tick, ESP_ERR_TIMEOUT, err, TAG, "Wait for measurement done timeout");
                }
                vTaskDelay(1);
            }
        }
    }
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_ll_channel_sw_measure_mask(0);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

err:
    xSemaphoreGive(sens_handle->mutex);
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    sens_handle->is_started = false;
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    return ret;
}

esp_err_t touch_sensor_register_callbacks(touch_sensor_handle_t sens_handle, const touch_event_callbacks_t *callbacks, void *user_ctx)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);
    TOUCH_NULL_POINTER_CHECK(callbacks);

#if CONFIG_TOUCH_ISR_IRAM_SAFE
    const uint32_t **ptr = (const uint32_t **)callbacks;
    for (int i = 0; i < sizeof(touch_event_callbacks_t) / sizeof(uint32_t *); i++) {
        ESP_RETURN_ON_FALSE(TOUCH_IRAM_CHECK(ptr[i]), ESP_ERR_INVALID_ARG, TAG, "callback not in IRAM");
    }
    ESP_RETURN_ON_FALSE(!user_ctx || esp_ptr_internal(user_ctx), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
#endif

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);

    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    memcpy(&sens_handle->cbs, callbacks, sizeof(touch_event_callbacks_t));
    sens_handle->user_ctx = user_ctx;

err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_channel_read_data(touch_channel_handle_t chan_handle, touch_chan_data_type_t type, uint32_t *data)
{
    TOUCH_NULL_POINTER_CHECK_ISR(chan_handle);
    TOUCH_NULL_POINTER_CHECK_ISR(data);
    return touch_priv_channel_read_data(chan_handle, type, data);
}

esp_err_t touch_channel_config_benchmark(touch_channel_handle_t chan_handle, const touch_chan_benchmark_config_t *benchmark_cfg)
{
    TOUCH_NULL_POINTER_CHECK_ISR(chan_handle);
    TOUCH_NULL_POINTER_CHECK_ISR(benchmark_cfg);
    touch_priv_config_benchmark(chan_handle, benchmark_cfg);
    return ESP_OK;
}
