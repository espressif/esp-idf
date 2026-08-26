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
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR __attribute__((deprecated)) __attribute__((pure)) uint32_t cpu_ll_get_core_id(void)
{
    return esp_cpu_get_core_id();
}

FORCE_INLINE_ATTR __attribute__((deprecated)) uint32_t cpu_ll_get_cycle_count(void)
{
    return (uint32_t)esp_cpu_get_cycle_count();
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_set_cycle_count(uint32_t val)
{
    esp_cpu_set_cycle_count((esp_cpu_cycle_count_t)val);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void *cpu_ll_get_sp(void)
{
    return esp_cpu_get_sp();
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_init_hwloop(void)
{
    ;   // Nothing to do. Contents moved to bootloader directly
}

#if SOC_CPU_BREAKPOINTS_NUM > 0
FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_set_breakpoint(int id, uint32_t pc)
{
    esp_cpu_set_breakpoint(id, (const void *)pc);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_clear_breakpoint(int id)
{
    esp_cpu_clear_breakpoint(id);
}
#endif // SOC_CPU_BREAKPOINTS_NUM > 0

FORCE_INLINE_ATTR __attribute__((deprecated)) __attribute__((pure)) uint32_t cpu_ll_ptr_to_pc(const void *addr)
{
    return ((uint32_t) addr);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) __attribute__((pure)) void *cpu_ll_pc_to_ptr(uint32_t pc)
{
    return esp_cpu_pc_to_addr(pc);
}


FORCE_INLINE_ATTR __attribute__((deprecated))
void cpu_ll_set_watchpoint(int id, const void *addr, size_t size, bool on_read, bool on_write)
{
    esp_cpu_watchpoint_trigger_t trigger;
    if (on_read && on_write) {
        trigger = ESP_CPU_WATCHPOINT_ACCESS;
    } else if (on_read) {
        trigger = ESP_CPU_WATCHPOINT_LOAD;
    } else {
        trigger = ESP_CPU_WATCHPOINT_STORE;
    }
    esp_cpu_set_watchpoint(id, addr, size, trigger);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_clear_watchpoint(int id)
{
    esp_cpu_clear_watchpoint(id);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) bool cpu_ll_is_debugger_attached(void)
{
    return esp_cpu_dbgr_is_attached();
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_break(void)
{
    esp_cpu_dbgr_break();
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_set_vecbase(const void *base)
{
    esp_cpu_intr_set_ivt_addr(base);
}

FORCE_INLINE_ATTR __attribute__((deprecated)) void cpu_ll_waiti(void)
{
    esp_cpu_wait_for_intr();
}

FORCE_INLINE_ATTR __attribute__((deprecated))
void cpu_ll_compare_and_set_native(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
#ifdef __clang_analyzer__
    //Teach clang-tidy that "addr" and "set" cannot be const as they can both be updated by S32C1I instruction
    volatile uint32_t temp;
    temp = *addr;
    *addr = temp;
    temp = *set;
    *set = temp;
#endif
#ifdef __XTENSA__
#if XCHAL_HAVE_S32C1I
    __asm__ __volatile__ (
        "WSR    %2, SCOMPARE1 \n"
        "S32C1I %0, %1, 0 \n"
        :"=r"(*set)
        :"r"(addr), "r"(compare), "0"(*set)
    );
#else // XCHAL_HAVE_S32C1I
    uint32_t old_value;

    // No S32C1I, so do this by disabling and re-enabling interrupts (slower)
    uint32_t intlevel;
    __asm__ __volatile__ ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) "\n"
                          : "=r"(intlevel));

    old_value = *addr;
    if (old_value == compare) {
        *addr = *set;
    }

    __asm__ __volatile__ ("memw \n"
                          "wsr %0, ps\n"
                          :: "r"(intlevel));

    *set = old_value;
#endif // XCHAL_HAVE_S32C1I
#else
    uint32_t old_value;
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);

    old_value = *addr;
    if (old_value == compare) {
        *addr = *set;
    }

    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);

    *set = old_value;
#endif
}

#ifdef __cplusplus
}
#endif
