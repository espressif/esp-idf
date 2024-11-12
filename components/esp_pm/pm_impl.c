/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <sys/param.h>

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_clk_tree.h"
#include "soc/soc_caps.h"

#include "esp_private/crosscore_int.h"
#include "esp_private/periph_ctrl.h"

#include "soc/rtc.h"
#include "hal/uart_ll.h"
#include "hal/uart_types.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
#include "xtensa_timer.h"
#include "xtensa/core-macros.h"
#endif

#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
#include "esp_private/mspi_timing_tuning.h"
#endif

#include "esp_private/pm_impl.h"
#include "esp_private/pm_trace.h"
#include "esp_private/esp_timer_private.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_gpio.h"
#include "esp_private/sleep_modem.h"
#include "esp_private/uart_share_hw_ctrl.h"
#include "esp_sleep.h"
#include "esp_memory_utils.h"


#if SOC_PERIPH_CLK_CTRL_SHARED
#define HP_UART_SRC_CLK_ATOMIC()       PERIPH_RCC_ATOMIC()
#else
#define HP_UART_SRC_CLK_ATOMIC()
#endif

#define MHZ (1000000)

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
/* CCOMPARE update timeout, in CPU cycles. Any value above ~600 cycles will work
 * for the purpose of detecting a deadlock.
 */
#define CCOMPARE_UPDATE_TIMEOUT 1000000

/* The number of CPU cycles required from obtaining the base ccount to configuring
   the calculated ccompare value. (In order to avoid ccompare being updated to a value
   smaller than the current ccount, this update should be discarded if the next tick
   is too close to this moment, and this value is used to calculate the threshold for
   determining whether or not a skip is required.)
 */
#define CCOMPARE_PREPARE_CYCLES_IN_FREQ_UPDATE  60
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT

/* When light sleep is used, wake this number of microseconds earlier than
 * the next tick.
 */
#define LIGHT_SLEEP_EARLY_WAKEUP_US 100

#if CONFIG_IDF_TARGET_ESP32
/* Minimal divider at which REF_CLK_FREQ can be obtained */
#define REF_CLK_DIV_MIN 10
#elif CONFIG_IDF_TARGET_ESP32S2
/* Minimal divider at which REF_CLK_FREQ can be obtained */
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32S3
/* Minimal divider at which REF_CLK_FREQ can be obtained */
#define REF_CLK_DIV_MIN 2         // TODO: IDF-5660
#elif CONFIG_IDF_TARGET_ESP32C3
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32C2
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32C6
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32C61
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32C5
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32H2
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32P4
#define REF_CLK_DIV_MIN 2
#endif

#ifdef CONFIG_PM_PROFILING
#define WITH_PROFILING
#endif

static portMUX_TYPE s_switch_lock = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE s_cpu_freq_switch_lock[CONFIG_FREERTOS_NUMBER_OF_CORES] = {
    [0 ... (CONFIG_FREERTOS_NUMBER_OF_CORES - 1)] = portMUX_INITIALIZER_UNLOCKED
};
/* The following state variables are protected using s_switch_lock: */
/* Current sleep mode; When switching, contains old mode until switch is complete */
static pm_mode_t s_mode = PM_MODE_CPU_MAX;
/* True when switch is in progress */
static volatile bool s_is_switching;
/* Number of times each mode was locked */
static size_t s_mode_lock_counts[PM_MODE_COUNT];
/* Bit mask of locked modes. BIT(i) is set iff s_mode_lock_counts[i] > 0. */
static uint32_t s_mode_mask;

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE

#define PERIPH_SKIP_LIGHT_SLEEP_NO 2

/* Indicates if light sleep should be skipped by peripherals. */
static skip_light_sleep_cb_t s_periph_skip_light_sleep_cb[PERIPH_SKIP_LIGHT_SLEEP_NO];

/* Indicates if light sleep entry was skipped in vApplicationSleep for given CPU.
 * This in turn gets used in IDLE hook to decide if `waiti` needs
 * to be invoked or not.
 */
static bool s_skipped_light_sleep[CONFIG_FREERTOS_NUMBER_OF_CORES];

#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
/* When light sleep is finished on one CPU, it is possible that the other CPU
 * will enter light sleep again very soon, before interrupts on the first CPU
 * get a chance to run. To avoid such situation, set a flag for the other CPU to
 * skip light sleep attempt.
 */
static bool s_skip_light_sleep[CONFIG_FREERTOS_NUMBER_OF_CORES];
#endif // CONFIG_FREERTOS_NUMBER_OF_CORES == 2

static _lock_t s_skip_light_sleep_lock;
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

/* A flag indicating that Idle hook has run on a given CPU;
 * Next interrupt on the same CPU will take s_rtos_lock_handle.
 */
static bool s_core_idle[CONFIG_FREERTOS_NUMBER_OF_CORES];

/* When no RTOS tasks are active, these locks are released to allow going into
 * a lower power mode. Used by ISR hook and idle hook.
 */
static esp_pm_lock_handle_t s_rtos_lock_handle[CONFIG_FREERTOS_NUMBER_OF_CORES];

/* Lookup table of CPU frequency configs to be used in each mode.
 * Initialized by esp_pm_impl_init and modified by esp_pm_configure.
 */
static rtc_cpu_freq_config_t s_cpu_freq_by_mode[PM_MODE_COUNT];

/* Whether automatic light sleep is enabled */
static bool s_light_sleep_en = false;

