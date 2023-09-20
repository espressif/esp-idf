/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_GPTIMER_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "driver/gptimer.h"
#include "hal/timer_types.h"
#include "hal/timer_hal.h"
#include "hal/timer_ll.h"
#include "soc/timer_periph.h"
#include "esp_memory_utils.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "clk_ctrl_os.h"
#include "esp_clk_tree.h"
#include "gptimer_priv.h"

static const char *TAG = "gptimer";

#if SOC_PERIPH_CLK_CTRL_SHARED
#define GPTIMER_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define GPTIMER_CLOCK_SRC_ATOMIC()
#endif

typedef struct gptimer_platform_t {
    _lock_t mutex;                             // platform level mutex lock
    gptimer_group_t *groups[SOC_TIMER_GROUPS]; // timer group pool
    int group_ref_counts[SOC_TIMER_GROUPS];    // reference count used to protect group install/uninstall
} gptimer_platform_t;

// gptimer driver platform, it's always a singleton
static gptimer_platform_t s_platform;

static gptimer_group_t *gptimer_acquire_group_handle(int group_id);
static void gptimer_release_group_handle(gptimer_group_t *group);
static esp_err_t gptimer_select_periph_clock(gptimer_t *timer, gptimer_clock_source_t src_clk, uint32_t resolution_hz);
static void gptimer_default_isr(void *args);

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
    return ESP_OK;
}

static void gptimer_unregister_from_group(gptimer_t *timer)
{
    gptimer_group_t *group = timer->group;
    int timer_id = timer->timer_id;
    portENTER_CRITICAL(&group->spinlock);
    group->timers[timer_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);
    // timer has a reference on group, release it now
    gptimer_release_group_handle(group);
}

static esp_err_t gptimer_destroy(gptimer_t *timer)
{
    if (timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(timer->pm_lock), TAG, "delete pm_lock failed");
    }
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
#if CONFIG_GPTIMER_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    gptimer_t *timer = NULL;
    ESP_RETURN_ON_FALSE(config && ret_timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->resolution_hz, ESP_ERR_INVALID_ARG, TAG, "invalid timer resolution:%"PRIu32, config->resolution_hz);
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & GPTIMER_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG,
                            TAG, "invalid interrupt priority:%d", config->intr_priority);
    }

    timer = heap_caps_calloc(1, sizeof(gptimer_t), GPTIMER_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(timer, ESP_ERR_NO_MEM, err, TAG, "no mem for gptimer");
    // register timer to the group (because one group can have several timers)
    ESP_GOTO_ON_ERROR(gptimer_register_to_group(timer), err, TAG, "register timer failed");
    gptimer_group_t *group = timer->group;
    int group_id = group->group_id;
    int timer_id = timer->timer_id;

    // initialize HAL layer
    timer_hal_init(&timer->hal, group_id, timer_id);
    // select clock source, set clock resolution
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
    ESP_LOGD(TAG, "new gptimer (%d,%d) at %p, resolution=%"PRIu32"Hz", group_id, timer_id, timer, timer->resolution_hz);
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
        timer_ll_enable_clock(hal->dev, hal->timer_id, false);
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
    ESP_RETURN_ON_FALSE_ISR(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    portENTER_CRITICAL_SAFE(&timer->spinlock);
    timer_hal_set_counter_value(&timer->hal, value);
    portEXIT_CRITICAL_SAFE(&timer->spinlock);
    return ESP_OK;
}

esp_err_t gptimer_get_raw_count(gptimer_handle_t timer, unsigned long long *value)
{
    ESP_RETURN_ON_FALSE_ISR(timer && value, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

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
    ESP_RETURN_ON_FALSE_ISR(timer && value, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

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

#if CONFIG_GPTIMER_ISR_IRAM_SAFE
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
    ESP_RETURN_ON_FALSE_ISR(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    if (config) {
        // When auto_reload is enabled, alarm_count should not be equal to reload_count
        bool valid_auto_reload = !config->flags.auto_reload_on_alarm || config->alarm_count != config->reload_count;
        ESP_RETURN_ON_FALSE_ISR(valid_auto_reload, ESP_ERR_INVALID_ARG, TAG, "reload count can't equal to alarm count");

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
    gptimer_fsm_t expected_fsm = GPTIMER_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_ENABLE),
                        ESP_ERR_INVALID_STATE, TAG, "timer not in init state");

    // acquire power manager lock
    if (timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(timer->pm_lock), TAG, "acquire pm_lock failed");
    }

    // enable interrupt service
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_enable(timer->intr), TAG, "enable interrupt service failed");
    }

    return ESP_OK;
}

