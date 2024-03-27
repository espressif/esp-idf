/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for DEBUG_ASSIST peripheral

#pragma once

#include "soc/assist_debug_reg.h"
#define ASSIST_DEBUG_SP_SPILL_BITS  (ASSIST_DEBUG_CORE_0_SP_SPILL_MIN_ENA | ASSIST_DEBUG_CORE_0_SP_SPILL_MAX_ENA)

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>
#include "esp_attr.h"
#include "hal/assert.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Most other peripherals have 4 interrupt-related registers: INT_ENA_REG, INT_CLR_REG, INT_RAW_REG, INT_ST_REG, the
 * meaning of which is well-understood.
 *
 * Assist_debug peripheral uses a different structure of interrupt registers:
 *     INT_ENA_REG, INT_RLS_REG, INT_CLR_REG, INT_RAW_REG.
 *
 * Their behavior can be explained using the following (verilog-like) pseudo-code:
 *      reg sp_spill_max_st
 *      assign sp_spill_max = (sp > SP_MAX_REG)
 *      assign SP_SPILL_MAX_RAW = sp_spill_max & SPILL_MAX_ENA
 *      always (@posedge clk) begin
 *          if (reset) then sp_spill_max_st <= 0
 *          elif SP_SPILL_MAX_CLR then sp_spill_max_st <= 0
 *          else sp_spill_max_st <= SP_SPILL_MAX_RAW & SP_SPILL_MAX_RLS
 *      end
 *      // ...same for sp_spill_min and other things dsoc/hp_sys_clkrst_struct.hebug_assist can check.
 *
 *      // this is the final interrupt line coming out of the peripheral:
 *      assign DEBUG_ASSIST_INT = sp_spill_max_st | sp_spill_min_st | ...
 *
 * Basically, there is no "ST" register showing the final (latched) interrupt state, and there is an additional
 * "RLS" register which just like "ENA" can be used to mask the interrupt.
 * Note that writing to CLR clears the (internal) latched interrupt state 'sp_spill_max_st',
 * but doesn't affect the software-readable RAW register.
 *
 * In this code, we use "ENA" to enable monitoring of a particular condition, and "RLS" to enable the interrupt.
 * This allows checking whether the condition (e.g. sp > SP_MAX) has occurred by reading the RAW register, without
 * actually triggering the interrupt. Hence you will see the somewhat counter-intuitive use of "RLS" to enable the
 * interrupt, instead of "ENA".
 */

 /* These functions are optimized and designed for internal usage.
  * So, the API may differ from general ll layer pattern */

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_monitor_enable(uint32_t core_id)
{
    REG_SET_BIT(core_id ? ASSIST_DEBUG_CORE_1_INTR_ENA_REG : ASSIST_DEBUG_CORE_0_INTR_ENA_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_monitor_disable(uint32_t core_id)
{
    REG_CLR_BIT(core_id ? ASSIST_DEBUG_CORE_1_INTR_ENA_REG : ASSIST_DEBUG_CORE_0_INTR_ENA_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_enable(uint32_t core_id)
{
    REG_SET_BIT(core_id ? ASSIST_DEBUG_CORE_1_INTR_RLS_REG : ASSIST_DEBUG_CORE_0_INTR_RLS_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_disable(uint32_t core_id)
{
    REG_CLR_BIT(core_id ? ASSIST_DEBUG_CORE_1_INTR_RLS_REG : ASSIST_DEBUG_CORE_0_INTR_RLS_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR bool assist_debug_ll_sp_spill_is_fired(uint32_t core_id)
{
    return REG_READ(core_id ? ASSIST_DEBUG_CORE_1_INTR_RAW_REG : ASSIST_DEBUG_CORE_0_INTR_RAW_REG) & ASSIST_DEBUG_SP_SPILL_BITS;
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_clear(uint32_t core_id)
{
    REG_WRITE(core_id ? ASSIST_DEBUG_CORE_1_INTR_CLR_REG : ASSIST_DEBUG_CORE_0_INTR_CLR_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_min(uint32_t core_id, uint32_t min)
{
    REG_WRITE(core_id ? ASSIST_DEBUG_CORE_1_SP_MIN_REG : ASSIST_DEBUG_CORE_0_SP_MIN_REG, min);
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_min(uint32_t core_id)
{
    return REG_READ(core_id ? ASSIST_DEBUG_CORE_1_SP_MIN_REG : ASSIST_DEBUG_CORE_0_SP_MIN_REG);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_max(uint32_t core_id, uint32_t max)
{
    REG_WRITE(core_id ? ASSIST_DEBUG_CORE_1_SP_MAX_REG : ASSIST_DEBUG_CORE_0_SP_MAX_REG, max);
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_max(uint32_t core_id)
{
    return REG_READ(core_id ? ASSIST_DEBUG_CORE_1_SP_MAX_REG : ASSIST_DEBUG_CORE_0_SP_MAX_REG);
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_pc(uint32_t core_id)
{
    return REG_READ(core_id ? ASSIST_DEBUG_CORE_1_SP_PC_REG : ASSIST_DEBUG_CORE_0_SP_PC_REG);
}

FORCE_INLINE_ATTR void assist_debug_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl0.reg_busmon_cpu_clk_en = enable;
}
#define assist_debug_ll_enable_bus_clock(...) \
    (void)__DECLARE_RCC_ATOMIC_ENV; assist_debug_ll_enable_bus_clock(__VA_ARGS__)

FORCE_INLINE_ATTR void assist_debug_ll_reset_register(void)
{
    /* esp32p4 has no assist_debug reset register: disable & clear interrupts manually.  */
    for (int i = 0; i < CONFIG_SOC_CPU_CORES_NUM; i++) {
        assist_debug_ll_sp_spill_monitor_disable(i);
        assist_debug_ll_sp_spill_interrupt_clear(i);
        assist_debug_ll_sp_spill_set_min(i, 0);
        assist_debug_ll_sp_spill_set_max(i, 0xffffffff);
        /* Enable PC register storing when trigger stack monitor.  */
        REG_WRITE(i ? ASSIST_DEBUG_CORE_1_RCD_EN_REG : ASSIST_DEBUG_CORE_0_RCD_EN_REG, ASSIST_DEBUG_CORE_1_RCD_PDEBUGEN | ASSIST_DEBUG_CORE_1_RCD_RECORDEN);
    }
}
#define assist_debug_ll_reset_register(...) \
    (void)__DECLARE_RCC_ATOMIC_ENV; assist_debug_ll_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__
