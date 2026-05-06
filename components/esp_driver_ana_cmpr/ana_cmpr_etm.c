/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/ana_cmpr_etm.h"
#include "esp_private/etm_interface.h"
#include "ana_cmpr_private.h"

typedef struct {
    esp_etm_event_t base;
    analog_cmpr_dev_t *dev;
    ana_cmpr_unit_t unit_id;
    int src_chan_id;
} ana_cmpr_etm_event_t;

typedef struct {
    esp_etm_task_t base;
} ana_cmpr_etm_task_t;

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
static uint32_t s_etm_event_ref_cnt[ANALOG_CMPR_LL_GET(INST_NUM)][ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM)] = {
    [0 ...(ANALOG_CMPR_LL_GET(INST_NUM) - 1)] = {0},
};

static void ana_cmpr_etm_acquire_src_chan(analog_cmpr_dev_t *dev, ana_cmpr_unit_t unit_id, int src_chan_id)
{
    ANA_CMPR_CRITICAL_SECTION() {
        if (s_etm_event_ref_cnt[unit_id][src_chan_id] == 0) {
            analog_cmpr_ll_enable_channel_etm(dev, src_chan_id, true);
        }
        s_etm_event_ref_cnt[unit_id][src_chan_id]++;
    }
}

static void ana_cmpr_etm_release_src_chan(analog_cmpr_dev_t *dev, ana_cmpr_unit_t unit_id, int src_chan_id)
{
    ANA_CMPR_CRITICAL_SECTION() {
        if (s_etm_event_ref_cnt[unit_id][src_chan_id] > 0) {
            s_etm_event_ref_cnt[unit_id][src_chan_id]--;
            if (s_etm_event_ref_cnt[unit_id][src_chan_id] == 0) {
                analog_cmpr_ll_enable_channel_etm(dev, src_chan_id, false);
            }
        }
    }
}
#endif

static esp_err_t ana_cmpr_del_etm_event(esp_etm_event_handle_t base_event)
{
    ana_cmpr_etm_event_t *event = __containerof(base_event, ana_cmpr_etm_event_t, base);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ana_cmpr_etm_release_src_chan(event->dev, event->unit_id, event->src_chan_id);
#endif
    free(event);
    return ESP_OK;
}

esp_err_t ana_cmpr_new_etm_event(ana_cmpr_handle_t cmpr, const ana_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *ret_event)
{
    ESP_RETURN_ON_FALSE(cmpr && config && ret_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->event_type < ANA_CMPR_EVENT_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid event type");
    ana_cmpr_unit_t unit_id = cmpr->unit_id;
    int src_chan_id = config->src_chan_id;
    ESP_RETURN_ON_FALSE(_ana_cmpr_is_src_chan_id_valid(src_chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid source channel id %d", src_chan_id);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ESP_RETURN_ON_FALSE(config->event_delay <= ANALOG_CMPR_LL_MAX_ETM_DELAY_CYCLES, ESP_ERR_INVALID_ARG, TAG,
                        "event delay out of range: %"PRIu32" cycles", config->event_delay);
#endif
    ana_cmpr_etm_event_t *event = heap_caps_calloc(1, sizeof(ana_cmpr_etm_event_t), ANA_CMPR_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no mem for analog comparator event");

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    // The ETM enable bit is shared by all events on the same source channel, so
    // keep it reference-counted across event handles.
    ana_cmpr_etm_acquire_src_chan(cmpr->dev, unit_id, src_chan_id);
    ANA_CMPR_CRITICAL_SECTION() {
        analog_cmpr_ll_set_etm_delay_cycles(cmpr->dev, config->event_delay);
    }
#endif

    uint32_t event_id = ANALOG_CMPR_LL_ETM_SOURCE(unit_id, src_chan_id, config->event_type);
    event->base.del = ana_cmpr_del_etm_event;
    event->base.event_id = event_id;
    event->base.trig_periph = ETM_TRIG_PERIPH_ANA_CMPR;
    event->dev = cmpr->dev;
    event->unit_id = unit_id;
    event->src_chan_id = src_chan_id;
    ESP_LOGD(TAG, "new event @%p, event_id=%"PRIu32", unit_id=%d", event, event_id, unit_id);
    *ret_event = &event->base;
    return ESP_OK;
}

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
static esp_err_t ana_cmpr_del_etm_task(esp_etm_task_handle_t base_task)
{
    ana_cmpr_etm_task_t *task = __containerof(base_task, ana_cmpr_etm_task_t, base);
    free(task);
    return ESP_OK;
}

esp_err_t ana_cmpr_new_etm_task(ana_cmpr_handle_t cmpr, const ana_cmpr_etm_task_config_t *config, esp_etm_task_handle_t *ret_task)
{
    ESP_RETURN_ON_FALSE(cmpr && config && ret_task, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->task_type < ANA_CMPR_TASK_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid task type");
    ana_cmpr_unit_t unit_id = cmpr->unit_id;
    ana_cmpr_etm_task_t *task = heap_caps_calloc(1, sizeof(ana_cmpr_etm_task_t), ANA_CMPR_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(task, ESP_ERR_NO_MEM, TAG, "no mem for analog comparator task");

    uint32_t task_id = ANALOG_CMPR_LL_ETM_TASK(unit_id, config->task_type);
    task->base.del = ana_cmpr_del_etm_task;
    task->base.task_id = task_id;
    task->base.trig_periph = ETM_TRIG_PERIPH_ANA_CMPR;

    ESP_LOGD(TAG, "new task @%p, task_id=%"PRIu32", unit_id=%d", task, task_id, unit_id);
    *ret_task = &task->base;
    return ESP_OK;
}
#endif
