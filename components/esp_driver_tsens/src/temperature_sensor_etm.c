/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_etm.h"
#include "esp_check.h"
#include "esp_private/etm_interface.h"
#include "hal/temperature_sensor_ll.h"
#include "driver/temperature_sensor_etm.h"
#include "esp_heap_caps.h"
#if CONFIG_ETM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "tsens-etm";

static esp_err_t temperature_sensor_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

static esp_err_t temperature_sensor_del_etm_task(esp_etm_task_t *task)
{
    free(task);
    return ESP_OK;
}

esp_err_t temperature_sensor_new_etm_event(temperature_sensor_handle_t tsens, const temperature_sensor_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_etm_event_t *event = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(tsens && config && out_event, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->event_type < TEMPERATURE_SENSOR_EVENT_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid event type");
    event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM event");

    uint32_t event_id = TEMPERATURE_SENSOR_LL_ETM_EVENT_TABLE(config->event_type);
    ESP_GOTO_ON_FALSE(event_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported event type");

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_TSENS;
    event->del = temperature_sensor_del_etm_event;
    ESP_LOGD(TAG, "new event @%p, event_id=%"PRIu32, event, event_id);
    *out_event = event;
    return ESP_OK;

err:
    if (event) {
        temperature_sensor_del_etm_event(event);
    }
    return ret;
}

esp_err_t temperature_sensor_new_etm_task(temperature_sensor_handle_t tsens, const temperature_sensor_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_etm_task_t *task = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(tsens && config && out_task, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->task_type < TEMPERATURE_SENSOR_TASK_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid task type");
    task = heap_caps_calloc(1, sizeof(esp_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(task, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM task");

    uint32_t task_id = TEMPERATURE_SENSOR_LL_ETM_TASK_TABLE(config->task_type);
    ESP_GOTO_ON_FALSE(task_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported task type");

    // fill the ETM task object
    task->task_id = task_id;
    task->trig_periph = ETM_TRIG_PERIPH_TSENS;
    task->del = temperature_sensor_del_etm_task;
    ESP_LOGD(TAG, "new task @%p, task_id=%"PRIu32, task, task_id);
    *out_task = task;
    return ESP_OK;

err:
    if (task) {
        temperature_sensor_del_etm_task(task);
    }
    return ret;
}
