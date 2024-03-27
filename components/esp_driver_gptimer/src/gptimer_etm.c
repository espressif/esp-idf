/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "driver/gptimer.h"
#include "gptimer_priv.h"
#include "hal/timer_ll.h"
#include "esp_private/etm_interface.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "gptimer-etm";

static esp_err_t gptimer_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

static esp_err_t gptimer_del_etm_task(esp_etm_task_t *task)
{
    free(task);
    return ESP_OK;
}

esp_err_t gptimer_new_etm_event(gptimer_handle_t timer, const gptimer_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    esp_etm_event_t *event = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(timer && config && out_event, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->event_type < GPTIMER_ETM_EVENT_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid event type");
    event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM event");

    gptimer_group_t *group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;
    uint32_t event_id = TIMER_LL_ETM_EVENT_TABLE(group_id, timer_id, config->event_type);
    ESP_GOTO_ON_FALSE(event_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported event type");

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_GPTIMER;
    event->del = gptimer_del_etm_event;
    *out_event = event;
    return ESP_OK;

err:
    if (event) {
        gptimer_del_etm_event(event);
    }
    return ret;
}

esp_err_t gptimer_new_etm_task(gptimer_handle_t timer, const gptimer_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    esp_etm_task_t *task = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(timer && config && out_task, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->task_type < GPTIMER_ETM_TASK_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid task type");
    task = heap_caps_calloc(1, sizeof(esp_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(task, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM task");

    gptimer_group_t *group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;
    uint32_t task_id = TIMER_LL_ETM_TASK_TABLE(group_id, timer_id, config->task_type);
    ESP_GOTO_ON_FALSE(task_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported task type");

    // fill the ETM task object
    task->task_id = task_id;
    task->trig_periph = ETM_TRIG_PERIPH_GPTIMER;
    task->del = gptimer_del_etm_task;
    *out_task = task;
    return ESP_OK;

err:
    if (task) {
        gptimer_del_etm_task(task);
    }
    return ret;
}
