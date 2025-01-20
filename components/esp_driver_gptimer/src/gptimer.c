/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/lock.h>
#include "driver/gptimer.h"
#include "gptimer_priv.h"
#include "esp_memory_utils.h"

static void gptimer_default_isr(void *args);

#if GPTIMER_USE_RETENTION_LINK
static esp_err_t gptimer_create_sleep_retention_link_cb(void *timer)
{
    int group_id = ((gptimer_t *)timer)->group->group_id;
    int timer_id = ((gptimer_t *)timer)->timer_id;
    esp_err_t err = sleep_retention_entries_create(tg_timer_reg_retention_info[group_id][timer_id].regdma_entry_array,
                                                   tg_timer_reg_retention_info[group_id][timer_id].array_size,
                                                   REGDMA_LINK_PRI_GPTIMER, tg_timer_reg_retention_info[group_id][timer_id].module);
    return err;
}

static void gptimer_create_retention_module(gptimer_t *timer)
{
    int group_id = timer->group->group_id;
    int timer_id = timer->timer_id;
    sleep_retention_module_t module = tg_timer_reg_retention_info[group_id][timer_id].module;
    if (sleep_retention_is_module_inited(module) && !sleep_retention_is_module_created(module)) {
        if (sleep_retention_module_allocate(module) != ESP_OK) {
            // even though the sleep retention module create failed, GPTimer driver should still work, so just warning here
            ESP_LOGW(TAG, "create retention link failed on TimerGroup%d Timer%d, power domain won't be turned off during sleep", group_id, timer_id);
        }
    }
}
#endif // GPTIMER_USE_RETENTION_LINK

static esp_err_t gptimer_register_to_group(gptimer_t *timer)
{
    gptimer_group_t *group = NULL;
    int timer_id = -1;
    for (int i = 0; i < SOC_TIMER_GROUPS; i++) {
        group = gptimer_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", i);
        // loop to search free timer in the group
        portENTER_CRITICAL(&group->spinlock);
        for (int j = 0; j < SOC_TIMER_GROUP_TIMERS_PER_GROUP; j++) {
            if (!group->timers[j]) {
                timer_id = j;
                group->timers[j] = timer;
                break;
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (timer_id < 0) {
            gptimer_release_group_handle(group);
        } else {
            timer->timer_id = timer_id;
            timer->group = group;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(timer_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free timer");

#if GPTIMER_USE_RETENTION_LINK
    sleep_retention_module_t module = tg_timer_reg_retention_info[group->group_id][timer_id].module;
    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create = {
                .handle = gptimer_create_sleep_retention_link_cb,
                .arg = (void *)timer
            },
        },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    if (sleep_retention_module_init(module, &init_param) != ESP_OK) {
        // even though the sleep retention module init failed, RMT driver should still work, so just warning here
        ESP_LOGW(TAG, "init sleep retention failed on TimerGroup%d Timer%d, power domain may be turned off during sleep", group->group_id, timer_id);
    }
#endif // GPTIMER_USE_RETENTION_LINK

    return ESP_OK;
}

static void gptimer_unregister_from_group(gptimer_t *timer)
{
    gptimer_group_t *group = timer->group;
    int timer_id = timer->timer_id;
    portENTER_CRITICAL(&group->spinlock);
    group->timers[timer_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);

#if GPTIMER_USE_RETENTION_LINK
    sleep_retention_module_t module = tg_timer_reg_retention_info[group->group_id][timer_id].module;
    if (sleep_retention_is_module_created(module)) {
        sleep_retention_module_free(module);
    }
    if (sleep_retention_is_module_inited(module)) {
        sleep_retention_module_deinit(module);
    }
#endif

    // timer has a reference on group, release it now
    gptimer_release_group_handle(group);
}

static esp_err_t gptimer_destroy(gptimer_t *timer)
{
    if (timer->clk_src) {
        ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)(timer->clk_src), false), TAG, "clock source disable failed");
    }
#if CONFIG_PM_ENABLE
    if (timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(timer->pm_lock), TAG, "delete pm_lock failed");
    }
#endif
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(timer->intr), TAG, "delete interrupt service failed");
    }
    if (timer->group) {
        gptimer_unregister_from_group(timer);
    }
    free(timer);
    return ESP_OK;
}

