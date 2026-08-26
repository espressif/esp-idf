/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/lock.h>
#include "driver/gptimer.h"
#include "gptimer_priv.h"
#include "esp_private/etm_interface.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

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
    ESP_RETURN_ON_FALSE(timer && config && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->event_type < GPTIMER_ETM_EVENT_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid event type");
    event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no memory for ETM event");

    // get the event ID that can be recognized by ETM hardware
    gptimer_group_t *group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;
    uint32_t event_id = TIMER_LL_ETM_EVENT_TABLE(group_id, timer_id, config->event_type);

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_GPTIMER;
    event->del = gptimer_del_etm_event;
    *out_event = event;
    return ESP_OK;
}

esp_err_t gptimer_new_etm_task(gptimer_handle_t timer, const gptimer_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    esp_etm_task_t *task = NULL;
    ESP_RETURN_ON_FALSE(timer && config && out_task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->task_type < GPTIMER_ETM_TASK_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid task type");
    task = heap_caps_calloc(1, sizeof(esp_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(task, ESP_ERR_NO_MEM, TAG, "no memory for ETM task");

    // get the task ID that can be recognized by ETM hardware
    gptimer_group_t *group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;
    uint32_t task_id = TIMER_LL_ETM_TASK_TABLE(group_id, timer_id, config->task_type);

    // fill the ETM task object
    task->task_id = task_id;
    task->trig_periph = ETM_TRIG_PERIPH_GPTIMER;
    task->del = gptimer_del_etm_task;
    *out_task = task;
    return ESP_OK;
}
