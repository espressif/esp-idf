/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#if CONFIG_PCNT_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_rom_gpio.h"
#include "soc/soc_caps.h"
#include "soc/pcnt_periph.h"
#include "soc/gpio_pins.h"
#include "hal/pcnt_hal.h"
#include "hal/pcnt_ll.h"
#include "hal/gpio_hal.h"
#include "esp_private/esp_clk.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/sleep_retention.h"
#include "driver/gpio.h"
#include "esp_private/gpio.h"
#include "hal/gpio_ll.h" // for io_loop_back flag only
#include "driver/pulse_cnt.h"
#include "esp_memory_utils.h"

// If ISR handler is allowed to run whilst cache is disabled,
// Make sure all the code and related variables used by the handler are in the SRAM
#if CONFIG_PCNT_ISR_IRAM_SAFE || CONFIG_PCNT_CTRL_FUNC_IN_IRAM
#define PCNT_MEM_ALLOC_CAPS   (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define PCNT_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT
#endif

#if CONFIG_PCNT_ISR_IRAM_SAFE
#define PCNT_INTR_ALLOC_FLAGS (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#else
#define PCNT_INTR_ALLOC_FLAGS (ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#endif

#define PCNT_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

#define PCNT_PM_LOCK_NAME_LEN_MAX 16

#if !SOC_RCC_IS_INDEPENDENT
#define PCNT_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define PCNT_RCC_ATOMIC()
#endif

static const char *TAG = "pcnt";

typedef struct pcnt_platform_t pcnt_platform_t;
typedef struct pcnt_group_t pcnt_group_t;
typedef struct pcnt_unit_t pcnt_unit_t;
typedef struct pcnt_chan_t pcnt_chan_t;

// Use retention link only when the target supports sleep retention
#define PCNT_USE_RETENTION_LINK  (SOC_PCNT_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

#if PCNT_USE_RETENTION_LINK
static esp_err_t pcnt_create_sleep_retention_link_cb(void *arg);
#endif

struct pcnt_platform_t {
    _lock_t mutex;                         // platform level mutex lock
    pcnt_group_t *groups[SOC_PCNT_GROUPS]; // pcnt group pool
    int group_ref_counts[SOC_PCNT_GROUPS]; // reference count used to protect group install/uninstall
};

struct pcnt_group_t {
    int group_id;          // Group ID, index from 0
    int intr_priority;     // PCNT interrupt priority
    portMUX_TYPE spinlock; // to protect per-group register level concurrent access
    pcnt_hal_context_t hal;
    pcnt_unit_t *units[SOC_PCNT_UNITS_PER_GROUP]; // array of PCNT units
};

typedef struct {
    pcnt_ll_watch_event_id_t event_id; // event type
    int watch_point_value;             // value to be watched
} pcnt_watch_point_t;

typedef enum {
    PCNT_UNIT_FSM_INIT,
    PCNT_UNIT_FSM_ENABLE,
} pcnt_unit_fsm_t;

