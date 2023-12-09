/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/hal_utils.h"
#include "soc/pcr_struct.h"
#include "soc/parl_io_struct.h"
#include "hal/parlio_types.h"

#define PARLIO_LL_RX_MAX_BYTES_PER_FRAME 0xFFFF
#define PARLIO_LL_RX_MAX_CLK_INT_DIV     0x10000
#define PARLIO_LL_RX_MAX_CLK_FRACT_DIV   0      // Not support fractional divider
#define PARLIO_LL_RX_MAX_TIMEOUT         0xFFFF

#define PARLIO_LL_TX_MAX_BYTES_PER_FRAME 0xFFFF
#define PARLIO_LL_TX_MAX_BITS_PER_FRAME  (PARLIO_LL_TX_MAX_BYTES_PER_FRAME * 8)
#define PARLIO_LL_TX_MAX_CLK_INT_DIV     0x10000
#define PARLIO_LL_TX_MAX_CLK_FRACT_DIV   0      // Not support fractional divider

#define PARLIO_LL_EVENT_TX_FIFO_EMPTY    (1 << 0)
#define PARLIO_LL_EVENT_RX_FIFO_FULL     (1 << 1)
#define PARLIO_LL_EVENT_TX_EOF           (1 << 2)
#define PARLIO_LL_EVENT_TX_MASK          (PARLIO_LL_EVENT_TX_FIFO_EMPTY | PARLIO_LL_EVENT_TX_EOF)
#define PARLIO_LL_EVENT_RX_MASK          (PARLIO_LL_EVENT_RX_FIFO_FULL)

#define PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG 15 // TXD[15] can be used a valid signal

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PARLIO_LL_CLK_SRC_XTAL = PARLIO_CLK_SRC_XTAL,
    PARLIO_LL_CLK_SRC_PLL_F240M = PARLIO_CLK_SRC_PLL_F240M,
    PARLIO_LL_CLK_SRC_PAD, // clock source from GPIO pad
} parlio_ll_clock_source_t;

typedef enum {
    PARLIO_LL_RX_EOF_COND_RX_FULL,     /*!< RX unit generates EOF event when it receives enough data */
    PARLIO_LL_RX_EOF_COND_EN_INACTIVE, /*!< RX unit generates EOF event when the external enable signal becomes inactive */
} parlio_ll_rx_eof_cond_t;

/**
 * @brief Enable or disable the parlio peripheral APB clock
 *
 * @param group_id  The group id of the parlio module
 * @param enable    Set true to enable, false to disable
 */
static inline void parlio_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    PCR.parl_io_conf.parl_clk_en = enable;
}

/**
 * @brief Reset the parlio module
 *
 * @param group_id  The group id of the parlio module
 */
static inline void parlio_ll_reset_register(int group_id)
{
    (void)group_id;
    PCR.parl_io_conf.parl_rst_en = 1;
    PCR.parl_io_conf.parl_rst_en = 0;
}

///////////////////////////////////////RX Unit///////////////////////////////////////

/**
 * @brief Set the clock source for the RX unit
 *
 * @param dev Parallel IO register base address
 * @param src Clock source
 */
static inline void parlio_ll_rx_set_clock_source(parl_io_dev_t *dev, parlio_ll_clock_source_t src)
{
    (void)dev;
    uint32_t clk_sel = 0;
    switch (src) {
    case PARLIO_LL_CLK_SRC_XTAL:
        clk_sel = 0;
        break;
    case PARLIO_LL_CLK_SRC_PLL_F240M:
        clk_sel = 1;
        break;
    case PARLIO_LL_CLK_SRC_PAD:
        clk_sel = 3;
        break;

    default: // unsupported clock source
        HAL_ASSERT(false);
        break;
    }
    PCR.parl_clk_rx_conf.parl_clk_rx_sel = clk_sel;
}

/**
 * @brief Set the clock divider for the RX unit
 *
 * @param dev Parallel IO register base address
 * @param clk_div   Clock division with integral part, no fractional part on C6
 */
