/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "modem/modem_etm.h"
#include "hal/modem_ll.h"
#include "esp_private/etm_interface.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "modem-etm";

static esp_err_t s_modem_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

static esp_err_t s_modem_del_etm_task(esp_etm_task_t *task)
{
    free(task);
    return ESP_OK;
}

esp_err_t modem_new_etm_event(const modem_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    ESP_RETURN_ON_FALSE(config && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->event_type < MODEM_ETM_EVENT_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid event type");
    esp_etm_event_t *event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no memory for ETM event");

    // Get the event id from the modem ETM event table
    uint32_t event_id = MODEM_LL_ETM_EVENT_TABLE((uint32_t)config->event_type);

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_MODEM;
    event->del = s_modem_del_etm_event;
    *out_event = event;

    return ESP_OK;
}

esp_err_t modem_new_etm_task(const modem_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    ESP_RETURN_ON_FALSE(config && out_task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->task_type < MODEM_ETM_TASK_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid task type");
    esp_etm_task_t *task = heap_caps_calloc(1, sizeof(esp_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(task, ESP_ERR_NO_MEM, TAG, "no memory for ETM task");

    // Get the task id from the modem ETM task table
    uint32_t task_id = MODEM_LL_ETM_TASK_TABLE((uint32_t)config->task_type);

    // fill the ETM task object
    task->task_id = task_id;
    task->trig_periph = ETM_TRIG_PERIPH_MODEM;
    task->del = s_modem_del_etm_task;
    *out_task = task;

    return ESP_OK;
}
