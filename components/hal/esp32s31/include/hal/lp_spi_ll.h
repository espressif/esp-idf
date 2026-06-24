/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use it in application code.
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/lp_spi_struct.h"
#include "soc/lp_peri_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef lp_dev_t lp_spi_ll_dev_t;

#define LP_SPI_LL_GET_HW() (&LP_SPI)
#define LP_SPI_LL_MAX_BUFFER_SIZE 64

/**
 * @brief Enable the LP SPI peripheral clock gate
 */
static inline void lp_spi_ll_enable_clock(lp_spi_ll_dev_t *hw)
{
    LP_PERI_CLKRST.spi_ctrl.lp_spi_clk_en = 1;
    hw->spi_clk_gate.reg_clk_en = 1;
    hw->spi_clk_gate.reg_mst_clk_active = 1;
}

/**
 * @brief Write a 32-bit word to data buffer register at index n
 */
static inline void lp_spi_ll_write_buffer_word(lp_spi_ll_dev_t *hw, int n, uint32_t val)
{
    hw->data_buf[n].val = val;
}

/**
 * @brief Read a 32-bit word from data buffer register at index n
 */
static inline uint32_t lp_spi_ll_read_buffer_word(lp_spi_ll_dev_t *hw, int n)
{
    return hw->data_buf[n].val;
}

/**
 * @brief Reset RX and TX AFIFOs
 */
static inline void lp_spi_ll_reset_fifos(lp_spi_ll_dev_t *hw)
{
    hw->spi_dma_conf.reg_rx_afifo_rst = 1;
    hw->spi_dma_conf.reg_rx_afifo_rst = 0;
    hw->spi_dma_conf.reg_buf_afifo_rst = 1;
    hw->spi_dma_conf.reg_buf_afifo_rst = 0;
}

/**
 * @brief Clear the transaction-done interrupt
 */
static inline void lp_spi_ll_clear_int_trans_done(lp_spi_ll_dev_t *hw)
{
    hw->spi_dma_int_clr.reg_trans_done_int_clr = 1;
}

/**
 * @brief Return true if a transaction is currently in progress
 */
static inline bool lp_spi_ll_is_busy(lp_spi_ll_dev_t *hw)
{
    return hw->spi_cmd.reg_usr != 0;
}

/**
 * @brief Return true if the transaction-done interrupt raw bit is set
 */
static inline bool lp_spi_ll_get_int_trans_done(lp_spi_ll_dev_t *hw)
{
    return hw->spi_dma_int_raw.reg_trans_done_int_raw != 0;
}

/**
 * @brief Enable or disable the dummy phase
 */
static inline void lp_spi_ll_set_dummy_en(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_user.reg_usr_dummy = enable ? 1 : 0;
}

/**
 * @brief Set the number of dummy cycles (value = cycles - 1)
 */
static inline void lp_spi_ll_set_dummy_cyclelen(lp_spi_ll_dev_t *hw, uint32_t cyclelen)
{
    hw->spi_user1.reg_usr_dummy_cyclelen = cyclelen - 1;
}

/**
 * @brief Enable or disable the command phase
 */
static inline void lp_spi_ll_set_command_en(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_user.reg_usr_command = enable ? 1 : 0;
}

/**
 * @brief Set the command bit length (value = bits - 1)
 */
static inline void lp_spi_ll_set_command_bitlen(lp_spi_ll_dev_t *hw, uint32_t bitlen)
{
    hw->spi_user2.reg_usr_command_bitlen = bitlen - 1;
}

/**
 * @brief Set the command value
 */
static inline void lp_spi_ll_set_command_value(lp_spi_ll_dev_t *hw, uint32_t value)
{
    hw->spi_user2.reg_usr_command_value = value;
}

/**
 * @brief Enable or disable the address phase
 */
static inline void lp_spi_ll_set_address_en(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_user.reg_usr_addr = enable ? 1 : 0;
}

/**
 * @brief Set the address bit length (value = bits - 1)
 */