static inline void parlio_ll_rx_set_clock_div(parl_io_dev_t *dev, const hal_utils_clk_div_t *clk_div)
{
    (void)dev;
    HAL_ASSERT(clk_div->integer > 0 && clk_div->integer <= PARLIO_LL_RX_MAX_CLK_INT_DIV);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.parl_clk_rx_conf, parl_clk_rx_div_num, clk_div->integer - 1);
}

/**
 * @brief Reset the RX unit Core clock domain
 *
 * @param dev Parallel IO register base address
 */
__attribute__((always_inline))
static inline void parlio_ll_rx_reset_clock(parl_io_dev_t *dev)
{
    (void)dev;
    PCR.parl_clk_rx_conf.parl_rx_rst_en = 1;
    PCR.parl_clk_rx_conf.parl_rx_rst_en = 0;
}

/**
 * @brief Enable the RX unit Core clock domain
 *
 * @param dev Parallel IO register base address
 * @param en True to enable, False to disable
 */
__attribute__((always_inline))
static inline void parlio_ll_rx_enable_clock(parl_io_dev_t *dev, bool en)
{
    (void)dev;
    PCR.parl_clk_rx_conf.parl_clk_rx_en = en;
}

/**
 * @brief Set the condition to generate the RX EOF event
 *
 * @param dev Parallel IO register base address
 * @param cond RX EOF condition
 */
static inline void parlio_ll_rx_set_eof_condition(parl_io_dev_t *dev, parlio_ll_rx_eof_cond_t cond)
{
    dev->rx_cfg0.rx_eof_gen_sel = cond;
}

/**
 * @brief Start RX unit to sample the input data
 *
 * @param dev Parallel IO register base address
 * @param en True to start, False to stop
 */
static inline void parlio_ll_rx_start(parl_io_dev_t *dev, bool en)
{
    dev->rx_cfg0.rx_start = en;
}

/**
 * @brief Set the receive length
 *
 * @note The receive length can be used to generate DMA EOF signal, or to work as a frame end delimiter
 *
 * @param dev Parallel IO register base address
 * @param bitlen Number of bits to receive in the next transaction, bitlen must be a multiple of 8
 */
static inline void parlio_ll_rx_set_recv_bit_len(parl_io_dev_t *dev, uint32_t bitlen)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->rx_cfg0, rx_data_bytelen, bitlen / 8);
}

/**
 * @brief Set the sub mode of the level controlled receive mode
 *
 * @param dev Parallel IO register base address
 * @param active_level Level of the external enable signal, true for active high, false for active low
 */
static inline void parlio_ll_rx_set_level_recv_mode(parl_io_dev_t *dev, bool active_level)
{
    dev->rx_cfg0.rx_smp_mode_sel = 0;
    dev->rx_cfg0.rx_level_submode_sel = !active_level; // 0: active low, 1: active high
}

/**
 * @brief Set the sub mode of the pulse controlled receive mode
 *
 * @param dev Parallel IO register base address
 * @param start_inc Whether the start pulse is counted
 * @param end_inc Whether the end pulse is counted
 * @param end_by_len Whether to use the frame length to determine the end of the frame
 * @param pulse_inv Whether the pulse is inverted
 */
static inline void parlio_ll_rx_set_pulse_recv_mode(parl_io_dev_t *dev, bool start_inc, bool end_inc, bool end_by_len, bool pulse_inv)
{
    uint32_t submode = 0;
    uint32_t step = 1;
    if (end_by_len) {
        submode += 4;
    } else {
        step = 2;
        if (!end_inc) {
            submode += 1;
        }
    }
    if (!start_inc) {
        submode += step;
    }
    if (pulse_inv) {
        submode += 6;
    }
    dev->rx_cfg0.rx_smp_mode_sel = 1;
    dev->rx_cfg0.rx_pulse_submode_sel = submode;
}

/**
 * @brief Set the receive mode to software controlled receive mode
 *
 * @param dev Parallel IO register base address
 */
static inline void parlio_ll_rx_set_soft_recv_mode(parl_io_dev_t *dev)
{
    dev->rx_cfg0.rx_smp_mode_sel = 2;
}