/* When configuration is changed, current frequency may not match the
 * newly configured frequency for the current mode. This is an indicator
 * to the mode switch code to get the actual current frequency instead of
 * relying on the current mode.
 */
static bool s_config_changed = false;

#ifdef WITH_PROFILING
/* Time, in microseconds, spent so far in each mode */
static pm_time_t s_time_in_mode[PM_MODE_COUNT];
/* Timestamp, in microseconds, when the mode switch last happened */
static pm_time_t s_last_mode_change_time;
/* User-readable mode names, used by esp_pm_impl_dump_stats */
static const char* s_mode_names[] = {
        "SLEEP",
        "APB_MIN",
        "APB_MAX",
        "CPU_MAX"
};
static uint32_t s_light_sleep_counts, s_light_sleep_reject_counts;
#endif // WITH_PROFILING

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
/* Indicates to the ISR hook that CCOMPARE needs to be updated on the given CPU.
 * Used in conjunction with cross-core interrupt to update CCOMPARE on the other CPU.
 */
static volatile bool s_need_update_ccompare[CONFIG_FREERTOS_NUMBER_OF_CORES];

/* Divider and multiplier used to adjust (ccompare - ccount) duration.
 * Only set to non-zero values when switch is in progress.
 */
static uint32_t s_ccount_div;
static uint32_t s_ccount_mul;

static void update_ccompare(void);
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT

static const char* TAG = "pm";

static void do_switch(pm_mode_t new_mode);
static void leave_idle(void);
static void on_freq_update(uint32_t old_ticks_per_us, uint32_t ticks_per_us);

pm_mode_t esp_pm_impl_get_mode(esp_pm_lock_type_t type, int arg)
{
    (void) arg;
    if (type == ESP_PM_CPU_FREQ_MAX) {
        return PM_MODE_CPU_MAX;
    } else if (type == ESP_PM_APB_FREQ_MAX) {
        return PM_MODE_APB_MAX;
    } else if (type == ESP_PM_NO_LIGHT_SLEEP) {
        return PM_MODE_APB_MIN;
    } else {
        // unsupported mode
        abort();
    }
}

#if CONFIG_PM_LIGHT_SLEEP_CALLBACKS
/**
 * @brief Function entry parameter types for light sleep callback functions (if CONFIG_FREERTOS_USE_TICKLESS_IDLE)
 */
struct _esp_pm_sleep_cb_config_t {
    /**
     * Callback function defined by user.
     */
    esp_pm_light_sleep_cb_t cb;
    /**
     * Input parameters of callback function defined by user.
     */
    void *arg;
    /**
     * Execution priority of callback function defined by user.
     */
    uint32_t prior;
    /**
     * Next callback function defined by user.
     */
    struct _esp_pm_sleep_cb_config_t *next;
};

typedef struct _esp_pm_sleep_cb_config_t esp_pm_sleep_cb_config_t;

static esp_pm_sleep_cb_config_t *s_light_sleep_enter_cb_config;
static esp_pm_sleep_cb_config_t *s_light_sleep_exit_cb_config;
static portMUX_TYPE s_sleep_pm_cb_mutex = portMUX_INITIALIZER_UNLOCKED;

esp_err_t esp_pm_light_sleep_register_cbs(esp_pm_sleep_cbs_register_config_t *cbs_conf)
{
    if (cbs_conf->enter_cb == NULL && cbs_conf->exit_cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&s_sleep_pm_cb_mutex);
    if (cbs_conf->enter_cb != NULL) {
        esp_pm_sleep_cb_config_t **current_enter_ptr = &(s_light_sleep_enter_cb_config);
        while (*current_enter_ptr != NULL) {
            if (((*current_enter_ptr)->cb) == (cbs_conf->enter_cb)) {
                portEXIT_CRITICAL(&s_sleep_pm_cb_mutex);
                return ESP_FAIL;
            }
            current_enter_ptr = &((*current_enter_ptr)->next);
        }
        esp_pm_sleep_cb_config_t *new_enter_config = (esp_pm_sleep_cb_config_t *)heap_caps_malloc(sizeof(esp_pm_sleep_cb_config_t), MALLOC_CAP_INTERNAL);
        if (new_enter_config == NULL) {
            portEXIT_CRITICAL(&s_sleep_pm_cb_mutex);
            return ESP_ERR_NO_MEM; /* Memory allocation failed */
        }
        new_enter_config->cb = cbs_conf->enter_cb;
        new_enter_config->arg = cbs_conf->enter_cb_user_arg;
        new_enter_config->prior = cbs_conf->enter_cb_prior;
        while (*current_enter_ptr != NULL && (*current_enter_ptr)->prior <= new_enter_config->prior) {
            current_enter_ptr = &((*current_enter_ptr)->next);
        }
        new_enter_config->next = *current_enter_ptr;
        *current_enter_ptr = new_enter_config;
    }

    if (cbs_conf->exit_cb != NULL) {
        esp_pm_sleep_cb_config_t **current_exit_ptr = &(s_light_sleep_exit_cb_config);
        while (*current_exit_ptr != NULL) {
            if (((*current_exit_ptr)->cb) == (cbs_conf->exit_cb)) {
                portEXIT_CRITICAL(&s_sleep_pm_cb_mutex);
                return ESP_FAIL;
            }
            current_exit_ptr = &((*current_exit_ptr)->next);
        }
        esp_pm_sleep_cb_config_t *new_exit_config = (esp_pm_sleep_cb_config_t *)heap_caps_malloc(sizeof(esp_pm_sleep_cb_config_t), MALLOC_CAP_INTERNAL);
        if (new_exit_config == NULL) {
            portEXIT_CRITICAL(&s_sleep_pm_cb_mutex);
            return ESP_ERR_NO_MEM; /* Memory allocation failed */
        }
        new_exit_config->cb = cbs_conf->exit_cb;
        new_exit_config->arg = cbs_conf->exit_cb_user_arg;
        new_exit_config->prior = cbs_conf->exit_cb_prior;
        while (*current_exit_ptr != NULL && (*current_exit_ptr)->prior <= new_exit_config->prior) {
            current_exit_ptr = &((*current_exit_ptr)->next);
        }
        new_exit_config->next = *current_exit_ptr;
        *current_exit_ptr = new_exit_config;
    }
    portEXIT_CRITICAL(&s_sleep_pm_cb_mutex);
    return ESP_OK;
}

