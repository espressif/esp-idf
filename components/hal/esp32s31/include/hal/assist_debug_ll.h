/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for DEBUG_ASSIST peripheral

#pragma once

#include "soc/assist_debug_reg.h"

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>
#include "esp_attr.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/bus_monitor_struct.h"
#include "soc/lp_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The bus monitor peripheral provides stack overflow/underflow monitoring.
 *
 */

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_monitor_enable(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.core_1_montr_ena.core_1_sp_spill_min_ena = 1;
        BUS_MONITOR.core_1_montr_ena.core_1_sp_spill_max_ena = 1;
    } else {
        BUS_MONITOR.core_0_montr_ena.core_0_sp_spill_min_ena = 1;
        BUS_MONITOR.core_0_montr_ena.core_0_sp_spill_max_ena = 1;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_monitor_disable(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.core_1_montr_ena.core_1_sp_spill_min_ena = 0;
        BUS_MONITOR.core_1_montr_ena.core_1_sp_spill_max_ena = 0;
    } else {
        BUS_MONITOR.core_0_montr_ena.core_0_sp_spill_min_ena = 0;
        BUS_MONITOR.core_0_montr_ena.core_0_sp_spill_max_ena = 0;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_enable(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.core_1_intr_ena.core_1_sp_spill_min_intr_ena = 1;
        BUS_MONITOR.core_1_intr_ena.core_1_sp_spill_max_intr_ena = 1;
    } else {
        BUS_MONITOR.core_0_intr_ena.core_0_sp_spill_min_intr_ena = 1;
        BUS_MONITOR.core_0_intr_ena.core_0_sp_spill_max_intr_ena = 1;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_disable(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.core_1_intr_ena.core_1_sp_spill_min_intr_ena = 0;
        BUS_MONITOR.core_1_intr_ena.core_1_sp_spill_max_intr_ena = 0;
    } else {
        BUS_MONITOR.core_0_intr_ena.core_0_sp_spill_min_intr_ena = 0;
        BUS_MONITOR.core_0_intr_ena.core_0_sp_spill_max_intr_ena = 0;
    }
}

FORCE_INLINE_ATTR bool assist_debug_ll_sp_spill_is_fired(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.core_1_intr_raw.core_1_sp_spill_min_raw || BUS_MONITOR.core_1_intr_raw.core_1_sp_spill_max_raw;
    } else {
        return BUS_MONITOR.core_0_intr_raw.core_0_sp_spill_min_raw || BUS_MONITOR.core_0_intr_raw.core_0_sp_spill_max_raw;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_clear(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.core_1_intr_clr.core_1_sp_spill_min_clr = 1;
        BUS_MONITOR.core_1_intr_clr.core_1_sp_spill_max_clr = 1;
    } else {
        BUS_MONITOR.core_0_intr_clr.core_0_sp_spill_min_clr = 1;
        BUS_MONITOR.core_0_intr_clr.core_0_sp_spill_max_clr = 1;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_min(uint32_t core_id, uint32_t min)
{
    if (core_id) {
        BUS_MONITOR.core_1_sp_min.core_1_sp_min = min;
    } else {
        BUS_MONITOR.core_0_sp_min.core_0_sp_min = min;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_min(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.core_1_sp_min.core_1_sp_min;
    } else {
        return BUS_MONITOR.core_0_sp_min.core_0_sp_min;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_max(uint32_t core_id, uint32_t max)
{
    if (core_id) {
        BUS_MONITOR.core_1_sp_max.core_1_sp_max = max;
    } else {
        BUS_MONITOR.core_0_sp_max.core_0_sp_max = max;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_max(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.core_1_sp_max.core_1_sp_max;
    } else {
        return BUS_MONITOR.core_0_sp_max.core_0_sp_max;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_pc(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.core_1_sp_pc.core_1_sp_pc;
    } else {
        return BUS_MONITOR.core_0_sp_pc.core_0_sp_pc;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_enable_pc_recording(uint32_t core_id, bool enable)
{
    if (core_id) {
        BUS_MONITOR.core_1_rcd_en.core_1_rcd_pdebugen = enable;
        BUS_MONITOR.core_1_rcd_en.core_1_rcd_recorden = enable;
    } else {
        BUS_MONITOR.core_0_rcd_en.core_0_rcd_pdebugen = enable;
        BUS_MONITOR.core_0_rcd_en.core_0_rcd_recorden = enable;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_enable_bus_clock(uint32_t core_id, bool enable)
{
    if (core_id) {
        HP_SYS_CLKRST.busmon_ctrl0.reg_busmon_core1_clk_en = enable;
    } else {
        HP_SYS_CLKRST.busmon_ctrl0.reg_busmon_core0_clk_en = enable;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_reset_register(uint32_t core_id)
{
    if (core_id) {
        HP_SYS_CLKRST.busmon_ctrl0.reg_busmon_core1_rst_en = true;
        HP_SYS_CLKRST.busmon_ctrl0.reg_busmon_core1_rst_en = false;
    } else {
        HP_SYS_CLKRST.busmon_ctrl0.reg_busmon_core0_rst_en = true;
        HP_SYS_CLKRST.busmon_ctrl0.reg_busmon_core0_rst_en = false;
    }
}

FORCE_INLINE_ATTR bool assist_debug_ll_is_debugger_active(void)
{
    return BUS_MONITOR.core_0_debug_mode.core_0_debug_module_active;
}

FORCE_INLINE_ATTR void assist_debug_ll_lockup_monitor_enable(uint32_t core_id, bool enable)
{
    if (core_id) {
        BUS_MONITOR.core_1_montr_ena.core_1_trace_lockup_ena = enable;
    } else {
        BUS_MONITOR.core_0_montr_ena.core_0_trace_lockup_ena = enable;
    }
}

// trap_idx: 0 = latest trap, 1 = previous trap
// MCause for the exception
FORCE_INLINE_ATTR uint32_t assist_debug_ll_lockup_get_cause(uint32_t core_id, int trap_idx)
{
    if (core_id) {
        return trap_idx ? BUS_MONITOR.core_1_trace_lockup_cause_1.core_1_trace_lockup_recording_cause_1
               : BUS_MONITOR.core_1_trace_lockup_cause_0.core_1_trace_lockup_recording_cause_0;
    } else {
        return trap_idx ? BUS_MONITOR.core_0_trace_lockup_cause_1.core_0_trace_lockup_recording_cause_1
               : BUS_MONITOR.core_0_trace_lockup_cause_0.core_0_trace_lockup_recording_cause_0;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_lockup_get_tval(uint32_t core_id, int trap_idx)
{
    if (core_id) {
        return trap_idx ? BUS_MONITOR.core_1_trace_lockup_tval_1.core_1_trace_lockup_recording_tval_1
               : BUS_MONITOR.core_1_trace_lockup_tval_0.core_1_trace_lockup_recording_tval_0;
    } else {
        return trap_idx ? BUS_MONITOR.core_0_trace_lockup_tval_1.core_0_trace_lockup_recording_tval_1
               : BUS_MONITOR.core_0_trace_lockup_tval_0.core_0_trace_lockup_recording_tval_0;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_lockup_get_iaddr(uint32_t core_id, int trap_idx)
{
    if (core_id) {
        return trap_idx ? BUS_MONITOR.core_1_trace_lockup_iaddr_1.core_1_trace_lockup_recording_iaddr_1
               : BUS_MONITOR.core_1_trace_lockup_iaddr_0.core_1_trace_lockup_recording_iaddr_0;
    } else {
        return trap_idx ? BUS_MONITOR.core_0_trace_lockup_iaddr_1.core_0_trace_lockup_recording_iaddr_1
               : BUS_MONITOR.core_0_trace_lockup_iaddr_0.core_0_trace_lockup_recording_iaddr_0;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_lockup_get_priv(uint32_t core_id, int trap_idx)
{
    if (core_id) {
        return trap_idx ? BUS_MONITOR.core_1_trace_lockup_priv_1.core_1_trace_lockup_recording_priv_1
               : BUS_MONITOR.core_1_trace_lockup_priv_0.core_1_trace_lockup_recording_priv_0;
    } else {
        return trap_idx ? BUS_MONITOR.core_0_trace_lockup_priv_1.core_0_trace_lockup_recording_priv_1
               : BUS_MONITOR.core_0_trace_lockup_priv_0.core_0_trace_lockup_recording_priv_0;
    }
}

/* Enable the hardware lockup reset for the given core via LP_CLKRST.
 * This controls whether a CPU lockup (exception inside exception handler) triggers
 * a system reset. Defaults to 0 on S31 so must be explicitly set. */
FORCE_INLINE_ATTR void assist_debug_ll_lockup_reset_enable(uint32_t core_id)
{
    if (core_id) {
        LP_CLKRST.hpcore1_reset_ctrl.hpcore1_lockup_reset_en = 1;
    } else {
        LP_CLKRST.hpcore0_reset_ctrl.hpcore0_lockup_reset_en = 1;
    }
}

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__
