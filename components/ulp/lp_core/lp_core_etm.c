/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "lp_core_etm.h"
#include "esp_private/etm_interface.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "soc/soc_etm_source.h"

#define LP_CORE_ETM_EVENT_TABLE(event)                     \
    (uint32_t [LP_CORE_EVENT_MAX]){                           \
        [LP_CORE_EVENT_ERR_INTR] = ULP_EVT_ERR_INTR,  \
        [LP_CORE_EVENT_START_INTR] = ULP_EVT_START_INTR,  \
    }[event]

#define LP_CORE_ETM_TASK_TABLE(task)                     \
    (uint32_t [LP_CORE_TASK_MAX]){                           \
        [LP_CORE_TASK_WAKEUP_CPU] = ULP_TASK_WAKEUP_CPU,    \
    }[task]

const static char* TAG = "lp-core-etm";

static esp_err_t lp_core_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

static esp_err_t lp_core_del_etm_task(esp_etm_task_t *task)
{
    free(task);
    return ESP_OK;
}

esp_err_t lp_core_new_etm_task(const lp_core_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    esp_etm_task_t *task = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(config && out_task, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->task_type < LP_CORE_TASK_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid task type");
    task = heap_caps_calloc(1, sizeof(esp_etm_task_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(task, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM task");

    uint32_t task_id = LP_CORE_ETM_TASK_TABLE(config->task_type);
    ESP_GOTO_ON_FALSE(task_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported task type");

    // fill the ETM task object
    task->task_id = task_id;
    task->trig_periph = ETM_TRIG_PERIPH_LP_CORE;
    task->del = lp_core_del_etm_task;
    ESP_LOGD(TAG, "new task @%p, task_id=%"PRIu32, task, task_id);
    *out_task = task;
    return ESP_OK;

err:
    if (task) {
        lp_core_del_etm_task(task);
    }
    return ret;
}

esp_err_t lp_core_new_etm_event(const lp_core_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    esp_etm_event_t *event = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(config && out_event, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->event_type < LP_CORE_EVENT_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid event type");
    event = heap_caps_calloc(1, sizeof(esp_etm_event_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM event");

    uint32_t event_id = LP_CORE_ETM_EVENT_TABLE(config->event_type);
    ESP_GOTO_ON_FALSE(event_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported event type");

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_LP_CORE;
    event->del = lp_core_del_etm_event;
    *out_event = event;
    return ESP_OK;

err:
    if (event) {
        lp_core_del_etm_event(event);
    }
    return ret;
}
