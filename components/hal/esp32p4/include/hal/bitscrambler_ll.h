/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "soc/bitscrambler_struct.h"
#include "hal/bitscrambler_types.h"
#include "hal/bitscrambler_hal.h"
#include "hal/misc.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/hp_system_struct.h"
#include "hal/hal_utils.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BITSCRAMBLER_LL_GET_HW(num)      (((num) == 0) ? (&BITSCRAMBLER) : NULL)

//Note: Most of the register operations refer tx_something fields. Because they
//are done to bitscramblers_reg[dir] members and rx register fields are the
//same, this code works for RX as well.

//Note: This code entirely ignores the 'hw' argument, which is fine, as for now
//we only have one BitScrambler peripheral. If we have a chip that has more, we
//need to do something to use hw to select one of multiple bitscramblers_reg
//versions.

/**
 * @brief Select peripheral BitScrambler is attached to
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_select_peripheral(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, int peri)
{
    if (dir == BITSCRAMBLER_DIR_TX) {
        HP_SYSTEM.bitscrambler_peri_sel.bitscrambler_peri_tx_sel = peri;
    } else { // RX
        HP_SYSTEM.bitscrambler_peri_sel.bitscrambler_peri_rx_sel = peri;
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
    bitscramblers_reg[dir].ctrl->tx_ena=1;
}

/**
 * @brief Disable the BitScrambler
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_disable(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    bitscramblers_reg[dir].ctrl->tx_ena=0;
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
    bitscramblers_reg[dir].inst_cfg0->tx_inst_idx=inst_idx;
    bitscramblers_reg[dir].inst_cfg0->tx_inst_pos=word_idx;
    bitscramblers_reg[dir].inst_cfg1->tx_inst=data;
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
    bitscramblers_reg[dir].inst_cfg0->tx_inst_idx=inst_idx;
    bitscramblers_reg[dir].inst_cfg0->tx_inst_pos=word_idx;
    return bitscramblers_reg[dir].inst_cfg1->tx_inst;
}

/**
 * @brief Write a word to LUT memory
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param word_idx Word within the LUT to write to
 * @param data Data to write
 */
static inline void bitscrambler_ll_lutmem_write(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, int word_idx, uint32_t data) {
    bitscramblers_reg[dir].lut_cfg0->tx_lut_idx=word_idx;
    bitscramblers_reg[dir].lut_cfg1->tx_lut=data;
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
    bitscramblers_reg[dir].lut_cfg0->tx_lut_mode=width;
}

/**
 * @brief Get width of LUT memory
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline int bitscrambler_ll_get_lut_width(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    return bitscramblers_reg[dir].lut_cfg0->tx_lut_mode;
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
    hw->sys.loop_mode=en?1:0;
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
    bitscramblers_reg[dir].ctrl->tx_cond_mode=mode;
}

/**
 * @brief Set prefetch mode
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 * @param mode Mode to set
 */
static inline void bitscrambler_ll_set_prefetch_mode(bitscrambler_dev_t *hw, bitscrambler_direction_t dir, bitscrambler_prefetch_mode_t mode)
{
    bitscramblers_reg[dir].ctrl->tx_fetch_mode=mode;
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
    bitscramblers_reg[dir].ctrl->tx_eof_mode=mode;
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
    bitscramblers_reg[dir].ctrl->tx_rd_dummy=mode;
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
    bitscramblers_reg[dir].ctrl->tx_halt_mode=mode;
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
    HAL_FORCE_MODIFY_U32_REG_FIELD((*bitscramblers_reg[dir].trailing_bits), tx_tailing_bits, bitcount);
}

/**
 * @brief Reset BitScrambler FIFO
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_reset_fifo(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    bitscramblers_reg[dir].ctrl->tx_fifo_rst=1;
    bitscramblers_reg[dir].ctrl->tx_fifo_rst=0;
}

/**
 * @brief Clear trace of EOF counts
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 */
static inline void bitscrambler_ll_clear_eof_trace(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    bitscramblers_reg[dir].state->tx_eof_trace_clr=1;
    bitscramblers_reg[dir].state->tx_eof_trace_clr=0;
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
    bitscramblers_reg[dir].ctrl->tx_pause=(state==BITSCRAMBLER_SET_STATE_PAUSE)?1:0;
    bitscramblers_reg[dir].ctrl->tx_halt=(state==BITSCRAMBLER_SET_STATE_HALT)?1:0;
}

/**
 * @brief Return current BitScrambler state
 *
 * @param hw BitScrambler hardware instance address.
 * @param dir Direction, BITSCRAMBLER_DIR_TX or BITSCRAMBLER_DIR_RX
 *
 * @returns one of BITSCRAMBLER_STATE_* values
 */
static inline bitscrambler_state_t bitscrambler_ll_current_state(bitscrambler_dev_t *hw, bitscrambler_direction_t dir)
{
    if (bitscramblers_reg[dir].state->tx_in_idle) return BITSCRAMBLER_STATE_IDLE;
    if (bitscramblers_reg[dir].state->tx_in_run) return BITSCRAMBLER_STATE_RUN;
    if (bitscramblers_reg[dir].state->tx_in_wait) return BITSCRAMBLER_STATE_WAIT;
    if (bitscramblers_reg[dir].state->tx_in_pause) return BITSCRAMBLER_STATE_PAUSED;
    return BITSCRAMBLER_STATE_UNKNOWN;
}

/**
 * @brief Enable the bus clock for BitScrambler module
 */
static inline void _bitscrambler_ll_set_bus_clock_sys_enable(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_bitscrambler_sys_clk_en = enable;
}

/**
 * @brief Enable the bus clock for RX BitScrambler module
 */
static inline void _bitscrambler_ll_set_bus_clock_rx_enable(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_bitscrambler_rx_sys_clk_en = enable;
}

/**
 * @brief Enable the bus clock for TX BitScrambler module
 */
static inline void _bitscrambler_ll_set_bus_clock_tx_enable(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_bitscrambler_tx_sys_clk_en = enable;
}

/**
 * @brief Reset the BitScrambler module
 */
static inline void _bitscrambler_ll_reset_sys(void)
{
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_bitscrambler = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_bitscrambler = 0;
}

/**
 * @brief Reset the BitScrambler RX module
 */
static inline void _bitscrambler_ll_reset_rx(void)
{
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_bitscrambler_rx = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_bitscrambler_rx = 0;
}

/**
 * @brief Reset the BitScrambler TX module
 */
static inline void _bitscrambler_ll_reset_tx(void)
{
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_bitscrambler_tx = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_bitscrambler_tx = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define bitscrambler_ll_set_bus_clock_sys_enable(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _bitscrambler_ll_set_bus_clock_sys_enable(__VA_ARGS__)
#define bitscrambler_ll_set_bus_clock_rx_enable(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _bitscrambler_ll_set_bus_clock_rx_enable(__VA_ARGS__)
#define bitscrambler_ll_set_bus_clock_tx_enable(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _bitscrambler_ll_set_bus_clock_tx_enable(__VA_ARGS__)

#define bitscrambler_ll_reset_sys(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _bitscrambler_ll_reset_sys(__VA_ARGS__)
#define bitscrambler_ll_reset_rx(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _bitscrambler_ll_reset_rx(__VA_ARGS__)
#define bitscrambler_ll_reset_tx(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _bitscrambler_ll_reset_tx(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