esp_err_t esp_pm_light_sleep_unregister_cbs(esp_pm_sleep_cbs_register_config_t *cbs_conf)
{
    if (cbs_conf->enter_cb == NULL && cbs_conf->exit_cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&s_sleep_pm_cb_mutex);
    if (cbs_conf->enter_cb != NULL) {
        esp_pm_sleep_cb_config_t **current_enter_ptr = &(s_light_sleep_enter_cb_config);
        while (*current_enter_ptr != NULL) {
            if ((*current_enter_ptr)->cb == cbs_conf->enter_cb) {
                esp_pm_sleep_cb_config_t *temp = *current_enter_ptr;
                *current_enter_ptr = (*current_enter_ptr)->next;
                free(temp);
                break;
            }
            current_enter_ptr = &((*current_enter_ptr)->next);
        }
    }

    if (cbs_conf->exit_cb != NULL) {
        esp_pm_sleep_cb_config_t **current_exit_ptr = &(s_light_sleep_exit_cb_config);
        while (*current_exit_ptr != NULL) {
            if ((*current_exit_ptr)->cb == cbs_conf->exit_cb) {
                esp_pm_sleep_cb_config_t *temp = *current_exit_ptr;
                *current_exit_ptr = (*current_exit_ptr)->next;
                free(temp);
                break;
            }
            current_exit_ptr = &((*current_exit_ptr)->next);
        }
    }
    portEXIT_CRITICAL(&s_sleep_pm_cb_mutex);
    return ESP_OK;
}

static void IRAM_ATTR esp_pm_execute_enter_sleep_callbacks(int64_t sleep_time_us)
{
    esp_pm_sleep_cb_config_t *enter_current = s_light_sleep_enter_cb_config;
    while (enter_current != NULL) {
        if (enter_current->cb != NULL) {
            if (ESP_OK != (*enter_current->cb)(sleep_time_us, enter_current->arg)) {
                ESP_EARLY_LOGW(TAG, "esp_pm_execute_enter_sleep_callbacks has an err, enter_current = %p", enter_current);
            }
        }
        enter_current = enter_current->next;
    }
}

static void IRAM_ATTR esp_pm_execute_exit_sleep_callbacks(int64_t sleep_time_us)
{
    esp_pm_sleep_cb_config_t *exit_current = s_light_sleep_exit_cb_config;
    while (exit_current != NULL) {
        if (exit_current->cb != NULL) {
            if (ESP_OK != (*exit_current->cb)(sleep_time_us, exit_current->arg)) {
                ESP_EARLY_LOGW(TAG, "esp_pm_execute_exit_sleep_callbacks has an err, exit_current = %p", exit_current);
            }
        }
        exit_current = exit_current->next;
    }
}
#endif

static esp_err_t esp_pm_sleep_configure(const void *vconfig)
{
    esp_err_t err = ESP_OK;
    const esp_pm_config_t* config = (const esp_pm_config_t*) vconfig;

#if ESP_SLEEP_POWER_DOWN_CPU
    err = sleep_cpu_configure(config->light_sleep_enable);
    if (err != ESP_OK) {
        return err;
    }
#endif

    err = sleep_modem_configure(config->max_freq_mhz, config->min_freq_mhz, config->light_sleep_enable);
    return err;
}

