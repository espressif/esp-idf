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
#include "driver/mcpwm_timer.h"
#include "esp_private/mcpwm.h"
#include "mcpwm_private.h"

static const char *TAG = "mcpwm";

static void mcpwm_timer_default_isr(void *args);

static esp_err_t mcpwm_timer_register_to_group(mcpwm_timer_t *timer, int group_id)
{
    mcpwm_group_t *group = mcpwm_acquire_group_handle(group_id);
    ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", group_id);

    int timer_id = -1;
    portENTER_CRITICAL(&group->spinlock);
    for (int i = 0; i < SOC_MCPWM_TIMERS_PER_GROUP; i++) {
        if (!group->timers[i]) {
            timer_id = i;
            group->timers[i] = timer;
            break;
        }
    }
    portEXIT_CRITICAL(&group->spinlock);
    if (timer_id < 0) {
        mcpwm_release_group_handle(group);
        group = NULL;
    } else {
        timer->group = group;
        timer->timer_id = timer_id;
    }
    ESP_RETURN_ON_FALSE(timer_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free timer in group (%d)", group_id);
    return ESP_OK;
}

static void mcpwm_timer_unregister_from_group(mcpwm_timer_t *timer)
{
    mcpwm_group_t *group = timer->group;
    int timer_id = timer->timer_id;

    portENTER_CRITICAL(&group->spinlock);
    group->timers[timer_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);

    // timer has a reference on group, release it now
    mcpwm_release_group_handle(group);
}

static esp_err_t mcpwm_timer_destroy(mcpwm_timer_t *timer)
{
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(timer->intr), TAG, "uninstall interrupt service failed");
    }
    if (timer->group) {
        mcpwm_timer_unregister_from_group(timer);
    }
    free(timer);
    return ESP_OK;
}

