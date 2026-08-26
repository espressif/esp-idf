/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
Note: This is a compatibility header. Call the interfaces in esp_cpu.h instead
*/

#include <stdint.h>
#include <stddef.h>
#include "soc/soc_caps.h"
#include "hal/cpu_ll.h"
#include "esp_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WATCHPOINT_TRIGGER_ON_RO = ESP_CPU_WATCHPOINT_LOAD,     // on read
    WATCHPOINT_TRIGGER_ON_WO = ESP_CPU_WATCHPOINT_STORE,    // on write
    WATCHPOINT_TRIGGER_ON_RW = ESP_CPU_WATCHPOINT_ACCESS,   // on either read or write
} watchpoint_trigger_t;

/**
 * Return the ID of the core currently executing this code.
 *
 * @return core id [0..SOC_CPU_CORES_NUM - 1]
 */
#define cpu_hal_get_core_id()           cpu_ll_get_core_id()

/**
 * Get the current value of the stack pointer.
 *
 * @return the current stack pointer
 */
#define cpu_hal_get_sp()                cpu_ll_get_sp()

/**
 * Get the current value of the internal counter that increments
 * every processor-clock cycle.
 *
 * @return cycle count; returns 0 if not supported
 */
#define cpu_hal_get_cycle_count()       cpu_ll_get_cycle_count()

/**
 * Set the given value into the internal counter that increments
 * every processor-clock cycle.
 */
#define cpu_hal_set_cycle_count(val)    cpu_ll_set_cycle_count(val)

/**
 * Check if some form of debugger is attached to CPU.
 *
 * @return true debugger is attached
 * @return false no debugger is attached/ no support for debuggers
 */
#define cpu_hal_is_debugger_attached()  cpu_ll_is_debugger_attached()

/**
 * Init HW loop status.
 */
#define cpu_hal_init_hwloop()           cpu_ll_init_hwloop()

/**
 * Trigger a call to debugger.
 */
#define cpu_hal_break()                 cpu_ll_break()

/**
 * Wait for interrupt.
 */
#define cpu_hal_waiti()                 cpu_ll_waiti()

#if SOC_CPU_BREAKPOINTS_NUM > 0

/**
 * Set and enable breakpoint at an instruction address.
 *
 * @note Overwrites previously set breakpoint with same breakpoint ID.
 *
 * @param id breakpoint to set [0..SOC_CPU_BREAKPOINTS_NUM - 1]
 * @param addr address to set a breakpoint on
 */
static inline __attribute__((deprecated)) void cpu_hal_set_breakpoint(int id, const void *addr)
{
    esp_cpu_set_breakpoint(id, addr);
}
/**
 * Clear and disable breakpoint.
 *
 * @param id breakpoint to clear [0..SOC_CPU_BREAKPOINTS_NUM - 1]
 */
static inline __attribute__((deprecated)) void cpu_hal_clear_breakpoint(int id)
{
    esp_cpu_clear_breakpoint(id);
}

#endif // SOC_CPU_BREAKPOINTS_NUM > 0

#if SOC_CPU_WATCHPOINTS_NUM > 0

/**
 * Set and enable a watchpoint, specifying the memory range and trigger operation.
 *
 * @param id watchpoint to set [0..SOC_CPU_WATCHPOINTS_NUM - 1]
 * @param addr starting address
 * @param size number of bytes from starting address to watch
 * @param trigger operation on specified memory range that triggers the watchpoint (read, write, read/write)
 */
static inline __attribute__((deprecated))
void cpu_hal_set_watchpoint(int id, const void *addr, size_t size, watchpoint_trigger_t trigger)
{
    esp_cpu_set_watchpoint(id, addr, size, (esp_cpu_watchpoint_trigger_t)trigger);
}

/**
 * Clear and disable watchpoint.
 *
 * @param id watchpoint to clear [0..SOC_CPU_WATCHPOINTS_NUM - 1]
 */
static inline __attribute__((deprecated)) void cpu_hal_clear_watchpoint(int id)
{
    esp_cpu_clear_watchpoint(id);
}

#endif // SOC_CPU_WATCHPOINTS_NUM > 0

/**
 * Set exception vector table base address.
 *
 * @param base address to move the exception vector table to
 */
static inline __attribute__((deprecated)) __attribute__((always_inline))
void cpu_hal_set_vecbase(const void *base)
{
    esp_cpu_intr_set_ivt_addr(base);
}

#ifdef __cplusplus
}
#endif
