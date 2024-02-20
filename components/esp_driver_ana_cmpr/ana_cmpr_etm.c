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
#include "ana_cmpr_private.h"

static const char *TAG = "ana-cmpr-etm";

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

typedef struct {
    esp_etm_event_t base;
} ana_cmpr_etm_event_t;

static esp_err_t ana_cmpr_del_etm_event(esp_etm_event_handle_t base_event)
{
    ana_cmpr_etm_event_t *event = __containerof(base_event, ana_cmpr_etm_event_t, base);
    free(event);
    event = NULL;
    return ESP_OK;
}

esp_err_t ana_cmpr_new_etm_event(ana_cmpr_handle_t cmpr, const ana_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *ret_event)
{
#if CONFIG_ETM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    ana_cmpr_etm_event_t *event = NULL;
    ana_cmpr_unit_t unit = ana_cmpr_priv_get_unit_by_handle(cmpr);
    ESP_RETURN_ON_FALSE(((int)unit) >= 0, ESP_ERR_INVALID_ARG, TAG, "invalid analog comparator handle");
    ESP_RETURN_ON_FALSE(config && ret_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    event = heap_caps_calloc(1, sizeof(ana_cmpr_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no mem for analog comparator event");

    uint32_t event_id = ANALOG_CMPR_LL_ETM_SOURCE(unit, config->event_type);
    event->base.del = ana_cmpr_del_etm_event;
    event->base.event_id = event_id;
    event->base.trig_periph = ETM_TRIG_PERIPH_ANA_CMPR;
    ESP_LOGD(TAG, "new event @%p, event_id=%"PRIu32", unit_id=%d", event, event_id, unit);
    *ret_event = &event->base;
    return ESP_OK;

err:
    if (event) {
        free(event);
        event = NULL;
    }
    return ret;
}