struct pcnt_unit_t {
    pcnt_group_t *group;                                  // which group the pcnt unit belongs to
    portMUX_TYPE spinlock;                                // Spinlock, stop one unit from accessing different parts of a same register concurrently
    int unit_id;                                          // allocated unit numerical ID
    int low_limit;                                        // low limit value
    int high_limit;                                       // high limit value
    int step_limit;                                       // step limit value
    int clear_signal_gpio_num;                            // which gpio clear signal input
    int accum_value;                                      // accumulated count value
    int step_interval;                             // PCNT step notify interval value
    pcnt_chan_t *channels[SOC_PCNT_CHANNELS_PER_UNIT];    // array of PCNT channels
    pcnt_watch_point_t watchers[PCNT_LL_WATCH_EVENT_MAX]; // array of PCNT watchers
    intr_handle_t intr;                                   // interrupt handle
    esp_pm_lock_handle_t pm_lock;                         // PM lock, for glitch filter, as that module can only be functional under APB
#if CONFIG_PM_ENABLE
    char pm_lock_name[PCNT_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
    pcnt_unit_fsm_t fsm;      // record PCNT unit's driver state
    pcnt_watch_cb_t on_reach; // user registered callback function
    void *user_data;          // user data registered by user, which would be passed to the right callback function
    struct {
        uint32_t accum_count: 1; /*!< Whether to accumulate the count value when overflows at the high/low limit */
#if SOC_PCNT_SUPPORT_STEP_NOTIFY
        uint32_t en_step_notify_up: 1;   /*!< Enable step notify in the positive direction*/
        uint32_t en_step_notify_down: 1; /*!< Enable step notify in the negative direction*/
#endif
    } flags;
};

struct pcnt_chan_t {
    pcnt_unit_t *unit;   // pointer to the PCNT unit where it derives from
    int channel_id;      // channel ID, index from 0
    int edge_gpio_num;
    int level_gpio_num;
};

// pcnt driver platform, it's always a singleton
static pcnt_platform_t s_platform;

static pcnt_group_t *pcnt_acquire_group_handle(int group_id);
static void pcnt_release_group_handle(pcnt_group_t *group);
static void pcnt_default_isr(void *args);

static esp_err_t pcnt_register_to_group(pcnt_unit_t *unit)
{
    pcnt_group_t *group = NULL;
    int unit_id = -1;
    for (int i = 0; i < SOC_PCNT_GROUPS; i++) {
        group = pcnt_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", i);
        // loop to search free unit in the group
        portENTER_CRITICAL(&group->spinlock);
        for (int j = 0; j < SOC_PCNT_UNITS_PER_GROUP; j++) {
            if (!group->units[j]) {
                unit_id = j;
                group->units[j] = unit;
                break;
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (unit_id < 0) {
            pcnt_release_group_handle(group);
        } else {
            unit->group = group;
            unit->unit_id = unit_id;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(unit_id != -1, ESP_ERR_NOT_FOUND, TAG, "no free unit");
    return ESP_OK;
}

static void pcnt_unregister_from_group(pcnt_unit_t *unit)
{
    pcnt_group_t *group = unit->group;
    int unit_id = unit->unit_id;
    portENTER_CRITICAL(&group->spinlock);
    group->units[unit_id] = NULL;
    portEXIT_CRITICAL(&group->spinlock);
    // unit has a reference on group, release it now
    pcnt_release_group_handle(group);
}

static esp_err_t pcnt_destroy(pcnt_unit_t *unit)
{
    if (unit->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(unit->pm_lock), TAG, "delete pm lock failed");
    }
    if (unit->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(unit->intr), TAG, "delete interrupt service failed");
    }
    if (unit->group) {
        pcnt_unregister_from_group(unit);
    }
    free(unit);
    return ESP_OK;
}

esp_err_t pcnt_new_unit(const pcnt_unit_config_t *config, pcnt_unit_handle_t *ret_unit)
{
#if CONFIG_PCNT_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    pcnt_unit_t *unit = NULL;
    ESP_GOTO_ON_FALSE(config && ret_unit, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(config->low_limit < 0 && config->high_limit > 0 && config->low_limit >= PCNT_LL_MIN_LIN &&
                      config->high_limit <= PCNT_LL_MAX_LIM, ESP_ERR_INVALID_ARG, err, TAG,
                      "invalid limit range:[%d,%d]", config->low_limit, config->high_limit);
    if (config->intr_priority) {
        ESP_GOTO_ON_FALSE(1 << (config->intr_priority) & PCNT_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, err,
                          TAG, "invalid interrupt priority:%d", config->intr_priority);
    }
#if PCNT_LL_STEP_NOTIFY_DIR_LIMIT
    ESP_GOTO_ON_FALSE(!(config->flags.en_step_notify_up && config->flags.en_step_notify_down), ESP_ERR_NOT_SUPPORTED, err, TAG, CONFIG_IDF_TARGET " can't trigger step notify in both direction");
    if (config->flags.en_step_notify_up) {
        ESP_LOGW(TAG, "can't overflow at low limit: %d due to hardware limitation", config->low_limit);
    }
    if (config->flags.en_step_notify_down) {
        ESP_LOGW(TAG, "can't overflow at high limit: %d due to hardware limitation", config->high_limit);
    }
#endif
    unit = heap_caps_calloc(1, sizeof(pcnt_unit_t), PCNT_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(unit, ESP_ERR_NO_MEM, err, TAG, "no mem for unit");
    // register unit to the group (because one group can have several units)
    ESP_GOTO_ON_ERROR(pcnt_register_to_group(unit), err, TAG, "register unit failed");
    pcnt_group_t *group = unit->group;
    int group_id = group->group_id;
    int unit_id = unit->unit_id;

    // if interrupt priority specified before, it cannot be changed until the group is released
    // check if the new priority specified consistents with the old one
    bool intr_priority_conflict = false;
    portENTER_CRITICAL(&group->spinlock);
    if (group->intr_priority == -1) {
        group->intr_priority = config->intr_priority;
    } else if (config->intr_priority != 0) {
        intr_priority_conflict = (group->intr_priority != config->intr_priority);
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_GOTO_ON_FALSE(!intr_priority_conflict, ESP_ERR_INVALID_STATE, err, TAG, "intr_priority conflict, already is %d but attempt to %d", group->intr_priority, config->intr_priority);

    // to accumulate count value, we should install the interrupt handler first, and in the ISR we do the accumulation
    bool to_install_isr = (config->flags.accum_count == 1);
    if (to_install_isr) {
        int isr_flags = PCNT_INTR_ALLOC_FLAGS;
        if (group->intr_priority) {
            isr_flags |= 1 << (group->intr_priority);
        } else {
            isr_flags |= PCNT_ALLOW_INTR_PRIORITY_MASK;
        }
        ESP_GOTO_ON_ERROR(esp_intr_alloc_intrstatus(pcnt_periph_signals.groups[group_id].irq, isr_flags,
                                                    (uint32_t)pcnt_ll_get_intr_status_reg(group->hal.dev), PCNT_LL_UNIT_WATCH_EVENT(unit_id),
                                                    pcnt_default_isr, unit, &unit->intr), err,
                          TAG, "install interrupt service failed");
    }

    // PCNT uses the APB as its function clock,
    // and its filter module is sensitive to the clock frequency
#if CONFIG_PM_ENABLE
    sprintf(unit->pm_lock_name, "pcnt_%d_%d", group_id, unit_id); // e.g. pcnt_0_0
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, unit->pm_lock_name, &unit->pm_lock), err, TAG, "install pm lock failed");
    ESP_LOGD(TAG, "install APB_FREQ_MAX lock for unit (%d,%d)", group_id, unit_id);
#endif

    // some events are enabled by default, disable them all
    pcnt_ll_disable_all_events(group->hal.dev, unit_id);
    // disable filter by default
    pcnt_ll_enable_glitch_filter(group->hal.dev, unit_id, false);
    // set default high/low limitation value
    // note: limit value takes effect only after counter clear
    pcnt_ll_set_high_limit_value(group->hal.dev, unit_id, config->high_limit);
    pcnt_ll_set_low_limit_value(group->hal.dev, unit_id, config->low_limit);
    unit->high_limit = config->high_limit;
    unit->low_limit = config->low_limit;
    unit->accum_value = 0;
    unit->clear_signal_gpio_num = -1;
    unit->flags.accum_count = config->flags.accum_count;

#if SOC_PCNT_SUPPORT_STEP_NOTIFY
    unit->flags.en_step_notify_down = config->flags.en_step_notify_down;
    unit->flags.en_step_notify_up = config->flags.en_step_notify_up;
#if PCNT_LL_STEP_NOTIFY_DIR_LIMIT
    if (config->flags.en_step_notify_up) {
        unit->step_limit = config->high_limit;
    } else if (config->flags.en_step_notify_down) {
        unit->step_limit = config->low_limit;
    }
    pcnt_ll_set_step_limit_value(group->hal.dev, unit_id, unit->step_limit);
#endif
#endif

    // clear/pause register is shared by all units, so using group's spinlock
    portENTER_CRITICAL(&group->spinlock);
    pcnt_ll_stop_count(group->hal.dev, unit_id);
    pcnt_ll_clear_count(group->hal.dev, unit_id);
    // enable the interrupt if we want to accumulate the counter in the ISR
    pcnt_ll_enable_intr(group->hal.dev, PCNT_LL_UNIT_WATCH_EVENT(unit_id), to_install_isr);
    pcnt_ll_clear_intr_status(group->hal.dev, PCNT_LL_UNIT_WATCH_EVENT(unit_id));
    portEXIT_CRITICAL(&group->spinlock);

    unit->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    unit->fsm = PCNT_UNIT_FSM_INIT;

    for (int i = 0; i < PCNT_LL_WATCH_EVENT_MAX; i++) {
        unit->watchers[i].event_id = PCNT_LL_WATCH_EVENT_INVALID; // invalid all watch point
    }

    ESP_LOGD(TAG, "new pcnt unit (%d,%d) at %p, count range:[%d,%d]", group_id, unit_id, unit, unit->low_limit, unit->high_limit);
    *ret_unit = unit;
    return ESP_OK;

err:
    if (unit) {
        pcnt_destroy(unit);
    }
    return ret;
}

esp_err_t pcnt_del_unit(pcnt_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(unit->fsm == PCNT_UNIT_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "unit not in init state");
    pcnt_group_t *group = unit->group;
    int group_id = group->group_id;
    int unit_id = unit->unit_id;

    for (int i = 0; i < SOC_PCNT_CHANNELS_PER_UNIT; i++) {
        ESP_RETURN_ON_FALSE(!unit->channels[i], ESP_ERR_INVALID_STATE, TAG, "channel %d still in working", i);
    }

#if SOC_PCNT_SUPPORT_CLEAR_SIGNAL
    if (unit->clear_signal_gpio_num >= 0) {
        gpio_reset_pin(unit->clear_signal_gpio_num);
    }
#endif // SOC_PCNT_SUPPORT_CLEAR_SIGNAL

    ESP_LOGD(TAG, "del unit (%d,%d)", group_id, unit_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(pcnt_destroy(unit), TAG, "destroy pcnt unit failed");
    return ESP_OK;
}

#if SOC_PCNT_SUPPORT_CLEAR_SIGNAL
esp_err_t pcnt_unit_set_clear_signal(pcnt_unit_handle_t unit, const pcnt_clear_signal_config_t *config)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    pcnt_group_t *group = unit->group;
    int group_id = group->group_id;
    int unit_id = unit->unit_id;
    uint32_t clear_signal_idx = pcnt_periph_signals.groups[group_id].units[unit_id].clear_sig;

    if (config) {
        int io_num = config->clear_signal_gpio_num;
        ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), ESP_ERR_INVALID_ARG, TAG, "gpio num is invalid");

        if (!config->flags.invert_clear_signal) {
            gpio_pulldown_en(io_num);
            gpio_pullup_dis(io_num);
        } else {
            gpio_pullup_en(io_num);
            gpio_pulldown_dis(io_num);
        }
        gpio_func_sel(io_num, PIN_FUNC_GPIO);
        gpio_input_enable(io_num);
        esp_rom_gpio_connect_in_signal(io_num, clear_signal_idx, config->flags.invert_clear_signal);
        unit->clear_signal_gpio_num = config->clear_signal_gpio_num;

        // io_loop_back is a deprecated flag, workaround for compatibility
        if (config->flags.io_loop_back) {
            gpio_ll_output_enable(&GPIO, io_num);
        }
    } else {
        ESP_RETURN_ON_FALSE(unit->clear_signal_gpio_num >= 0, ESP_ERR_INVALID_STATE, TAG, "zero signal not set yet");
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, clear_signal_idx, 0);
        gpio_pullup_dis(unit->clear_signal_gpio_num);
        gpio_pulldown_dis(unit->clear_signal_gpio_num);
        unit->clear_signal_gpio_num = -1;
    }
    return ESP_OK;
}
#endif // SOC_PCNT_SUPPORT_CLEAR_SIGNAL

esp_err_t pcnt_unit_set_glitch_filter(pcnt_unit_handle_t unit, const pcnt_glitch_filter_config_t *config)
{
    pcnt_group_t *group = NULL;
    uint32_t glitch_filter_thres = 0;
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // glitch filter should be set only when unit is in init state
    ESP_RETURN_ON_FALSE(unit->fsm == PCNT_UNIT_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "unit not in init state");
    group = unit->group;
    if (config) {
        glitch_filter_thres = esp_clk_apb_freq() / 1000000 * config->max_glitch_ns / 1000;
        ESP_RETURN_ON_FALSE(glitch_filter_thres <= PCNT_LL_MAX_GLITCH_WIDTH, ESP_ERR_INVALID_ARG, TAG, "glitch width out of range");
    }

    // filter control bit is mixed with other PCNT control bits in the same register
    portENTER_CRITICAL(&unit->spinlock);
    if (config) {
        pcnt_ll_set_glitch_filter_thres(group->hal.dev, unit->unit_id, glitch_filter_thres);
        pcnt_ll_enable_glitch_filter(group->hal.dev, unit->unit_id, true);
    } else {
        pcnt_ll_enable_glitch_filter(group->hal.dev, unit->unit_id, false);
    }
    portEXIT_CRITICAL(&unit->spinlock);

    return ESP_OK;
}

esp_err_t pcnt_unit_enable(pcnt_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(unit->fsm == PCNT_UNIT_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "unit not in init state");

    // acquire power manager lock
    if (unit->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(unit->pm_lock), TAG, "acquire pm_lock failed");
    }
    // enable interrupt service
    if (unit->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_enable(unit->intr), TAG, "enable interrupt service failed");
    }

    unit->fsm = PCNT_UNIT_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t pcnt_unit_disable(pcnt_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(unit->fsm == PCNT_UNIT_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "unit not in enable state");

    // disable interrupt service
    if (unit->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_disable(unit->intr), TAG, "disable interrupt service failed");
    }
    // release power manager lock
    if (unit->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(unit->pm_lock), TAG, "release APB_FREQ_MAX lock failed");
    }

    unit->fsm = PCNT_UNIT_FSM_INIT;
    return ESP_OK;
}

