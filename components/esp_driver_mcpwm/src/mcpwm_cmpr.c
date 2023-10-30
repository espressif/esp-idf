/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_memory_utils.h"
#include "soc/soc_caps.h"
#include "soc/mcpwm_periph.h"
#include "hal/mcpwm_ll.h"
#include "driver/mcpwm_cmpr.h"
#include "mcpwm_private.h"

static const char *TAG = "mcpwm";

static void mcpwm_comparator_default_isr(void *args);

static esp_err_t mcpwm_comparator_register_to_operator(mcpwm_cmpr_t *cmpr, mcpwm_oper_t *oper)
{
    int cmpr_id = -1;
    portENTER_CRITICAL(&oper->spinlock);
    switch (cmpr->type) {
    case MCPWM_OPERATOR_COMPARATOR: {
        mcpwm_oper_cmpr_t *oper_cmpr = __containerof(cmpr, mcpwm_oper_cmpr_t, base);
        for (int i = 0; i < SOC_MCPWM_COMPARATORS_PER_OPERATOR; i++) {
            if (!oper->comparators[i]) {
                oper->comparators[i] = oper_cmpr;
                cmpr_id = i;
                break;
            }
        }
        break;
    }
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    case MCPWM_EVENT_COMPARATOR: {
        mcpwm_evt_cmpr_t *evt_cmpr = __containerof(cmpr, mcpwm_evt_cmpr_t, base);
        for (int i = 0; i < SOC_MCPWM_EVENT_COMPARATORS_PER_OPERATOR; i++) {
            if (!oper->event_comparators[i]) {
                oper->event_comparators[i] = evt_cmpr;
                cmpr_id = i;
                break;
            }
        }
        break;
    }
#endif
    }
    portEXIT_CRITICAL(&oper->spinlock);
    ESP_RETURN_ON_FALSE(cmpr_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free comparator in operator (%d,%d)", oper->group->group_id, oper->oper_id);

    cmpr->cmpr_id = cmpr_id;
    cmpr->oper = oper;
    return ESP_OK;
}

static void mcpwm_comparator_unregister_from_operator(mcpwm_cmpr_t *cmpr)
{
    mcpwm_oper_t *oper = cmpr->oper;
    int cmpr_id = cmpr->cmpr_id;

    portENTER_CRITICAL(&oper->spinlock);
    switch (cmpr->type) {
    case MCPWM_OPERATOR_COMPARATOR:
        oper->comparators[cmpr_id] = NULL;
        break;
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    case MCPWM_EVENT_COMPARATOR:
        oper->event_comparators[cmpr_id] = NULL;
        break;
#endif
    }
    portEXIT_CRITICAL(&oper->spinlock);
}

static esp_err_t mcpwm_comparator_destroy(mcpwm_cmpr_t *cmpr)
{
    if (cmpr->oper) {
        mcpwm_comparator_unregister_from_operator(cmpr);
    }
    switch (cmpr->type) {
    case MCPWM_OPERATOR_COMPARATOR: {
        mcpwm_oper_cmpr_t *oper_cmpr = __containerof(cmpr, mcpwm_oper_cmpr_t, base);
        if (oper_cmpr->intr) {
            ESP_RETURN_ON_ERROR(esp_intr_free(oper_cmpr->intr), TAG, "uninstall interrupt service failed");
        }
        free(oper_cmpr);
        break;
    }
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    case MCPWM_EVENT_COMPARATOR: {
        mcpwm_evt_cmpr_t *evt_cmpr = __containerof(cmpr, mcpwm_evt_cmpr_t, base);
        free(evt_cmpr);
        break;
    }
#endif // SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    }
    return ESP_OK;
}

esp_err_t mcpwm_new_comparator(mcpwm_oper_handle_t oper, const mcpwm_comparator_config_t *config, mcpwm_cmpr_handle_t *ret_cmpr)
{
    esp_err_t ret = ESP_OK;
    mcpwm_oper_cmpr_t *cmpr = NULL;
    ESP_GOTO_ON_FALSE(oper && config && ret_cmpr, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    if (config->intr_priority) {
        ESP_GOTO_ON_FALSE(1 << (config->intr_priority) & MCPWM_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, err,
                          TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    cmpr = heap_caps_calloc(1, sizeof(mcpwm_oper_cmpr_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(cmpr, ESP_ERR_NO_MEM, err, TAG, "no mem for comparator");
    cmpr->base.type = MCPWM_OPERATOR_COMPARATOR;

    ESP_GOTO_ON_ERROR(mcpwm_comparator_register_to_operator(&cmpr->base, oper), err, TAG, "register comparator failed");
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;
    int cmpr_id = cmpr->base.cmpr_id;

    // if interrupt priority specified before, it cannot be changed until the group is released
    // check if the new priority specified consistent with the old one
    ESP_GOTO_ON_ERROR(mcpwm_check_intr_priority(group, config->intr_priority), err, TAG, "set group interrupt priority failed");

    mcpwm_ll_operator_enable_update_compare_on_tez(hal->dev, oper_id, cmpr_id, config->flags.update_cmp_on_tez);
    mcpwm_ll_operator_enable_update_compare_on_tep(hal->dev, oper_id, cmpr_id, config->flags.update_cmp_on_tep);
    mcpwm_ll_operator_enable_update_compare_on_sync(hal->dev, oper_id, cmpr_id, config->flags.update_cmp_on_sync);

    // fill in other comparator members
    cmpr->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    *ret_cmpr = &cmpr->base;
    ESP_LOGD(TAG, "new operator comparator (%d,%d,%d) at %p", group->group_id, oper_id, cmpr_id, cmpr);
    return ESP_OK;

err:
    if (cmpr) {
        mcpwm_comparator_destroy(&cmpr->base);
    }
    return ret;
}

esp_err_t mcpwm_del_comparator(mcpwm_cmpr_handle_t cmpr)
{
    ESP_RETURN_ON_FALSE(cmpr, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_oper_t *oper = cmpr->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;
    int cmpr_id = cmpr->cmpr_id;

    portENTER_CRITICAL(&group->spinlock);
    switch (cmpr->type) {
    case MCPWM_OPERATOR_COMPARATOR:
        mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_CMP_EQUAL(oper_id, cmpr_id), false);
        mcpwm_ll_intr_clear_status(hal->dev, MCPWM_LL_EVENT_CMP_EQUAL(oper_id, cmpr_id));
        break;
    default:
        break;
    }
    portEXIT_CRITICAL(&group->spinlock);

    ESP_LOGD(TAG, "del comparator (%d,%d,%d)", group->group_id, oper_id, cmpr_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_comparator_destroy(cmpr), TAG, "destroy comparator failed");
    return ESP_OK;
}

#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
esp_err_t mcpwm_new_event_comparator(mcpwm_oper_handle_t oper, const mcpwm_event_comparator_config_t *config, mcpwm_cmpr_handle_t *ret_cmpr)
{
    esp_err_t ret = ESP_OK;
    mcpwm_evt_cmpr_t *cmpr = NULL;
    ESP_GOTO_ON_FALSE(oper && config && ret_cmpr, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    cmpr = heap_caps_calloc(1, sizeof(mcpwm_evt_cmpr_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(cmpr, ESP_ERR_NO_MEM, err, TAG, "no mem for event comparator");
    cmpr->base.type = MCPWM_EVENT_COMPARATOR;

    ESP_GOTO_ON_ERROR(mcpwm_comparator_register_to_operator(&cmpr->base, oper), err, TAG, "register event comparator failed");

    // fill in other comparator members
    cmpr->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    *ret_cmpr = &cmpr->base;
    ESP_LOGD(TAG, "new event comparator (%d,%d,%d) at %p", oper->group->group_id, oper->oper_id, cmpr->base.cmpr_id, cmpr);
    return ESP_OK;

err:
    if (cmpr) {
        mcpwm_comparator_destroy(&cmpr->base);
    }
    return ret;
}
#endif // SOC_MCPWM_SUPPORT_EVENT_COMPARATOR

esp_err_t mcpwm_comparator_set_compare_value(mcpwm_cmpr_handle_t cmpr, uint32_t cmp_ticks)
{
    ESP_RETURN_ON_FALSE_ISR(cmpr, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_oper_t *oper = cmpr->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_timer_t *timer = oper->timer;
    ESP_RETURN_ON_FALSE_ISR(timer, ESP_ERR_INVALID_STATE, TAG, "timer and operator are not connected");
    ESP_RETURN_ON_FALSE_ISR(cmp_ticks <= timer->peak_ticks, ESP_ERR_INVALID_ARG, TAG, "compare value out of range");

    switch (cmpr->type) {
    case MCPWM_OPERATOR_COMPARATOR:
        mcpwm_ll_operator_set_compare_value(group->hal.dev, oper->oper_id, cmpr->cmpr_id, cmp_ticks);
        break;
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    case MCPWM_EVENT_COMPARATOR:
        mcpwm_ll_operator_set_event_compare_value(group->hal.dev, oper->oper_id, cmpr->cmpr_id, cmp_ticks);
        break;
#endif
    }

    cmpr->compare_ticks = cmp_ticks;
    return ESP_OK;
}

esp_err_t mcpwm_comparator_register_event_callbacks(mcpwm_cmpr_handle_t cmpr, const mcpwm_comparator_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(cmpr && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cmpr->type == MCPWM_OPERATOR_COMPARATOR, ESP_ERR_INVALID_ARG, TAG, "only oper cmpr can register event callback");
    mcpwm_oper_t *oper = cmpr->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int oper_id = oper->oper_id;
    int cmpr_id = cmpr->cmpr_id;

#if CONFIG_MCPWM_ISR_IRAM_SAFE
    if (cbs->on_reach) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_reach), ESP_ERR_INVALID_ARG, TAG, "on_reach callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    mcpwm_oper_cmpr_t *oper_cmpr = __containerof(cmpr, mcpwm_oper_cmpr_t, base);
    // lazy install interrupt service
    if (!oper_cmpr->intr) {
        // we want the interrupt service to be enabled after allocation successfully
        int isr_flags = MCPWM_INTR_ALLOC_FLAG & ~ ESP_INTR_FLAG_INTRDISABLED;
        isr_flags |= mcpwm_get_intr_priority_flag(group);
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(mcpwm_periph_signals.groups[group_id].irq_id, isr_flags,
                                                      (uint32_t)mcpwm_ll_intr_get_status_reg(hal->dev), MCPWM_LL_EVENT_CMP_EQUAL(oper_id, cmpr_id),
                                                      mcpwm_comparator_default_isr, oper_cmpr, &oper_cmpr->intr), TAG, "install interrupt service for comparator failed");
    }

    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_CMP_EQUAL(oper_id, cmpr_id), cbs->on_reach != NULL);
    portEXIT_CRITICAL(&group->spinlock);

    oper_cmpr->on_reach = cbs->on_reach;
    oper_cmpr->user_data = user_data;

    return ESP_OK;
}

static void IRAM_ATTR mcpwm_comparator_default_isr(void *args)
{
    mcpwm_oper_cmpr_t *cmpr = (mcpwm_oper_cmpr_t *)args;
    mcpwm_oper_t *oper = cmpr->base.oper;
    mcpwm_timer_t *timer = oper->timer;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;
    int cmpr_id = cmpr->base.cmpr_id;
    bool need_yield = false;

    uint32_t status = mcpwm_ll_intr_get_status(hal->dev);
    mcpwm_ll_intr_clear_status(hal->dev, status & MCPWM_LL_EVENT_CMP_EQUAL(oper_id, cmpr_id));

    mcpwm_compare_event_data_t edata = {
        .compare_ticks = cmpr->base.compare_ticks,
        .direction = mcpwm_ll_timer_get_count_direction(hal->dev, timer->timer_id),
    };

    if (status & MCPWM_LL_EVENT_CMP_EQUAL(oper_id, cmpr_id)) {
        mcpwm_compare_event_cb_t cb = cmpr->on_reach;
        if (cb) {
            if (cb(&cmpr->base, &edata, cmpr->user_data)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