/**
 * @brief Whether to start the software controlled receive mode
 *
 * @param dev Parallel IO register base address
 * @param en True to enable, False to disable
 */
static inline void parlio_ll_rx_start_soft_recv(parl_io_dev_t *dev, bool en)
{
    dev->rx_cfg0.rx_sw_en = en;
}

/**
 * @brief Set the sample clock edge
 *
 * @param dev Parallel IO register base address
 * @param edge Sample clock edge
 */
static inline void parlio_ll_rx_set_sample_clock_edge(parl_io_dev_t *dev, parlio_sample_edge_t edge)
{
    dev->rx_cfg0.rx_clk_edge_sel = edge;
}

/**
 * @brief Set the order to pack bits into one byte
 *
 * @param dev Parallel IO register base address
 * @param order Packing order
 */
static inline void parlio_ll_rx_set_bit_pack_order(parl_io_dev_t *dev, parlio_bit_pack_order_t order)
{
    dev->rx_cfg0.rx_bit_pack_order = order;
}

/**
 * @brief Set the bus width of the RX unit
 *
 * @param dev Parallel IO register base address
 * @param width Bus width
 */
static inline void parlio_ll_rx_set_bus_width(parl_io_dev_t *dev, uint32_t width)
{
    uint32_t width_sel = 0;
    switch (width) {
    case 16:
        width_sel = 0;
        break;
    case 8:
        width_sel = 1;
        break;
    case 4:
        width_sel = 2;
        break;
    case 2:
        width_sel = 3;
        break;
    case 1:
        width_sel = 4;
        break;
    default:
        HAL_ASSERT(false);
    }
    dev->rx_cfg0.rx_bus_wid_sel = width_sel;
}

/**
 * @brief Reset RX Async FIFO
 *
 * @note During the reset of the asynchronous FIFO, it takes two clock cycles to synchronize within AHB clock domain (GDMA) and Core clock domain.
 *       The reset synchronization must be performed two clock cycles in advance.
 * @note If the next frame transfer needs to be reset, you need to first switch to the internal free-running clock,
 *       and then switch to the actual clock after the reset is completed.
 *
 * @param dev Parallel IO register base address
 */
__attribute__((always_inline))
static inline void parlio_ll_rx_reset_fifo(parl_io_dev_t *dev)
{
    dev->rx_cfg0.rx_fifo_srst = 1;
    dev->rx_cfg0.rx_fifo_srst = 0;
}

/**
 * @brief Set which data line as the enable signal
 *
 * @param dev Parallel IO register base address
 * @param line_num Data line number (0-15)
 */
static inline void parlio_ll_rx_treat_data_line_as_en(parl_io_dev_t *dev, uint32_t line_num)
{
    dev->rx_cfg1.rx_ext_en_sel = line_num;
}

/**
 * @brief Enable RX timeout feature
 *
 * @param dev Parallel IO register base address
 * @param en True to enable, False to disable
 */
static inline void parlio_ll_rx_enable_timeout(parl_io_dev_t *dev, bool en)
{
    dev->rx_cfg1.rx_timeout_en = en;
}

/**
 * @brief Set the threshold of RX timeout
 *
 * @param dev Parallel IO register base address
 * @param thres Threshold of RX timeout
 */
static inline void parlio_ll_rx_set_timeout_thres(parl_io_dev_t *dev, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->rx_cfg1, rx_timeout_threshold, thres);
}

/**
 * @brief Update the RX configuration, to make the new configuration take effect
 *
 * @param dev Parallel IO register base address
 */
static inline void parlio_ll_rx_update_config(parl_io_dev_t *dev)
{
    dev->rx_cfg1.rx_reg_update = 1;
    while (dev->rx_cfg1.rx_reg_update);
}

///////////////////////////////////TX Unit///////////////////////////////////////

/**
 * @brief Set the clock source for the TX unit
 *
 * @param dev Parallel IO register base address
 * @param src Clock source
 */
