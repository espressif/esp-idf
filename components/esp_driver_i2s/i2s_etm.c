/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <stdlib.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "driver/i2s_etm.h"
#include "i2s_private.h"
#include "hal/i2s_ll.h"
#include "esp_private/etm_interface.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "i2s-etm";

static esp_err_t i2s_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

static esp_err_t i2s_del_etm_task(esp_etm_task_t *task)
{
    free(task);
    return ESP_OK;
}

esp_err_t i2s_new_etm_event(i2s_chan_handle_t handle, const i2s_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(handle && config && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->event_type < I2S_ETM_EVENT_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid event type");
    esp_etm_event_t *event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no memory for ETM event");

    uint32_t event_id = I2S_LL_ETM_EVENT_TABLE(handle->controller->id, handle->dir, config->event_type);
    if (config->event_type == I2S_ETM_EVENT_REACH_THRESH) {
        ESP_GOTO_ON_FALSE(config->threshold <= I2S_LL_ETM_MAX_THRESH_NUM, ESP_ERR_INVALID_ARG, err, TAG,
                          "exceed the max threshold %"PRIu32, (uint32_t)I2S_LL_ETM_MAX_THRESH_NUM);
        if (handle->dir == I2S_DIR_TX) {
            i2s_ll_tx_set_etm_threshold(handle->controller->hal.dev, config->threshold);
        } else {
            i2s_ll_rx_set_etm_threshold(handle->controller->hal.dev, config->threshold);
        }
    }

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_I2S;
    event->del = i2s_del_etm_event;
    *out_event = event;
    return ret;
err:
    free(event);
    return ret;
}

esp_err_t i2s_new_etm_task(i2s_chan_handle_t handle, const i2s_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    ESP_RETURN_ON_FALSE(handle && config && out_task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->task_type < I2S_ETM_TASK_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid task type");
    esp_etm_task_t *task = heap_caps_calloc(1, sizeof(esp_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(task, ESP_ERR_NO_MEM, TAG, "no memory for ETM task");

    uint32_t task_id = I2S_LL_ETM_TASK_TABLE(handle->controller->id, handle->dir, config->task_type);

    // fill the ETM task object
    task->task_id = task_id;
    task->trig_periph = ETM_TRIG_PERIPH_I2S;
    task->del = i2s_del_etm_task;
    *out_task = task;
    return ESP_OK;
}