esp_err_t mcpwm_new_timer(const mcpwm_timer_config_t *config, mcpwm_timer_handle_t *ret_timer)
{
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    mcpwm_timer_t *timer = NULL;
    ESP_GOTO_ON_FALSE(config && ret_timer, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->group_id < SOC_MCPWM_GROUPS && config->group_id >= 0, ESP_ERR_INVALID_ARG,
                      err, TAG, "invalid group ID:%d", config->group_id);
    if (config->intr_priority) {
        ESP_GOTO_ON_FALSE(1 << (config->intr_priority) & MCPWM_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, err,
                          TAG, "invalid interrupt priority:%d", config->intr_priority);
    }
    // check the peak ticks that the timer can reach to
    uint32_t peak_ticks = config->period_ticks;
    if (config->count_mode == MCPWM_TIMER_COUNT_MODE_UP_DOWN) {
        peak_ticks /= 2; // in symmetric mode, peak_ticks = period_ticks / 2
    }
    ESP_GOTO_ON_FALSE(peak_ticks > 0 && peak_ticks < MCPWM_LL_MAX_COUNT_VALUE, ESP_ERR_INVALID_ARG, err, TAG, "invalid period ticks");

    timer = heap_caps_calloc(1, sizeof(mcpwm_timer_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(timer, ESP_ERR_NO_MEM, err, TAG, "no mem for timer");

    ESP_GOTO_ON_ERROR(mcpwm_timer_register_to_group(timer, config->group_id), err, TAG, "register timer failed");
    mcpwm_group_t *group = timer->group;
    int group_id = group->group_id;
    mcpwm_hal_context_t *hal = &group->hal;
    int timer_id = timer->timer_id;

    // if interrupt priority specified before, it cannot be changed until the group is released
    // check if the new priority specified consistents with the old one
    ESP_GOTO_ON_ERROR(mcpwm_check_intr_priority(group, config->intr_priority), err, TAG, "set group interrupt priority failed");

    // select the clock source
    mcpwm_timer_clock_source_t clk_src = config->clk_src ? config->clk_src : MCPWM_TIMER_CLK_SRC_DEFAULT;
    ESP_GOTO_ON_ERROR(mcpwm_select_periph_clock(group, (soc_module_clk_t)clk_src), err, TAG, "set group clock failed");
    // reset the timer to a determined state
    mcpwm_hal_timer_reset(hal, timer_id);
    // set timer resolution
    uint32_t prescale = 0;
    ESP_GOTO_ON_ERROR(mcpwm_set_prescale(group, config->resolution_hz, MCPWM_LL_MAX_TIMER_PRESCALE, &prescale), err, TAG, "set prescale failed");
    mcpwm_ll_timer_set_clock_prescale(hal->dev, timer_id, prescale);
    timer->resolution_hz = group->resolution_hz / prescale;
    if (timer->resolution_hz != config->resolution_hz) {
        ESP_LOGW(TAG, "adjust timer resolution to %"PRIu32"Hz", timer->resolution_hz);
    }

    // set the peak tickes that the timer can reach to
    timer->count_mode = config->count_mode;
    timer->peak_ticks = peak_ticks;
    mcpwm_ll_timer_set_peak(hal->dev, timer_id, peak_ticks, timer->count_mode == MCPWM_TIMER_COUNT_MODE_UP_DOWN);
    // set count direction
    mcpwm_ll_timer_set_count_mode(hal->dev, timer_id, timer->count_mode);
    // what time is allowed to update the period
    mcpwm_ll_timer_enable_update_period_on_sync(hal->dev, timer_id, config->flags.update_period_on_sync);
    mcpwm_ll_timer_enable_update_period_on_tez(hal->dev, timer_id, config->flags.update_period_on_empty);

    // fill in other timer specific members
    timer->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    timer->fsm = MCPWM_TIMER_FSM_INIT;
    *ret_timer = timer;
    ESP_LOGD(TAG, "new timer(%d,%d) at %p, resolution:%"PRIu32"Hz, peak:%"PRIu32", count_mod:%c",
             group_id, timer_id, timer, timer->resolution_hz, timer->peak_ticks, "SUDB"[timer->count_mode]);
    return ESP_OK;

err:
    if (timer) {
        mcpwm_timer_destroy(timer);
    }
    return ret;
}

esp_err_t mcpwm_del_timer(mcpwm_timer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // check child resources are in free state
    ESP_RETURN_ON_FALSE(!timer->sync_src, ESP_ERR_INVALID_STATE, TAG, "timer sync_src still in working");
    ESP_RETURN_ON_FALSE(timer->fsm == MCPWM_TIMER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "timer not in init state");
    mcpwm_group_t *group = timer->group;
    int timer_id = timer->timer_id;
    mcpwm_hal_context_t *hal = &group->hal;

    // disable and clear the pending interrupt
    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_TIMER_MASK(timer_id), false);
    mcpwm_ll_intr_clear_status(hal->dev, MCPWM_LL_EVENT_TIMER_MASK(timer_id));
    portEXIT_CRITICAL(&group->spinlock);

    ESP_LOGD(TAG, "del timer (%d,%d)", group->group_id, timer_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_timer_destroy(timer), TAG, "destroy timer failed");
    return ESP_OK;
}

esp_err_t mcpwm_timer_set_period(mcpwm_timer_handle_t timer, uint32_t period_ticks)
{
    ESP_RETURN_ON_FALSE_ISR(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = timer->group;
    int timer_id = timer->timer_id;
    mcpwm_hal_context_t *hal = &group->hal;
    uint32_t peak_ticks = period_ticks;
    if (timer->count_mode == MCPWM_TIMER_COUNT_MODE_UP_DOWN) {
        peak_ticks /= 2; // in symmetric mode, peak_ticks = period_ticks / 2
    }
    ESP_RETURN_ON_FALSE_ISR(peak_ticks > 0 && peak_ticks < MCPWM_LL_MAX_COUNT_VALUE, ESP_ERR_INVALID_ARG, TAG, "invalid period ticks");
    mcpwm_ll_timer_set_peak(hal->dev, timer_id, peak_ticks, timer->count_mode == MCPWM_TIMER_COUNT_MODE_UP_DOWN);
    timer->peak_ticks = peak_ticks;
    return ESP_OK;
}

esp_err_t mcpwm_timer_register_event_callbacks(mcpwm_timer_handle_t timer, const mcpwm_timer_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(timer && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;
    mcpwm_hal_context_t *hal = &group->hal;

#if CONFIG_MCPWM_ISR_IRAM_SAFE
    if (cbs->on_empty) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_empty), ESP_ERR_INVALID_ARG, TAG, "on_empty callback not in IRAM");
    }
    if (cbs->on_full) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_full), ESP_ERR_INVALID_ARG, TAG, "on_full callback not in IRAM");
    }
    if (cbs->on_stop) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_stop), ESP_ERR_INVALID_ARG, TAG, "on_stop callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    // lazy install interrupt service
    if (!timer->intr) {
        ESP_RETURN_ON_FALSE(timer->fsm == MCPWM_TIMER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "timer not in init state");
        int isr_flags = MCPWM_INTR_ALLOC_FLAG;
        isr_flags |= mcpwm_get_intr_priority_flag(group);
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(mcpwm_periph_signals.groups[group_id].irq_id, isr_flags,
                                                      (uint32_t)mcpwm_ll_intr_get_status_reg(hal->dev), MCPWM_LL_EVENT_TIMER_MASK(timer_id),
                                                      mcpwm_timer_default_isr, timer, &timer->intr), TAG, "install interrupt service for timer failed");
    }

    // enable/disable interrupt events
    portENTER_CRITICAL(&group->spinlock);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_TIMER_FULL(timer_id), cbs->on_full != NULL);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_TIMER_EMPTY(timer_id), cbs->on_empty != NULL);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_TIMER_STOP(timer_id), cbs->on_stop != NULL);
    portEXIT_CRITICAL(&group->spinlock);

    timer->on_stop = cbs->on_stop;
    timer->on_full = cbs->on_full;
    timer->on_empty = cbs->on_empty;
    timer->user_data = user_data;
    return ESP_OK;
}

