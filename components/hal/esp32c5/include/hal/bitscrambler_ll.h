/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "soc/bitscrambler_struct.h"
#include "hal/bitscrambler_types.h"
#include "soc/pcr_struct.h"
#include "soc/hp_system_struct.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BITSCRAMBLER_LL_GET_HW(num)      (((num) == 0) ? (&BITSCRAMBLER) : NULL)

#define BITSCRAMBLER_LL_INST_LEN_WORDS   9 //length of one instruction in 32-bit words as defined by HW

/**
 * @brief Select peripheral BitScrambler is attached to
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param peri Peripheral to select, should pick the value from soc/bitscrambler_peri_select.h
 */
static inline void bitscrambler_ll_select_peripheral(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, int peri)
{
    if (dir == BITSCRAMBLER_DIR_TX) {
        HP_SYSTEM.bitscrambler_peri_sel.bitscrambler_tx_sel = peri;
    } else { // RX
        HP_SYSTEM.bitscrambler_peri_sel.bitscrambler_rx_sel = peri;
    }
}

/**
 * @brief Enable the BitScrambler
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_enable(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    hw->ctrl[dir].ena = 1;
}

/**
 * @brief Disable the BitScrambler
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_disable(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    hw->ctrl[dir].ena = 0;
}


/**
 * @brief Write a word to the instruction memory
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param inst_idx Instruction to write to
 * @param word_idx Word within the instruction to write to
 * @param data Data to write
 */
static inline void bitscrambler_ll_instmem_write(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, int inst_idx, int word_idx, uint32_t data)
{
    hw->inst_cfg[dir].cfg0.inst_idx = inst_idx;
    hw->inst_cfg[dir].cfg0.inst_pos = word_idx;
    hw->inst_cfg[dir].cfg1.inst = data;
}

/**
 * @brief Read a word from the instruction memory
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param inst_idx Instruction to write to
 * @param word_idx Word within the instruction to write to
 *
 * @returns Word read from instruction memory
 */
static inline uint32_t bitscrambler_ll_instmem_read(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, int inst_idx, int word_idx)
{
    hw->inst_cfg[dir].cfg0.inst_idx = inst_idx;
    hw->inst_cfg[dir].cfg0.inst_pos = word_idx;
    return hw->inst_cfg[dir].cfg1.inst;
}

/**
 * @brief Write a word to LUT memory
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param word_idx Word within the LUT to write to
 * @param data Data to write
 */
static inline void bitscrambler_ll_lutmem_write(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, int word_idx, uint32_t data)
{
    hw->lut_cfg[dir].cfg0.lut_idx = word_idx;
    hw->lut_cfg[dir].cfg1.lut = data;
}

/**
 * @brief Set width of LUT memory (as seen by Bitscrambler, not by host)
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param width Width selection
 */
static inline void bitscrambler_ll_set_lut_width(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bitscrambler_lut_width_t width)
{
    hw->lut_cfg[dir].cfg0.lut_mode = width;
}

/**
 * @brief Get width of LUT memory
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline int bitscrambler_ll_get_lut_width(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    return hw->lut_cfg[dir].cfg0.lut_mode;
}

/**
 * @brief Enable loopback mode, where the RX BitScrambler is disabled and the TX BitScrambler loops
 * back to the receive DMA path to memory.
 *
 * @param hw BitScrambler hardware instance address.
 * @param en True if loopback mode is enabled; false otherwise
 */
static inline void bitscrambler_ll_enable_loopback(bitscrambler_dev_t *hw, bool en)
{
    hw->sys.loop_mode = en ? 1 : 0;
}

/**
 * @brief Set condition-checking mode
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param mode Mode to set
 */
static inline void bitscrambler_ll_set_cond_mode(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bitscrambler_cond_mode_t mode)
{
    hw->ctrl[dir].cond_mode = mode;
}

/**
 * @brief Enable prefetch mode
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param en True to enable prefetch mode; false to disable
 */
static inline void bitscrambler_ll_enable_prefetch_on_reset(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bool en)
{
    // 0: means do data prefetch on reset, 1: means no reset prefetch, user has to load the data manually in the assembly code
    hw->ctrl[dir].fetch_mode = en ? 0 : 1;
}

/**
 * @brief Set EOF mode
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param mode Mode to set
 */
static inline void bitscrambler_ll_set_eof_mode(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bitscrambler_eof_mode_t mode)
{
    hw->ctrl[dir].eof_mode = mode;
}

/**
 * @brief Set mode of dummy-reading after EOF
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param mode Mode to set
 */
static inline void bitscrambler_ll_set_dummy_mode(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bitscrambler_dummy_mode_t mode)
{
    hw->ctrl[dir].rd_dummy = mode;
}

/**
 * @brief Set halting mode
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param mode Mode to set
 */
