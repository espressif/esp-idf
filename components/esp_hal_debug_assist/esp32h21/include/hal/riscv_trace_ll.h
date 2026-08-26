/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * ESP32-H21 has a v1.0-spec trace encoder: no filter unit, no encoder config
 * register (delta/full address, stall, halt, reset, debug trigger) and no AHB
 * config register, so the LL only exposes the base register set. Its resync
 * mode field is 1 bit with a different encoding than the v2.0 targets, handled
 * in riscv_trace_ll_set_resync_mode() below.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "soc/trace_reg.h"
#include "soc/trace_struct.h"
#include "soc/pcr_struct.h"
#include "hal/assert.h"
#include "hal/riscv_trace_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Return the register block for the given core. */
static inline trace_dev_t *riscv_trace_ll_get_hw(int core)
{
    HAL_ASSERT(core == 0);
    (void)core;
    return &TRACE;
}

/*---------------------------------------------------------------------------
 * Clock and reset
 *--------------------------------------------------------------------------*/

/** @brief Enable or disable the TRACE clock. */
static inline void riscv_trace_ll_enable_bus_clock(bool enable)
{
    PCR.trace_conf.trace_clk_en = enable;
}

/** @brief Assert and release the reset of the encoder. */
static inline void riscv_trace_ll_reset_register(int core)
{
    (void)core;
    PCR.trace_conf.trace_rst_en = 1;
    PCR.trace_conf.trace_rst_en = 0;
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
 * Resynchronization
 *--------------------------------------------------------------------------*/

/** @brief Whether the resync counter can represent the given mode. */
static inline bool riscv_trace_ll_resync_mode_is_supported(uint32_t mode)
{
    // 1-bit field: this encoder always counts and has no "disabled" mode.
    return mode == RISCV_TRACE_RESYNC_PACKET || mode == RISCV_TRACE_RESYNC_CYCLE;
}

static inline void riscv_trace_ll_set_resync_mode(trace_dev_t *hw, uint32_t mode)
{
    // 1-bit field: 0 = count by cycle, 1 = count by packet.
    hw->resync_prolonged.resync_mode = (mode == RISCV_TRACE_RESYNC_PACKET);
}

static inline void riscv_trace_ll_set_resync_threshold(trace_dev_t *hw, uint32_t threshold)
{
    hw->resync_prolonged.resync_prolonged = threshold;
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

#ifdef __cplusplus
}
#endif