esp_err_t mcpwm_timer_get_phase(mcpwm_timer_handle_t timer, uint32_t *count_value, mcpwm_timer_direction_t *direction)
{
    ESP_RETURN_ON_FALSE(timer && count_value && direction, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = timer->group;
    int timer_id = timer->timer_id;
    mcpwm_hal_context_t *hal = &group->hal;

    portENTER_CRITICAL(&timer->spinlock);
    *count_value = mcpwm_ll_timer_get_count_value(hal->dev, timer_id);
    *direction = mcpwm_ll_timer_get_count_direction(hal->dev, timer_id);
    portEXIT_CRITICAL(&timer->spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_timer_enable(mcpwm_timer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(timer->fsm == MCPWM_TIMER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "timer not in init state");
    mcpwm_group_t *group = timer->group;
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_enable(timer->intr), TAG, "enable interrupt failed");
    }
    if (group->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(group->pm_lock), TAG, "acquire pm lock failed");
    }
    timer->fsm = MCPWM_TIMER_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t mcpwm_timer_disable(mcpwm_timer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(timer->fsm == MCPWM_TIMER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "timer not in enable state");
    mcpwm_group_t *group = timer->group;
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_disable(timer->intr), TAG, "disable interrupt failed");
    }
    if (group->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(group->pm_lock), TAG, "acquire pm lock failed");
    }
    timer->fsm = MCPWM_TIMER_FSM_INIT;
    return ESP_OK;
}

