/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_systick_etm.h"
#include "soc/soc_caps.h"
#include "soc/soc_etm_source.h"
#include "esp_private/systimer.h"
#include "esp_private/etm_interface.h"

#define ETM_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

static const char *TAG = "systick-etm";

static esp_err_t systick_etm_event_del(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

esp_err_t esp_systick_new_etm_alarm_event(int core_id, esp_etm_event_handle_t *out_event)
{
    esp_etm_event_t *event = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(out_event && core_id < SOC_CPU_CORES_NUM, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    event = heap_caps_calloc(1, sizeof(esp_etm_event_t), ETM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM event");

    // fill the ETM event object
    uint32_t event_id = SYSTIMER_EVT_CNT_CMP0 + SYSTIMER_ALARM_OS_TICK_CORE0 + core_id;
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_SYSTIMER;
    event->del = systick_etm_event_del;
    *out_event = event;
    return ESP_OK;

err:
    if (event) {
        systick_etm_event_del(event);
    }
    return ret;
}