esp_err_t esp_pm_configure(const void* vconfig)
{
#ifndef CONFIG_PM_ENABLE
    return ESP_ERR_NOT_SUPPORTED;
#endif

    const esp_pm_config_t* config = (const esp_pm_config_t*) vconfig;

#ifndef CONFIG_FREERTOS_USE_TICKLESS_IDLE
    if (config->light_sleep_enable) {
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif

    int min_freq_mhz = config->min_freq_mhz;
    int max_freq_mhz = config->max_freq_mhz;

    if (min_freq_mhz > max_freq_mhz) {
        return ESP_ERR_INVALID_ARG;
    }

    rtc_cpu_freq_config_t freq_config;
    if (!rtc_clk_cpu_freq_mhz_to_config(min_freq_mhz, &freq_config)) {
        ESP_LOGW(TAG, "invalid min_freq_mhz value (%d)", min_freq_mhz);
        return ESP_ERR_INVALID_ARG;
    }

    int xtal_freq_mhz = esp_clk_xtal_freq() / MHZ;
    if (min_freq_mhz < xtal_freq_mhz && min_freq_mhz * MHZ / REF_CLK_FREQ < REF_CLK_DIV_MIN) {
        ESP_LOGW(TAG, "min_freq_mhz should be >= %d", REF_CLK_FREQ * REF_CLK_DIV_MIN / MHZ);
        return ESP_ERR_INVALID_ARG;
    }

    if (!rtc_clk_cpu_freq_mhz_to_config(max_freq_mhz, &freq_config)) {
        ESP_LOGW(TAG, "invalid max_freq_mhz value (%d)", max_freq_mhz);
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_IDF_TARGET_ESP32
    int apb_max_freq = max_freq_mhz; /* CPU frequency in APB_MAX mode */
    if (max_freq_mhz == 240) {
        /* We can't switch between 240 and 80/160 without disabling PLL,
         * so use 240MHz CPU frequency when 80MHz APB frequency is requested.
         */
        apb_max_freq = 240;
    } else if (max_freq_mhz == 160 || max_freq_mhz == 80) {
        /* Otherwise, can use 80MHz
         * CPU frequency when 80MHz APB frequency is requested.
         */
        apb_max_freq = 80;
    }
#else
    /* Maximum SOC APB clock frequency is 40 MHz, maximum Modem (WiFi,
     * Bluetooth, etc..) APB clock frequency is 80 MHz */
    int apb_clk_freq = esp_clk_apb_freq() / MHZ;
#if (CONFIG_ESP_WIFI_ENABLED || CONFIG_BT_ENABLED || CONFIG_IEEE802154_ENABLED) && SOC_PHY_SUPPORTED
    apb_clk_freq = MAX(apb_clk_freq, MODEM_REQUIRED_MIN_APB_CLK_FREQ / MHZ);
#endif
    int apb_max_freq = MIN(max_freq_mhz, apb_clk_freq); /* CPU frequency in APB_MAX mode */
#endif

    apb_max_freq = MAX(apb_max_freq, min_freq_mhz);

    ESP_LOGI(TAG, "Frequency switching config: "
                  "CPU_MAX: %d, APB_MAX: %d, APB_MIN: %d, Light sleep: %s",
                  max_freq_mhz,
                  apb_max_freq,
                  min_freq_mhz,
                  config->light_sleep_enable ? "ENABLED" : "DISABLED");

    // CPU & Modem power down initialization, which must be initialized before s_light_sleep_en set true,
    // to avoid entering idle and sleep in this function.
    esp_pm_sleep_configure(config);

    portENTER_CRITICAL(&s_switch_lock);
    bool res __attribute__((unused));
    res = rtc_clk_cpu_freq_mhz_to_config(max_freq_mhz, &s_cpu_freq_by_mode[PM_MODE_CPU_MAX]);
    assert(res);
    res = rtc_clk_cpu_freq_mhz_to_config(apb_max_freq, &s_cpu_freq_by_mode[PM_MODE_APB_MAX]);
    assert(res);
    res = rtc_clk_cpu_freq_mhz_to_config(min_freq_mhz, &s_cpu_freq_by_mode[PM_MODE_APB_MIN]);
    assert(res);
    s_cpu_freq_by_mode[PM_MODE_LIGHT_SLEEP] = s_cpu_freq_by_mode[PM_MODE_APB_MIN];

    if (config->light_sleep_enable) {
        // Enable the wakeup source here because the `esp_sleep_disable_wakeup_source` in the `else`
        // branch must be called if corresponding wakeup source is already enabled.
        esp_sleep_enable_timer_wakeup(0);
    } else if (s_light_sleep_en) {
        // Since auto light-sleep will enable the timer wakeup source, to avoid affecting subsequent possible
        // deepsleep requests, disable the timer wakeup source here.
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    }
    s_light_sleep_en = config->light_sleep_enable;
    s_config_changed = true;
    portEXIT_CRITICAL(&s_switch_lock);

    do_switch(PM_MODE_CPU_MAX);
    return ESP_OK;
}

esp_err_t esp_pm_get_configuration(void* vconfig)
{
    if (vconfig == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_pm_config_t* config = (esp_pm_config_t*) vconfig;

    portENTER_CRITICAL(&s_switch_lock);
    config->light_sleep_enable = s_light_sleep_en;
    config->max_freq_mhz = s_cpu_freq_by_mode[PM_MODE_CPU_MAX].freq_mhz;
    config->min_freq_mhz = s_cpu_freq_by_mode[PM_MODE_APB_MIN].freq_mhz;
    portEXIT_CRITICAL(&s_switch_lock);

    return ESP_OK;
}

static pm_mode_t IRAM_ATTR get_lowest_allowed_mode(void)
{
    /* TODO: optimize using ffs/clz */
    if (s_mode_mask >= BIT(PM_MODE_CPU_MAX)) {
        return PM_MODE_CPU_MAX;
    } else if (s_mode_mask >= BIT(PM_MODE_APB_MAX)) {
        return PM_MODE_APB_MAX;
    } else if (s_mode_mask >= BIT(PM_MODE_APB_MIN) || !s_light_sleep_en) {
        return PM_MODE_APB_MIN;
    } else {
        return PM_MODE_LIGHT_SLEEP;
    }
}

void IRAM_ATTR esp_pm_impl_switch_mode(pm_mode_t mode,
        pm_mode_switch_t lock_or_unlock, pm_time_t now)
{
    bool need_switch = false;
    uint32_t mode_mask = BIT(mode);
    portENTER_CRITICAL_SAFE(&s_switch_lock);
    uint32_t count;
    if (lock_or_unlock == MODE_LOCK) {
        count = ++s_mode_lock_counts[mode];
    } else {
        count = s_mode_lock_counts[mode]--;
    }
    if (count == 1) {
        if (lock_or_unlock == MODE_LOCK) {
            s_mode_mask |= mode_mask;
        } else {
            s_mode_mask &= ~mode_mask;
        }
        need_switch = true;
    }

    pm_mode_t new_mode = s_mode;
    if (need_switch) {
        new_mode = get_lowest_allowed_mode();
#ifdef WITH_PROFILING
        if (s_last_mode_change_time != 0) {
            pm_time_t diff = now - s_last_mode_change_time;
            s_time_in_mode[s_mode] += diff;
        }
        s_last_mode_change_time = now;
#endif // WITH_PROFILING
    }
    portEXIT_CRITICAL_SAFE(&s_switch_lock);
    if (need_switch) {
        do_switch(new_mode);
    }
}

/**
 * @brief Update clock dividers in esp_timer and FreeRTOS, and adjust CCOMPARE
 * values on both CPUs.
 * @param old_ticks_per_us old CPU frequency
 * @param ticks_per_us new CPU frequency
 */
static void IRAM_ATTR on_freq_update(uint32_t old_ticks_per_us, uint32_t ticks_per_us)
{
    uint32_t old_apb_ticks_per_us = MIN(old_ticks_per_us, 80);
    uint32_t apb_ticks_per_us = MIN(ticks_per_us, 80);
    /* Update APB frequency value used by the timer */
    if (old_apb_ticks_per_us != apb_ticks_per_us) {
        esp_timer_private_update_apb_freq(apb_ticks_per_us);
    }

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
#ifdef XT_RTOS_TIMER_INT
    /* Calculate new tick divisor */
    _xt_tick_divisor = ticks_per_us * MHZ / XT_TICK_PER_SEC;
#endif

    int core_id = xPortGetCoreID();
    if (s_rtos_lock_handle[core_id] != NULL) {
        ESP_PM_TRACE_ENTER(CCOMPARE_UPDATE, core_id);
        /* ccount_div and ccount_mul are used in esp_pm_impl_update_ccompare
         * to calculate new CCOMPARE value.
         */
        s_ccount_div = old_ticks_per_us;
        s_ccount_mul = ticks_per_us;

        /* Update CCOMPARE value on this CPU */
        update_ccompare();

#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
        /* Send interrupt to the other CPU to update CCOMPARE value */
        int other_core_id = (core_id == 0) ? 1 : 0;

        s_need_update_ccompare[other_core_id] = true;
        esp_crosscore_int_send_freq_switch(other_core_id);

        int timeout = 0;
        while (s_need_update_ccompare[other_core_id]) {
            if (++timeout == CCOMPARE_UPDATE_TIMEOUT) {
                assert(false && "failed to update CCOMPARE, possible deadlock");
            }
        }
#endif // CONFIG_FREERTOS_NUMBER_OF_CORES == 2

        s_ccount_mul = 0;
        s_ccount_div = 0;
        ESP_PM_TRACE_EXIT(CCOMPARE_UPDATE, core_id);
    }
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
}

/**
 * Perform the switch to new power mode.
 * Currently only changes the CPU frequency and adjusts clock dividers.
 * No light sleep yet.
 * @param new_mode mode to switch to
 */
static void IRAM_ATTR do_switch(pm_mode_t new_mode)
{
    const int core_id = xPortGetCoreID();

    do {
        portENTER_CRITICAL_ISR(&s_switch_lock);
        if (!s_is_switching) {
            break;
        }
#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
        if (s_need_update_ccompare[core_id]) {
            update_ccompare();
            s_need_update_ccompare[core_id] = false;
        }
#endif
        portEXIT_CRITICAL_ISR(&s_switch_lock);
    } while (true);
    if ((new_mode == s_mode) && !s_config_changed) {
        portEXIT_CRITICAL_ISR(&s_switch_lock);
        return;
    }
    s_is_switching = true;
    bool config_changed = s_config_changed;
    s_config_changed = false;
    portENTER_CRITICAL_ISR(&s_cpu_freq_switch_lock[core_id]);
    portEXIT_CRITICAL_ISR(&s_switch_lock);

    rtc_cpu_freq_config_t new_config = s_cpu_freq_by_mode[new_mode];
    rtc_cpu_freq_config_t old_config;

    if (!config_changed) {
        old_config = s_cpu_freq_by_mode[s_mode];
    } else {
        rtc_clk_cpu_freq_get_config(&old_config);
    }

    if (new_config.freq_mhz != old_config.freq_mhz) {
        uint32_t old_ticks_per_us = old_config.freq_mhz;
        uint32_t new_ticks_per_us = new_config.freq_mhz;

        bool switch_down = new_ticks_per_us < old_ticks_per_us;

        ESP_PM_TRACE_ENTER(FREQ_SWITCH, core_id);
        if (switch_down) {
            on_freq_update(old_ticks_per_us, new_ticks_per_us);
        }
#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    if (new_config.source == SOC_CPU_CLK_SRC_PLL) {
        rtc_clk_cpu_freq_set_config_fast(&new_config);
        mspi_timing_change_speed_mode_cache_safe(false);
    } else {
        mspi_timing_change_speed_mode_cache_safe(true);
        rtc_clk_cpu_freq_set_config_fast(&new_config);
    }
#else
    rtc_clk_cpu_freq_set_config_fast(&new_config);
#endif
        if (!switch_down) {
            on_freq_update(old_ticks_per_us, new_ticks_per_us);
        }
        ESP_PM_TRACE_EXIT(FREQ_SWITCH, core_id);
    }

    portENTER_CRITICAL_ISR(&s_switch_lock);
    portEXIT_CRITICAL_ISR(&s_cpu_freq_switch_lock[core_id]);
    s_mode = new_mode;
    s_is_switching = false;
    portEXIT_CRITICAL_ISR(&s_switch_lock);
}

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
/**
 * @brief Calculate new CCOMPARE value based on s_ccount_{mul,div}
 *
 * Adjusts CCOMPARE value so that the interrupt happens at the same time as it
 * would happen without the frequency change.
 * Assumes that the new_frequency = old_frequency * s_ccount_mul / s_ccount_div.
 */
static __attribute__((optimize("-O2"))) void IRAM_ATTR update_ccompare(void)
{
    uint32_t ccompare_min_cycles_in_future = ((s_ccount_div + s_ccount_mul - 1) / s_ccount_mul) * CCOMPARE_PREPARE_CYCLES_IN_FREQ_UPDATE;
#if CONFIG_PM_UPDATE_CCOMPARE_HLI_WORKAROUND
    /* disable level 4 and below */
    uint32_t irq_status = XTOS_SET_INTLEVEL(XCHAL_DEBUGLEVEL - 2);
#endif
    uint32_t ccount = esp_cpu_get_cycle_count();
    uint32_t ccompare = XTHAL_GET_CCOMPARE(XT_TIMER_INDEX);

    if ((ccompare - ccompare_min_cycles_in_future) - ccount < UINT32_MAX / 2) {
        uint32_t diff = ccompare - ccount;
        uint32_t diff_scaled = (diff * s_ccount_mul + s_ccount_div - 1) / s_ccount_div;
        if (diff_scaled < _xt_tick_divisor) {
            uint32_t new_ccompare = ccount + diff_scaled;
            XTHAL_SET_CCOMPARE(XT_TIMER_INDEX, new_ccompare);
        }
    }
#if CONFIG_PM_UPDATE_CCOMPARE_HLI_WORKAROUND
    XTOS_RESTORE_INTLEVEL(irq_status);
#endif
}
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT

static void IRAM_ATTR leave_idle(void)
{
    int core_id = xPortGetCoreID();
    if (s_core_idle[core_id]) {
        // TODO: possible optimization: raise frequency here first
        esp_pm_lock_acquire(s_rtos_lock_handle[core_id]);
        s_core_idle[core_id] = false;
    }
}

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE

esp_err_t esp_pm_register_skip_light_sleep_callback(skip_light_sleep_cb_t cb)
{
    _lock_acquire(&s_skip_light_sleep_lock);
    for (int i = 0; i < PERIPH_SKIP_LIGHT_SLEEP_NO; i++) {
        if (s_periph_skip_light_sleep_cb[i] == cb) {
            _lock_release(&s_skip_light_sleep_lock);
            return ESP_OK;
        } else if (s_periph_skip_light_sleep_cb[i] == NULL) {
            s_periph_skip_light_sleep_cb[i] = cb;
            _lock_release(&s_skip_light_sleep_lock);
            return ESP_OK;
        }
    }
    _lock_release(&s_skip_light_sleep_lock);
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_pm_unregister_skip_light_sleep_callback(skip_light_sleep_cb_t cb)
{
    _lock_acquire(&s_skip_light_sleep_lock);
    for (int i = 0; i < PERIPH_SKIP_LIGHT_SLEEP_NO; i++) {
        if (s_periph_skip_light_sleep_cb[i] == cb) {
            s_periph_skip_light_sleep_cb[i] = NULL;
            _lock_release(&s_skip_light_sleep_lock);
            return ESP_OK;
        }
    }
    _lock_release(&s_skip_light_sleep_lock);
    return ESP_ERR_INVALID_STATE;
}

static inline bool IRAM_ATTR periph_should_skip_light_sleep(void)
{
    if (s_light_sleep_en) {
        for (int i = 0; i < PERIPH_SKIP_LIGHT_SLEEP_NO; i++) {
            if (s_periph_skip_light_sleep_cb[i]) {
                if (s_periph_skip_light_sleep_cb[i]() == true) {
                    return true;
                }
            }
        }
    }
    return false;
}

static inline bool IRAM_ATTR should_skip_light_sleep(int core_id)
{
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    if (s_skip_light_sleep[core_id]) {
        s_skip_light_sleep[core_id] = false;
        s_skipped_light_sleep[core_id] = true;
        return true;
    }
#endif // CONFIG_FREERTOS_NUMBER_OF_CORES == 2

    if (s_mode != PM_MODE_LIGHT_SLEEP || s_is_switching || periph_should_skip_light_sleep()) {
        s_skipped_light_sleep[core_id] = true;
    } else {
        s_skipped_light_sleep[core_id] = false;
    }
    return s_skipped_light_sleep[core_id];
}

static inline void IRAM_ATTR other_core_should_skip_light_sleep(int core_id)
{
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    s_skip_light_sleep[!core_id] = true;
#endif
}

void IRAM_ATTR vApplicationSleep( TickType_t xExpectedIdleTime )
{
    portENTER_CRITICAL(&s_switch_lock);
    int core_id = xPortGetCoreID();
    if (!should_skip_light_sleep(core_id)) {
        /* Calculate how much we can sleep */
        int64_t next_esp_timer_alarm = esp_timer_get_next_alarm_for_wake_up();
        int64_t now = esp_timer_get_time();
        int64_t time_until_next_alarm = next_esp_timer_alarm - now;
        int64_t wakeup_delay_us = portTICK_PERIOD_MS * 1000LL * xExpectedIdleTime;
        int64_t sleep_time_us = MIN(wakeup_delay_us, time_until_next_alarm);
        int64_t slept_us = 0;
#if CONFIG_PM_LIGHT_SLEEP_CALLBACKS
        uint32_t cycle = esp_cpu_get_cycle_count();
        esp_pm_execute_enter_sleep_callbacks(sleep_time_us);
        sleep_time_us -= (esp_cpu_get_cycle_count() - cycle) / (esp_clk_cpu_freq() / 1000000ULL);
#endif
        if (sleep_time_us >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP * portTICK_PERIOD_MS * 1000LL) {
            esp_sleep_enable_timer_wakeup(sleep_time_us - LIGHT_SLEEP_EARLY_WAKEUP_US);
#if CONFIG_PM_TRACE && SOC_PM_SUPPORT_RTC_PERIPH_PD
            /* to force tracing GPIOs to keep state */
            esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#endif
            /* Enter sleep */
            ESP_PM_TRACE_ENTER(SLEEP, core_id);
            int64_t sleep_start = esp_timer_get_time();
            if (esp_light_sleep_start() != ESP_OK){
#ifdef WITH_PROFILING
                s_light_sleep_reject_counts++;
            } else {
                s_light_sleep_counts++;
#endif
            }
            slept_us = esp_timer_get_time() - sleep_start;
            ESP_PM_TRACE_EXIT(SLEEP, core_id);

            uint32_t slept_ticks = slept_us / (portTICK_PERIOD_MS * 1000LL);
            if (slept_ticks > 0) {
                /* Adjust RTOS tick count based on the amount of time spent in sleep */
                vTaskStepTick(slept_ticks);

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
                /* Trigger tick interrupt, since sleep time was longer
                 * than portTICK_PERIOD_MS. Note that setting INTSET does not
                 * work for timer interrupt, and changing CCOMPARE would clear
                 * the interrupt flag.
                 */
                esp_cpu_set_cycle_count(XTHAL_GET_CCOMPARE(XT_TIMER_INDEX) - 16);
                while (!(XTHAL_GET_INTERRUPT() & BIT(XT_TIMER_INTNUM))) {
                    ;
                }
#else
                portYIELD_WITHIN_API();
#endif
            }
            other_core_should_skip_light_sleep(core_id);
        }
#if CONFIG_PM_LIGHT_SLEEP_CALLBACKS
        esp_pm_execute_exit_sleep_callbacks(slept_us);
#endif
    }
    portEXIT_CRITICAL(&s_switch_lock);
}
#endif //CONFIG_FREERTOS_USE_TICKLESS_IDLE

#ifdef WITH_PROFILING
void esp_pm_impl_dump_stats(FILE* out)
{
    pm_time_t time_in_mode[PM_MODE_COUNT];

    portENTER_CRITICAL_ISR(&s_switch_lock);
    memcpy(time_in_mode, s_time_in_mode, sizeof(time_in_mode));
    pm_time_t last_mode_change_time = s_last_mode_change_time;
    pm_mode_t cur_mode = s_mode;
    pm_time_t now = pm_get_time();
    bool light_sleep_en = s_light_sleep_en;
    uint32_t light_sleep_counts = s_light_sleep_counts;
    uint32_t light_sleep_reject_counts = s_light_sleep_reject_counts;
    portEXIT_CRITICAL_ISR(&s_switch_lock);

    time_in_mode[cur_mode] += now - last_mode_change_time;

    fprintf(out, "\nMode stats:\n");
    fprintf(out, "%-8s  %-10s  %-10s  %-10s\n", "Mode", "CPU_freq", "Time(us)", "Time(%)");
    for (int i = 0; i < PM_MODE_COUNT; ++i) {
        if (i == PM_MODE_LIGHT_SLEEP && !light_sleep_en) {
            /* don't display light sleep mode if it's not enabled */
            continue;
        }
        fprintf(out, "%-8s  %-3"PRIu32"M%-7s %-10lld  %-2d%%\n",
                s_mode_names[i],
                s_cpu_freq_by_mode[i].freq_mhz,
                "",                                     //Empty space to align columns
                time_in_mode[i],
                (int) (time_in_mode[i] * 100 / now));
    }
    if (light_sleep_en){
        fprintf(out, "\nSleep stats:\n");
        fprintf(out, "light_sleep_counts:%ld  light_sleep_reject_counts:%ld\n", light_sleep_counts, light_sleep_reject_counts);
    }
}
#endif // WITH_PROFILING

int esp_pm_impl_get_cpu_freq(pm_mode_t mode)
{
    int freq_mhz;
    if (mode >= PM_MODE_LIGHT_SLEEP && mode < PM_MODE_COUNT) {
        portENTER_CRITICAL(&s_switch_lock);
        freq_mhz = s_cpu_freq_by_mode[mode].freq_mhz;
        portEXIT_CRITICAL(&s_switch_lock);
    } else {
        abort();
    }
    return freq_mhz;
}

void esp_pm_impl_init(void)
{
#if defined(CONFIG_ESP_CONSOLE_UART)
    //This clock source should be a source which won't be affected by DFS
    uart_sclk_t clk_source = UART_SCLK_DEFAULT;
#if SOC_UART_SUPPORT_REF_TICK
    clk_source = UART_SCLK_REF_TICK;
#elif SOC_UART_SUPPORT_XTAL_CLK
    clk_source = UART_SCLK_XTAL;
#else
    #error "No UART clock source is aware of DFS"
#endif // SOC_UART_SUPPORT_xxx
    while (!uart_ll_is_tx_idle(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM))) {
        ;
    }

    esp_clk_tree_enable_src((soc_module_clk_t)clk_source, true);
    /* When DFS is enabled, override system setting and use REFTICK as UART clock source */
    HP_UART_SRC_CLK_ATOMIC() {
        uart_ll_set_sclk(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM), (soc_module_clk_t)clk_source);
    }
    uint32_t sclk_freq;

    // Return value unused if asserts are disabled
    esp_err_t __attribute__((unused)) err = esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_source, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sclk_freq);
    assert(err == ESP_OK);
    HP_UART_SRC_CLK_ATOMIC() {
        uart_ll_set_baudrate(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM), CONFIG_ESP_CONSOLE_UART_BAUDRATE, sclk_freq);
    }
