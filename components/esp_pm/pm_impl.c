/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "esp_log.h"
#include "esp_cpu.h"

#include "esp_private/crosscore_int.h"

#include "soc/rtc.h"
#include "hal/uart_ll.h"
#include "hal/uart_types.h"
#include "driver/uart.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
#include "freertos/xtensa_timer.h"
#include "xtensa/core-macros.h"
#endif

#include "esp_private/pm_impl.h"
#include "esp_private/pm_trace.h"
#include "esp_private/esp_timer_private.h"
#include "esp_private/esp_clk.h"

#include "esp_sleep.h"

#include "sdkconfig.h"

// [refactor-todo] opportunity for further refactor
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/pm.h"
#include "driver/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/pm.h"
#include "driver/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/pm.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/pm.h"
#include "driver/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32H4
#include "esp32h4/pm.h"
#include "driver/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/pm.h"
#include "driver/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/pm.h"
#include "driver/gpio.h"
#endif

#define MHZ (1000000)

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
/* CCOMPARE update timeout, in CPU cycles. Any value above ~600 cycles will work
 * for the purpose of detecting a deadlock.
 */
#define CCOMPARE_UPDATE_TIMEOUT 1000000

/* When changing CCOMPARE, don't allow changes if the difference is less
 * than this. This is to prevent setting CCOMPARE below CCOUNT.
 */
#define CCOMPARE_MIN_CYCLES_IN_FUTURE 1000
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
#elif CONFIG_IDF_TARGET_ESP32H4
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32C2
#define REF_CLK_DIV_MIN 2
#elif CONFIG_IDF_TARGET_ESP32C6
#define REF_CLK_DIV_MIN 2
#endif

#ifdef CONFIG_PM_PROFILING
#define WITH_PROFILING
#endif

static portMUX_TYPE s_switch_lock = portMUX_INITIALIZER_UNLOCKED;
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

#define PERIPH_SKIP_LIGHT_SLEEP_NO 1

/* Indicates if light sleep shoule be skipped by peripherals. */
static skip_light_sleep_cb_t s_periph_skip_light_sleep_cb[PERIPH_SKIP_LIGHT_SLEEP_NO];

/* Indicates if light sleep entry was skipped in vApplicationSleep for given CPU.
 * This in turn gets used in IDLE hook to decide if `waiti` needs
 * to be invoked or not.
 */
static bool s_skipped_light_sleep[portNUM_PROCESSORS];

#if portNUM_PROCESSORS == 2
/* When light sleep is finished on one CPU, it is possible that the other CPU
 * will enter light sleep again very soon, before interrupts on the first CPU
 * get a chance to run. To avoid such situation, set a flag for the other CPU to
 * skip light sleep attempt.
 */
static bool s_skip_light_sleep[portNUM_PROCESSORS];
#endif // portNUM_PROCESSORS == 2
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

/* A flag indicating that Idle hook has run on a given CPU;
 * Next interrupt on the same CPU will take s_rtos_lock_handle.
 */
static bool s_core_idle[portNUM_PROCESSORS];

/* When no RTOS tasks are active, these locks are released to allow going into
 * a lower power mode. Used by ISR hook and idle hook.
 */
static esp_pm_lock_handle_t s_rtos_lock_handle[portNUM_PROCESSORS];

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
#endif // WITH_PROFILING

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
/* Indicates to the ISR hook that CCOMPARE needs to be updated on the given CPU.
 * Used in conjunction with cross-core interrupt to update CCOMPARE on the other CPU.
 */
static volatile bool s_need_update_ccompare[portNUM_PROCESSORS];

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
#if CONFIG_PM_SLP_DEFAULT_PARAMS_OPT
static void esp_pm_light_sleep_default_params_config(int min_freq_mhz, int max_freq_mhz);
#endif

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

