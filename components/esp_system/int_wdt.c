/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/mwdt_periph.h"
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

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_MWDT_SUPPORT_SLEEP_RETENTION
#include "esp_private/sleep_retention.h"
#endif

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
#include "esp_private/eco3_livelock_workaround.h"
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
static portMUX_TYPE s_iwdt_configure_lock = portMUX_INITIALIZER_UNLOCKED;

static void ESP_SYSTEM_IRAM_ATTR reconfigure_ticks(uint32_t stage0_ticks, uint32_t stage1_ticks)
{
    portENTER_CRITICAL_SAFE(&s_iwdt_configure_lock);
    wdt_hal_write_protect_disable(&iwdt_context);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE0, stage0_ticks, WDT_STAGE_ACTION_INT);
    wdt_hal_config_stage(&iwdt_context, WDT_STAGE1, stage1_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_feed(&iwdt_context);
    wdt_hal_write_protect_enable(&iwdt_context);
    portEXIT_CRITICAL_SAFE(&s_iwdt_configure_lock);
}

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
void ESP_SYSTEM_IRAM_ATTR esp_int_wdt_reconfigure_ticks(uint32_t stage0_ticks, uint32_t stage1_ticks)
{
    reconfigure_ticks(stage0_ticks, stage1_ticks);
}
#endif

#if CONFIG_ESP_INT_WDT_CHECK_CPU1
volatile bool int_wdt_cpu1_ticked = false;
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
    if (int_wdt_cpu1_ticked) {
        int_wdt_cpu1_ticked = false;
    } else {
        return;
    }
#endif
#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    esp_int_wdt_set_livelock_params(CONFIG_ESP_INT_WDT_TIMEOUT_MS);
    reconfigure_ticks(esp_int_wdt_livelock_get_feed_stage0_ticks(), IWDT_STAGE1_TIMEOUT_US);
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

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
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

#endif // CONFIG_ESP_INT_WDT