#endif // CONFIG_ESP_CONSOLE_UART

#ifdef CONFIG_PM_TRACE
    esp_pm_trace_init();
#endif

    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "rtos0",
            &s_rtos_lock_handle[0]));
    ESP_ERROR_CHECK(esp_pm_lock_acquire(s_rtos_lock_handle[0]));

#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "rtos1",
            &s_rtos_lock_handle[1]));
    ESP_ERROR_CHECK(esp_pm_lock_acquire(s_rtos_lock_handle[1]));
#endif // CONFIG_FREERTOS_NUMBER_OF_CORES == 2

    /* Configure all modes to use the default CPU frequency.
     * This will be modified later by a call to esp_pm_configure.
     */
    rtc_cpu_freq_config_t default_config;
    if (!rtc_clk_cpu_freq_mhz_to_config(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, &default_config)) {
        assert(false && "unsupported frequency");
    }
    for (size_t i = 0; i < PM_MODE_COUNT; ++i) {
        s_cpu_freq_by_mode[i] = default_config;
    }

#ifdef CONFIG_PM_DFS_INIT_AUTO
    int xtal_freq_mhz = esp_clk_xtal_freq() / MHZ;
    esp_pm_config_t cfg = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = xtal_freq_mhz,
    };

    esp_pm_configure(&cfg);
