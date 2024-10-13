/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/assist_debug_hal.h"
#include "esp_private/hw_stack_guard.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/startup_internal.h"
#include "soc/soc_caps.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"

ESP_SYSTEM_INIT_FN(esp_hw_stack_guard_init, SECONDARY, ESP_SYSTEM_INIT_ALL_CORES, 101)
{
    uint32_t core_id = esp_cpu_get_core_id();

    ESP_INTR_DISABLE(ETS_ASSIST_DEBUG_INUM);

#if SOC_CPU_CORES_NUM > 1
    PERIPH_RCC_ATOMIC()
#endif
    {
        assist_debug_ll_enable_bus_clock(true);
        assist_debug_ll_reset_register();
    }

    /* set interrupt to matrix */
    esp_rom_route_intr_matrix(core_id, ETS_ASSIST_DEBUG_INTR_SOURCE, ETS_ASSIST_DEBUG_INUM);
    esprv_int_set_type(ETS_ASSIST_DEBUG_INUM, INTR_TYPE_LEVEL);
    esprv_int_set_priority(ETS_ASSIST_DEBUG_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

    /*
     * enable interrupt
     * Note: to control hw_stack_guard use monitor enable/disable because in case:
     * - monitor == active
     * - interrupt != active
     * - trigger event happened
     * - you get an interrupt right after enabling interrupts
     * So, use monitor to disable hw_guard to avoid false-positives.
     * And keep interrupt always enabled for better performace (don't spend cpu time for enable/disable)
     */
    assist_debug_hal_sp_int_enable(core_id);

    ESP_INTR_ENABLE(ETS_ASSIST_DEBUG_INUM);
    return ESP_OK;
}

/* The functions below are designed to be used in interrupt/panic handler
 * In case using them in user's code put them into critical section */

void esp_hw_stack_guard_monitor_start(void)
{
    uint32_t core_id = esp_cpu_get_core_id();

    /* enable monitor. Interrupt is always enabled (see comment in esp_hw_stack_guard_init())  */
    assist_debug_hal_sp_mon_enable(core_id);
}

void esp_hw_stack_guard_monitor_stop(void)
{
    uint32_t core_id = esp_cpu_get_core_id();
    /* disable monitor. Interrupt is always enabled (see comment in esp_hw_stack_guard_init())  */
    assist_debug_hal_sp_mon_disable(core_id);
}

void esp_hw_stack_guard_set_bounds(uint32_t sp_min, uint32_t sp_max)
{
    uint32_t core_id = esp_cpu_get_core_id();

    assist_debug_hal_set_sp_bounds(core_id, sp_min, sp_max);
}

void esp_hw_stack_guard_get_bounds(uint32_t core_id, uint32_t *sp_min, uint32_t *sp_max)
{
    assist_debug_hal_get_sp_bounds(core_id, sp_min, sp_max);
}

uint32_t esp_hw_stack_guard_get_fired_cpu(void)
{
    for (uint32_t i = 0; i < SOC_CPU_CORES_NUM; i++) {
        if (assist_debug_hal_is_sp_ovf_fired(i)) {
            return i;
        }
    }
    return ESP_HW_STACK_GUARD_NOT_FIRED;
}

uint32_t esp_hw_stack_guard_get_pc(uint32_t core_id)
{
    return assist_debug_hal_get_sp_ovf_pc(core_id);
}
