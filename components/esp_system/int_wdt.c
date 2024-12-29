/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/wdt_hal.h"
#include "hal/mwdt_ll.h"
#include "hal/timer_ll.h"
#include "freertos/FreeRTOS.h"
#include "esp_cpu.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_chip_info.h"
#include "esp_freertos_hooks.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_int_wdt.h"

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_TIMER_SUPPORT_SLEEP_RETENTION
#include "esp_private/sleep_retention.h"
#endif

#if SOC_TIMER_GROUPS > 1

/* If we have two hardware timer groups, use the second one for interrupt watchdog. */
#define WDT_LEVEL_INTR_SOURCE   ETS_TG1_WDT_LEVEL_INTR_SOURCE
#define IWDT_PRESCALER          MWDT_LL_DEFAULT_CLK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define IWDT_TICKS_PER_US       500
#define IWDT_INSTANCE           WDT_MWDT1
#define IWDT_INITIAL_TIMEOUT_S  5
#define IWDT_PERIPH             PERIPH_TIMG1_MODULE
#define IWDT_TIMER_GROUP        1

#else

#define WDT_LEVEL_INTR_SOURCE   ETS_TG0_WDT_LEVEL_INTR_SOURCE
#define IWDT_PRESCALER          MWDT_LL_DEFAULT_CLK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define IWDT_TICKS_PER_US       500
#define IWDT_INSTANCE           WDT_MWDT0
#define IWDT_INITIAL_TIMEOUT_S  5
#define IWDT_PERIPH             PERIPH_TIMG0_MODULE
#define IWDT_TIMER_GROUP        0

#endif // SOC_TIMER_GROUPS > 1

#if CONFIG_ESP_INT_WDT
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_TIMER_SUPPORT_SLEEP_RETENTION
static const char* TAG = "int_wdt";
static esp_err_t sleep_int_wdt_retention_init(void *arg)
{
    uint32_t group_id = *(uint32_t *)arg;
    esp_err_t err = sleep_retention_entries_create(tg_wdt_regs_retention[group_id].link_list,
                                                   tg_wdt_regs_retention[group_id].link_num,
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
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    esp_err_t err = sleep_retention_module_init((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate((group_id == 0) ? SLEEP_RETENTION_MODULE_TG0_WDT : SLEEP_RETENTION_MODULE_TG1_WDT);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Failed to allocate sleep retention linked list for interrupt watchdog timer retention");
        }
    }
    return err;
}
#endif

static wdt_hal_context_t iwdt_context;

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
/*
 * This parameter is used to indicate the response time of Interrupt watchdog to
 * identify the live lock.
 */
#define IWDT_LIVELOCK_TIMEOUT_MS    (20)
extern uint32_t _lx_intr_livelock_counter, _lx_intr_livelock_max;
#endif

#if CONFIG_ESP_INT_WDT_CHECK_CPU1
volatile bool int_wdt_cpu1_ticked = false;
#endif

static void IRAM_ATTR tick_hook(void)
{
#if CONFIG_ESP_INT_WDT_CHECK_CPU1
    if (esp_cpu_get_core_id() != 0) {
        int_wdt_cpu1_ticked = true;
    } else {
        // Only feed wdt if app cpu also ticked.
        if (int_wdt_cpu1_ticked) {
            // Todo: Check if there's a way to avoid reconfiguring the stages on each feed.
            wdt_hal_write_protect_disable(&iwdt_context);
            // Reconfigure stage timeouts
#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
            _lx_intr_livelock_counter = 0;
            wdt_hal_config_stage(&iwdt_context, WDT_STAGE0,
                                 CONFIG_ESP_INT_WDT_TIMEOUT_MS * 1000 / IWDT_TICKS_PER_US / (_lx_intr_livelock_max + 1), WDT_STAGE_ACTION_INT);                    // Set timeout before interrupt
#else
            wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, CONFIG_ESP_INT_WDT_TIMEOUT_MS * 1000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_INT);          // Set timeout before interrupt
#endif
            wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, 2 * CONFIG_ESP_INT_WDT_TIMEOUT_MS * 1000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_RESET_SYSTEM); // Set timeout before reset
            wdt_hal_feed(&iwdt_context);
            wdt_hal_write_protect_enable(&iwdt_context);
            int_wdt_cpu1_ticked = false;
        }
    }
