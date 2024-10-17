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

typedef struct {
    esp_etm_task_t base;            /*!< Base ETM task object */
    i2s_chan_handle_t handle;       /*!< I2S channel handle of this etm task */
    i2s_etm_task_type_t task_type;  /*!< I2S ETM task type */
} i2s_etm_task_t;

static esp_err_t s_i2s_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

static esp_err_t s_i2s_del_etm_task(esp_etm_task_t *task)
{
    i2s_etm_task_t *i2s_task = __containerof(task, i2s_etm_task_t, base);
    if (i2s_task->task_type == I2S_ETM_TASK_START) {
        // The i2s start no longer be controlled by etm
        i2s_task->handle->is_etm_start = false;
    } else {
        // The i2s stop no longer be controlled by etm
        i2s_task->handle->is_etm_stop = false;
    }
    free(i2s_task);
    return ESP_OK;
}

esp_err_t i2s_new_etm_event(i2s_chan_handle_t handle, const i2s_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(handle && config && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->event_type < I2S_ETM_EVENT_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid event type");
    esp_etm_event_t *event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no memory for ETM event");

    // Get the event id from the I2S ETM event table
    uint32_t event_id = I2S_LL_ETM_EVENT_TABLE(handle->controller->id, handle->dir, config->event_type);
    // If the event type is threshold, set the threshold to the hardware
    if (config->event_type == I2S_ETM_EVENT_REACH_THRESH) {
        // Check if the threshold within the supported range
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
    event->del = s_i2s_del_etm_event;
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
    i2s_etm_task_t *task = heap_caps_calloc(1, sizeof(i2s_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(task, ESP_ERR_NO_MEM, TAG, "no memory for ETM task");

    // Get the task id from the I2S ETM task table
    uint32_t task_id = I2S_LL_ETM_TASK_TABLE(handle->controller->id, handle->dir, config->task_type);

    // fill the ETM task object
    task->base.task_id = task_id;
    task->base.trig_periph = ETM_TRIG_PERIPH_I2S;
    task->base.del = s_i2s_del_etm_task;
    task->handle = handle;
    task->task_type = config->task_type;
    if (config->task_type == I2S_ETM_TASK_START) {
        // The i2s start will be controlled by etm
        handle->is_etm_start = true;
    } else {
        // The i2s stop will be controlled by etm
        handle->is_etm_stop = true;
    }
    *out_task = &(task->base);

    return ESP_OK;
}