esp_err_t gptimer_new_timer(const gptimer_config_t *config, gptimer_handle_t *ret_timer)
{
    esp_err_t ret = ESP_OK;
    gptimer_t *timer = NULL;
    ESP_RETURN_ON_FALSE(config && ret_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->resolution_hz, ESP_ERR_INVALID_ARG, TAG, "invalid timer resolution:%"PRIu32, config->resolution_hz);
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & GPTIMER_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG,
                            TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    bool allow_pd = (config->flags.allow_pd == 1) || (config->flags.backup_before_sleep == 1);
#if !SOC_TIMER_SUPPORT_SLEEP_RETENTION
    ESP_RETURN_ON_FALSE(allow_pd == false, ESP_ERR_NOT_SUPPORTED, TAG, "not able to power down in light sleep");
#endif // SOC_TIMER_SUPPORT_SLEEP_RETENTION

    // always allocate memory from internal memory because the driver object contains atomic variable
    timer = heap_caps_calloc(1, sizeof(gptimer_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_GOTO_ON_FALSE(timer, ESP_ERR_NO_MEM, err, TAG, "no mem for gptimer");
    // register timer to the group (because one group can have several timers)
    ESP_GOTO_ON_ERROR(gptimer_register_to_group(timer), err, TAG, "register timer failed");
    gptimer_group_t *group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;

    if (allow_pd) {
#if GPTIMER_USE_RETENTION_LINK
        gptimer_create_retention_module(timer);
#endif // GPTIMER_USE_RETENTION_LINK
    }

    // initialize HAL layer
    timer_hal_init(&timer->hal, group_id, timer_id);
    // select clock source, set clock resolution
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)config->clk_src, true), err, TAG, "clock source enable failed");
    ESP_GOTO_ON_ERROR(gptimer_select_periph_clock(timer, config->clk_src, config->resolution_hz), err, TAG, "set periph clock failed");
    // initialize counter value to zero
    timer_hal_set_counter_value(&timer->hal, 0);
    // set counting direction
    timer_ll_set_count_direction(timer->hal.dev, timer_id, config->direction);

    // interrupt register is shared by all timers in the same group
    portENTER_CRITICAL(&group->spinlock);
    timer_ll_enable_intr(timer->hal.dev, TIMER_LL_EVENT_ALARM(timer_id), false); // disable interrupt
    timer_ll_clear_intr_status(timer->hal.dev, TIMER_LL_EVENT_ALARM(timer_id)); // clear pending interrupt event
    portEXIT_CRITICAL(&group->spinlock);
    // initialize other members of timer
    timer->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    // put the timer driver to the init state
    atomic_init(&timer->fsm, GPTIMER_FSM_INIT);
    timer->direction = config->direction;
    timer->intr_priority = config->intr_priority;
    timer->flags.intr_shared = config->flags.intr_shared;
    ESP_LOGD(TAG, "new gptimer (%d,%d) at %p, %zu bytes used", group_id, timer_id, timer, heap_caps_get_allocated_size(timer));
    *ret_timer = timer;
    return ESP_OK;

err:
    if (timer) {
        gptimer_destroy(timer);
    }
    return ret;
}