static inline void bitscrambler_ll_set_halt_mode(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bitscrambler_halt_mode_t mode)
{
    hw->ctrl[dir].halt_mode = mode;
}

/**
 * @brief Set amount of bits to ignore after EOF
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param bitcount Number of bits to ignore
 */
static inline void bitscrambler_ll_set_tailing_bits(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, int bitcount)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->tail_bits[dir], tailing_bits, bitcount);
}

/**
 * @brief Reset BitScrambler FIFO
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_reset_fifo(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    hw->ctrl[dir].fifo_rst = 1;
    hw->ctrl[dir].fifo_rst = 0;
}

/**
 * @brief Clear trace of EOF counts
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_clear_eof_trace(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    hw->state[dir].eof_trace_clr = 1;
    hw->state[dir].eof_trace_clr = 0;
}

/**
 * @brief Set state of BitScrambler
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param state One of BITSCRAMBLER_SET_STATE_[RUN|HALT|PAUSE]. Note the WAIT state cannot be set externally.
 */
static inline void bitscrambler_ll_set_state(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bitscrambler_set_state_t state)
{
    hw->ctrl[dir].pause = (state == BITSCRAMBLER_SET_STATE_PAUSE) ? 1 : 0;
    hw->ctrl[dir].halt = (state == BITSCRAMBLER_SET_STATE_HALT) ? 1 : 0;
}

/**
 * @brief Return current BitScrambler state
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 *
 * @returns one of BITSCRAMBLER_STATE_* values
 */
static inline bitscrambler_state_t bitscrambler_ll_get_current_state(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    if (hw->state[dir].in_idle) {
        return BITSCRAMBLER_STATE_IDLE;
    }
    if (hw->state[dir].in_run) {
        return BITSCRAMBLER_STATE_RUN;
    }
    if (hw->state[dir].in_wait) {
        return BITSCRAMBLER_STATE_WAIT;
    }
    if (hw->state[dir].in_pause) {
        return BITSCRAMBLER_STATE_PAUSED;
    }
    return BITSCRAMBLER_STATE_UNKNOWN;
}

/**
 * @brief Return if the bitscrambler FIFO is ready
 *
 * @note For TX, means the outfifo is not empty, then we can start the peripheral to transmit the data
 *       For RX, means the infifo is not full, then we can start the peripheral to receive the data
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @return true if FIFO is ready, false otherwise
 */
static inline bool bitscrambler_ll_is_fifo_ready(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    return hw->state[dir].fifo_empty;
}

/**
 * @brief Enable the bus clock for BitScrambler module
 */
static inline void bitscrambler_ll_set_bus_clock_sys_enable(bool enable)
{
    PCR.bs_conf.bs_clk_en = enable;
}

/**
 * @brief Enable the bus clock for RX BitScrambler module
 */
static inline void bitscrambler_ll_set_bus_clock_rx_enable(bool enable)
{
    // empty
}

/**
 * @brief Enable the bus clock for TX BitScrambler module
 */
static inline void bitscrambler_ll_set_bus_clock_tx_enable(bool enable)
{
    // empty
}

/**
 * @brief Force power on the bitscrambler memory block, regardless of the outside PMU logic
 */
static inline void bitscrambler_ll_mem_force_power_on(void)
{
    PCR.bs_pd_ctrl.bs_mem_force_pu = 1;
    PCR.bs_pd_ctrl.bs_mem_force_pd = 0;
}

/**
 * @brief Force power off the bitscrambler memory block, regardless of the outside PMU logic
 */
static inline void bitscrambler_ll_mem_force_power_off(void)
{
    PCR.bs_pd_ctrl.bs_mem_force_pd = 1;
    PCR.bs_pd_ctrl.bs_mem_force_pu = 0;
}

/**
 * @brief Power control the bitscrambler memory block by the outside PMU logic
 */
static inline void bitscrambler_ll_mem_power_by_pmu(void)
{
    PCR.bs_pd_ctrl.bs_mem_force_pd = 0;
    PCR.bs_pd_ctrl.bs_mem_force_pu = 0;
}

/**
 * @brief Reset the BitScrambler module
 */
static inline void bitscrambler_ll_reset_sys(void)
{
    PCR.bs_conf.bs_rst_en = 1;
    PCR.bs_conf.bs_rst_en = 0;
}

/**
 * @brief Reset the BitScrambler RX module
 */
static inline void bitscrambler_ll_reset_rx(void)
{
    PCR.bs_func_conf.bs_rx_rst_en = 1;
    PCR.bs_func_conf.bs_rx_rst_en = 0;
}

/**
 * @brief Reset the BitScrambler TX module
 */
static inline void bitscrambler_ll_reset_tx(void)
{
    PCR.bs_func_conf.bs_tx_rst_en = 1;
    PCR.bs_func_conf.bs_tx_rst_en = 0;
}

#ifdef __cplusplus
}
#endif
