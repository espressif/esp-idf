/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/wdt_hal.h"
#include "soc/system_intr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_cpu.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_private/esp_system_attr.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_chip_info.h"
#include "esp_freertos_hooks.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_int_wdt.h"
#include "esp_private/esp_sys_event_app_init.h"
#include "mwdt_priv.h"

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_MWDT_SUPPORT_SLEEP_RETENTION
#include "esp_private/sleep_retention.h"
#endif

#if CONFIG_ESP_INT_WDT && CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
#include "esp_private/eco3_livelock_workaround.h"
#include "esp_timer.h"
#include "hal/mwdt_periph.h"
#include "soc/timer_group_reg.h"
#endif

#if TIMG_LL_GET(INST_NUM) > 1

/* If we have two hardware timer groups, use the second one for interrupt watchdog. */
#define WDT_LEVEL_INTR_SOURCE   SYS_TG1_WDT_INTR_SOURCE
#define IWDT_PRESCALER          MWDT_LL_DEFAULT_CLK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define IWDT_INSTANCE           WDT_MWDT1
#define IWDT_PERIPH             PERIPH_TIMG1_MODULE
#define IWDT_TIMER_GROUP        1

#else

#define WDT_LEVEL_INTR_SOURCE   SYS_TG0_WDT_INTR_SOURCE
#define IWDT_PRESCALER          MWDT_LL_DEFAULT_CLK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define IWDT_INSTANCE           WDT_MWDT0
#define IWDT_PERIPH             PERIPH_TIMG0_MODULE
#define IWDT_TIMER_GROUP        0

#endif // TIMG_LL_GET(INST_NUM) > 1

#if CONFIG_ESP_INT_WDT
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_MWDT_SUPPORT_SLEEP_RETENTION
ESP_LOG_ATTR_TAG(TAG, "int_wdt");
static esp_err_t sleep_int_wdt_retention_init(void *arg)
{
    uint32_t group_id = *(uint32_t *)arg;
    esp_err_t err = sleep_retention_entries_create(mwdt_reg_retention_info[group_id].regdma_entry_array,
                                                   mwdt_reg_retention_info[group_id].array_size,
                                                   REGDMA_LINK_PRI_SYS_PERIPH_LOW,
                                                   (group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "Interrupt watchdog timer retention initialization");
    }
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to create sleep retention linked list for interrupt watchdog timer");
    return err;
}

static esp_err_t esp_int_wdt_retention_enable(uint32_t group_id)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs = { .create = { .handle = sleep_int_wdt_retention_init, .arg = &group_id } },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_ATTACH,
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    esp_err_t err = sleep_retention_module_init((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Failed to allocate sleep retention linked list for interrupt watchdog timer retention");
        } else {
            err = sleep_retention_module_attach((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "Failed to attach sleep retention linked list for interrupt watchdog timer retention");
            }
        }
    }
    return err;
}
#endif

static wdt_hal_context_t iwdt_context;
static void ESP_SYSTEM_IRAM_ATTR reconfigure_ticks(uint32_t stage0_ticks, uint32_t stage1_ticks)
{
    wdt_hal_write_protect_disable(&iwdt_context);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, stage0_ticks, WDT_STAGE_ACTION_INT);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, stage1_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_feed(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);
}

#if CONFIG_ESP_INT_WDT && CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
static portMUX_TYPE s_iwdt_configure_lock = portMUX_INITIALIZER_UNLOCKED;
void ESP_SYSTEM_IRAM_ATTR esp_int_wdt_reconfigure_ticks(uint32_t stage0_ticks, uint32_t stage1_ticks)
{
    portENTER_CRITICAL_SAFE(&s_iwdt_configure_lock);
    reconfigure_ticks(stage0_ticks, stage1_ticks);
    portEXIT_CRITICAL_SAFE(&s_iwdt_configure_lock);
}
#endif

#if CONFIG_ESP_INT_WDT && CONFIG_ESP32_ECO3_CACHE_LOCK_FIX

static inline __attribute__((always_inline)) void iwdt_observe_barrier(void)
{
    __asm__ __volatile__("memw" ::: "memory");
}

static inline __attribute__((always_inline)) unsigned iwdt_mask_high_interrupts(void)
{
    unsigned ps;
    __asm__ __volatile__("rsil %0, 5; memw" : "=a"(ps) :: "memory");
    return ps;
}