esp_err_t gptimer_disable(gptimer_handle_t timer)
{
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gptimer_fsm_t expected_fsm = GPTIMER_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_INIT),
                        ESP_ERR_INVALID_STATE, TAG, "timer not in enable state");

    // disable interrupt service
    if (timer->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_disable(timer->intr), TAG, "disable interrupt service failed");
    }

    // release power manager lock
    if (timer->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(timer->pm_lock), TAG, "release pm_lock failed");
    }

    return ESP_OK;
}

esp_err_t gptimer_start(gptimer_handle_t timer)
{
    ESP_RETURN_ON_FALSE_ISR(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    gptimer_fsm_t expected_fsm = GPTIMER_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_RUN_WAIT)) {
        // the register used by the following LL functions are shared with other API,
        // which is possible to run along with this function, so we need to protect
        portENTER_CRITICAL_SAFE(&timer->spinlock);
        timer_ll_enable_counter(timer->hal.dev, timer->timer_id, true);
        timer_ll_enable_alarm(timer->hal.dev, timer->timer_id, timer->flags.alarm_en);
        portEXIT_CRITICAL_SAFE(&timer->spinlock);
    } else {
        ESP_RETURN_ON_FALSE_ISR(false, ESP_ERR_INVALID_STATE, TAG, "timer is not enabled yet");
    }

    atomic_store(&timer->fsm, GPTIMER_FSM_RUN);
    return ESP_OK;
}

esp_err_t gptimer_stop(gptimer_handle_t timer)
{
    ESP_RETURN_ON_FALSE_ISR(timer, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    gptimer_fsm_t expected_fsm = GPTIMER_FSM_RUN;
    if (atomic_compare_exchange_strong(&timer->fsm, &expected_fsm, GPTIMER_FSM_ENABLE_WAIT)) {
        // disable counter, alarm, auto-reload
        portENTER_CRITICAL_SAFE(&timer->spinlock);
        timer_ll_enable_counter(timer->hal.dev, timer->timer_id, false);
        timer_ll_enable_alarm(timer->hal.dev, timer->timer_id, false);
        portEXIT_CRITICAL_SAFE(&timer->spinlock);
    } else {
        ESP_RETURN_ON_FALSE_ISR(false, ESP_ERR_INVALID_STATE, TAG, "timer is not running");
    }

    atomic_store(&timer->fsm, GPTIMER_FSM_ENABLE);
    return ESP_OK;
}

static gptimer_group_t *gptimer_acquire_group_handle(int group_id)
{
    bool new_group = false;
    gptimer_group_t *group = NULL;

    // prevent install timer group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(gptimer_group_t), GPTIMER_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group;
            // initialize timer group members
            group->group_id = group_id;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        }
    } else {
        group = s_platform.groups[group_id];
    }
    if (group) {
        // someone acquired the group handle means we have a new object that refer to this group
        s_platform.group_ref_counts[group_id]++;
    }
    _lock_release(&s_platform.mutex);

    if (new_group) {
        // !!! HARDWARE SHARED RESOURCE !!!
        // the gptimer and watchdog reside in the same the timer group
        // we need to increase/decrease the reference count before enable/disable/reset the peripheral
        PERIPH_RCC_ACQUIRE_ATOMIC(timer_group_periph_signals.groups[group_id].module, ref_count) {
            if (ref_count == 0) {
                timer_ll_enable_bus_clock(group_id, true);
                timer_ll_reset_register(group_id);
            }
        }
        ESP_LOGD(TAG, "new group (%d) @%p", group_id, group);
    }

    return group;
}