static inline void parlio_ll_tx_set_clock_source(parl_io_dev_t *dev, parlio_ll_clock_source_t src)
{
    (void)dev;
    uint32_t clk_sel = 0;
    switch (src) {
    case PARLIO_LL_CLK_SRC_XTAL:
        clk_sel = 0;
        break;
    case PARLIO_LL_CLK_SRC_PLL_F240M:
        clk_sel = 1;
        break;
    case PARLIO_LL_CLK_SRC_PAD:
        clk_sel = 3;
        break;

    default: // unsupported clock source
        HAL_ASSERT(false);
        break;
    }
    PCR.parl_clk_tx_conf.parl_clk_tx_sel = clk_sel;
}

/**
 * @brief Set the clock divider for the TX unit
 *
 * @param dev Parallel IO register base address
 * @param clk_div   Clock division with integral part, no fractional part on C6
 */
static inline void parlio_ll_tx_set_clock_div(parl_io_dev_t *dev, const hal_utils_clk_div_t *clk_div)
{
    (void)dev;
    HAL_ASSERT(clk_div->integer > 0 && clk_div->integer <= PARLIO_LL_RX_MAX_CLK_INT_DIV);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.parl_clk_tx_conf, parl_clk_tx_div_num, clk_div->integer - 1);
}

/**
 * @brief Reset the TX unit Core clock domain
 *
 * @param dev Parallel IO register base address
 */
__attribute__((always_inline))
static inline void parlio_ll_tx_reset_clock(parl_io_dev_t *dev)
{
    (void)dev;
    PCR.parl_clk_tx_conf.parl_tx_rst_en = 1;
    PCR.parl_clk_tx_conf.parl_tx_rst_en = 0;
}

/**
 * @brief Enable the TX unit Core clock domain
 *
 * @param dev Parallel IO register base address
 * @param en True to enable, False to disable
 */
__attribute__((always_inline))
static inline void parlio_ll_tx_enable_clock(parl_io_dev_t *dev, bool en)
{
    (void)dev;
    PCR.parl_clk_tx_conf.parl_clk_tx_en = en;
}

/**
 * @brief Set the data length to be transmitted
 *
 * @param dev Parallel IO register base address
 * @param bitlen Data length in bits, must be a multiple of 8
 */
__attribute__((always_inline))
static inline void parlio_ll_tx_set_trans_bit_len(parl_io_dev_t *dev, uint32_t bitlen)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->tx_cfg0, tx_bytelen, bitlen / 8);
}

/**
 * @brief Wether to enable the TX clock gating
 *
 * @note The TXD[7] will be taken as the gating enable signal
 *
 * @param dev Parallel IO register base address
 * @param en True to enable, False to disable
 */
static inline void parlio_ll_tx_enable_clock_gating(parl_io_dev_t *dev, bool en)
{
    dev->tx_cfg0.tx_gating_en = en;
}

/**
 * @brief Start TX unit to transmit data
 *
 * @param dev Parallel IO register base address
 * @param en True to start, False to stop
 */
__attribute__((always_inline))
static inline void parlio_ll_tx_start(parl_io_dev_t *dev, bool en)
{
    dev->tx_cfg0.tx_start = en;
}

/**
 * @brief Whether to treat the MSB of TXD as the valid signal
 *
 * @note If enabled, TXD[15] will work as valid signal, which stay high during data transmission.
 *
 * @param dev Parallel IO register base address
 * @param en True to enable, False to disable
 */
static inline void parlio_ll_tx_treat_msb_as_valid(parl_io_dev_t *dev, bool en)
{
    dev->tx_cfg0.tx_hw_valid_en = en;
}

/**
 * @brief Set the sample clock edge
 *
 * @param dev Parallel IO register base address
 * @param edge Sample clock edge
 */
static inline void parlio_ll_tx_set_sample_clock_edge(parl_io_dev_t *dev, parlio_sample_edge_t edge)
{
    dev->tx_cfg0.tx_smp_edge_sel = edge;
}

/**
 * @brief Set the order to unpack bits from a byte
 *
 * @param dev Parallel IO register base address
 * @param order Packing order
 */
