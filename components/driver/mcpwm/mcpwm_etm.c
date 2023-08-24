/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/mcpwm_periph.h"
#include "hal/mcpwm_ll.h"
#include "driver/mcpwm_etm.h"
#include "mcpwm_private.h"
#include "esp_private/etm_interface.h"

static const char *TAG = "mcpwm-etm";

static esp_err_t mcpwm_del_etm_event(esp_etm_event_t *event)
{
    free(event);
    return ESP_OK;
}

esp_err_t mcpwm_comparator_new_etm_event(mcpwm_cmpr_handle_t cmpr, const mcpwm_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    esp_etm_event_t *event = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(cmpr && config && out_event, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    event = heap_caps_calloc(1, sizeof(esp_etm_event_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(event, ESP_ERR_NO_MEM, err, TAG, "no memory for ETM event");

    mcpwm_oper_t *oper = cmpr->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int oper_id = oper->oper_id;
    int cmpr_id = cmpr->cmpr_id;
    uint32_t event_id = 0;

    switch (cmpr->type) {
    case MCPWM_OPERATOR_COMPARATOR:
        portENTER_CRITICAL(&group->spinlock);
        mcpwm_ll_etm_enable_comparator_event(hal->dev, oper_id, cmpr_id, true);
        portEXIT_CRITICAL(&group->spinlock);
        event_id = MCPWM_LL_ETM_COMPARATOR_EVENT_TABLE(group_id, oper_id, cmpr_id, config->event_type);
        break;
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    case MCPWM_EVENT_COMPARATOR:
        event_id = MCPWM_LL_ETM_EVENT_COMPARATOR_EVENT_TABLE(group_id, oper_id, cmpr_id, config->event_type);
        break;
#endif
    }
    ESP_GOTO_ON_FALSE(event_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported event type");
    ESP_LOGD(TAG, "MCPWM (%d) oper (%d) cmpr(%d) event_id (%"PRId32")", group_id, oper_id, cmpr_id, event_id);

    // fill the ETM event object
    event->event_id = event_id;
    event->trig_periph = ETM_TRIG_PERIPH_MCPWM;
    event->del = mcpwm_del_etm_event;
    *out_event = event;
    return ESP_OK;

err:
    if (event) {
        mcpwm_del_etm_event(event);
    }
    return ret;
}