#else // CONFIG_ESP_INT_WDT_CHECK_CPU1
    if (esp_cpu_get_core_id() != 0) {
        return;
    } else {
        // Todo: Check if there's a way to avoid reconfiguring the stages on each feed.
        wdt_hal_write_protect_disable(&iwdt_context);
        // Reconfigure stage timeouts
        wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, CONFIG_ESP_INT_WDT_TIMEOUT_MS * 1000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_INT);          // Set timeout before interrupt
        wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, 2 * CONFIG_ESP_INT_WDT_TIMEOUT_MS * 1000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_RESET_SYSTEM); // Set timeout before reset
        wdt_hal_feed(&iwdt_context);
        wdt_hal_write_protect_enable(&iwdt_context);
    }
#endif // CONFIG_ESP_INT_WDT_CHECK_CPU1
}

void esp_int_wdt_init(void)
{
    PERIPH_RCC_ACQUIRE_ATOMIC(IWDT_PERIPH, ref_count) {
        if (ref_count == 0) {
            timer_ll_enable_bus_clock(IWDT_TIMER_GROUP, true);
            timer_ll_reset_register(IWDT_TIMER_GROUP);
        }
    }
    /*
     * Initialize the WDT timeout stages. Note that the initial timeout is set to 5 seconds as variable startup times of
     * each CPU can lead to a timeout. The tick hooks will set the WDT timers to the actual timeout.
     * Todo: Fix this
     */
    wdt_hal_init(&iwdt_context, IWDT_INSTANCE, IWDT_PRESCALER, true);
    wdt_hal_write_protect_disable(&iwdt_context);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, IWDT_INITIAL_TIMEOUT_S * 1000000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_INT);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, IWDT_INITIAL_TIMEOUT_S * 1000000 / IWDT_TICKS_PER_US, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_enable(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_TIMER_SUPPORT_SLEEP_RETENTION
    esp_int_wdt_retention_enable(IWDT_TIMER_GROUP);
#endif

#if (CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_BTDM_CTRL_HLI)
#define APB_DCRSET      (0x200c)
#define APB_ITCTRL      (0x3f00)
#define ERI_ADDR(APB)   (0x100000 + (APB))
#define _SYM2STR(x)     # x
#define SYM2STR(x)      _SYM2STR(x)

    uint32_t eriadrs, scratch = 0, immediate = 0;
    if (soc_has_cache_lock_bug()) {
        if (xPortGetCoreID() != CONFIG_BTDM_CTRL_PINNED_TO_CORE) {
            __asm__ __volatile__(
                /* Enable Xtensa Debug Module Integration Mode */
                "movi   %[ERI], " SYM2STR(ERI_ADDR(APB_ITCTRL)) "\n"
                "rer    %[REG], %[ERI]\n"
                "movi   %[IMM], 1\n"
                "or     %[REG], %[IMM], %[REG]\n"
                "wer    %[REG], %[ERI]\n"
                /* Enable Xtensa Debug Module Break_In signal */
                "movi   %[ERI], " SYM2STR(ERI_ADDR(APB_DCRSET)) "\n"
                "rer    %[REG], %[ERI]\n"
                "movi   %[IMM], 0x10000\n"
                "or     %[REG], %[IMM], %[REG]\n"
                "wer    %[REG], %[ERI]\n"
                : [ERI] "=r"(eriadrs), [REG] "+r"(scratch), [IMM] "+r"(immediate)
            );
        }
    }
#endif // (CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_BTDM_CTRL_HLI)
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
#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    /*
     * This is a workaround for issue 3.15 in "ESP32 ECO and workarounds for
     * Bugs" document.
     */
    _lx_intr_livelock_counter = 0;
    if (soc_has_cache_lock_bug()) {
        assert((portTICK_PERIOD_MS << 1) <= IWDT_LIVELOCK_TIMEOUT_MS);
        assert(CONFIG_ESP_INT_WDT_TIMEOUT_MS >= (IWDT_LIVELOCK_TIMEOUT_MS * 3));
        _lx_intr_livelock_max = CONFIG_ESP_INT_WDT_TIMEOUT_MS / IWDT_LIVELOCK_TIMEOUT_MS - 1;
    }
#endif
    esp_intr_enable_source(ETS_INT_WDT_INUM);
}

#endif // CONFIG_ESP_INT_WDT
