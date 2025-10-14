/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcpwm_private.h"
#include "driver/mcpwm_etm.h"
#include "esp_private/etm_interface.h"

typedef struct {
    esp_etm_event_t base;
    mcpwm_cmpr_handle_t cmpr;
} mcpwm_comparator_etm_event_t;

static esp_err_t mcpwm_del_etm_event(esp_etm_event_t *event)
{
    mcpwm_comparator_etm_event_t *etm_event = __containerof(event, mcpwm_comparator_etm_event_t, base);
    mcpwm_cmpr_handle_t cmpr = etm_event->cmpr;
    mcpwm_oper_t *oper = cmpr->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;

    switch (cmpr->type) {
    case MCPWM_OPERATOR_COMPARATOR:
        portENTER_CRITICAL(&group->spinlock);
        mcpwm_ll_etm_enable_comparator_event(hal->dev, oper->oper_id, cmpr->cmpr_id, false);
        portEXIT_CRITICAL(&group->spinlock);
        break;
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    case MCPWM_EVENT_COMPARATOR:
        portENTER_CRITICAL(&group->spinlock);
        mcpwm_ll_etm_enable_evt_comparator_event(hal->dev, oper->oper_id, cmpr->cmpr_id, false);
        portEXIT_CRITICAL(&group->spinlock);
        break;
#endif // SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    }
    free(etm_event);
    return ESP_OK;
}

esp_err_t mcpwm_comparator_new_etm_event(mcpwm_cmpr_handle_t cmpr, const mcpwm_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *out_event)
{
    esp_err_t ret = ESP_OK;
    mcpwm_comparator_etm_event_t *event = NULL;
    ESP_RETURN_ON_FALSE(cmpr && config && out_event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    event = heap_caps_calloc(1, sizeof(mcpwm_comparator_etm_event_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(event, ESP_ERR_NO_MEM, TAG, "no memory for ETM event");

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
        portENTER_CRITICAL(&group->spinlock);
        mcpwm_ll_etm_enable_evt_comparator_event(hal->dev, oper->oper_id, cmpr->cmpr_id, true);
        portEXIT_CRITICAL(&group->spinlock);
        event_id = MCPWM_LL_ETM_EVENT_COMPARATOR_EVENT_TABLE(group_id, oper_id, cmpr_id, config->event_type);
        break;
#endif // SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    }
    ESP_GOTO_ON_FALSE(event_id != 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "not supported event type");
    ESP_LOGD(TAG, "MCPWM (%d) oper (%d) cmpr(%d) event_id (%"PRId32")", group_id, oper_id, cmpr_id, event_id);

    // fill the ETM event object
    event->cmpr = cmpr;
    event->base.event_id = event_id;
    event->base.trig_periph = ETM_TRIG_PERIPH_MCPWM;
    event->base.del = mcpwm_del_etm_event;

    *out_event = &event->base;
    return ESP_OK;

err:
    if (event) {
        mcpwm_del_etm_event(&event->base);
    }
    return ret;
}