static inline void parlio_ll_tx_set_bit_pack_order(parl_io_dev_t *dev, parlio_bit_pack_order_t order)
{
    dev->tx_cfg0.tx_bit_unpack_order = order;
}

/**
 * @brief Set the bus width of the TX unit
 *
 * @param dev Parallel IO register base address
 * @param width Bus width
 */
static inline void parlio_ll_tx_set_bus_width(parl_io_dev_t *dev, uint32_t width)
{
    uint32_t width_sel = 0;
    switch (width) {
    case 16:
        width_sel = 0;
        break;
    case 8:
        width_sel = 1;
        break;
    case 4:
        width_sel = 2;
        break;
    case 2:
        width_sel = 3;
        break;
    case 1:
        width_sel = 4;
        break;
    default:
        HAL_ASSERT(false);
    }
    dev->tx_cfg0.tx_bus_wid_sel = width_sel;
}

/**
 * @brief Reset TX Async FIFO
 *
 * @note During the reset of the asynchronous FIFO, it takes two clock cycles to synchronize within AHB clock domain (GDMA) and Core clock domain.
 *       The reset synchronization must be performed two clock cycles in advance.
 * @note If the next frame transfer needs to be reset, you need to first switch to the internal free-running clock,
 *       and then switch to the actual clock after the reset is completed.
 *
 * @param dev Parallel IO register base address
 */
__attribute__((always_inline))
static inline void parlio_ll_tx_reset_fifo(parl_io_dev_t *dev)
{
    dev->tx_cfg0.tx_fifo_srst = 1;
    dev->tx_cfg0.tx_fifo_srst = 0;
}

/**
 * @brief Set the value to output on the TXD when the TX unit is in IDLE state
 *
 * @param dev Parallel IO register base address
 * @param value Value to output
 */
__attribute__((always_inline))
static inline void parlio_ll_tx_set_idle_data_value(parl_io_dev_t *dev, uint32_t value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->tx_cfg1, tx_idle_value, value);
}

/**
 * @brief Check whether the TX unit is ready
 *
 * @param dev Parallel IO register base address
 * @return true: ready, false: busy
 */
__attribute__((always_inline))
static inline bool parlio_ll_tx_is_ready(parl_io_dev_t *dev)
{
    return dev->st.tx_ready;
}

////////////////////////////////////Interrupt////////////////////////////////////////////////

/**
 * @brief Enable Parallel IO interrupt for specific event mask
 *
 * @param dev Parallel IO register base address
 * @param mask Event mask
 * @param enable True to enable, False to disable
 */
static inline void parlio_ll_enable_interrupt(parl_io_dev_t *dev, uint32_t mask, bool enable)
{
    if (enable) {
        dev->int_ena.val |= mask;
    } else {
        dev->int_ena.val &= ~mask;
    }
}

/**
 * @brief Get interrupt status for TX unit
 *
 * @param dev Parallel IO register base address
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t parlio_ll_tx_get_interrupt_status(parl_io_dev_t *dev)
{
    return dev->int_st.val & PARLIO_LL_EVENT_TX_MASK;
}

/**
 * @brief Get interrupt status for RX unit
 *
 * @param dev Parallel IO register base address
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t parlio_ll_rx_get_interrupt_status(parl_io_dev_t *dev)
{
    return dev->int_st.val & PARLIO_LL_EVENT_RX_MASK;
}

/**
 * @brief Clear Parallel IO interrupt status by mask
 *
 * @param dev Parallel IO register base address
 * @param mask Interrupt status mask
 */
__attribute__((always_inline))
static inline void parlio_ll_clear_interrupt_status(parl_io_dev_t *dev, uint32_t mask)
{
    dev->int_clr.val = mask;
}

/**
 * @brief Get interrupt status register address
 *
 * @param dev Parallel IO register base address
 * @return Register address
 */
static inline volatile void *parlio_ll_get_interrupt_status_reg(parl_io_dev_t *dev)
{
    return &dev->int_st;
}

#ifdef __cplusplus
}
#endif
