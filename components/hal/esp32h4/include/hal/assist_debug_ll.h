/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/pcr_struct.h"
#include "soc/bus_monitor_struct.h"

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
        BUS_MONITOR.monitor_core_1_montr_ena.monitor_core_1_sp_spill_min_ena = 1;
        BUS_MONITOR.monitor_core_1_montr_ena.monitor_core_1_sp_spill_max_ena = 1;
    } else {
        BUS_MONITOR.monitor_core_0_montr_ena.monitor_core_0_sp_spill_min_ena = 1;
        BUS_MONITOR.monitor_core_0_montr_ena.monitor_core_0_sp_spill_max_ena = 1;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_monitor_disable(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.monitor_core_1_montr_ena.monitor_core_1_sp_spill_min_ena = 0;
        BUS_MONITOR.monitor_core_1_montr_ena.monitor_core_1_sp_spill_max_ena = 0;
    } else {
        BUS_MONITOR.monitor_core_0_montr_ena.monitor_core_0_sp_spill_min_ena = 0;
        BUS_MONITOR.monitor_core_0_montr_ena.monitor_core_0_sp_spill_max_ena = 0;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_enable(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.monitor_core_1_intr_ena.monitor_core_1_sp_spill_min_intr_ena = 1;
        BUS_MONITOR.monitor_core_1_intr_ena.monitor_core_1_sp_spill_max_intr_ena = 1;
    } else {
        BUS_MONITOR.monitor_core_0_intr_ena.monitor_core_0_sp_spill_min_intr_ena = 1;
        BUS_MONITOR.monitor_core_0_intr_ena.monitor_core_0_sp_spill_max_intr_ena = 1;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_disable(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.monitor_core_1_intr_ena.monitor_core_1_sp_spill_min_intr_ena = 0;
        BUS_MONITOR.monitor_core_1_intr_ena.monitor_core_1_sp_spill_max_intr_ena = 0;
    } else {
        BUS_MONITOR.monitor_core_0_intr_ena.monitor_core_0_sp_spill_min_intr_ena = 0;
        BUS_MONITOR.monitor_core_0_intr_ena.monitor_core_0_sp_spill_max_intr_ena = 0;
    }
}

FORCE_INLINE_ATTR bool assist_debug_ll_sp_spill_is_fired(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.monitor_core_1_intr_raw.monitor_core_1_sp_spill_min_raw || BUS_MONITOR.monitor_core_1_intr_raw.monitor_core_1_sp_spill_max_raw;
    } else {
        return BUS_MONITOR.monitor_core_0_intr_raw.monitor_core_0_sp_spill_min_raw || BUS_MONITOR.monitor_core_0_intr_raw.monitor_core_0_sp_spill_max_raw;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_interrupt_clear(uint32_t core_id)
{
    if (core_id) {
        BUS_MONITOR.monitor_core_1_intr_clr.monitor_core_1_sp_spill_min_clr = 1;
        BUS_MONITOR.monitor_core_1_intr_clr.monitor_core_1_sp_spill_max_clr = 1;
    } else {
        BUS_MONITOR.monitor_core_0_intr_clr.monitor_core_0_sp_spill_min_clr = 1;
        BUS_MONITOR.monitor_core_0_intr_clr.monitor_core_0_sp_spill_max_clr = 1;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_min(uint32_t core_id, uint32_t min)
{
    if (core_id) {
        BUS_MONITOR.monitor_core_1_sp_min.monitor_core_1_sp_min = min;
    } else {
        BUS_MONITOR.monitor_core_0_sp_min.monitor_core_0_sp_min = min;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_min(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.monitor_core_1_sp_min.monitor_core_1_sp_min;
    } else {
        return BUS_MONITOR.monitor_core_0_sp_min.monitor_core_0_sp_min;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_sp_spill_set_max(uint32_t core_id, uint32_t max)
{
    if (core_id) {
        BUS_MONITOR.monitor_core_1_sp_max.monitor_core_1_sp_max = max;
    } else {
        BUS_MONITOR.monitor_core_0_sp_max.monitor_core_0_sp_max = max;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_max(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.monitor_core_1_sp_max.monitor_core_1_sp_max;
    } else {
        return BUS_MONITOR.monitor_core_0_sp_max.monitor_core_0_sp_max;
    }
}

FORCE_INLINE_ATTR uint32_t assist_debug_ll_sp_spill_get_pc(uint32_t core_id)
{
    if (core_id) {
        return BUS_MONITOR.monitor_core_1_sp_pc.monitor_core_1_sp_pc;
    } else {
        return BUS_MONITOR.monitor_core_0_sp_pc.monitor_core_0_sp_pc;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_enable_pc_recording(uint32_t core_id, bool enable)
{
    if (core_id) {
        BUS_MONITOR.monitor_core_1_rcd_en.monitor_core_1_rcd_pdebugen = enable;
        BUS_MONITOR.monitor_core_1_rcd_en.monitor_core_1_rcd_recorden = enable;
    } else {
        BUS_MONITOR.monitor_core_0_rcd_en.monitor_core_0_rcd_pdebugen = enable;
        BUS_MONITOR.monitor_core_0_rcd_en.monitor_core_0_rcd_recorden = enable;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_enable_bus_clock(uint32_t core_id, bool enable)
{
    if (core_id) {
        PCR.assist_conf.assist1_clk_en = enable;
    } else {
        PCR.assist_conf.assist_clk_en = enable;
    }
}

FORCE_INLINE_ATTR void assist_debug_ll_reset_register(uint32_t core_id)
{
    if (core_id) {
        PCR.assist_conf.assist1_rst_en = true;
        PCR.assist_conf.assist1_rst_en = false;
    } else {
        PCR.assist_conf.assist_rst_en = true;
        PCR.assist_conf.assist_rst_en = false;
    }
}

FORCE_INLINE_ATTR bool assist_debug_ll_is_debugger_active(void)
{
    return BUS_MONITOR.monitor_core_0_debug_mode.monitor_core_0_debug_module_active;
}

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__