esp_err_t gptimer_del_timer(gptimer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(atomic_load(&timer->fsm) == GPTIMER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "timer not in init state");
    gptimer_group_t *group = timer->group;
    gptimer_clock_source_t clk_src = timer->clk_src;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;
    timer_hal_context_t *hal = &timer->hal;
    ESP_LOGD(TAG, "del timer (%d,%d)", group_id, timer_id);
    // disable the source clock
    GPTIMER_CLOCK_SRC_ATOMIC() {
        timer_ll_enable_clock(group_id, hal->timer_id, false);
    }
    timer_hal_deinit(hal);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(gptimer_destroy(timer), TAG, "destroy gptimer failed");

    switch (clk_src) {
#if SOC_TIMER_GROUP_SUPPORT_RC_FAST
    case GPTIMER_CLK_SRC_RC_FAST:
        periph_rtc_dig_clk8m_disable();
        break;
#endif // SOC_TIMER_GROUP_SUPPORT_RC_FAST
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t gptimer_set_raw_count(gptimer_handle_t timer, unsigned long long value)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_SAFE(&timer->spinlock);
    timer_hal_set_counter_value(&timer->hal, value);
    portEXIT_CRITICAL_SAFE(&timer->spinlock);
    return ESP_OK;
}

esp_err_t gptimer_get_raw_count(gptimer_handle_t timer, unsigned long long *value)
{
    if (timer == NULL || value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_SAFE(&timer->spinlock);
    *value = timer_hal_capture_and_get_counter_value(&timer->hal);
    portEXIT_CRITICAL_SAFE(&timer->spinlock);
    return ESP_OK;
}

esp_err_t gptimer_get_resolution(gptimer_handle_t timer, uint32_t *out_resolution)
{
    ESP_RETURN_ON_FALSE(timer && out_resolution, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *out_resolution = timer->resolution_hz;
    return ESP_OK;
}

esp_err_t gptimer_get_captured_count(gptimer_handle_t timer, uint64_t *value)
{
    if (timer == NULL || value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_SAFE(&timer->spinlock);
    *value = timer_ll_get_counter_value(timer->hal.dev, timer->timer_id);
    portEXIT_CRITICAL_SAFE(&timer->spinlock);
    return ESP_OK;
}

esp_err_t gptimer_register_event_callbacks(gptimer_handle_t timer, const gptimer_event_callbacks_t *cbs, void *user_data)
{
    gptimer_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(timer && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;

#if CONFIG_GPTIMER_ISR_CACHE_SAFE
    if (cbs->on_alarm) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_alarm), ESP_ERR_INVALID_ARG, TAG, "on_alarm callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    // lazy install interrupt service
    if (!timer->intr) {
        ESP_RETURN_ON_FALSE(atomic_load(&timer->fsm) == GPTIMER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "timer not in init state");
        // if user wants to control the interrupt allocation more precisely, we can expose more flags in `gptimer_config_t`
        int isr_flags = timer->flags.intr_shared ? ESP_INTR_FLAG_SHARED | GPTIMER_INTR_ALLOC_FLAGS : GPTIMER_INTR_ALLOC_FLAGS;
        if (timer->intr_priority) {
            isr_flags |= 1 << (timer->intr_priority);
        }
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(timer_group_periph_signals.groups[group_id].timer_irq_id[timer_id], isr_flags,
                                                      (uint32_t)timer_ll_get_intr_status_reg(timer->hal.dev), TIMER_LL_EVENT_ALARM(timer_id),
                                                      gptimer_default_isr, timer, &timer->intr), TAG, "install interrupt service failed");
    }

    // enable/disable GPTimer interrupt events
    portENTER_CRITICAL(&group->spinlock);
    timer_ll_enable_intr(timer->hal.dev, TIMER_LL_EVENT_ALARM(timer->timer_id), cbs->on_alarm != NULL); // enable timer interrupt
    portEXIT_CRITICAL(&group->spinlock);

    timer->on_alarm = cbs->on_alarm;
    timer->user_ctx = user_data;
    return ESP_OK;
}

esp_err_t gptimer_set_alarm_action(gptimer_handle_t timer, const gptimer_alarm_config_t *config)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (config) {
#if CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM
        // when the function is placed in IRAM, we expect the config struct is also placed in internal RAM
        // if the cache is disabled, the function can still access the config struct
        if (esp_ptr_internal(config) == false) {
            return ESP_ERR_INVALID_ARG;
        }
#endif
        // When auto_reload is enabled, alarm_count should not be equal to reload_count
        bool valid_auto_reload = !config->flags.auto_reload_on_alarm || config->alarm_count != config->reload_count;
        if (valid_auto_reload == false) {
            return ESP_ERR_INVALID_ARG;
        }

        portENTER_CRITICAL_SAFE(&timer->spinlock);
        timer->reload_count = config->reload_count;
        timer->alarm_count = config->alarm_count;
        timer->flags.auto_reload_on_alarm = config->flags.auto_reload_on_alarm;
        timer->flags.alarm_en = true;

        timer_ll_set_reload_value(timer->hal.dev, timer->timer_id, config->reload_count);
        timer_ll_set_alarm_value(timer->hal.dev, timer->timer_id, config->alarm_count);
        portEXIT_CRITICAL_SAFE(&timer->spinlock);
    } else {
        portENTER_CRITICAL_SAFE(&timer->spinlock);
        timer->flags.auto_reload_on_alarm = false;
        timer->flags.alarm_en = false;
        portEXIT_CRITICAL_SAFE(&timer->spinlock);
    }

    portENTER_CRITICAL_SAFE(&timer->spinlock);
    timer_ll_enable_auto_reload(timer->hal.dev, timer->timer_id, timer->flags.auto_reload_on_alarm);
    timer_ll_enable_alarm(timer->hal.dev, timer->timer_id, timer->flags.alarm_en);
    portEXIT_CRITICAL_SAFE(&timer->spinlock);
    return ESP_OK;
}

esp_err_t gptimer_enable(gptimer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // the only acceptable FSM change: init->enable
    gptimer_fsm_t expected_fsm = GPTIMER_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_ENABLE),
                        ESP_ERR_INVALID_STATE, TAG, "timer not in init state");

#if CONFIG_PM_ENABLE
    // acquire power manager lock
    if (timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(timer->pm_lock), TAG, "acquire pm_lock failed");
    }
