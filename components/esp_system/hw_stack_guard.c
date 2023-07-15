/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

ESP_SYSTEM_INIT_FN(esp_hw_stack_guard_init, ESP_SYSTEM_INIT_ALL_CORES, 101)
{
    uint32_t core_id = esp_cpu_get_core_id();

    if (core_id == 0) {
        /* initialize the peripheral only when running on core 0  */
        periph_module_enable(PERIPH_ASSIST_DEBUG_MODULE);
        periph_module_reset(PERIPH_ASSIST_DEBUG_MODULE);
    }

    /* just in case, disable the interrupt and clear pending status  */
    assist_debug_hal_sp_int_disable(core_id);
    assist_debug_hal_sp_int_clear(core_id);

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

    /* enable interrup routine  */
    esp_rom_route_intr_matrix(core_id, ETS_ASSIST_DEBUG_INTR_SOURCE, ETS_ASSIST_DEBUG_INUM);

    esprv_intc_int_set_type(ETS_ASSIST_DEBUG_INUM, INTR_TYPE_LEVEL);
    esprv_intc_int_set_priority(ETS_ASSIST_DEBUG_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

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

void esp_hw_stack_guard_get_bounds(uint32_t *sp_min, uint32_t *sp_max)
{
    uint32_t core_id = esp_cpu_get_core_id();

    assist_debug_hal_get_sp_bounds(core_id, sp_min, sp_max);
}

bool esp_hw_stack_guard_is_fired(void)
{
    uint32_t core_id = esp_cpu_get_core_id();

    return assist_debug_hal_is_sp_ovf_fired(core_id);
}

uint32_t esp_hw_stack_guard_get_pc(void)
{
    uint32_t core_id = esp_cpu_get_core_id();

    return assist_debug_hal_get_sp_ovf_pc(core_id);
}