static inline __attribute__((always_inline)) void iwdt_restore_high_interrupts(unsigned ps)
{
    __asm__ __volatile__("memw; wsr %0, ps; rsync" :: "a"(ps) : "memory");
}

static inline __attribute__((always_inline)) bool iwdt_interrupt_pending(void)
{
    return (REG_READ(TIMG_INT_RAW_TIMERS_REG(1)) & TIMG_WDT_INT_RAW) != 0;
}

static inline __attribute__((always_inline)) uint32_t iwdt_stage0_ticks(void)
{
    return REG_READ(TIMG_WDTCONFIG2_REG(1));
}

static inline __attribute__((always_inline)) uint32_t iwdt_stage1_ticks(void)
{
    return REG_READ(TIMG_WDTCONFIG3_REG(1));
}

static inline __attribute__((always_inline)) bool iwdt_write_protect_is_open(void)
{
    return REG_READ(TIMG_WDTWPROTECT_REG(1)) == TIMG_WDT_WKEY_VALUE;
}

/* Serialize the livelock handler mode with the hardware timeout.
 * Handler completion relies on MEMW ordering its peripheral writes before
 * publishing idle bookkeeping in internal memory.
 */
static bool s_livelock_enabled = true;

extern volatile uint32_t _lx_intr_livelock_sync[2];
extern volatile uint32_t _lx_intr_livelock_app;
extern volatile uint32_t _lx_intr_livelock_pro;

/* Bound retries without feeding a stalled handler. The hardware watchdog
 * continues running and may reset the system before this bound expires.
 */
#ifndef IWDT_MAX_WAIT_CYCLES
#define IWDT_MAX_WAIT_CYCLES (1u << 30)
#endif

/* Observe both handler halves with local high interrupts masked. These
 * separate loads are rechecked after installing a fresh full timeout.
 */
static inline __attribute__((always_inline)) bool iwdt_handler_idle(void)
{
    iwdt_observe_barrier();
    bool idle = !iwdt_interrupt_pending()
                && _lx_intr_livelock_app == 0 && _lx_intr_livelock_pro == 0
                && _lx_intr_livelock_sync[0] == 0 && _lx_intr_livelock_sync[1] == 0;
    iwdt_observe_barrier();
    return idle;
}

/* Read-back detects rejected writes; handler exclusion provides ownership. */
static bool ESP_SYSTEM_IRAM_ATTR iwdt_write_stages(uint32_t stage0_ticks)
{
    wdt_hal_write_protect_disable(&iwdt_context);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, stage0_ticks, WDT_STAGE_ACTION_INT);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, IWDT_STAGE1_TIMEOUT_US, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_feed(&iwdt_context);
    bool accepted = iwdt_write_protect_is_open();
    wdt_hal_write_protect_enable(&iwdt_context);
    return accepted
           && iwdt_stage0_ticks() == stage0_ticks
           && iwdt_stage1_ticks() == IWDT_STAGE1_TIMEOUT_US;
}

/* Caller masks local high interrupts. The second check catches a handler
 * that started during the writes and has not yet finished. */
static bool ESP_SYSTEM_IRAM_ATTR iwdt_prepare_stages(bool *stages_touched)
{
    if (!iwdt_handler_idle()) {
        return false;
    }

    if (stages_touched != NULL) {
        *stages_touched = true;
    }
    return iwdt_write_stages(IWDT_STAGE0_TIMEOUT_US) && iwdt_handler_idle();
}

static void ESP_SYSTEM_IRAM_ATTR iwdt_finish_recovery(unsigned ps)
{
    bool restored = !s_livelock_enabled
                    || iwdt_write_stages(esp_int_wdt_livelock_get_feed_stage0_ticks());
    iwdt_restore_high_interrupts(ps);
    if (!restored) {
        abort();
    }
}

/* Sleep entry has already expired. Reestablish a verified full timeout with
 * no handler in flight before restoring the previous stage. Use the remainder
 * of the fixed cycle budget so recovery cannot extend the hard limit. */
static void ESP_SYSTEM_IRAM_ATTR iwdt_recover_mode(uint32_t t0)
{
    while ((uint32_t)(esp_cpu_get_cycle_count() - t0) < IWDT_MAX_WAIT_CYCLES) {
        unsigned ps = iwdt_mask_high_interrupts();
        if (iwdt_prepare_stages(NULL)) {
            iwdt_finish_recovery(ps);
            return;
        }
        iwdt_restore_high_interrupts(ps);
    }
    abort();
}

