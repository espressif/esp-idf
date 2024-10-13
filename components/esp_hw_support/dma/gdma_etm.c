/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#include "hal/gdma_hal.h"
#include "hal/gdma_ll.h"
#include "soc/gdma_periph.h"
#include "esp_private/gdma.h"
#include "esp_private/etm_interface.h"
#include "gdma_priv.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "gdma-etm";

typedef struct gdma_etm_task_t {
    esp_etm_task_t base;
    gdma_channel_t *chan;
} gdma_etm_task_t;

static esp_err_t gdma_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

static esp_err_t gdma_del_etm_task(esp_etm_task_t *task)
{
    gdma_etm_task_t *gdma_task = __containerof(task, gdma_etm_task_t, base);
    gdma_channel_t *dma_chan = gdma_task->chan;
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t* hal = &group->hal;

    gdma_hal_enable_etm_task(hal, pair->pair_id, dma_chan->direction, false);
    free(gdma_task);
    dma_chan->flags.start_stop_by_etm = false;
    return ESP_OK;
}

esp_err_t gdma_new_etm_event(gdma_channel_handle_t dma_chan, const gdma_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    esp_etm_event_t *event = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(dma_chan && config && out_event, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->event_type < GDMA_ETM_EVENT_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid event type");
    event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM event");

    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    uint32_t event_id = 0;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        event_id = GDMA_LL_RX_ETM_EVENT_TABLE(group->group_id, pair->pair_id, config->event_type);
    } else {
        event_id = GDMA_LL_TX_ETM_EVENT_TABLE(group->group_id, pair->pair_id, config->event_type);
    }
    ESP_GOTO_ON_FALSE(event_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported event type");

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_GDMA;
    event->del = gdma_del_etm_event;
    *out_event = event;
    return ESP_OK;

err:
    if (event) {
        gdma_del_etm_event(event);
    }
    return ret;
}

esp_err_t gdma_new_etm_task(gdma_channel_handle_t dma_chan, const gdma_etm_task_config_t *config, esp_etm_task_handle_t *out_task)
{
    gdma_etm_task_t *task = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(dma_chan && config && out_task, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->task_type < GDMA_ETM_TASK_MAX, ESP_ERR_INVALID_ARG, err, TAG, "invalid task type");
    task = heap_caps_calloc(1, sizeof(gdma_etm_task_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(task, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM task");

    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t* hal = &group->hal;
    uint32_t task_id = 0;

    gdma_hal_enable_etm_task(hal, pair->pair_id, dma_chan->direction, true);
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        task_id = GDMA_LL_RX_ETM_TASK_TABLE(group->group_id, pair->pair_id, config->task_type);
    } else {
        task_id = GDMA_LL_TX_ETM_TASK_TABLE(group->group_id, pair->pair_id, config->task_type);
    }
    ESP_GOTO_ON_FALSE(task_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported task type");

    // set a flag, now the GDMA channel is start/stop by ETM subsystem
    dma_chan->flags.start_stop_by_etm = true;
    // fill the ETM task object
    task->chan = dma_chan;
    task->base.task_id = task_id;
    task->base.trig_periph = ETM_TRIG_PERIPH_GDMA;
    task->base.del = gdma_del_etm_task;
    *out_task = &(task->base);
    return ESP_OK;

err:
    if (task) {
        gdma_del_etm_task(&task->base);
    }
    return ret;
}