esp_err_t mcpwm_timer_start_stop(mcpwm_timer_handle_t timer, mcpwm_timer_start_stop_cmd_t command)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(timer->fsm == MCPWM_TIMER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "timer not in enable state");
    mcpwm_group_t *group = timer->group;

    portENTER_CRITICAL_SAFE(&timer->spinlock);
    mcpwm_ll_timer_set_start_stop_command(group->hal.dev, timer->timer_id, command);
    portEXIT_CRITICAL_SAFE(&timer->spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_timer_set_phase_on_sync(mcpwm_timer_handle_t timer, const mcpwm_timer_sync_phase_config_t *config)
{
    ESP_RETURN_ON_FALSE(timer && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_group_t *group = timer->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;
    mcpwm_sync_handle_t sync_source = config->sync_src;
    // check if the sync direction is valid
    bool valid_direction = true;
    if (timer->count_mode == MCPWM_TIMER_COUNT_MODE_UP) {
        valid_direction = config->direction == MCPWM_TIMER_DIRECTION_UP;
    } else if (timer->count_mode == MCPWM_TIMER_COUNT_MODE_DOWN) {
        valid_direction = config->direction == MCPWM_TIMER_DIRECTION_DOWN;
    } else if (timer->count_mode == MCPWM_TIMER_COUNT_MODE_PAUSE) {
        valid_direction = false;
    } else {
        valid_direction = true;
    }
    ESP_RETURN_ON_FALSE(valid_direction, ESP_ERR_INVALID_ARG, TAG, "invalid sync direction");

    // enable sync feature and set sync phase
    if (sync_source) {
        ESP_RETURN_ON_FALSE(config->count_value < MCPWM_LL_MAX_COUNT_VALUE, ESP_ERR_INVALID_ARG, TAG, "invalid sync count value");
        switch (sync_source->type) {
        case MCPWM_SYNC_TYPE_TIMER: {
            ESP_RETURN_ON_FALSE(group == sync_source->group, ESP_ERR_INVALID_ARG, TAG, "timer and sync source are not in the same group");
            mcpwm_timer_sync_src_t *timer_sync_src = __containerof(sync_source, mcpwm_timer_sync_src_t, base);
            mcpwm_ll_timer_set_timer_sync_input(hal->dev, timer_id, timer_sync_src->timer->timer_id);
            ESP_LOGD(TAG, "enable sync to timer (%d,%d) for timer (%d,%d)",
                     group_id, timer_sync_src->timer->timer_id, group_id, timer_id);
            break;
        }
        case MCPWM_SYNC_TYPE_GPIO: {
            ESP_RETURN_ON_FALSE(group == sync_source->group, ESP_ERR_INVALID_ARG, TAG, "timer and sync source are not in the same group");
            mcpwm_gpio_sync_src_t *gpio_sync_src = __containerof(sync_source, mcpwm_gpio_sync_src_t, base);
            mcpwm_ll_timer_set_gpio_sync_input(hal->dev, timer_id, gpio_sync_src->sync_id);
            ESP_LOGD(TAG, "enable sync to gpio (%d) for timer (%d,%d)",
                     gpio_sync_src->gpio_num, group_id, timer_id);
            break;
        }
        case MCPWM_SYNC_TYPE_SOFT: {
            mcpwm_soft_sync_src_t *soft_sync = __containerof(sync_source, mcpwm_soft_sync_src_t, base);
            if (soft_sync->soft_sync_from == MCPWM_SOFT_SYNC_FROM_TIMER && soft_sync->timer != timer) {
                ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "soft sync already used by another timer");
            }
            soft_sync->soft_sync_from = MCPWM_SOFT_SYNC_FROM_TIMER;
            soft_sync->timer = timer;
            soft_sync->base.group = group;
            break;
        }
        }

        mcpwm_ll_timer_set_sync_phase_direction(hal->dev, timer_id, config->direction);
        mcpwm_ll_timer_set_sync_phase_value(hal->dev, timer_id, config->count_value);
        mcpwm_ll_timer_enable_sync_input(hal->dev, timer_id, true);
    } else { // disable sync feature
        mcpwm_ll_timer_enable_sync_input(hal->dev, timer_id, false);
        ESP_LOGD(TAG, "disable sync for timer (%d,%d)", group_id, timer_id);
    }
    return ESP_OK;
}

static void IRAM_ATTR mcpwm_timer_default_isr(void *args)
{
    mcpwm_timer_t *timer = (mcpwm_timer_t *)args;
    mcpwm_group_t *group = timer->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int timer_id = timer->timer_id;
    bool need_yield = false;
    uint32_t status = mcpwm_ll_intr_get_status(hal->dev);
    mcpwm_ll_intr_clear_status(hal->dev, status & MCPWM_LL_EVENT_TIMER_MASK(timer_id));

    mcpwm_timer_event_data_t edata = {
        .direction = mcpwm_ll_timer_get_count_direction(hal->dev, timer_id),
        .count_value = mcpwm_ll_timer_get_count_value(hal->dev, timer_id),
    };

    if (status & MCPWM_LL_EVENT_TIMER_STOP(timer_id)) {
        mcpwm_timer_event_cb_t cb = timer->on_stop;
        if (cb) {
            if (cb(timer, &edata, timer->user_data)) {
                need_yield = true;
            }
        }
    }

    if (status & MCPWM_LL_EVENT_TIMER_FULL(timer_id)) {
        mcpwm_timer_event_cb_t cb = timer->on_full;
        if (cb) {
            if (cb(timer, &edata, timer->user_data)) {
                need_yield = true;
            }
        }
    }

    if (status & MCPWM_LL_EVENT_TIMER_EMPTY(timer_id)) {
        mcpwm_timer_event_cb_t cb = timer->on_empty;
        if (cb) {
            if (cb(timer, &edata, timer->user_data)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