static inline void lp_spi_ll_set_address_bitlen(lp_spi_ll_dev_t *hw, uint32_t bitlen)
{
    hw->spi_user1.reg_usr_addr_bitlen = bitlen;
}

/**
 * @brief Set the address value
 */
static inline void lp_spi_ll_set_address_value(lp_spi_ll_dev_t *hw, uint32_t value)
{
    hw->spi_addr.reg_usr_addr_value = value;
}

/**
 * @brief Enable or disable the MOSI (write-data) phase
 */
static inline void lp_spi_ll_set_mosi_en(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_user.reg_usr_mosi = enable ? 1 : 0;
}

/**
 * @brief Enable or disable the MISO (read-data) phase
 */
static inline void lp_spi_ll_set_miso_en(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_user.reg_usr_miso = enable ? 1 : 0;
}

/**
 * @brief Set the data bit length for master/slave transfers (value = bits - 1)
 */
static inline void lp_spi_ll_set_data_bitlen(lp_spi_ll_dev_t *hw, uint32_t bitlen)
{
    hw->spi_ms_dlen.reg_ms_data_bitlen = bitlen;
}

/**
 * @brief Trigger a configuration update (master mode, synchronises APB->SPI domain)
 */
static inline void lp_spi_ll_apply_config(lp_spi_ll_dev_t *hw)
{
    hw->spi_cmd.reg_update = 1;
    while (hw->spi_cmd.reg_update) {
        ;
    }
}

/**
 * @brief Start a user-defined SPI transaction
 */
static inline void lp_spi_ll_start_user_transaction(lp_spi_ll_dev_t *hw)
{
    hw->spi_cmd.reg_usr = 1;
}

/**
 * @brief Get the number of bits received during the last slave transfer
 */
static inline uint32_t lp_spi_ll_get_slave_rcv_bitlen(lp_spi_ll_dev_t *hw)
{
    return hw->spi_slave1.reg_slv_data_bitlen;
}

/**
 * @brief Return true if wr_bit_order (MOSI LSB-first) is set
 */
static inline bool lp_spi_ll_get_wr_bit_order(lp_spi_ll_dev_t *hw)
{
    return hw->spi_ctrl.reg_wr_bit_order != 0;
}

/**
 * @brief Set MOSI/MISO bit order (0 = MSB first, 1 = LSB first)
 */
static inline void lp_spi_ll_set_bit_order(lp_spi_ll_dev_t *hw, bool rd_lsb_first, bool wr_lsb_first)
{
    hw->spi_ctrl.reg_rd_bit_order = rd_lsb_first ? 1 : 0;
    hw->spi_ctrl.reg_wr_bit_order = wr_lsb_first ? 1 : 0;
}

/**
 * @brief Set SPI clock polarity (CPOL) idle edge
 */
static inline void lp_spi_ll_set_ck_idle_edge(lp_spi_ll_dev_t *hw, bool idle_high)
{
    hw->spi_misc.reg_ck_idle_edge = idle_high ? 1 : 0;
}

/**
 * @brief Set clock-out edge (used with CPOL/CPHA in master mode)
 */
static inline void lp_spi_ll_set_ck_out_edge(lp_spi_ll_dev_t *hw, bool edge)
{
    hw->spi_user.reg_ck_out_edge = edge ? 1 : 0;
}

/**
 * @brief Set slave mode rising/falling clock edge for Rx and Tx sampling
 */
static inline void lp_spi_ll_set_slave_clk_edges(lp_spi_ll_dev_t *hw, bool rsck_i_edge, bool tsck_i_edge)
{
    hw->spi_user.reg_rsck_i_edge = rsck_i_edge ? 1 : 0;
    hw->spi_user.reg_tsck_i_edge = tsck_i_edge ? 1 : 0;
}

/**
 * @brief Set slave clock mode 1/3 support bit
 */
static inline void lp_spi_ll_set_slave_clk_mode_13(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_slave.reg_clk_mode_13 = enable ? 1 : 0;
}

/**
 * @brief Set master CS polarity (active high / active low)
 */