esp_err_t esp_pm_configure(const void* vconfig)
{
#ifndef CONFIG_PM_ENABLE
    return ESP_ERR_NOT_SUPPORTED;
#endif

#if CONFIG_IDF_TARGET_ESP32
    const esp_pm_config_esp32_t* config = (const esp_pm_config_esp32_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32S2
    const esp_pm_config_esp32s2_t* config = (const esp_pm_config_esp32s2_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32S3
    const esp_pm_config_esp32s3_t* config = (const esp_pm_config_esp32s3_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32C3
    const esp_pm_config_esp32c3_t* config = (const esp_pm_config_esp32c3_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32H4
    const esp_pm_config_esp32h4_t* config = (const esp_pm_config_esp32h4_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32C2
    const esp_pm_config_esp32c2_t* config = (const esp_pm_config_esp32c2_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32C6
    const esp_pm_config_esp32c6_t* config = (const esp_pm_config_esp32c6_t*) vconfig;
#endif

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
    int apb_max_freq = MIN(max_freq_mhz, 80); /* CPU frequency in APB_MAX mode */
#endif

    apb_max_freq = MAX(apb_max_freq, min_freq_mhz);

    ESP_LOGI(TAG, "Frequency switching config: "
                  "CPU_MAX: %d, APB_MAX: %d, APB_MIN: %d, Light sleep: %s",
                  max_freq_mhz,
                  apb_max_freq,
                  min_freq_mhz,
                  config->light_sleep_enable ? "ENABLED" : "DISABLED");

    portENTER_CRITICAL(&s_switch_lock);

    bool res __attribute__((unused));
    res = rtc_clk_cpu_freq_mhz_to_config(max_freq_mhz, &s_cpu_freq_by_mode[PM_MODE_CPU_MAX]);
    assert(res);
    res = rtc_clk_cpu_freq_mhz_to_config(apb_max_freq, &s_cpu_freq_by_mode[PM_MODE_APB_MAX]);
    assert(res);
    res = rtc_clk_cpu_freq_mhz_to_config(min_freq_mhz, &s_cpu_freq_by_mode[PM_MODE_APB_MIN]);
    assert(res);
    s_cpu_freq_by_mode[PM_MODE_LIGHT_SLEEP] = s_cpu_freq_by_mode[PM_MODE_APB_MIN];
    s_light_sleep_en = config->light_sleep_enable;
    s_config_changed = true;
    portEXIT_CRITICAL(&s_switch_lock);

#if CONFIG_PM_SLP_DISABLE_GPIO && SOC_GPIO_SUPPORT_SLP_SWITCH
    esp_sleep_enable_gpio_switch(config->light_sleep_enable);
#endif

#if CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP && SOC_PM_SUPPORT_CPU_PD
    esp_err_t ret = esp_sleep_cpu_pd_low_init(config->light_sleep_enable);
    if (config->light_sleep_enable && ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to enable CPU power down during light sleep.");
    }
#endif

#if CONFIG_PM_SLP_DEFAULT_PARAMS_OPT
    if (config->light_sleep_enable) {
        esp_pm_light_sleep_default_params_config(min_freq_mhz, max_freq_mhz);
    }
#endif

    return ESP_OK;
}

esp_err_t esp_pm_get_configuration(void* vconfig)
{
    if (vconfig == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_IDF_TARGET_ESP32
    esp_pm_config_esp32_t* config = (esp_pm_config_esp32_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32S2
    esp_pm_config_esp32s2_t* config = (esp_pm_config_esp32s2_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32S3
    esp_pm_config_esp32s3_t* config = (esp_pm_config_esp32s3_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32C3
    esp_pm_config_esp32c3_t* config = (esp_pm_config_esp32c3_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32H4
    esp_pm_config_esp32h4_t* config = (esp_pm_config_esp32h4_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32C2
    esp_pm_config_esp32c2_t* config = (esp_pm_config_esp32c2_t*) vconfig;
#elif CONFIG_IDF_TARGET_ESP32C6
    esp_pm_config_esp32c6_t* config = (esp_pm_config_esp32c6_t*) vconfig;
#endif

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

#if portNUM_PROCESSORS == 2
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
#endif // portNUM_PROCESSORS == 2

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
            s_need_update_ccompare[core_id] = false;
        }
#endif
        portEXIT_CRITICAL_ISR(&s_switch_lock);
    } while (true);
    if (new_mode == s_mode) {
        portEXIT_CRITICAL_ISR(&s_switch_lock);
        return;
    }
    s_is_switching = true;
    bool config_changed = s_config_changed;
    s_config_changed = false;
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
        rtc_clk_cpu_freq_set_config_fast(&new_config);
        if (!switch_down) {
            on_freq_update(old_ticks_per_us, new_ticks_per_us);
        }
        ESP_PM_TRACE_EXIT(FREQ_SWITCH, core_id);
    }

    portENTER_CRITICAL_ISR(&s_switch_lock);
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
static void IRAM_ATTR update_ccompare(void)
{
#if CONFIG_PM_UPDATE_CCOMPARE_HLI_WORKAROUND
    /* disable level 4 and below */
    uint32_t irq_status = XTOS_SET_INTLEVEL(XCHAL_DEBUGLEVEL - 2);
#endif
    uint32_t ccount = esp_cpu_get_cycle_count();
    uint32_t ccompare = XTHAL_GET_CCOMPARE(XT_TIMER_INDEX);
    if ((ccompare - CCOMPARE_MIN_CYCLES_IN_FUTURE) - ccount < UINT32_MAX / 2) {
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
    for (int i = 0; i < PERIPH_SKIP_LIGHT_SLEEP_NO; i++) {
        if (s_periph_skip_light_sleep_cb[i] == cb) {
            return ESP_OK;
        } else if (s_periph_skip_light_sleep_cb[i] == NULL) {
            s_periph_skip_light_sleep_cb[i] = cb;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_pm_unregister_skip_light_sleep_callback(skip_light_sleep_cb_t cb)
{
    for (int i = 0; i < PERIPH_SKIP_LIGHT_SLEEP_NO; i++) {
        if (s_periph_skip_light_sleep_cb[i] == cb) {
            s_periph_skip_light_sleep_cb[i] = NULL;
            return ESP_OK;
        }
    }
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
#if portNUM_PROCESSORS == 2
    if (s_skip_light_sleep[core_id]) {
        s_skip_light_sleep[core_id] = false;
        s_skipped_light_sleep[core_id] = true;
        return true;
    }
#endif // portNUM_PROCESSORS == 2

    if (s_mode != PM_MODE_LIGHT_SLEEP || s_is_switching || periph_should_skip_light_sleep()) {
        s_skipped_light_sleep[core_id] = true;
    } else {
        s_skipped_light_sleep[core_id] = false;
    }
    return s_skipped_light_sleep[core_id];
}

static inline void IRAM_ATTR other_core_should_skip_light_sleep(int core_id)
{
#if portNUM_PROCESSORS == 2
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
        if (sleep_time_us >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP * portTICK_PERIOD_MS * 1000LL) {
            esp_sleep_enable_timer_wakeup(sleep_time_us - LIGHT_SLEEP_EARLY_WAKEUP_US);
#ifdef CONFIG_PM_TRACE
            /* to force tracing GPIOs to keep state */
            esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#endif
            /* Enter sleep */
            ESP_PM_TRACE_ENTER(SLEEP, core_id);
            int64_t sleep_start = esp_timer_get_time();
            esp_light_sleep_start();
            int64_t slept_us = esp_timer_get_time() - sleep_start;
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
    portEXIT_CRITICAL_ISR(&s_switch_lock);

    time_in_mode[cur_mode] += now - last_mode_change_time;

    fprintf(out, "\nMode stats:\n");
    fprintf(out, "%-8s  %-10s  %-10s  %-10s\n", "Mode", "CPU_freq", "Time(us)", "Time(%)");
    for (int i = 0; i < PM_MODE_COUNT; ++i) {
        if (i == PM_MODE_LIGHT_SLEEP && !s_light_sleep_en) {
            /* don't display light sleep mode if it's not enabled */
            continue;
        }
        fprintf(out, "%-8s  %-3dM%-7s %-10lld  %-2d%%\n",
                s_mode_names[i],
                s_cpu_freq_by_mode[i].freq_mhz,
                "",                                     //Empty space to align columns
                time_in_mode[i],
                (int) (time_in_mode[i] * 100 / now));
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
    while(!uart_ll_is_tx_idle(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM)));
    /* When DFS is enabled, override system setting and use REFTICK as UART clock source */
    uart_ll_set_sclk(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM), clk_source);

    uint32_t sclk_freq;
    esp_err_t err = uart_get_sclk_freq(clk_source, &sclk_freq);
    assert(err == ESP_OK);
    uart_ll_set_baudrate(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM), CONFIG_ESP_CONSOLE_UART_BAUDRATE, sclk_freq);
#endif // CONFIG_ESP_CONSOLE_UART

#ifdef CONFIG_PM_TRACE
    esp_pm_trace_init();
#endif

#if CONFIG_PM_SLP_DISABLE_GPIO && SOC_GPIO_SUPPORT_SLP_SWITCH
    esp_sleep_config_gpio_isolate();
#endif
    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "rtos0",
            &s_rtos_lock_handle[0]));
    ESP_ERROR_CHECK(esp_pm_lock_acquire(s_rtos_lock_handle[0]));

#if portNUM_PROCESSORS == 2
    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "rtos1",
            &s_rtos_lock_handle[1]));
    ESP_ERROR_CHECK(esp_pm_lock_acquire(s_rtos_lock_handle[1]));
#endif // portNUM_PROCESSORS == 2

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
#if CONFIG_IDF_TARGET_ESP32
    esp_pm_config_esp32_t cfg = {
#elif CONFIG_IDF_TARGET_ESP32S2
    esp_pm_config_esp32s2_t cfg = {
#elif CONFIG_IDF_TARGET_ESP32S3
    esp_pm_config_esp32s3_t cfg = {
#elif CONFIG_IDF_TARGET_ESP32C3
    esp_pm_config_esp32c3_t cfg = {
#elif CONFIG_IDF_TARGET_ESP32H4
    esp_pm_config_esp32h4_t cfg = {
#elif CONFIG_IDF_TARGET_ESP32C2
    esp_pm_config_esp32c2_t cfg = {
#elif CONFIG_IDF_TARGET_ESP32C6
    esp_pm_config_esp32c6_t cfg = {
#endif
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
#if defined(CONFIG_FREERTOS_SYSTICK_USES_CCOUNT) && (portNUM_PROCESSORS == 2)
    if (s_need_update_ccompare[core_id]) {
        update_ccompare();
        s_need_update_ccompare[core_id] = false;
    } else {
        leave_idle();
    }
#else
    leave_idle();
#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT && portNUM_PROCESSORS == 2
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

#define PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO 1

/* Inform peripherals of light sleep wakeup overhead time */
static inform_out_light_sleep_overhead_cb_t s_periph_inform_out_light_sleep_overhead_cb[PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO];

esp_err_t esp_pm_register_inform_out_light_sleep_overhead_callback(inform_out_light_sleep_overhead_cb_t cb)
{
    for (int i = 0; i < PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO; i++) {
        if (s_periph_inform_out_light_sleep_overhead_cb[i] == cb) {
            return ESP_OK;
        } else if (s_periph_inform_out_light_sleep_overhead_cb[i] == NULL) {
            s_periph_inform_out_light_sleep_overhead_cb[i] = cb;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_pm_unregister_inform_out_light_sleep_overhead_callback(inform_out_light_sleep_overhead_cb_t cb)
{
    for (int i = 0; i < PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO; i++) {
        if (s_periph_inform_out_light_sleep_overhead_cb[i] == cb) {
            s_periph_inform_out_light_sleep_overhead_cb[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void periph_inform_out_light_sleep_overhead(uint32_t out_light_sleep_time)
{
    for (int i = 0; i < PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO; i++) {
        if (s_periph_inform_out_light_sleep_overhead_cb[i]) {
            s_periph_inform_out_light_sleep_overhead_cb[i](out_light_sleep_time);
        }
    }
}

static update_light_sleep_default_params_config_cb_t s_light_sleep_default_params_config_cb = NULL;

void esp_pm_register_light_sleep_default_params_config_callback(update_light_sleep_default_params_config_cb_t cb)
{
    if (s_light_sleep_default_params_config_cb == NULL) {
        s_light_sleep_default_params_config_cb = cb;
    }
}

void esp_pm_unregister_light_sleep_default_params_config_callback(void)
{
    if (s_light_sleep_default_params_config_cb) {
        s_light_sleep_default_params_config_cb = NULL;
    }
}

#if CONFIG_PM_SLP_DEFAULT_PARAMS_OPT
static void esp_pm_light_sleep_default_params_config(int min_freq_mhz, int max_freq_mhz)
{
    if (s_light_sleep_default_params_config_cb) {
        (*s_light_sleep_default_params_config_cb)(min_freq_mhz, max_freq_mhz);
    }
}
#endif