esp_err_t pcnt_unit_start(pcnt_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE_ISR(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(unit->fsm == PCNT_UNIT_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "unit not enabled yet");
    pcnt_group_t *group = unit->group;

    // all PCNT units share the same register to control counter
    portENTER_CRITICAL_SAFE(&group->spinlock);
    pcnt_ll_start_count(group->hal.dev, unit->unit_id);
    portEXIT_CRITICAL_SAFE(&group->spinlock);

    return ESP_OK;
}

esp_err_t pcnt_unit_stop(pcnt_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE_ISR(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(unit->fsm == PCNT_UNIT_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "unit not enabled yet");
    pcnt_group_t *group = unit->group;

    // all PCNT units share the same register to control counter
    portENTER_CRITICAL_SAFE(&group->spinlock);
    pcnt_ll_stop_count(group->hal.dev, unit->unit_id);
    portEXIT_CRITICAL_SAFE(&group->spinlock);

    return ESP_OK;
}

esp_err_t pcnt_unit_clear_count(pcnt_unit_handle_t unit)
{
    pcnt_group_t *group = NULL;
    ESP_RETURN_ON_FALSE_ISR(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    group = unit->group;

    // all PCNT units share the same register to control counter
    portENTER_CRITICAL_SAFE(&group->spinlock);
    pcnt_ll_clear_count(group->hal.dev, unit->unit_id);
    portEXIT_CRITICAL_SAFE(&group->spinlock);

    // reset the accumulated count as well
    portENTER_CRITICAL_SAFE(&unit->spinlock);
    unit->accum_value = 0;
    portEXIT_CRITICAL_SAFE(&unit->spinlock);

    return ESP_OK;
}

esp_err_t pcnt_unit_get_count(pcnt_unit_handle_t unit, int *value)
{
    pcnt_group_t *group = NULL;
    ESP_RETURN_ON_FALSE_ISR(unit && value, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    group = unit->group;

    // the accum_value is also accessed by the ISR, so adding a critical section
    portENTER_CRITICAL_SAFE(&unit->spinlock);
    *value = pcnt_ll_get_count(group->hal.dev, unit->unit_id) + unit->accum_value;
    portEXIT_CRITICAL_SAFE(&unit->spinlock);

    return ESP_OK;
}

esp_err_t pcnt_unit_register_event_callbacks(pcnt_unit_handle_t unit, const pcnt_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(unit && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // unit event callbacks should be registered in init state
    pcnt_group_t *group = unit->group;
    int group_id = group->group_id;
    int unit_id = unit->unit_id;

#if CONFIG_PCNT_ISR_IRAM_SAFE
    if (cbs->on_reach) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_reach), ESP_ERR_INVALID_ARG, TAG, "on_reach callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    // lazy install interrupt service
    if (!unit->intr) {
        ESP_RETURN_ON_FALSE(unit->fsm == PCNT_UNIT_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "unit not in init state");
        int isr_flags = PCNT_INTR_ALLOC_FLAGS;
        if (group->intr_priority) {
            isr_flags |= 1 << (group->intr_priority);
        } else {
            isr_flags |= PCNT_ALLOW_INTR_PRIORITY_MASK;
        }
        ESP_RETURN_ON_ERROR(esp_intr_alloc_intrstatus(pcnt_periph_signals.groups[group_id].irq, isr_flags,
                                                      (uint32_t)pcnt_ll_get_intr_status_reg(group->hal.dev), PCNT_LL_UNIT_WATCH_EVENT(unit_id),
                                                      pcnt_default_isr, unit, &unit->intr),
                            TAG, "install interrupt service failed");
    }
    // enable/disable PCNT interrupt events
    portENTER_CRITICAL(&group->spinlock);
    pcnt_ll_enable_intr(group->hal.dev, PCNT_LL_UNIT_WATCH_EVENT(unit_id), cbs->on_reach != NULL);
    portEXIT_CRITICAL(&group->spinlock);

    unit->on_reach = cbs->on_reach;
    unit->user_data = user_data;
    return ESP_OK;
}

esp_err_t pcnt_unit_add_watch_point(pcnt_unit_handle_t unit, int watch_point)
{
    esp_err_t ret = ESP_OK;
    pcnt_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(watch_point <= unit->high_limit && watch_point >= unit->low_limit,
                        ESP_ERR_INVALID_ARG, TAG, "watch_point out of limit");
    group = unit->group;

    // event enable/disable is mixed with other control function in the same register
    portENTER_CRITICAL(&unit->spinlock);
    // zero cross watch point
    if (watch_point == 0) {
        if (unit->watchers[PCNT_LL_WATCH_EVENT_ZERO_CROSS].event_id != PCNT_LL_WATCH_EVENT_INVALID) {
            ret = ESP_ERR_INVALID_STATE; // zero cross event watcher has been installed already
        } else {
            unit->watchers[PCNT_LL_WATCH_EVENT_ZERO_CROSS].event_id = PCNT_LL_WATCH_EVENT_ZERO_CROSS;
            unit->watchers[PCNT_LL_WATCH_EVENT_ZERO_CROSS].watch_point_value = 0;
            pcnt_ll_enable_zero_cross_event(group->hal.dev, unit->unit_id, true);
        }
    }
    // high limit watch point
    else if (watch_point == unit->high_limit) {
        if (unit->watchers[PCNT_LL_WATCH_EVENT_HIGH_LIMIT].event_id != PCNT_LL_WATCH_EVENT_INVALID) {
            ret = ESP_ERR_INVALID_STATE; // high limit event watcher has been installed already
        } else {
            unit->watchers[PCNT_LL_WATCH_EVENT_HIGH_LIMIT].event_id = PCNT_LL_WATCH_EVENT_HIGH_LIMIT;
            unit->watchers[PCNT_LL_WATCH_EVENT_HIGH_LIMIT].watch_point_value = unit->high_limit;
            pcnt_ll_enable_high_limit_event(group->hal.dev, unit->unit_id, true);
        }
    }
    // low limit watch point
    else if (watch_point == unit->low_limit) {
        if (unit->watchers[PCNT_LL_WATCH_EVENT_LOW_LIMIT].event_id != PCNT_LL_WATCH_EVENT_INVALID) {
            ret = ESP_ERR_INVALID_STATE; // low limit event watcher has been installed already
        } else {
            unit->watchers[PCNT_LL_WATCH_EVENT_LOW_LIMIT].event_id = PCNT_LL_WATCH_EVENT_LOW_LIMIT;
            unit->watchers[PCNT_LL_WATCH_EVENT_LOW_LIMIT].watch_point_value = unit->low_limit;
            pcnt_ll_enable_low_limit_event(group->hal.dev, unit->unit_id, true);
        }
    }
    // other threshold watch point
    else {
        int thres_num = SOC_PCNT_THRES_POINT_PER_UNIT - 1;
        switch (thres_num) {
        case 1:
            if (unit->watchers[PCNT_LL_WATCH_EVENT_THRES1].event_id == PCNT_LL_WATCH_EVENT_INVALID) {
                unit->watchers[PCNT_LL_WATCH_EVENT_THRES1].event_id = PCNT_LL_WATCH_EVENT_THRES1;
                unit->watchers[PCNT_LL_WATCH_EVENT_THRES1].watch_point_value = watch_point;
                pcnt_ll_set_thres_value(group->hal.dev, unit->unit_id, 1, watch_point);
                pcnt_ll_enable_thres_event(group->hal.dev, unit->unit_id, 1, true);
                break;
            } else if (unit->watchers[PCNT_LL_WATCH_EVENT_THRES1].watch_point_value == watch_point) {
                ret = ESP_ERR_INVALID_STATE;
                break;
            }
        /* fall-through */
        case 0:
            if (unit->watchers[PCNT_LL_WATCH_EVENT_THRES0].event_id == PCNT_LL_WATCH_EVENT_INVALID) {
                unit->watchers[PCNT_LL_WATCH_EVENT_THRES0].event_id = PCNT_LL_WATCH_EVENT_THRES0;
                unit->watchers[PCNT_LL_WATCH_EVENT_THRES0].watch_point_value = watch_point;
                pcnt_ll_set_thres_value(group->hal.dev, unit->unit_id, 0, watch_point);
                pcnt_ll_enable_thres_event(group->hal.dev, unit->unit_id, 0, true);
                break;
            } else if (unit->watchers[PCNT_LL_WATCH_EVENT_THRES0].watch_point_value == watch_point) {
                ret = ESP_ERR_INVALID_STATE;
                break;
            }
        /* fall-through */
        default:
            ret = ESP_ERR_NOT_FOUND; // no free threshold watch point available
            break;
        }
    }
    portEXIT_CRITICAL(&unit->spinlock);
    ESP_RETURN_ON_ERROR(ret, TAG, "add watchpoint %d failed", watch_point);

    return ESP_OK;
}

esp_err_t pcnt_unit_remove_watch_point(pcnt_unit_handle_t unit, int watch_point)
{
    pcnt_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    group = unit->group;
    pcnt_ll_watch_event_id_t event_id = PCNT_LL_WATCH_EVENT_INVALID;

    // event enable/disable is mixed with other control function in the same register
    portENTER_CRITICAL(&unit->spinlock);
    for (int i = 0; i < PCNT_LL_WATCH_EVENT_MAX; i++) {
        if (unit->watchers[i].event_id != PCNT_LL_WATCH_EVENT_INVALID && unit->watchers[i].watch_point_value == watch_point) {
            event_id = unit->watchers[i].event_id;
            unit->watchers[i].event_id = PCNT_LL_WATCH_EVENT_INVALID;
            break;
        }
    }
    switch (event_id) {
    case PCNT_LL_WATCH_EVENT_ZERO_CROSS:
        pcnt_ll_enable_zero_cross_event(group->hal.dev, unit->unit_id, false);
        break;
    case PCNT_LL_WATCH_EVENT_LOW_LIMIT:
        pcnt_ll_enable_low_limit_event(group->hal.dev, unit->unit_id, false);
        break;
    case PCNT_LL_WATCH_EVENT_HIGH_LIMIT:
        pcnt_ll_enable_high_limit_event(group->hal.dev, unit->unit_id, false);
        break;
    case PCNT_LL_WATCH_EVENT_THRES0:
        pcnt_ll_enable_thres_event(group->hal.dev, unit->unit_id, 0, false);
        break;
    case PCNT_LL_WATCH_EVENT_THRES1:
        pcnt_ll_enable_thres_event(group->hal.dev, unit->unit_id, 1, false);
        break;
    default:
        break;
    }
    portEXIT_CRITICAL(&unit->spinlock);
    ESP_RETURN_ON_FALSE(event_id != PCNT_LL_WATCH_EVENT_INVALID, ESP_ERR_INVALID_STATE, TAG, "watch point %d not added yet", watch_point);
    return ESP_OK;
}

#if SOC_PCNT_SUPPORT_STEP_NOTIFY
esp_err_t pcnt_unit_add_watch_step(pcnt_unit_handle_t unit, int step_interval)
{
    pcnt_group_t *group = NULL;

    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE((step_interval > 0 && unit->flags.en_step_notify_up) || (step_interval < 0 && unit->flags.en_step_notify_down),
                        ESP_ERR_INVALID_ARG, TAG, "invalid step interval");
    ESP_RETURN_ON_FALSE(step_interval >= unit->low_limit && step_interval <= unit->high_limit,
                        ESP_ERR_INVALID_ARG, TAG, "step interval out of range [%d,%d]", unit->low_limit, unit->high_limit);
    ESP_RETURN_ON_FALSE(unit->step_interval == 0,
                        ESP_ERR_INVALID_STATE, TAG, "watch step has been set to %d already", unit->step_interval);

    group = unit->group;
    unit->step_interval = step_interval;
    pcnt_ll_set_step_value(group->hal.dev, unit->unit_id, step_interval);
    // different units are mixing in the same register, so we use the group's spinlock here
    portENTER_CRITICAL(&group->spinlock);
    pcnt_ll_enable_step_notify(group->hal.dev, unit->unit_id, true);
    portEXIT_CRITICAL(&group->spinlock);

    return ESP_OK;
}

esp_err_t pcnt_unit_remove_watch_step(pcnt_unit_handle_t unit)
{
    pcnt_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    group = unit->group;
    ESP_RETURN_ON_FALSE(unit->step_interval != 0, ESP_ERR_INVALID_STATE, TAG, "watch step not added yet");

    unit->step_interval = 0;

    portENTER_CRITICAL(&group->spinlock);
    pcnt_ll_enable_step_notify(group->hal.dev, unit->unit_id, false);
    portEXIT_CRITICAL(&group->spinlock);

    return ESP_OK;
}
#endif //SOC_PCNT_SUPPORT_STEP_NOTIFY

esp_err_t pcnt_new_channel(pcnt_unit_handle_t unit, const pcnt_chan_config_t *config, pcnt_channel_handle_t *ret_chan)
{
    esp_err_t ret = ESP_OK;
    pcnt_chan_t *channel = NULL;
    pcnt_group_t *group = NULL;
    ESP_GOTO_ON_FALSE(unit && config && ret_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(unit->fsm == PCNT_UNIT_FSM_INIT, ESP_ERR_INVALID_STATE, err, TAG, "unit not in init state");
    group = unit->group;
    int group_id = group->group_id;
    int unit_id = unit->unit_id;

    channel = heap_caps_calloc(1, sizeof(pcnt_chan_t), PCNT_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(channel, ESP_ERR_NO_MEM, err, TAG, "no mem for channel");

    // search for a free channel
    int channel_id = -1;
    portENTER_CRITICAL(&unit->spinlock);
    for (int i = 0; i < SOC_PCNT_CHANNELS_PER_UNIT; i++) {
        if (!unit->channels[i]) {
            channel_id = i;
            unit->channels[channel_id] = channel;
            break;
        }
    }
    portEXIT_CRITICAL(&unit->spinlock);
    ESP_GOTO_ON_FALSE(channel_id != -1, ESP_ERR_NOT_FOUND, err, TAG, "no free channel in unit (%d,%d)", group_id, unit_id);

    // GPIO configuration
    if (config->edge_gpio_num >= 0) {
        gpio_pullup_en(config->edge_gpio_num);
        gpio_pulldown_dis(config->edge_gpio_num);
        gpio_func_sel(config->edge_gpio_num, PIN_FUNC_GPIO);
        gpio_input_enable(config->edge_gpio_num);
        esp_rom_gpio_connect_in_signal(config->edge_gpio_num,
                                       pcnt_periph_signals.groups[group_id].units[unit_id].channels[channel_id].pulse_sig,
                                       config->flags.invert_edge_input);

        // io_loop_back is a deprecated flag, workaround for compatibility
        if (config->flags.io_loop_back) {
            gpio_ll_output_enable(&GPIO, config->edge_gpio_num);
        }
    } else {
        // using virtual IO
        esp_rom_gpio_connect_in_signal(config->flags.virt_edge_io_level ? GPIO_MATRIX_CONST_ONE_INPUT : GPIO_MATRIX_CONST_ZERO_INPUT,
                                       pcnt_periph_signals.groups[group_id].units[unit_id].channels[channel_id].pulse_sig,
                                       config->flags.invert_edge_input);
    }

    if (config->level_gpio_num >= 0) {
        gpio_pullup_en(config->level_gpio_num);
        gpio_pulldown_dis(config->level_gpio_num);
        gpio_func_sel(config->level_gpio_num, PIN_FUNC_GPIO);
        gpio_input_enable(config->level_gpio_num);
        esp_rom_gpio_connect_in_signal(config->level_gpio_num,
                                       pcnt_periph_signals.groups[group_id].units[unit_id].channels[channel_id].control_sig,
                                       config->flags.invert_level_input);

        // io_loop_back is a deprecated flag, workaround for compatibility
        if (config->flags.io_loop_back) {
            gpio_ll_output_enable(&GPIO, config->level_gpio_num);
        }
    } else {
        // using virtual IO
        esp_rom_gpio_connect_in_signal(config->flags.virt_level_io_level ? GPIO_MATRIX_CONST_ONE_INPUT : GPIO_MATRIX_CONST_ZERO_INPUT,
                                       pcnt_periph_signals.groups[group_id].units[unit_id].channels[channel_id].control_sig,
                                       config->flags.invert_level_input);
    }

    channel->channel_id = channel_id;
    channel->unit = unit;
    channel->edge_gpio_num = config->edge_gpio_num;
    channel->level_gpio_num = config->level_gpio_num;
    ESP_LOGD(TAG, "new pcnt channel(%d,%d,%d) at %p", group_id, unit_id, channel_id, channel);

    *ret_chan = channel;
    return ESP_OK;
err:
    if (channel) {
        free(channel);
    }
    return ret;
}

esp_err_t pcnt_del_channel(pcnt_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    pcnt_unit_t *unit = chan->unit;
    pcnt_group_t *group = unit->group;
    int group_id = group->group_id;
    int unit_id = unit->unit_id;
    int channel_id = chan->channel_id;

    portENTER_CRITICAL(&unit->spinlock);
    unit->channels[channel_id] = NULL;
    portEXIT_CRITICAL(&unit->spinlock);

    if (chan->level_gpio_num >= 0) {
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT,
                                       pcnt_periph_signals.groups[group_id].units[unit_id].channels[channel_id].control_sig,
                                       0);
        gpio_pullup_dis(chan->level_gpio_num);
        gpio_pulldown_dis(chan->level_gpio_num);
    }
    if (chan->edge_gpio_num >= 0) {
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT,
                                       pcnt_periph_signals.groups[group_id].units[unit_id].channels[channel_id].pulse_sig,
                                       0);
        gpio_pullup_dis(chan->edge_gpio_num);
        gpio_pulldown_dis(chan->edge_gpio_num);
    }

    free(chan);
    ESP_LOGD(TAG, "del pcnt channel(%d,%d,%d)", group_id, unit_id, channel_id);
    return ESP_OK;
}

esp_err_t pcnt_channel_set_edge_action(pcnt_channel_handle_t chan, pcnt_channel_edge_action_t pos_act, pcnt_channel_edge_action_t neg_act)
{
    pcnt_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    pcnt_unit_t *unit = chan->unit;
    group = unit->group;

    // mode control bits are mixed with other PCNT control bits in a same register
    portENTER_CRITICAL(&unit->spinlock);
    pcnt_ll_set_edge_action(group->hal.dev, unit->unit_id, chan->channel_id, pos_act, neg_act);
    portEXIT_CRITICAL(&unit->spinlock);

    return ESP_OK;
}

esp_err_t pcnt_channel_set_level_action(pcnt_channel_handle_t chan, pcnt_channel_level_action_t high_act, pcnt_channel_level_action_t low_act)
{
    pcnt_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    pcnt_unit_t *unit = chan->unit;
    group = unit->group;

    // mode control bits are mixed with other PCNT control bits in a same register
    portENTER_CRITICAL(&unit->spinlock);
    pcnt_ll_set_level_action(group->hal.dev, unit->unit_id, chan->channel_id, high_act, low_act);
    portEXIT_CRITICAL(&unit->spinlock);

    return ESP_OK;
}

static pcnt_group_t *pcnt_acquire_group_handle(int group_id)
{
    bool new_group = false;
    pcnt_group_t *group = NULL;

    // prevent install pcnt group concurrently
    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        group = heap_caps_calloc(1, sizeof(pcnt_group_t), PCNT_MEM_ALLOC_CAPS);
        if (group) {
            new_group = true;
            s_platform.groups[group_id] = group; // register to platform
            // initialize pcnt group members
            group->group_id = group_id;
            group->intr_priority = -1;
            group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            // enable APB access pcnt registers
            PCNT_RCC_ATOMIC() {
                pcnt_ll_enable_bus_clock(group_id, true);
                pcnt_ll_reset_register(group_id);
            }
#if PCNT_USE_RETENTION_LINK
            sleep_retention_module_t module_id = pcnt_reg_retention_info[group_id].retention_module;
            sleep_retention_module_init_param_t init_param = {
                .cbs = {
                    .create = {
                        .handle = pcnt_create_sleep_retention_link_cb,
                        .arg = group,
                    },
                },
                .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
            };
            // we only do retention init here. Allocate retention module in the unit initialization
            if (sleep_retention_module_init(module_id, &init_param) != ESP_OK) {
                // even though the sleep retention module init failed, PCNT driver should still work, so just warning here
                ESP_LOGW(TAG, "init sleep retention failed %d, power domain may be turned off during sleep", group_id);
            }
#endif // PCNT_USE_RETENTION_LINK
            // initialize HAL context
            pcnt_hal_init(&group->hal, group_id);
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
        ESP_LOGD(TAG, "new group (%d) at %p", group_id, group);
    }

    return group;
}

static void pcnt_release_group_handle(pcnt_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    _lock_acquire(&s_platform.mutex);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        s_platform.groups[group_id] = NULL; // deregister from platform
        PCNT_RCC_ATOMIC() {
            pcnt_ll_enable_bus_clock(group_id, false);
        }
    }
    _lock_release(&s_platform.mutex);

    if (do_deinitialize) {
#if PCNT_USE_RETENTION_LINK
        const periph_retention_module_t module_id = pcnt_reg_retention_info[group_id].retention_module;
        if (sleep_retention_is_module_inited(module_id)) {
            sleep_retention_module_deinit(module_id);
        }
#endif // PCNT_USE_RETENTION_LINK
        free(group);
        ESP_LOGD(TAG, "del group (%d)", group_id);
    }
}

IRAM_ATTR static void pcnt_default_isr(void *args)
{
    bool need_yield = false;
    pcnt_unit_t *unit = (pcnt_unit_t *)args;
    int unit_id = unit->unit_id;
    pcnt_group_t *group = unit->group;
    pcnt_watch_cb_t on_reach = unit->on_reach;

    uint32_t intr_status = pcnt_ll_get_intr_status(group->hal.dev);
    if (intr_status & PCNT_LL_UNIT_WATCH_EVENT(unit_id)) {
        pcnt_ll_clear_intr_status(group->hal.dev, PCNT_LL_UNIT_WATCH_EVENT(unit_id));

        // event status word contains information about the real watch event type
        uint32_t event_status = pcnt_ll_get_event_status(group->hal.dev, unit_id);

        int count_value = 0;
        pcnt_unit_zero_cross_mode_t zc_mode = PCNT_UNIT_ZERO_CROSS_INVALID;

        // using while loop so that we don't miss any event
        while (event_status) {
#if SOC_PCNT_SUPPORT_STEP_NOTIFY
            // step event has higher priority than pointer event
            if (event_status & (BIT(PCNT_LL_STEP_EVENT_REACH_INTERVAL) | BIT(PCNT_LL_STEP_EVENT_REACH_LIMIT))) {
                if (event_status & BIT(PCNT_LL_STEP_EVENT_REACH_INTERVAL)) {
                    event_status &= ~BIT(PCNT_LL_STEP_EVENT_REACH_INTERVAL);
                    // align current count value to the step interval
                    count_value = pcnt_ll_get_count(group->hal.dev, unit_id);
                }
                if (event_status & BIT(PCNT_LL_STEP_EVENT_REACH_LIMIT)) {
                    event_status &= ~BIT(PCNT_LL_STEP_EVENT_REACH_LIMIT);
                    // adjust current count value to the step limit
                    count_value = unit->step_limit;
                    if (unit->flags.accum_count) {
                        portENTER_CRITICAL_ISR(&unit->spinlock);
                        unit->accum_value += unit->step_limit;
                        portEXIT_CRITICAL_ISR(&unit->spinlock);
                    }
                }
                // step event may happen with other pointer event at the same time, we don't need to process them again
                event_status &= ~(BIT(PCNT_LL_WATCH_EVENT_LOW_LIMIT) | BIT(PCNT_LL_WATCH_EVENT_HIGH_LIMIT) |
                                  BIT(PCNT_LL_WATCH_EVENT_THRES0) | BIT(PCNT_LL_WATCH_EVENT_THRES1));
            } else
#endif // SOC_PCNT_SUPPORT_STEP_NOTIFY
                if (event_status & BIT(PCNT_LL_WATCH_EVENT_LOW_LIMIT)) {
                    event_status &= ~(BIT(PCNT_LL_WATCH_EVENT_LOW_LIMIT));
                    // adjust the count value to reflect the actual watch point value
                    count_value = unit->low_limit;
                    if (unit->flags.accum_count) {
                        portENTER_CRITICAL_ISR(&unit->spinlock);
                        unit->accum_value += unit->low_limit;
                        portEXIT_CRITICAL_ISR(&unit->spinlock);
                    }
                } else if (event_status & BIT(PCNT_LL_WATCH_EVENT_HIGH_LIMIT)) {
                    event_status &= ~(BIT(PCNT_LL_WATCH_EVENT_HIGH_LIMIT));
                    // adjust the count value to reflect the actual watch point value
                    count_value = unit->high_limit;
                    if (unit->flags.accum_count) {
                        portENTER_CRITICAL_ISR(&unit->spinlock);
                        unit->accum_value += unit->high_limit;
                        portEXIT_CRITICAL_ISR(&unit->spinlock);
                    }
                } else if (event_status & BIT(PCNT_LL_WATCH_EVENT_THRES0)) {
                    event_status &= ~(BIT(PCNT_LL_WATCH_EVENT_THRES0));
                    // adjust the count value to reflect the actual watch point value
                    count_value = pcnt_ll_get_thres_value(group->hal.dev, unit_id, 0);
                } else if (event_status & BIT(PCNT_LL_WATCH_EVENT_THRES1)) {
                    event_status &= ~(BIT(PCNT_LL_WATCH_EVENT_THRES1));
                    // adjust the count value to reflect the actual watch point value
                    count_value = pcnt_ll_get_thres_value(group->hal.dev, unit_id, 1);
                } else if (event_status & BIT(PCNT_LL_WATCH_EVENT_ZERO_CROSS)) {
                    event_status &= ~(BIT(PCNT_LL_WATCH_EVENT_ZERO_CROSS));
                    count_value = 0;
                    zc_mode = pcnt_ll_get_zero_cross_mode(group->hal.dev, unit_id);
                }

            // invoked user registered callback
            if (on_reach) {
                pcnt_watch_event_data_t edata = {
                    .watch_point_value = count_value,
                    .zero_cross_mode = zc_mode,
                };
                if (on_reach(unit, &edata, unit->user_data)) {
                    // check if we need to yield for high priority task
                    need_yield = true;
                }
            }
        }
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

#if PCNT_USE_RETENTION_LINK
static esp_err_t pcnt_create_sleep_retention_link_cb(void *arg)
{
    pcnt_group_t *group = (pcnt_group_t *)arg;
    int group_id = group->group_id;
    sleep_retention_module_t module_id = pcnt_reg_retention_info[group_id].retention_module;
    esp_err_t err = sleep_retention_entries_create(pcnt_reg_retention_info[group_id].regdma_entry_array,
                                                   pcnt_reg_retention_info[group_id].array_size,
                                                   REGDMA_LINK_PRI_PCNT, module_id);
    ESP_RETURN_ON_ERROR(err, TAG, "create retention link failed");
    return ESP_OK;
}
#endif // PCNT_USE_RETENTION_LINK
