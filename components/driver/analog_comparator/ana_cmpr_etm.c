/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#if CONFIG_ETM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "hal/ana_cmpr_ll.h"
#include "driver/ana_cmpr_types.h"
#include "driver/ana_cmpr_etm.h"
#include "esp_private/etm_interface.h"

static const char *TAG = "ana-cmpr-etm";

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

typedef struct ana_cmpr_etm_event_t ana_cmpr_etm_event_t;

typedef struct {
    portMUX_TYPE spinlock;
    ana_cmpr_etm_event_t *events[SOC_ANA_CMPR_ETM_EVENTS_PER_UNIT];
} ana_cmpr_etm_unit_t;

struct ana_cmpr_etm_event_t {
    esp_etm_event_t base;
    ana_cmpr_unit_t unit;
    ana_cmpr_event_type_t type;
};

static ana_cmpr_etm_unit_t s_ana_cmpr_etm_unit[SOC_ANA_CMPR_NUM] = {
    [0 ... SOC_ANA_CMPR_NUM - 1] = {
        .spinlock = portMUX_INITIALIZER_UNLOCKED,
        .events[0 ... SOC_ANA_CMPR_ETM_EVENTS_PER_UNIT - 1] = NULL,
    },
};

static esp_err_t ana_cmpr_etm_event_register_to_unit(ana_cmpr_etm_event_t *evt)
{
    ESP_RETURN_ON_FALSE(!s_ana_cmpr_etm_unit[evt->unit].events[evt->type],
                        ESP_ERR_INVALID_STATE, TAG, "this event has been registered on the unit");
    portENTER_CRITICAL(&s_ana_cmpr_etm_unit[evt->unit].spinlock);
    s_ana_cmpr_etm_unit[evt->unit].events[evt->type] = evt;
    portEXIT_CRITICAL(&s_ana_cmpr_etm_unit[evt->unit].spinlock);
    return ESP_OK;
}

static esp_err_t ana_cmpr_etm_event_unregister_from_unit(ana_cmpr_etm_event_t *evt)
{
    ESP_RETURN_ON_FALSE(s_ana_cmpr_etm_unit[evt->unit].events[evt->type] == evt,
                        ESP_ERR_INVALID_ARG, TAG, "the event handle is not match to registered handle");
    portENTER_CRITICAL(&s_ana_cmpr_etm_unit[evt->unit].spinlock);
    s_ana_cmpr_etm_unit[evt->unit].events[evt->type] = NULL;
    portEXIT_CRITICAL(&s_ana_cmpr_etm_unit[evt->unit].spinlock);
    return ESP_OK;
}

static esp_err_t ana_cmpr_del_etm_event(esp_etm_event_handle_t base_event)
{
    ana_cmpr_etm_event_t *event = __containerof(base_event, ana_cmpr_etm_event_t, base);
    ESP_RETURN_ON_ERROR(ana_cmpr_etm_event_unregister_from_unit(event), TAG, "unregister the event from the unit");
    free(event);
    event = NULL;
    return ESP_OK;
}

esp_err_t ana_cmpr_new_etm_event(const ana_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *ret_event)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    ana_cmpr_etm_event_t *event = NULL;
    ESP_GOTO_ON_FALSE(config && ret_event, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    event = heap_caps_calloc(1, sizeof(ana_cmpr_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no mem for analog comparator event");
    event->unit = config->unit;
    event->type = config->event_type;
    // register the event channel to the group
    ESP_GOTO_ON_ERROR(ana_cmpr_etm_event_register_to_unit(event), err, TAG, "register event channel to group failed");

    uint32_t event_id = ANALOG_CMPR_LL_ETM_SOURCE(config->unit, config->event_type);
    event->base.del = ana_cmpr_del_etm_event;
    event->base.event_id = event_id;
    event->base.trig_periph = ETM_TRIG_PERIPH_ANA_CMPR;
    ESP_LOGD(TAG, "new event @%p, event_id=%"PRIu32", unit_id=%d", event, event_id, config->unit);
    *ret_event = &event->base;
    return ESP_OK;

err:
    if (event) {
        free(event);
        event = NULL;
    }
    return ret;
}
