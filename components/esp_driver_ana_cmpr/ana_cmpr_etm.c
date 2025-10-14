/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/ana_cmpr_etm.h"
#include "esp_private/etm_interface.h"
#include "ana_cmpr_private.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

typedef struct {
    esp_etm_event_t base;
} ana_cmpr_etm_event_t;

static esp_err_t ana_cmpr_del_etm_event(esp_etm_event_handle_t base_event)
{
    ana_cmpr_etm_event_t *event = __containerof(base_event, ana_cmpr_etm_event_t, base);
    free(event);
    return ESP_OK;
}

esp_err_t ana_cmpr_new_etm_event(ana_cmpr_handle_t cmpr, const ana_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *ret_event)
{
    ana_cmpr_unit_t unit = ana_cmpr_get_unit_id(cmpr);
    ESP_RETURN_ON_FALSE(((int)unit) >= 0, ESP_ERR_INVALID_ARG, TAG, "invalid analog comparator handle");
    ESP_RETURN_ON_FALSE(config && ret_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ana_cmpr_etm_event_t *event = heap_caps_calloc(1, sizeof(ana_cmpr_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no mem for analog comparator event");

    uint32_t event_id = ANALOG_CMPR_LL_ETM_SOURCE(unit, config->event_type);
    event->base.del = ana_cmpr_del_etm_event;
    event->base.event_id = event_id;
    event->base.trig_periph = ETM_TRIG_PERIPH_ANA_CMPR;
    ESP_LOGD(TAG, "new event @%p, event_id=%"PRIu32", unit_id=%d", event, event_id, unit);
    *ret_event = &event->base;
    return ESP_OK;
}
