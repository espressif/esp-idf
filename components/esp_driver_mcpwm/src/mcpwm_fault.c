/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "hal/gpio_hal.h"
#include "driver/mcpwm_fault.h"
#include "driver/gpio.h"
#include "mcpwm_private.h"
#include "esp_private/gpio.h"

static const char *TAG = "mcpwm";

static void mcpwm_gpio_fault_default_isr(void *args);
static esp_err_t mcpwm_del_gpio_fault(mcpwm_fault_handle_t fault);
static esp_err_t mcpwm_del_soft_fault(mcpwm_fault_handle_t fault);

static esp_err_t mcpwm_gpio_fault_register_to_group(mcpwm_gpio_fault_t *fault, int group_id)
{
    mcpwm_group_t *group = mcpwm_acquire_group_handle(group_id);
    ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", group_id);

    int fault_id = -1;
    portENTER_CRITICAL(&group->spinlock);
    for (int i = 0; i < SOC_MCPWM_GPIO_FAULTS_PER_GROUP; i++) {
        if (!group->gpio_faults[i]) {
            fault_id = i;
            group->gpio_faults[i] = fault;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);
    if (fault_id < 0) {
        mcpwm_release_group_handle(group);
        group = NULL;
    } else {
        fault->base.group = group;
        fault->fault_id = fault_id;
    }
    ESP_RETURN_ON_FALSE(fault_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free gpio fault in group (%d)", group_id);
    return ESP_OK;
}

static void mcpwm_gpio_fault_unregister_from_group(mcpwm_gpio_fault_t *fault)
{
    mcpwm_group_t *group = fault->base.group;
    int fault_id = fault->fault_id;

    portENTER_CRITICAL(&group->spinlock);
    group->gpio_faults[fault_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);

    // fault has a reference on group, release it now
    mcpwm_release_group_handle(group);
}

static esp_err_t mcpwm_gpio_fault_destroy(mcpwm_gpio_fault_t *fault)
{
    if (fault->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(fault->intr), TAG, "uninstall interrupt service failed");
    }
    if (fault->base.group) {
        mcpwm_gpio_fault_unregister_from_group(fault);
    }
    free(fault);
    return ESP_OK;
}

esp_err_t mcpwm_new_gpio_fault(const mcpwm_gpio_fault_config_t *config, mcpwm_fault_handle_t *ret_fault)
{
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    mcpwm_gpio_fault_t *fault = NULL;
    ESP_GOTO_ON_FALSE(config && ret_fault, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->group_id < SOC_MCPWM_GROUPS && config->group_id >= 0, ESP_ERR_INVALID_ARG,
                      err, TAG, "invalid group ID:%d", config->group_id);
    if (config->intr_priority) {
        ESP_GOTO_ON_FALSE(1 << (config->intr_priority) & MCPWM_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, err,
                          TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    fault = heap_caps_calloc(1, sizeof(mcpwm_gpio_fault_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(fault, ESP_ERR_NO_MEM, err, TAG, "no mem for gpio fault");

    ESP_GOTO_ON_ERROR(mcpwm_gpio_fault_register_to_group(fault, config->group_id), err, TAG, "register gpio fault failed");
    mcpwm_group_t *group = fault->base.group;
    int group_id = group->group_id;
    mcpwm_hal_context_t *hal = &group->hal;
    int fault_id = fault->fault_id;

    // if interrupt priority specified before, it cannot be changed until the group is released
    // check if the new priority specified consistents with the old one
    ESP_GOTO_ON_ERROR(mcpwm_check_intr_priority(group, config->intr_priority), err, TAG, "set group interrupt priority failed");

    // GPIO configuration
    gpio_func_sel(config->gpio_num, PIN_FUNC_GPIO);
    gpio_input_enable(config->gpio_num);
    esp_rom_gpio_connect_in_signal(config->gpio_num, mcpwm_periph_signals.groups[group_id].gpio_faults[fault_id].fault_sig, 0);

    if (config->flags.pull_down) {
        gpio_pulldown_en(config->gpio_num);
    }
    if (config->flags.pull_up) {
        gpio_pullup_en(config->gpio_num);
    }

    // deprecated, to be removed in in esp-idf v6.0
    if (config->flags.io_loop_back) {
        gpio_ll_output_enable(&GPIO, config->gpio_num);
    }

    // set fault detection polarity
    // different gpio faults share the same config register, using a group level spin lock
    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_fault_set_active_level(hal->dev, fault_id, config->flags.active_level);
    portEXIT_CRITICAL(&group->spinlock);

    // enable fault detection
    mcpwm_ll_fault_enable_detection(hal->dev, fault_id, true);

    // fill in other operator members
    fault->base.type = MCPWM_FAULT_TYPE_GPIO;
    fault->gpio_num = config->gpio_num;
    fault->base.del = mcpwm_del_gpio_fault;
    *ret_fault = &fault->base;
    ESP_LOGD(TAG, "new gpio fault (%d,%d) at %p, GPIO: %d", group_id, fault_id, fault, config->gpio_num);
    return ESP_OK;

err:
    if (fault) {
        mcpwm_gpio_fault_destroy(fault);
    }
    return ret;
}

static esp_err_t mcpwm_del_gpio_fault(mcpwm_fault_handle_t fault)
{
    mcpwm_gpio_fault_t *gpio_fault = __containerof(fault, mcpwm_gpio_fault_t, base);
    mcpwm_group_t *group = fault->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int fault_id = gpio_fault->fault_id;

    ESP_LOGD(TAG, "del GPIO fault (%d,%d)", group->group_id, fault_id);

    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_FAULT_MASK(fault_id), false);
    mcpwm_ll_intr_clear_status(hal->dev, MCPWM_LL_EVENT_FAULT_MASK(fault_id));
    portEXIT_CRITICAL(&group->spinlock);

    // disable fault detection
    mcpwm_ll_fault_enable_detection(hal->dev, fault_id, false);

    // disconnect signal from the GPIO pin
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT,
                                   mcpwm_periph_signals.groups[group_id].gpio_faults[fault_id].fault_sig, 0);

    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_gpio_fault_destroy(gpio_fault), TAG, "destroy GPIO fault failed");
    return ESP_OK;
}

esp_err_t mcpwm_new_soft_fault(const mcpwm_soft_fault_config_t *config, mcpwm_fault_handle_t *ret_fault)
{
    esp_err_t ret = ESP_OK;
    mcpwm_soft_fault_t *soft_fault = NULL;
    ESP_GOTO_ON_FALSE(config && ret_fault, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    soft_fault = heap_caps_calloc(1, sizeof(mcpwm_soft_fault_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(soft_fault, ESP_ERR_NO_MEM, err, TAG, "no mem for soft fault");

    // fill in other fault members
    soft_fault->base.type = MCPWM_FAULT_TYPE_SOFT;
    soft_fault->base.del = mcpwm_del_soft_fault;
    *ret_fault = &soft_fault->base;
    ESP_LOGD(TAG, "new soft fault at %p", soft_fault);
    return ESP_OK;

err:
    // soft_fault must be NULL in the error handling path, and it's a determined behaviour to free a NULL pointer in esp-idf
    free(soft_fault);
    return ret;
}

static esp_err_t mcpwm_del_soft_fault(mcpwm_fault_handle_t fault)
{
    mcpwm_soft_fault_t *soft_fault = __containerof(fault, mcpwm_soft_fault_t, base);
    ESP_LOGD(TAG, "del soft fault %p", soft_fault);
    free(soft_fault);
    return ESP_OK;
}

esp_err_t mcpwm_soft_fault_activate(mcpwm_fault_handle_t fault)
{
    ESP_RETURN_ON_FALSE(fault, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(fault->type == MCPWM_FAULT_TYPE_SOFT, ESP_ERR_INVALID_ARG, TAG, "not a valid soft fault");
    mcpwm_group_t *group = fault->group;
    mcpwm_soft_fault_t *soft_fault = __containerof(fault, mcpwm_soft_fault_t, base);
    mcpwm_oper_t *oper = soft_fault->oper;
    ESP_RETURN_ON_FALSE(oper, ESP_ERR_INVALID_STATE, TAG, "no operator is assigned to the fault");

    switch (oper->brake_mode_on_soft_fault) {
    case MCPWM_OPER_BRAKE_MODE_CBC:
        mcpwm_ll_brake_trigger_soft_cbc(group->hal.dev, oper->oper_id);
        break;
    case MCPWM_OPER_BRAKE_MODE_OST:
        mcpwm_ll_brake_trigger_soft_ost(group->hal.dev, oper->oper_id);
        break;
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "unknown brake mode:%d", oper->brake_mode_on_soft_fault);
        break;
    }
    return ESP_OK;
}

esp_err_t mcpwm_del_fault(mcpwm_fault_handle_t fault)
{
    ESP_RETURN_ON_FALSE(fault, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return fault->del(fault);
}

esp_err_t mcpwm_fault_register_event_callbacks(mcpwm_fault_handle_t fault, const mcpwm_fault_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(fault && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(fault->type == MCPWM_FAULT_TYPE_GPIO, ESP_ERR_INVALID_ARG, TAG, "only gpio fault can register event callback");
    mcpwm_gpio_fault_t *gpio_fault = __containerof(fault, mcpwm_gpio_fault_t, base);
    mcpwm_group_t *group = fault->group;
    int group_id = group->group_id;
    mcpwm_hal_context_t *hal = &group->hal;
    int fault_id = gpio_fault->fault_id;

#if CONFIG_MCPWM_ISR_IRAM_SAFE
    if (cbs->on_fault_enter) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_fault_enter), ESP_ERR_INVALID_ARG, TAG, "on_fault_enter callback not in IRAM");
    }
    if (cbs->on_fault_exit) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_fault_exit), ESP_ERR_INVALID_ARG, TAG, "on_fault_exit callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    // lazy install interrupt service
    if (!gpio_fault->intr) {
        // we want the interrupt service to be enabled after allocation successfully
        int isr_flags = MCPWM_INTR_ALLOC_FLAG & ~ESP_INTR_FLAG_INTRDISABLED;
        isr_flags |= mcpwm_get_intr_priority_flag(group);
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(mcpwm_periph_signals.groups[group_id].irq_id, isr_flags,
                                                      (uint32_t)mcpwm_ll_intr_get_status_reg(hal->dev), MCPWM_LL_EVENT_FAULT_MASK(fault_id),
                                                      mcpwm_gpio_fault_default_isr, gpio_fault, &gpio_fault->intr), TAG, "install interrupt service for gpio fault failed");
    }

    // different mcpwm events share the same interrupt control register
    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_FAULT_ENTER(fault_id), cbs->on_fault_enter != NULL);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_FAULT_EXIT(fault_id), cbs->on_fault_exit != NULL);
    portEXIT_CRITICAL(&group->spinlock);

    gpio_fault->on_fault_enter = cbs->on_fault_enter;
    gpio_fault->on_fault_exit = cbs->on_fault_exit;
    gpio_fault->user_data = user_data;
    return ESP_OK;
}

static void IRAM_ATTR mcpwm_gpio_fault_default_isr(void *args)
{
    mcpwm_gpio_fault_t *fault = (mcpwm_gpio_fault_t *)args;
    mcpwm_group_t *group = fault->base.group;
    mcpwm_hal_context_t *hal = &group->hal;
    int fault_id = fault->fault_id;
    bool need_yield = false;

    uint32_t status = mcpwm_ll_intr_get_status(hal->dev);
    mcpwm_ll_intr_clear_status(hal->dev, status & MCPWM_LL_EVENT_FAULT_MASK(fault_id));

    mcpwm_fault_event_data_t edata = {
        // TBD
    };

    if (status & MCPWM_LL_EVENT_FAULT_ENTER(fault_id)) {
        mcpwm_fault_event_cb_t cb = fault->on_fault_enter;
        if (cb) {
            if (cb(&fault->base, &edata, fault->user_data)) {
                need_yield = true;
            }
        }
    }

    if (status & MCPWM_LL_EVENT_FAULT_EXIT(fault_id)) {
        mcpwm_fault_event_cb_t cb = fault->on_fault_exit;
        if (cb) {
            if (cb(&fault->base, &edata, fault->user_data)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