#endif //CONFIG_PM_DFS_INIT_AUTO
}

void esp_pm_impl_idle_hook(void)
{
    int core_id = xPortGetCoreID();
#if CONFIG_FREERTOS_SMP
    uint32_t state = portDISABLE_INTERRUPTS();
#else
    uint32_t state = portSET_INTERRUPT_MASK_FROM_ISR();
#endif
    if (!s_core_idle[core_id]
#ifdef CONFIG_FREERTOS_USE_TICKLESS_IDLE
    && !periph_should_skip_light_sleep()
#endif
    ) {
        esp_pm_lock_release(s_rtos_lock_handle[core_id]);
        s_core_idle[core_id] = true;
    }
#if CONFIG_FREERTOS_SMP
    portRESTORE_INTERRUPTS(state);
#else
    portCLEAR_INTERRUPT_MASK_FROM_ISR(state);
#endif
    ESP_PM_TRACE_ENTER(IDLE, core_id);
}

void IRAM_ATTR esp_pm_impl_isr_hook(void)
{
    int core_id = xPortGetCoreID();
    ESP_PM_TRACE_ENTER(ISR_HOOK, core_id);
    /* Prevent higher level interrupts (than the one this function was called from)
     * from happening in this section, since they will also call into esp_pm_impl_isr_hook.
     */
#if CONFIG_FREERTOS_SMP
    uint32_t state = portDISABLE_INTERRUPTS();
#else
    uint32_t state = portSET_INTERRUPT_MASK_FROM_ISR();
#endif
#if defined(CONFIG_FREERTOS_SYSTICK_USES_CCOUNT) && (CONFIG_FREERTOS_NUMBER_OF_CORES == 2)
    if (s_need_update_ccompare[core_id]) {
        update_ccompare();
        s_need_update_ccompare[core_id] = false;
    } else {
        leave_idle();
    }
#else
    leave_idle();
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT && CONFIG_FREERTOS_NUMBER_OF_CORES == 2
#if CONFIG_FREERTOS_SMP
    portRESTORE_INTERRUPTS(state);
#else
    portCLEAR_INTERRUPT_MASK_FROM_ISR(state);
#endif
    ESP_PM_TRACE_EXIT(ISR_HOOK, core_id);
}

void esp_pm_impl_waiti(void)
{
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    int core_id = xPortGetCoreID();
    if (s_skipped_light_sleep[core_id]) {
        esp_cpu_wait_for_intr();
        /* Interrupt took the CPU out of waiti and s_rtos_lock_handle[core_id]
         * is now taken. However since we are back to idle task, we can release
         * the lock so that vApplicationSleep can attempt to enter light sleep.
         */
        esp_pm_impl_idle_hook();
    }
    s_skipped_light_sleep[core_id] = true;
#else
    esp_cpu_wait_for_intr();
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE
}