static bool ESP_SYSTEM_IRAM_ATTR iwdt_commit_mode(bool enable, int64_t deadline_us)
{
    uint32_t t0 = esp_cpu_get_cycle_count();
    bool stages_touched = false;
    while ((uint32_t)(esp_cpu_get_cycle_count() - t0) < IWDT_MAX_WAIT_CYCLES) {
        if (deadline_us && esp_timer_get_time() >= deadline_us) {
            if (stages_touched) {
                iwdt_recover_mode(t0);
            }
            return false;
        }

        unsigned ps = iwdt_mask_high_interrupts();
        if (!iwdt_prepare_stages(&stages_touched)) {
            /* Allow pending handlers to run before checking the deadline again. */
            iwdt_restore_high_interrupts(ps);
            continue;
        }

        if (deadline_us && esp_timer_get_time() >= deadline_us) {
            iwdt_finish_recovery(ps);
            return false;
        }

        /* Disable only after installing the full timeout. On enable, publish
         * the recovery budget before installing the short timeout. */
        bool ok;
        if (enable) {
            esp_int_wdt_set_livelock_params(CONFIG_ESP_INT_WDT_TIMEOUT_MS);
            ok = iwdt_write_stages(esp_int_wdt_livelock_get_feed_stage0_ticks());
        } else {
            esp_int_wdt_reset_livelock_params();
            ok = !iwdt_write_protect_is_open()
                 && iwdt_stage0_ticks() == IWDT_STAGE0_TIMEOUT_US
                 && iwdt_stage1_ticks() == IWDT_STAGE1_TIMEOUT_US;
        }
        iwdt_restore_high_interrupts(ps);
        if (!ok) {
            /* Do not continue with an unverified watchdog configuration. */
            abort();
        }
        return true;
    }
    abort();
}

bool ESP_SYSTEM_IRAM_ATTR esp_int_wdt_set_livelock_mode(bool enable, int64_t deadline_us)
{
    bool committed = true;
    portENTER_CRITICAL_SAFE(&s_iwdt_configure_lock);
    if (soc_has_cache_lock_bug()) {
        committed = iwdt_commit_mode(enable, deadline_us);
        if (committed) {
            s_livelock_enabled = enable;
        }
    } else {
        reconfigure_ticks(IWDT_STAGE0_TIMEOUT_US, IWDT_STAGE1_TIMEOUT_US);
    }
    portEXIT_CRITICAL_SAFE(&s_iwdt_configure_lock);
    return committed;
}

/* Preserve the selected mode when the tick feeds the watchdog. */
static void ESP_SYSTEM_IRAM_ATTR iwdt_tick(void)
{
    portENTER_CRITICAL_SAFE(&s_iwdt_configure_lock);
    if (soc_has_cache_lock_bug() && s_livelock_enabled) {
        esp_int_wdt_set_livelock_params(CONFIG_ESP_INT_WDT_TIMEOUT_MS);
        reconfigure_ticks(esp_int_wdt_livelock_get_feed_stage0_ticks(), IWDT_STAGE1_TIMEOUT_US);
    } else {
        reconfigure_ticks(IWDT_STAGE0_TIMEOUT_US, IWDT_STAGE1_TIMEOUT_US);
    }
    portEXIT_CRITICAL_SAFE(&s_iwdt_configure_lock);
}

#endif

#if CONFIG_ESP_INT_WDT_CHECK_CPU1
volatile bool int_wdt_cpu1_ticked = false;
/* Set while CPU1 is intentionally idle (e.g. tickless idle) and therefore not ticking. The CPU1
 * liveness requirement is bypassed so CPU0 keeps feeding the watchdog (and stays protected). */
static volatile bool s_int_wdt_cpu1_idle = false;
#endif

static void ESP_SYSTEM_IRAM_ATTR tick_hook(void)
{
    if (esp_cpu_get_core_id() != 0) {
#if CONFIG_ESP_INT_WDT_CHECK_CPU1
        int_wdt_cpu1_ticked = true;
#endif
        return;
    }
#if CONFIG_ESP_INT_WDT_CHECK_CPU1
    if (int_wdt_cpu1_ticked || s_int_wdt_cpu1_idle) {
        int_wdt_cpu1_ticked = false;
    } else {
        return;
    }
#endif
#if CONFIG_ESP_INT_WDT && CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    iwdt_tick();
#else
    reconfigure_ticks(IWDT_STAGE0_TIMEOUT_US, IWDT_STAGE1_TIMEOUT_US);
#endif
}

