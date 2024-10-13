/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/pcr_struct.h"

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
 *      // ...same for sp_spill_min and other things debug_assist can check.
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

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_monitor_enable(__attribute__((unused)) uint32_t core_id)
{
    REG_SET_BIT(ASSIST_DEBUG_CORE_0_INTR_ENA_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_monitor_disable(__attribute__((unused)) uint32_t core_id)
{
    REG_CLR_BIT(ASSIST_DEBUG_CORE_0_INTR_ENA_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_enable(__attribute__((unused)) uint32_t core_id)
{
    REG_SET_BIT(ASSIST_DEBUG_CORE_0_INTR_RLS_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_disable(__attribute__((unused)) uint32_t core_id)
{
    REG_CLR_BIT(ASSIST_DEBUG_CORE_0_INTR_RLS_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR bool assist_debug_ll_sp_spill_is_fired(__attribute__((unused)) uint32_t core_id)
{
    return REG_READ(ASSIST_DEBUG_CORE_0_INTR_RAW_REG) & ASSIST_DEBUG_SP_SPILL_BITS;
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_clear(__attribute__((unused)) uint32_t core_id)
{
    REG_WRITE(ASSIST_DEBUG_CORE_0_INTR_CLR_REG, ASSIST_DEBUG_SP_SPILL_BITS);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_min(__attribute__((unused)) uint32_t core_id, uint32_t min)
{
    REG_WRITE(ASSIST_DEBUG_CORE_0_SP_MIN_REG, min);
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_min(__attribute__((unused)) uint32_t core_id)
{
    return REG_READ(ASSIST_DEBUG_CORE_0_SP_MIN_REG);
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_max(__attribute__((unused)) uint32_t core_id, uint32_t max)
{
    REG_WRITE(ASSIST_DEBUG_CORE_0_SP_MAX_REG, max);
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_max(__attribute__((unused)) uint32_t core_id)
{
    return REG_READ(ASSIST_DEBUG_CORE_0_SP_MAX_REG);
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_pc(__attribute__((unused)) uint32_t core_id)
{
    return REG_READ(ASSIST_DEBUG_CORE_0_SP_PC_REG);
}

FORCE_INLINE_ATTR void assist_debug_ll_enable_bus_clock(bool enable)
{
    PCR.assist_conf.assist_clk_en = enable;
}

FORCE_INLINE_ATTR void assist_debug_ll_reset_register(void)
{
    PCR.assist_conf.assist_rst_en = true;
    PCR.assist_conf.assist_rst_en = false;
}

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__