static void gptimer_release_group_handle(gptimer_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL;
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
        // disable bus clock for the timer group
        PERIPH_RCC_RELEASE_ATOMIC(timer_group_periph_signals.groups[group_id].module, ref_count) {
            if (ref_count == 0) {
                timer_ll_enable_bus_clock(group_id, false);
            }
        }
        free(group);
        ESP_LOGD(TAG, "del group (%d)", group_id);
    }
}

static esp_err_t gptimer_select_periph_clock(gptimer_t *timer, gptimer_clock_source_t src_clk, uint32_t resolution_hz)
{
    uint32_t counter_src_hz = 0;
    int timer_id = timer->timer_id;

    // TODO: [clk_tree] to use a generic clock enable/disable or acquire/release function for all clock source
#if SOC_TIMER_GROUP_SUPPORT_RC_FAST
    if (src_clk == GPTIMER_CLK_SRC_RC_FAST) {
        // RC_FAST clock is not enabled automatically on start up, we enable it here manually.
        // Note there's a ref count in the enable/disable function, we must call them in pair in the driver.
        periph_rtc_dig_clk8m_enable();
    }
#endif // SOC_TIMER_GROUP_SUPPORT_RC_FAST

    // get clock source frequency
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)src_clk, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &counter_src_hz),
                        TAG, "get clock source frequency failed");

#if CONFIG_PM_ENABLE
    bool need_pm_lock = true;
    // to make the gptimer work reliable, the source clock must stay alive and unchanged
    // driver will create different pm lock for that purpose, according to different clock source
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;

#if SOC_TIMER_GROUP_SUPPORT_RC_FAST
    if (src_clk == GPTIMER_CLK_SRC_RC_FAST) {
        // RC_FAST won't be turn off in sleep and won't change its frequency during DFS
        need_pm_lock = false;
    }
#endif // SOC_TIMER_GROUP_SUPPORT_RC_FAST

#if SOC_TIMER_GROUP_SUPPORT_APB
    if (src_clk == GPTIMER_CLK_SRC_APB) {
        // APB clock frequency can be changed during DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // SOC_TIMER_GROUP_SUPPORT_APB

#if CONFIG_IDF_TARGET_ESP32C2
    if (src_clk == GPTIMER_CLK_SRC_PLL_F40M) {
        // although PLL_F40M clock is a fixed PLL clock, which is unchangeable
        // on ESP32C2, PLL_F40M can be turned off even during DFS (unlike other PLL clocks)
        // so we're acquiring a fake "APB" lock here to prevent the system from doing DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // CONFIG_IDF_TARGET_ESP32C2

    if (need_pm_lock) {
        sprintf(timer->pm_lock_name, "gptimer_%d_%d", timer->group->group_id, timer_id); // e.g. gptimer_0_0
        ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_lock_type, 0, timer->pm_lock_name, &timer->pm_lock),
                            TAG, "create pm lock failed");
    }
#endif // CONFIG_PM_ENABLE

    // !!! HARDWARE SHARED RESOURCE !!!
    // on some ESP chip, different peripheral's clock source setting are mixed in the same register
    // so we need to make this done in an atomic way
    GPTIMER_CLOCK_SRC_ATOMIC() {
        timer_ll_set_clock_source(timer->hal.dev, timer_id, src_clk);
        timer_ll_enable_clock(timer->hal.dev, timer_id, true);
    }
    timer->clk_src = src_clk;
    uint32_t prescale = counter_src_hz / resolution_hz; // potential resolution loss here
    timer_ll_set_clock_prescale(timer->hal.dev, timer_id, prescale);
    timer->resolution_hz = counter_src_hz / prescale; // this is the real resolution
    if (timer->resolution_hz != resolution_hz) {
        ESP_LOGW(TAG, "resolution lost, expect %"PRIu32", real %"PRIu32, resolution_hz, timer->resolution_hz);
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
