/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "soc/trace_reg.h"
#include "soc/trace_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Return the register block for the given core. */
static inline trace_dev_t *riscv_trace_ll_get_hw(int core)
{
    HAL_ASSERT(core == 0 || core == 1);
    return core == 0 ? &TRACE0 : &TRACE1;
}

/*---------------------------------------------------------------------------
 * Clock and reset
 *--------------------------------------------------------------------------*/

/** @brief Enable or disable the common TRACE CPU and system clocks. */
static inline void riscv_trace_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl0.reg_trace_cpu_clk_en = enable;
    HP_SYS_CLKRST.soc_clk_ctrl0.reg_trace_sys_clk_en = enable;
}

/** @brief Assert and release the reset of the given encoder core. */
static inline void riscv_trace_ll_reset_register(int core)
{
    if (core == 0) {
        HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_coretrace0 = 1;
        HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_coretrace0 = 0;
    } else {
        HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_coretrace1 = 1;
        HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_coretrace1 = 0;
    }
}

/** @brief Enable the per-module register clock gate. */
static inline void riscv_trace_ll_enable_module_clock(trace_dev_t *hw, bool enable)
{
    hw->clock_gate.clk_en = enable;
}

/*---------------------------------------------------------------------------
 * Memory configuration
 *--------------------------------------------------------------------------*/

static inline void riscv_trace_ll_set_mem_start_addr(trace_dev_t *hw, uint32_t addr)
{
    hw->mem_start_addr.mem_start_addr = addr;
}

static inline void riscv_trace_ll_set_mem_end_addr(trace_dev_t *hw, uint32_t addr)
{
    hw->mem_end_addr.mem_end_addr = addr;
}

static inline uint32_t riscv_trace_ll_get_mem_current_addr(trace_dev_t *hw)
{
    return hw->mem_current_addr.mem_current_addr;
}

/** @brief Reload the current address from the start address. */
static inline void riscv_trace_ll_update_mem_current_addr(trace_dev_t *hw)
{
    hw->mem_addr_update.mem_current_addr_update = 1;
}

/*---------------------------------------------------------------------------
 * Status
 *--------------------------------------------------------------------------*/

static inline uint32_t riscv_trace_ll_get_fifo_status(trace_dev_t *hw)
{
    return hw->fifo_status.val;
}

/*---------------------------------------------------------------------------
 * Trigger, loop mode and restart
 *--------------------------------------------------------------------------*/

static inline void riscv_trace_ll_trigger_on(trace_dev_t *hw)
{
    hw->trigger.trigger_on = 1;
}

static inline void riscv_trace_ll_trigger_off(trace_dev_t *hw)
{
    hw->trigger.trigger_off = 1;
}

static inline void riscv_trace_ll_set_mem_loop(trace_dev_t *hw, bool loop)
{
    hw->trigger.mem_loop = loop;
}

static inline void riscv_trace_ll_set_restart_ena(trace_dev_t *hw, bool enable)
{
    hw->trigger.restart_ena = enable;
}

/*---------------------------------------------------------------------------
 * Encoder options (config register)
 *--------------------------------------------------------------------------*/

static inline void riscv_trace_ll_set_full_address(trace_dev_t *hw, bool full)
{
    hw->config.full_address = full;
}

static inline void riscv_trace_ll_set_stall_ena(trace_dev_t *hw, bool enable)
{
    hw->config.stall_ena = enable;
}

static inline void riscv_trace_ll_set_halt_ena(trace_dev_t *hw, bool enable)
{
    hw->config.halt_ena = enable;
}

static inline void riscv_trace_ll_set_reset_ena(trace_dev_t *hw, bool enable)
{
    hw->config.reset_ena = enable;
}

static inline void riscv_trace_ll_set_dm_trigger_ena(trace_dev_t *hw, bool enable)
{
    hw->config.dm_trigger_ena = enable;
}

/*---------------------------------------------------------------------------
 * Resynchronization
 *--------------------------------------------------------------------------*/

static inline void riscv_trace_ll_set_resync_mode(trace_dev_t *hw, uint32_t mode)
{
    hw->resync_prolonged.resync_mode = mode;
}

static inline void riscv_trace_ll_set_resync_threshold(trace_dev_t *hw, uint32_t threshold)
{
    hw->resync_prolonged.resync_prolonged = threshold;
}

/*---------------------------------------------------------------------------
 * AHB configuration
 *--------------------------------------------------------------------------*/

static inline void riscv_trace_ll_set_ahb_burst(trace_dev_t *hw, uint32_t hburst)
{
    hw->ahb_config.hburst = hburst;
}

static inline void riscv_trace_ll_set_ahb_max_incr(trace_dev_t *hw, uint32_t max_incr)
{
    hw->ahb_config.max_incr = max_incr;
}

/*---------------------------------------------------------------------------
 * Interrupts
 *--------------------------------------------------------------------------*/

static inline void riscv_trace_ll_set_intr_ena(trace_dev_t *hw, uint32_t mask)
{
    hw->intr_ena.val = mask;
}

static inline uint32_t riscv_trace_ll_get_intr_raw(trace_dev_t *hw)
{
    return hw->intr_raw.val;
}

static inline void riscv_trace_ll_clear_intr(trace_dev_t *hw, uint32_t mask)
{
    hw->intr_clr.val = mask;
}

/*---------------------------------------------------------------------------
 * Filter unit
 *--------------------------------------------------------------------------*/

static inline void riscv_trace_ll_set_filter_en(trace_dev_t *hw, bool enable)
{
    hw->filter_control.filter_en = enable;
}

static inline void riscv_trace_ll_set_filter_control(trace_dev_t *hw, bool match_comp,
                                                     bool match_privilege, bool match_ecause,
                                                     bool match_interrupt)
{
    hw->filter_control.match_comp = match_comp;
    hw->filter_control.match_privilege = match_privilege;
    hw->filter_control.match_ecause = match_ecause;
    hw->filter_control.match_interrupt = match_interrupt;
}

static inline void riscv_trace_ll_set_filter_match_control(trace_dev_t *hw, bool priv_choice,
                                                           bool intr_value, uint32_t ecause_choice)
{
    hw->filter_match_control.match_choice_privilege = priv_choice;
    hw->filter_match_control.match_value_interrupt = intr_value;
    hw->filter_match_control.match_choice_ecause = ecause_choice;
}

static inline void riscv_trace_ll_set_p_comparator(trace_dev_t *hw, uint32_t input,
                                                   uint32_t function, bool notify)
{
    hw->filter_comparator_control.p_input = input;
    hw->filter_comparator_control.p_function = function;
    hw->filter_comparator_control.p_notify = notify;
}

static inline void riscv_trace_ll_set_s_comparator(trace_dev_t *hw, uint32_t input,
                                                   uint32_t function, bool notify)
{
    hw->filter_comparator_control.s_input = input;
    hw->filter_comparator_control.s_function = function;
    hw->filter_comparator_control.s_notify = notify;
}

static inline void riscv_trace_ll_set_match_mode(trace_dev_t *hw, uint32_t mode)
{
    hw->filter_comparator_control.match_mode = mode;
}

static inline void riscv_trace_ll_set_p_match_value(trace_dev_t *hw, uint32_t value)
{
    hw->filter_p_comparator_match.p_match = value;
}

static inline void riscv_trace_ll_set_s_match_value(trace_dev_t *hw, uint32_t value)
{
    hw->filter_s_comparator_match.s_match = value;
}

#ifdef __cplusplus
}
#endif