static inline void lp_spi_ll_set_master_cs_pol(lp_spi_ll_dev_t *hw, bool active_high)
{
    hw->spi_misc.reg_master_cs_pol = active_high ? 1 : 0;
}

/**
 * @brief Set slave CS polarity (0 = active low, 1 = inverted)
 */
static inline void lp_spi_ll_set_slave_cs_pol(lp_spi_ll_dev_t *hw, bool inverted)
{
    hw->spi_misc.reg_slave_cs_pol = inverted ? 1 : 0;
}

/**
 * @brief Enable or disable full-duplex mode (doutdin)
 */
static inline void lp_spi_ll_set_full_duplex(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_user.reg_doutdin = enable ? 1 : 0;
}

/**
 * @brief Enable or disable 3-wire half-duplex (SIO) mode
 */
static inline void lp_spi_ll_set_sio_mode(lp_spi_ll_dev_t *hw, bool enable)
{
    hw->spi_user.reg_sio = enable ? 1 : 0;
}

/**
 * @brief Configure CS setup (pre-transaction) timing
 */
static inline void lp_spi_ll_set_cs_setup(lp_spi_ll_dev_t *hw, bool enable, uint32_t setup_time)
{
    hw->spi_user.reg_cs_setup = enable ? 1 : 0;
    hw->spi_user1.reg_cs_setup_time = setup_time;
}

/**
 * @brief Configure CS hold (post-transaction) timing
 */
static inline void lp_spi_ll_set_cs_hold(lp_spi_ll_dev_t *hw, bool enable, uint32_t hold_time)
{
    hw->spi_user.reg_cs_hold = enable ? 1 : 0;
    hw->spi_user1.reg_cs_hold_time = hold_time;
}

/**
 * @brief Enable CS0 (disable the CS0_DIS bit)
 */
static inline void lp_spi_ll_enable_cs0(lp_spi_ll_dev_t *hw)
{
    hw->spi_misc.reg_cs0_dis = 0;
}

/**
 * @brief Disable MOSI/MISO high-part buffer access
 */
static inline void lp_spi_ll_disable_highpart(lp_spi_ll_dev_t *hw)
{
    hw->spi_user.reg_usr_mosi_highpart = 0;
    hw->spi_user.reg_usr_miso_highpart = 0;
}

/**
 * @brief Set slave mode enable bit
 */
static inline void lp_spi_ll_set_slave_mode(lp_spi_ll_dev_t *hw, bool slave)
{
    hw->spi_slave.reg_slave_mode = slave ? 1 : 0;
}

/**
 * @brief Set slave clock mode (clk_mode field)
 */
static inline void lp_spi_ll_set_slave_clk_mode(lp_spi_ll_dev_t *hw, uint32_t clk_mode)
{
    hw->spi_slave.reg_clk_mode = clk_mode;
}

/**
 * @brief Issue a software reset of the SPI peripheral
 */
static inline void lp_spi_ll_soft_reset(lp_spi_ll_dev_t *hw)
{
    hw->spi_slave.reg_soft_reset = 1;
    hw->spi_slave.reg_soft_reset = 0;
}

/**
 * @brief Write the raw clock register value (used with spi_ll_master_cal_clock output)
 */
static inline void lp_spi_ll_set_clock_val(lp_spi_ll_dev_t *hw, uint32_t clock_val)
{
    hw->spi_clock.val = clock_val;
}

/**
 * @brief Zero out the clock, user, and ctrl registers (slave init)
 */
static inline void lp_spi_ll_reset_slave_regs(lp_spi_ll_dev_t *hw)
{
    hw->spi_clock.val = 0;
    hw->spi_user.val = 0;
    hw->spi_ctrl.val = 0;
}

/**
 * @brief Reset CS timing registers in master mode
 */
static inline void lp_spi_ll_reset_cs_timing(lp_spi_ll_dev_t *hw)
{
    hw->spi_user1.reg_cs_setup_time = 0;
    hw->spi_user1.reg_cs_hold_time = 0;
}

#ifdef __cplusplus
}
#endif
