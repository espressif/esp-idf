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
#include "driver/mcpwm_oper.h"
#include "mcpwm_private.h"

static const char *TAG = "mcpwm";

static void mcpwm_operator_default_isr(void *args);

static esp_err_t mcpwm_operator_register_to_group(mcpwm_oper_t *oper, int group_id)
{
    mcpwm_group_t *group = mcpwm_acquire_group_handle(group_id);
    ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", group_id);

    int oper_id = -1;
    portENTER_CRITICAL(&group->spinlock);
    for (int i = 0; i < SOC_MCPWM_OPERATORS_PER_GROUP; i++) {
        if (!group->operators[i]) {
            oper_id = i;
            group->operators[i] = oper;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);
    if (oper_id < 0) {
        mcpwm_release_group_handle(group);
        group = NULL;
    } else {
        oper->group = group;
        oper->oper_id = oper_id;
    }
    ESP_RETURN_ON_FALSE(oper_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free operators in group (%d)", group_id);
    return ESP_OK;
}

static void mcpwm_operator_unregister_from_group(mcpwm_oper_t *oper)
{
    mcpwm_group_t *group = oper->group;
    int oper_id =  oper->oper_id;

    portENTER_CRITICAL(&group->spinlock);
    group->operators[oper_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);

    // operator has a reference on group, release it now
    mcpwm_release_group_handle(group);
}

static esp_err_t mcpwm_operator_destroy(mcpwm_oper_t *oper)
{
    if (oper->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(oper->intr), TAG, "uninstall interrupt service failed");
    }
    if (oper->group) {
        mcpwm_operator_unregister_from_group(oper);
    }
    free(oper);
    return ESP_OK;
}

esp_err_t mcpwm_new_operator(const mcpwm_operator_config_t *config, mcpwm_oper_handle_t *ret_oper)
{
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    mcpwm_oper_t *oper = NULL;
    ESP_GOTO_ON_FALSE(config && ret_oper, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->group_id < SOC_MCPWM_GROUPS && config->group_id >= 0, ESP_ERR_INVALID_ARG,
                      err, TAG, "invalid group ID:%d", config->group_id);
    if (config->intr_priority) {
        ESP_GOTO_ON_FALSE(1 << (config->intr_priority) & MCPWM_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, err,
                          TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    oper = heap_caps_calloc(1, sizeof(mcpwm_oper_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(oper, ESP_ERR_NO_MEM, err, TAG, "no mem for operator");

    ESP_GOTO_ON_ERROR(mcpwm_operator_register_to_group(oper, config->group_id), err, TAG, "register operator failed");
    mcpwm_group_t *group = oper->group;
    int group_id = group->group_id;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;

    // if interrupt priority specified before, it cannot be changed until the group is released
    // check if the new priority specified consistents with the old one
    ESP_GOTO_ON_ERROR(mcpwm_check_intr_priority(group, config->intr_priority), err, TAG, "set group interrupt priority failed");

    // reset MCPWM operator
    mcpwm_hal_operator_reset(hal, oper_id);

    // set the time point that the generator can update the action
    mcpwm_ll_operator_enable_update_action_on_tez(hal->dev, oper_id, config->flags.update_gen_action_on_tez);
    mcpwm_ll_operator_enable_update_action_on_tep(hal->dev, oper_id, config->flags.update_gen_action_on_tep);
    mcpwm_ll_operator_enable_update_action_on_sync(hal->dev, oper_id, config->flags.update_gen_action_on_sync);
    // set the time point that the deadtime can update the delay parameter
    mcpwm_ll_deadtime_enable_update_delay_on_tez(hal->dev, oper_id, config->flags.update_dead_time_on_tez);
    mcpwm_ll_deadtime_enable_update_delay_on_tep(hal->dev, oper_id, config->flags.update_dead_time_on_tep);
    mcpwm_ll_deadtime_enable_update_delay_on_sync(hal->dev, oper_id, config->flags.update_dead_time_on_sync);
    // set the clock source for dead time submodule, the resolution is the same to the MCPWM group
    mcpwm_ll_operator_set_deadtime_clock_src(hal->dev, oper_id, MCPWM_LL_DEADTIME_CLK_SRC_GROUP);
    oper->deadtime_resolution_hz = group->resolution_hz;

    // fill in other operator members
    oper->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    *ret_oper = oper;
    ESP_LOGD(TAG, "new operator (%d,%d) at %p", group_id, oper_id, oper);
    return ESP_OK;

err:
    if (oper) {
        mcpwm_operator_destroy(oper);
    }
    return ret;
}

esp_err_t mcpwm_del_operator(mcpwm_oper_handle_t oper)
{
    ESP_RETURN_ON_FALSE(oper, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    for (int i = 0; i < SOC_MCPWM_COMPARATORS_PER_OPERATOR; i++) {
        ESP_RETURN_ON_FALSE(!oper->comparators[i], ESP_ERR_INVALID_STATE, TAG, "comparator still in working");
    }
    for (int i = 0; i < SOC_MCPWM_GENERATORS_PER_OPERATOR; i++) {
        ESP_RETURN_ON_FALSE(!oper->generators[i], ESP_ERR_INVALID_STATE, TAG, "generator still in working");
    }
    ESP_RETURN_ON_FALSE(!oper->soft_fault, ESP_ERR_INVALID_STATE, TAG, "soft fault still in working");
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    for (int i = 0; i < SOC_MCPWM_EVENT_COMPARATORS_PER_OPERATOR; i++) {
        ESP_RETURN_ON_FALSE(!oper->event_comparators[i], ESP_ERR_INVALID_STATE, TAG, "event comparator still in working");
    }
#endif
    mcpwm_group_t *group = oper->group;
    int oper_id = oper->oper_id;
    mcpwm_hal_context_t *hal = &group->hal;

    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_OPER_MASK(oper_id), false);
    mcpwm_ll_intr_clear_status(hal->dev, MCPWM_LL_EVENT_OPER_MASK(oper_id));
    portEXIT_CRITICAL(&group->spinlock);

    ESP_LOGD(TAG, "del operator (%d,%d)", group->group_id, oper_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_operator_destroy(oper), TAG, "destroy operator failed");
    return ESP_OK;
}

esp_err_t mcpwm_operator_connect_timer(mcpwm_oper_handle_t oper, mcpwm_timer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(oper && timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(oper->group == timer->group, ESP_ERR_INVALID_ARG, TAG, "operator and timer should reside in the same group");
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;

    // connect operator and timer
    mcpwm_ll_operator_connect_timer(hal->dev, oper->oper_id, timer->timer_id);
    // change the the clock source of deadtime submodule to use MCPWM timer
    mcpwm_ll_operator_set_deadtime_clock_src(hal->dev, oper->oper_id, MCPWM_LL_DEADTIME_CLK_SRC_TIMER);
    oper->deadtime_resolution_hz = timer->resolution_hz;

    oper->timer = timer;
    ESP_LOGD(TAG, "connect operator (%d) and timer (%d) in group (%d)", oper->oper_id, timer->timer_id, group->group_id);
    return ESP_OK;
}

esp_err_t mcpwm_operator_apply_carrier(mcpwm_oper_handle_t oper, const mcpwm_carrier_config_t *config)
{
    ESP_RETURN_ON_FALSE(oper, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;
    uint32_t real_frequency = 0;
    uint32_t real_fpd = 0;
    float real_duty = 0.0;

    if (config && config->frequency_hz) {
        // select the clock source
        mcpwm_carrier_clock_source_t clk_src = config->clk_src ? config->clk_src : MCPWM_CARRIER_CLK_SRC_DEFAULT;
        ESP_RETURN_ON_ERROR(mcpwm_select_periph_clock(group, (soc_module_clk_t)clk_src), TAG, "set group clock failed");

        uint32_t prescale = 0;
        ESP_RETURN_ON_ERROR(mcpwm_set_prescale(group, config->frequency_hz, MCPWM_LL_MAX_CARRIER_PRESCALE * 8, &prescale), TAG, "set prescale failed");
        // here div 8 because the duty has 3 register bits
        prescale /= 8;
        ESP_RETURN_ON_FALSE(prescale > 0 && prescale <= MCPWM_LL_MAX_CARRIER_PRESCALE, ESP_ERR_INVALID_STATE, TAG, "group clock cannot match the frequency");
        mcpwm_ll_carrier_set_prescale(hal->dev, oper_id, prescale);
        real_frequency = group->resolution_hz / 8 / prescale;

        uint8_t duty = (uint8_t)(config->duty_cycle * 8);
        mcpwm_ll_carrier_set_duty(hal->dev, oper_id, duty);
        real_duty = (float) duty / 8.0F;

        uint8_t first_pulse_ticks = (uint8_t)(config->first_pulse_duration_us * real_frequency / 1000000UL);
        ESP_RETURN_ON_FALSE(first_pulse_ticks > 0 && first_pulse_ticks <= MCPWM_LL_MAX_CARRIER_ONESHOT,
                            ESP_ERR_INVALID_ARG, TAG, "invalid first pulse duration");
        mcpwm_ll_carrier_set_first_pulse_width(hal->dev, oper_id, first_pulse_ticks);
        real_fpd = first_pulse_ticks * 1000000UL / real_frequency;

        mcpwm_ll_carrier_in_invert(hal->dev, oper_id, config->flags.invert_before_modulate);
        mcpwm_ll_carrier_out_invert(hal->dev, oper_id, config->flags.invert_after_modulate);
    }

    mcpwm_ll_carrier_enable(hal->dev, oper_id, real_frequency > 0);

    if (real_frequency > 0) {
        ESP_LOGD(TAG, "enable carrier modulation for operator(%d,%d), freq=%"PRIu32"Hz, duty=%.2f, FPD=%"PRIu32"us",
                 group->group_id, oper_id, real_frequency, real_duty, real_fpd);
    } else {
        ESP_LOGD(TAG, "disable carrier for operator (%d,%d)", group->group_id, oper_id);
    }
    return ESP_OK;
}

esp_err_t mcpwm_operator_register_event_callbacks(mcpwm_oper_handle_t oper, const mcpwm_operator_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(oper && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int oper_id = oper->oper_id;

#if CONFIG_MCPWM_ISR_IRAM_SAFE
    if (cbs->on_brake_cbc) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_brake_cbc), ESP_ERR_INVALID_ARG, TAG, "on_brake_cbc callback not in IRAM");
    }
    if (cbs->on_brake_ost) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_brake_ost), ESP_ERR_INVALID_ARG, TAG, "on_brake_ost callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    // lazy install interrupt service
    if (!oper->intr) {
        // we want the interrupt service to be enabled after allocation successfully
        int isr_flags = MCPWM_INTR_ALLOC_FLAG & ~ ESP_INTR_FLAG_INTRDISABLED;
        isr_flags |= mcpwm_get_intr_priority_flag(group);
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(mcpwm_periph_signals.groups[group_id].irq_id, isr_flags,
                                                      (uint32_t)mcpwm_ll_intr_get_status_reg(hal->dev), MCPWM_LL_EVENT_OPER_MASK(oper_id),
                                                      mcpwm_operator_default_isr, oper, &oper->intr), TAG, "install interrupt service for operator failed");
    }

    // enable/disable interrupt events
    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_OPER_BRAKE_CBC(oper_id), cbs->on_brake_cbc != NULL);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_OPER_BRAKE_OST(oper_id), cbs->on_brake_ost != NULL);
    portEXIT_CRITICAL(&group->spinlock);

    oper->on_brake_cbc = cbs->on_brake_cbc;
    oper->on_brake_ost = cbs->on_brake_ost;
    oper->user_data = user_data;

    return ESP_OK;
}

esp_err_t mcpwm_operator_set_brake_on_fault(mcpwm_oper_handle_t oper, const mcpwm_brake_config_t *config)
{
    ESP_RETURN_ON_FALSE(oper && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = oper->group;
    mcpwm_fault_t *fault = config->fault;

    int oper_id = oper->oper_id;
    mcpwm_ll_brake_enable_cbc_refresh_on_tez(group->hal.dev, oper_id, config->flags.cbc_recover_on_tez);
    mcpwm_ll_fault_enable_cbc_refresh_on_tep(group->hal.dev, oper_id, config->flags.cbc_recover_on_tep);

    switch (fault->type) {
    case MCPWM_FAULT_TYPE_GPIO: {
        ESP_RETURN_ON_FALSE(group == fault->group, ESP_ERR_INVALID_ARG, TAG, "fault and operator not in the same group");
        mcpwm_gpio_fault_t *gpio_fault = __containerof(fault, mcpwm_gpio_fault_t, base);
        mcpwm_ll_brake_enable_cbc_mode(group->hal.dev, oper_id, gpio_fault->fault_id, config->brake_mode == MCPWM_OPER_BRAKE_MODE_CBC);
        mcpwm_ll_brake_enable_oneshot_mode(group->hal.dev, oper_id, gpio_fault->fault_id, config->brake_mode  == MCPWM_OPER_BRAKE_MODE_OST);
        oper->brake_mode_on_gpio_fault[gpio_fault->fault_id] = config->brake_mode;
        break;
    }
    case MCPWM_FAULT_TYPE_SOFT: {
        mcpwm_soft_fault_t *soft_fault = __containerof(fault, mcpwm_soft_fault_t, base);
        ESP_RETURN_ON_FALSE(!soft_fault->oper || soft_fault->oper == oper, ESP_ERR_INVALID_STATE, TAG, "soft fault already used by another operator");
        soft_fault->oper = oper;
        soft_fault->base.group = oper->group;
        mcpwm_ll_brake_enable_soft_cbc(group->hal.dev, oper_id, config->brake_mode == MCPWM_OPER_BRAKE_MODE_CBC);
        mcpwm_ll_brake_enable_soft_ost(group->hal.dev, oper_id, config->brake_mode == MCPWM_OPER_BRAKE_MODE_OST);
        oper->brake_mode_on_soft_fault = config->brake_mode;
        break;
    }
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "unknown fault type:%d", fault->type);
        break;
    }
    return ESP_OK;
}

esp_err_t mcpwm_operator_recover_from_fault(mcpwm_oper_handle_t oper, mcpwm_fault_handle_t fault)
{
    ESP_RETURN_ON_FALSE(oper && fault, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = oper->group;
    mcpwm_operator_brake_mode_t brake_mode;

    // check the brake mode on the fault event
    switch (fault->type) {
    case MCPWM_FAULT_TYPE_GPIO: {
        mcpwm_gpio_fault_t *gpio_fault = __containerof(fault, mcpwm_gpio_fault_t, base);
        brake_mode = oper->brake_mode_on_gpio_fault[gpio_fault->fault_id];
        break;
    }
    case MCPWM_FAULT_TYPE_SOFT:
        brake_mode = oper->brake_mode_on_soft_fault;
        break;
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "unknown fault type:%d", fault->type);
        break;
    }

    bool fault_signal_is_active = false;
    if (brake_mode == MCPWM_OPER_BRAKE_MODE_OST) {
        fault_signal_is_active = mcpwm_ll_ost_brake_active(group->hal.dev, oper->oper_id);
        // OST brake can't recover automatically, need to manually recovery the operator
        if (!fault_signal_is_active) {
            mcpwm_ll_brake_clear_ost(group->hal.dev, oper->oper_id);
        }
    } else {
        fault_signal_is_active = mcpwm_ll_cbc_brake_active(group->hal.dev, oper->oper_id);
        // CBC brake can recover automatically after deactivating the fault signal
    }

    ESP_RETURN_ON_FALSE(!fault_signal_is_active, ESP_ERR_INVALID_STATE, TAG, "recover fail, fault signal still active");
    return ESP_OK;
}

static void IRAM_ATTR mcpwm_operator_default_isr(void *args)
{
    mcpwm_oper_t *oper = (mcpwm_oper_t *)args;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;
    bool need_yield = false;

    uint32_t status = mcpwm_ll_intr_get_status(hal->dev);
    mcpwm_ll_intr_clear_status(hal->dev, status & MCPWM_LL_EVENT_OPER_MASK(oper_id));

    mcpwm_brake_event_data_t edata = {};

    if (status & MCPWM_LL_EVENT_OPER_BRAKE_CBC(oper_id)) {
        mcpwm_brake_event_cb_t cb = oper->on_brake_cbc;
        if (cb) {
            if (cb(oper, &edata, oper->user_data)) {
                need_yield = true;
            }
        }
    }

    if (status & MCPWM_LL_EVENT_OPER_BRAKE_OST(oper_id)) {
        mcpwm_brake_event_cb_t cb = oper->on_brake_ost;
        if (cb) {
            if (cb(oper, &edata, oper->user_data)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