#endif

    // enable interrupt service
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_enable(timer->intr), TAG, "enable interrupt service failed");
    }

    return ESP_OK;
}

esp_err_t gptimer_disable(gptimer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // the only acceptable FSM change: enable->init
    gptimer_fsm_t expected_fsm = GPTIMER_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_INIT),
                        ESP_ERR_INVALID_STATE, TAG, "timer not in enable state");

    // disable interrupt service
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_disable(timer->intr), TAG, "disable interrupt service failed");
    }

#if CONFIG_PM_ENABLE
    // release power manager lock
    if (timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(timer->pm_lock), TAG, "release pm_lock failed");
    }
#endif

    return ESP_OK;
}

esp_err_t gptimer_start(gptimer_handle_t timer)
{
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // if the timer is already started, do nothing
    if (atomic_load(&timer->fsm) == GPTIMER_FSM_RUN) {
        return ESP_OK;
    }

    gptimer_fsm_t expected_fsm = GPTIMER_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_WAIT)) {
        // the register used by the following LL functions are shared with other API,
        // which is possible to run along with this function, so we need to protect
        portENTER_CRITICAL_SAFE(&timer->spinlock);
        timer_ll_enable_alarm(timer->hal.dev, timer->timer_id, timer->flags.alarm_en);
        // Note here, if the alarm target is set very close to the current counter value
        // an alarm interrupt may be triggered very quickly after we start the timer
        timer_ll_enable_counter(timer->hal.dev, timer->timer_id, true);
        atomic_store(&timer->fsm, GPTIMER_FSM_RUN);
        portEXIT_CRITICAL_SAFE(&timer->spinlock);
    } else {
        // return error if the timer is not in the expected state
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

esp_err_t gptimer_stop(gptimer_handle_t timer)
{
    if (timer == NULL) {
        // not printing error message here because the return value already indicates the error well
        return ESP_ERR_INVALID_ARG;
    }

    // if the timer is not started, do nothing
    if (atomic_load(&timer->fsm) == GPTIMER_FSM_ENABLE) {
        return ESP_OK;
    }

    gptimer_fsm_t expected_fsm = GPTIMER_FSM_RUN;
    if (atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_WAIT)) {
        // disable counter, alarm, auto-reload
        portENTER_CRITICAL_SAFE(&timer->spinlock);
        timer_ll_enable_counter(timer->hal.dev, timer->timer_id, false);
        timer_ll_enable_alarm(timer->hal.dev, timer->timer_id, false);
        atomic_store(&timer->fsm, GPTIMER_FSM_ENABLE);
        portEXIT_CRITICAL_SAFE(&timer->spinlock);
    } else {
        // return error if the timer is not in the expected state
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

static void gptimer_default_isr(void *args)
{
    bool need_yield = false;
    gptimer_t *timer = (gptimer_t *)args;
    gptimer_group_t *group = timer->group;
    gptimer_alarm_cb_t on_alarm_cb = timer->on_alarm;
    uint32_t intr_status = timer_ll_get_intr_status(timer->hal.dev);

    if (intr_status & TIMER_LL_EVENT_ALARM(timer->timer_id)) {
        // Note: when alarm event happens, the alarm will be disabled automatically by hardware
        gptimer_alarm_event_data_t edata = {
            .count_value = timer_hal_capture_and_get_counter_value(&timer->hal),
            .alarm_value = timer->alarm_count,
        };

        portENTER_CRITICAL_ISR(&group->spinlock);
        timer_ll_clear_intr_status(timer->hal.dev, TIMER_LL_EVENT_ALARM(timer->timer_id));
        // for auto-reload, we need to re-enable the alarm manually
        if (timer->flags.auto_reload_on_alarm) {
            timer_ll_enable_alarm(timer->hal.dev, timer->timer_id, true);
        }
        portEXIT_CRITICAL_ISR(&group->spinlock);

        if (on_alarm_cb) {
            if (on_alarm_cb(timer, &edata, timer->user_ctx)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