void esp_int_wdt_init(void)
{
    PERIPH_RCC_ACQUIRE_ATOMIC(IWDT_PERIPH, ref_count) {
        if (ref_count == 0) {
            timg_ll_enable_bus_clock(IWDT_TIMER_GROUP, true);
            timg_ll_reset_register(IWDT_TIMER_GROUP);
        }
    }
    /*
     * Initialize the WDT timeout stages. Note that the initial timeout is set to 5 seconds as variable startup times of
     * each CPU can lead to a timeout. The tick hooks will set the WDT timers to the actual timeout.
     * Todo: Fix this
     */
    wdt_hal_init(&iwdt_context, IWDT_INSTANCE, IWDT_PRESCALER, true);
    reconfigure_ticks(IWDT_INITIAL_TIMEOUT_US, IWDT_INITIAL_TIMEOUT_US);

    wdt_hal_write_protect_disable(&iwdt_context);
    wdt_hal_enable(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_MWDT_SUPPORT_SLEEP_RETENTION
    esp_int_wdt_retention_enable(IWDT_TIMER_GROUP);
#endif

#if CONFIG_ESP_INT_WDT && CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    /*
     * This is a workaround for issue WDT-3.15 in "ESP32 ECO and workarounds for
     * Bugs" document.
     */
    esp_int_wdt_init_for_livelock_fix();
    esp_int_wdt_set_livelock_params(CONFIG_ESP_INT_WDT_TIMEOUT_MS);
#endif
}

void esp_int_wdt_cpu_init(void)
{
    assert((CONFIG_ESP_INT_WDT_TIMEOUT_MS >= (portTICK_PERIOD_MS << 1)) && "Interrupt watchdog timeout needs to be at least twice the RTOS tick period!");
    // Register tick hook for current CPU to feed the INT WDT
    esp_register_freertos_tick_hook_for_cpu(tick_hook, esp_cpu_get_core_id());
    /*
     * Register INT WDT interrupt for current CPU. We do this manually as the timeout interrupt should call an assembly
     * panic handler (see riscv/vector.S and xtensa_vectors.S).
     */
    esp_intr_disable_source(ETS_INT_WDT_INUM);
    esp_rom_route_intr_matrix(esp_cpu_get_core_id(), WDT_LEVEL_INTR_SOURCE, ETS_INT_WDT_INUM);
#if SOC_CPU_HAS_FLEXIBLE_INTC
    esp_cpu_intr_set_type(ETS_INT_WDT_INUM, INTR_TYPE_LEVEL);
    esp_cpu_intr_set_priority(ETS_INT_WDT_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);
#endif
    esp_intr_enable_source(ETS_INT_WDT_INUM);
}

ESP_PRE_SCHEDULER_HANDLER_REGISTER(init_int_wdt, 100)
{
    (void)user_arg;
    (void)ctx;
    esp_int_wdt_init();
    return ESP_OK;
}

ESP_PRE_SCHEDULER_HANDLER_REGISTER_PER_CPU(init_int_wdt_cpu, 110)
{
    (void)user_arg;
    (void)ctx;
    esp_int_wdt_cpu_init();
    return ESP_OK;
}

void ESP_SYSTEM_IRAM_ATTR esp_int_wdt_pause(void)
{
    wdt_hal_write_protect_disable(&iwdt_context);
    wdt_hal_disable(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);
}

void ESP_SYSTEM_IRAM_ATTR esp_int_wdt_resume(void)
{
    wdt_hal_write_protect_disable(&iwdt_context);
    wdt_hal_feed(&iwdt_context);
    wdt_hal_enable(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);
}

#if CONFIG_ESP_INT_WDT_CHECK_CPU1
void ESP_SYSTEM_IRAM_ATTR esp_int_wdt_pause_cpu1_checking(bool pause)
{
    s_int_wdt_cpu1_idle = pause;
}
#endif // CONFIG_ESP_INT_WDT_CHECK_CPU1

#endif // CONFIG_ESP_INT_WDT
