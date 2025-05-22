/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h> /* Required for NULL constant */
#include <stdint.h>
#include <stdbool.h>
#include "hal/dma2d_types.h"
#include "soc/dma2d_channel.h"
#include "soc/dma2d_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DMA2D_LL_GET_HW(id)                  (((id) == 0) ? (&DMA2D) : NULL)

// 2D-DMA interrupts
#define DMA2D_LL_RX_EVENT_MASK               (0x3FFF)
#define DMA2D_LL_TX_EVENT_MASK               (0x1FFF)

#define DMA2D_LL_EVENT_RX_DESC_TASK_OVF      (1<<13)
#define DMA2D_LL_EVENT_RX_FIFO_REORDER_UDF   (1<<12)
#define DMA2D_LL_EVENT_RX_FIFO_REORDER_OVF   (1<<11)
#define DMA2D_LL_EVENT_RX_DESC_EMPTY         (1<<10)
#define DMA2D_LL_EVENT_RX_FIFO_L3_UDF        (1<<9)
#define DMA2D_LL_EVENT_RX_FIFO_L3_OVF        (1<<8)
#define DMA2D_LL_EVENT_RX_FIFO_L2_UDF        (1<<7)
#define DMA2D_LL_EVENT_RX_FIFO_L2_OVF        (1<<6)
#define DMA2D_LL_EVENT_RX_FIFO_L1_UDF        (1<<5)
#define DMA2D_LL_EVENT_RX_FIFO_L1_OVF        (1<<4)
#define DMA2D_LL_EVENT_RX_DESC_ERROR         (1<<3)
#define DMA2D_LL_EVENT_RX_ERR_EOF            (1<<2)     // Raised when the err_eof bit in the descriptor is set by auto writeback (Only JPEG)
#define DMA2D_LL_EVENT_RX_SUC_EOF            (1<<1)     // Raised when the suc_eof bit in the descriptor is set by auto writeback
#define DMA2D_LL_EVENT_RX_DONE               (1<<0)     // Raised when all data descripted in the descriptor are processed (every succeed descriptor will have one RX_DONE interrupt)

#define DMA2D_LL_EVENT_TX_DESC_TASK_OVF      (1<<12)
#define DMA2D_LL_EVENT_TX_FIFO_REORDER_UDF   (1<<11)
#define DMA2D_LL_EVENT_TX_FIFO_REORDER_OVF   (1<<10)
#define DMA2D_LL_EVENT_TX_FIFO_L3_UDF        (1<<9)
#define DMA2D_LL_EVENT_TX_FIFO_L3_OVF        (1<<8)
#define DMA2D_LL_EVENT_TX_FIFO_L2_UDF        (1<<7)
#define DMA2D_LL_EVENT_TX_FIFO_L2_OVF        (1<<6)
#define DMA2D_LL_EVENT_TX_FIFO_L1_UDF        (1<<5)
#define DMA2D_LL_EVENT_TX_FIFO_L1_OVF        (1<<4)
#define DMA2D_LL_EVENT_TX_TOTAL_EOF          (1<<3)
#define DMA2D_LL_EVENT_TX_DESC_ERROR         (1<<2)
#define DMA2D_LL_EVENT_TX_EOF                (1<<1)
#define DMA2D_LL_EVENT_TX_DONE               (1<<0)

// Bit masks that are used to indicate availability of some sub-features in the channels
#define DMA2D_LL_TX_CHANNEL_SUPPORT_RO_MASK        (0U | BIT0) // TX channels that support reorder feature
#define DMA2D_LL_TX_CHANNEL_SUPPORT_CSC_MASK       (0U | BIT0 | BIT1 | BIT2) // TX channels that support color space conversion feature

#define DMA2D_LL_RX_CHANNEL_SUPPORT_RO_MASK        (0U | BIT0) // RX channels that support reorder feature
#define DMA2D_LL_RX_CHANNEL_SUPPORT_CSC_MASK       (0U | BIT0) // RX channels that support color space conversion feature

// Any "dummy" peripheral selection ID can be used for M2M mode
#define DMA2D_LL_TX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK   (0xF0)
#define DMA2D_LL_RX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK   (0xF8)
// Peripheral selection ID that disconnects 2D-DMA channel from any peripherals
#define DMA2D_LL_CHANNEL_PERIPH_NO_CHOICE          (7)
// Peripheral selection ID register field width
#define DMA2D_LL_CHANNEL_PERIPH_SEL_BIT_WIDTH      (3)

#define DMA2D_LL_DESC_ALIGNMENT 8 // Descriptor must be aligned to 8 bytes

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Enable the bus clock for 2D-DMA module
 *
 * @param group_id Group ID
 * @param enable True to enable; false to disable
 */
static inline void dma2d_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_dma2d_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define dma2d_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        dma2d_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the 2D-DMA module
 *
 * @param group_id Group ID
 */
static inline void dma2d_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dma2d = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dma2d = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define dma2d_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        dma2d_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * @brief Check if the bus clock is enabled for the DMA module
 */
__attribute__((always_inline))
static inline bool dma2d_ll_is_bus_clock_enabled(int group_id)
{
    (void) group_id;
    return HP_SYS_CLKRST.soc_clk_ctrl1.reg_dma2d_sys_clk_en;
}

/**
 * @brief Enable 2D-DMA module
 */
static inline void dma2d_ll_hw_enable(dma2d_dev_t *dev, bool enable)
{
    dev->rst_conf.clk_en = enable;
    // Reset AXI master read data FIFO
    dev->rst_conf.axim_rd_rst = 1;
    dev->rst_conf.axim_rd_rst = 0;
    // Reset AXI master write data FIFO
    dev->rst_conf.axim_wr_rst = 1;
    dev->rst_conf.axim_wr_rst = 0;
    // Disable TX/RX arbiter weight config
    dev->out_arb_config.val = 0;
    dev->in_arb_config.val = 0;
}

/**
 * @brief Helper function to get the register field value for different pixel bytes scramble order
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_get_scramble_order_sel(dma2d_scramble_order_t order)
{
    switch (order) {
    case DMA2D_SCRAMBLE_ORDER_BYTE2_1_0:
        return 0;
    case DMA2D_SCRAMBLE_ORDER_BYTE2_0_1:
        return 1;
    case DMA2D_SCRAMBLE_ORDER_BYTE1_0_2:
        return 2;
    case DMA2D_SCRAMBLE_ORDER_BYTE1_2_0:
        return 3;
    case DMA2D_SCRAMBLE_ORDER_BYTE0_2_1:
        return 4;
    case DMA2D_SCRAMBLE_ORDER_BYTE0_1_2:
        return 5;
    default:
        // Unsupported scramble order
        abort();
    }
}

/////////////////////////////////////// RX ///////////////////////////////////////////
#define DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, ch, reg)      ((volatile void*[]){&dev->in_channel0.reg, &dev->in_channel1.reg}[(ch)])

/**
 * @brief Get 2D-DMA RX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_interrupt_status(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_int_st_chn_reg_t *reg = (volatile dma2d_in_int_st_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_int_st);
    return reg->val;
}

/**
 * @brief Enable 2D-DMA RX channel interrupt
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_interrupt(dma2d_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    volatile dma2d_in_int_ena_chn_reg_t *reg = (volatile dma2d_in_int_ena_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_int_ena);
    if (enable) {
        reg->val = reg->val | (mask & DMA2D_LL_RX_EVENT_MASK);
    } else {
        reg->val = reg->val & ~(mask & DMA2D_LL_RX_EVENT_MASK);
    }
}

/**
 * @brief Clear 2D-DMA RX channel interrupt
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_clear_interrupt_status(dma2d_dev_t *dev, uint32_t channel, uint32_t mask)
{
    volatile dma2d_in_int_clr_chn_reg_t *reg = (volatile dma2d_in_int_clr_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_int_clr);
    reg->val = (mask & DMA2D_LL_RX_EVENT_MASK);
}

/**
 * @brief Get 2D-DMA RX channel interrupt status register address
 */
static inline volatile void *dma2d_ll_rx_get_interrupt_status_reg(dma2d_dev_t *dev, uint32_t channel)
{
    return (volatile void *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_int_st);
}

/**
 * @brief Enable 2D-DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_owner_check(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_check_owner_chn = enable;
}

/**
 * @brief Enable 2D-DMA RX channel page boundary wrap
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_page_bound_wrap(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_page_bound_en_chn = enable;
}

/**
 * @brief Set 2D-DMA RX channel maximum burst reading data length in bytes
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_data_burst_length(dma2d_dev_t *dev, uint32_t channel, dma2d_data_burst_length_t length)
{
    uint32_t sel;
    switch (length) {
    case DMA2D_DATA_BURST_LENGTH_8:
        sel = 0;
        break;
    case DMA2D_DATA_BURST_LENGTH_16:
        sel = 1;
        break;
    case DMA2D_DATA_BURST_LENGTH_32:
        sel = 2;
        break;
    case DMA2D_DATA_BURST_LENGTH_64:
        sel = 3;
        break;
    case DMA2D_DATA_BURST_LENGTH_128:
        sel = 4;
        break;
    default:
        // Unsupported data burst length
        abort();
    }
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_mem_burst_length_chn = sel;
}

/**
 * @brief Enable 2D-DMA RX channel burst reading descriptor link, disabled by default
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_descriptor_burst(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->indscr_burst_en_chn = enable;
}

/**
 * @brief Reset 2D-DMA RX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_reset_channel(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_rst_chn = 1;
    reg->in_rst_chn = 0;
}

/**
 * @brief Check if 2D-DMA RX channel is safe to reset
 */
__attribute__((always_inline))
static inline bool dma2d_ll_rx_is_reset_avail(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_state_chn_reg_t *reg = (volatile dma2d_in_state_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_state);
    return reg->in_reset_avail_chn;
}

/**
 * @brief Disable 2D-DMA RX channel via a command
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_abort(dma2d_dev_t *dev, uint32_t channel, bool disable)
{
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_cmd_disable_chn = disable;
}

/**
 * @brief Enable 2D-DMA RX channel to obtain descriptor from IP port (here referring the PPA module)
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_dscr_port(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_dscr_port_en_chn = enable;
}

/**
 * @brief Select 2D-DMA RX channel macro block size (Only useful in mode 1 of the link descriptor)
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_macro_block_size(dma2d_dev_t *dev, uint32_t channel, dma2d_macro_block_size_t size)
{
    uint32_t sel;
    switch (size) {
    case DMA2D_MACRO_BLOCK_SIZE_NONE:
        sel = 3;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_8:
        sel = 0;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_16:
        sel = 1;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_16_16:
        sel = 2;
        break;
    default:
        // Unsupported macro block size
        abort();
    }
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_macro_block_size_chn = sel;
}

/**
 * @brief Pop data from 2D-DMA RX FIFO
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_pop_data(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_pop_chn_reg_t *reg = (volatile dma2d_in_pop_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_pop);
    reg->infifo_pop_chn = 1;
    return reg->infifo_rdata_chn;
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_desc_addr(dma2d_dev_t *dev, uint32_t channel, uint32_t addr)
{
    volatile dma2d_in_link_addr_chn_reg_t *reg = (volatile dma2d_in_link_addr_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_link_addr);
    reg->inlink_addr_chn = addr;
}

/**
 * @brief Start dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_start(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_link_conf_chn_reg_t *reg = (volatile dma2d_in_link_conf_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_link_conf);
    reg->inlink_start_chn = 1;
}

/**
 * @brief Stop dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_stop(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_link_conf_chn_reg_t *reg = (volatile dma2d_in_link_conf_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_link_conf);
    reg->inlink_stop_chn = 1;
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_restart(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_link_conf_chn_reg_t *reg = (volatile dma2d_in_link_conf_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_link_conf);
    reg->inlink_restart_chn = 1;
}

/**
 * @brief Configure the value of the owner field written back to the 2D-DMA RX descriptor
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_auto_return_owner(dma2d_dev_t *dev, uint32_t channel, int owner)
{
    volatile dma2d_in_link_conf_chn_reg_t *reg = (volatile dma2d_in_link_conf_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_link_conf);
    reg->inlink_auto_ret_chn = owner;
}

/**
 * @brief Check if 2D-DMA RX descriptor FSM is in IDLE state
 */
__attribute__((always_inline))
static inline bool dma2d_ll_rx_is_desc_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_link_conf_chn_reg_t *reg = (volatile dma2d_in_link_conf_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_link_conf);
    return reg->inlink_park_chn;
}

/**
 * @brief Check if 2D-DMA RX FSM is in IDLE state
 */
__attribute__((always_inline))
static inline bool dma2d_ll_rx_is_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_state_chn_reg_t *reg = (volatile dma2d_in_state_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_state);
    return (reg->in_state_chn == 0);
}

/**
 * @brief Get RX success EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_success_eof_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_suc_eof_des_addr_chn_reg_t *reg = (volatile dma2d_in_suc_eof_des_addr_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_suc_eof_des_addr);
    return reg->val;
}

/**
 * @brief Get RX error EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_error_eof_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_err_eof_des_addr_chn_reg_t *reg = (volatile dma2d_in_err_eof_des_addr_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_err_eof_des_addr);
    return reg->val;
}

/**
 * @brief Get the pre-fetched RX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_prefetched_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_dscr_chn_reg_t *reg = (volatile dma2d_in_dscr_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_dscr);
    return reg->val;
}

/**
 * @brief Set priority for 2D-DMA RX channel
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_priority(dma2d_dev_t *dev, uint32_t channel, uint32_t prio)
{
    volatile dma2d_in_arb_chn_reg_t *reg = (volatile dma2d_in_arb_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_arb);
    reg->in_arb_priority_chn = prio;
}

/**
 * @brief Connect 2D-DMA RX channel to a given peripheral
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_connect_to_periph(dma2d_dev_t *dev, uint32_t channel, dma2d_trigger_peripheral_t periph, int periph_id)
{
    volatile dma2d_in_peri_sel_chn_reg_t *peri_sel_reg = (volatile dma2d_in_peri_sel_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_peri_sel);
    peri_sel_reg->in_peri_sel_chn = periph_id;
    volatile dma2d_in_conf0_chn_reg_t *conf0_reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    conf0_reg->in_mem_trans_en_chn = (periph == DMA2D_TRIG_PERIPH_M2M);
}

/**
 * @brief Disconnect 2D-DMA RX channel from peripheral
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_disconnect_from_periph(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_peri_sel_chn_reg_t *peri_sel_reg = (volatile dma2d_in_peri_sel_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_peri_sel);
    peri_sel_reg->in_peri_sel_chn = DMA2D_LL_CHANNEL_PERIPH_NO_CHOICE;
    volatile dma2d_in_conf0_chn_reg_t *conf0_reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    conf0_reg->in_mem_trans_en_chn = false;
}

// REORDER FUNCTION (Only CH0 supports this feature)

/**
 * @brief Enable 2D-DMA RX channel macro block reorder function
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_enable_reorder(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    volatile dma2d_in_conf0_chn_reg_t *reg = (volatile dma2d_in_conf0_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_conf0);
    reg->in_reorder_en_chn = enable;
}

// COLOR SPACE CONVERSION FUNCTION

/**
 * @brief Configure 2D-DMA RX channel color space conversion parameters
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_configure_color_space_conv(dma2d_dev_t *dev, uint32_t channel, dma2d_csc_rx_option_t csc_sel)
{
    HAL_ASSERT(channel == 0); // Only channel 0 supports color space conversion
    // L1
    uint32_t input_sel = 7; // Disable CSC
    // L2
    bool proc_en = false; // Disable generic color convert module between color input & color output
    int (*table)[4] = NULL;
    // L3
    uint32_t output_sel = 1; // Output directly

    const int color_space_conv_param_yuv2rgb_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT601;
    const int color_space_conv_param_yuv2rgb_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT709;

    switch (csc_sel) {
    case DMA2D_CSC_RX_NONE:
        input_sel = 7;
        break;
    case DMA2D_CSC_RX_SCRAMBLE:
        input_sel = 1; // Or 2
        proc_en = false;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV422_TO_YUV444:
    case DMA2D_CSC_RX_YUV420_TO_YUV444:
        input_sel = 0;
        proc_en = false;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB888_601:
    case DMA2D_CSC_RX_YUV422_TO_RGB888_601:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB565_601:
    case DMA2D_CSC_RX_YUV422_TO_RGB565_601:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 0;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB888_709:
    case DMA2D_CSC_RX_YUV422_TO_RGB888_709:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV420_TO_RGB565_709:
    case DMA2D_CSC_RX_YUV422_TO_RGB565_709:
        input_sel = 0;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 0;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB888_601:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB565_601:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 0;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB888_709:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_RX_YUV444_TO_RGB565_709:
        input_sel = 2;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 0;
        break;
    default:
        // Unsupported color space conversion type
        abort();
    }

    dev->in_channel0.in_color_convert.in_color_input_sel_chn = input_sel;
    dev->in_channel0.in_color_convert.in_color_3b_proc_en_chn = proc_en;
    dev->in_channel0.in_color_convert.in_color_output_sel_chn = output_sel;

    if (proc_en) {
        HAL_ASSERT(table);
        typeof(dev->in_channel0.in_color_param_group) color_param_group;

        color_param_group.param_h.a = table[0][0];
        color_param_group.param_h.b = table[0][1];
        color_param_group.param_h.c = table[0][2];
        color_param_group.param_h.d = table[0][3];

        color_param_group.param_m.a = table[1][0];
        color_param_group.param_m.b = table[1][1];
        color_param_group.param_m.c = table[1][2];
        color_param_group.param_m.d = table[1][3];

        color_param_group.param_l.a = table[2][0];
        color_param_group.param_l.b = table[2][1];
        color_param_group.param_l.c = table[2][2];
        color_param_group.param_l.d = table[2][3];

        dev->in_channel0.in_color_param_group.param_h.val[0] = color_param_group.param_h.val[0];
        dev->in_channel0.in_color_param_group.param_h.val[1] = color_param_group.param_h.val[1];
        dev->in_channel0.in_color_param_group.param_m.val[0] = color_param_group.param_m.val[0];
        dev->in_channel0.in_color_param_group.param_m.val[1] = color_param_group.param_m.val[1];
        dev->in_channel0.in_color_param_group.param_l.val[0] = color_param_group.param_l.val[0];
        dev->in_channel0.in_color_param_group.param_l.val[1] = color_param_group.param_l.val[1];
    }
}

/**
 * @brief Configure 2D-DMA RX channel data scramble before color conversion
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_csc_pre_scramble(dma2d_dev_t *dev, uint32_t channel, dma2d_scramble_order_t order)
{
    HAL_ASSERT(channel == 0); // Only channel 0 supports scramble
    dev->in_channel0.in_scramble.in_scramble_sel_pre_chn = dma2d_ll_get_scramble_order_sel(order);
}

/**
 * @brief Configure 2D-DMA RX channel data scramble after color conversion
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_csc_post_scramble(dma2d_dev_t *dev, uint32_t channel, dma2d_scramble_order_t order)
{
    HAL_ASSERT(channel == 0); // Only channel 0 supports scramble
    dev->in_channel0.in_scramble.in_scramble_sel_post_chn = dma2d_ll_get_scramble_order_sel(order);
}

// Arbiter

/**
 * @brief Enable 2D-DMA RX side arbiter weight configuration
 */
static inline void dma2d_ll_rx_enable_arb_weight(dma2d_dev_t *dev, bool enable)
{
    dev->in_arb_config.in_weight_en = enable;
}

/**
 * @brief Set 2D-DMA RX side arbiter period
 *
 * @param timeout_num Number of AXI clock cycles (240MHz) in one arbiter period
 */
static inline void dma2d_ll_rx_set_arb_timeout(dma2d_dev_t *dev, uint32_t timeout_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->in_arb_config, in_arb_timeout_num, timeout_num);
}

/**
 * @brief Set 2D-DMA RX channel arbiter token number
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_arb_token_num(dma2d_dev_t *dev, uint32_t channel, uint32_t token_num)
{
    volatile dma2d_in_arb_chn_reg_t *reg = (volatile dma2d_in_arb_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_arb);
    reg->in_arb_token_num_chn = token_num;
}

/**
 * @brief Get 2D-DMA RX channel arbiter token number
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_rx_get_arb_token_num(dma2d_dev_t *dev, uint32_t channel)
{
    volatile dma2d_in_arb_chn_reg_t *reg = (volatile dma2d_in_arb_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_arb);
    return reg->in_arb_token_num_chn;
}

/**
 * @brief Set 2D-DMA RX channel arbiter priority
 */
__attribute__((always_inline))
static inline void dma2d_ll_rx_set_arb_priority(dma2d_dev_t *dev, uint32_t channel, uint32_t priority)
{
    volatile dma2d_in_arb_chn_reg_t *reg = (volatile dma2d_in_arb_chn_reg_t *)DMA2D_LL_IN_CHANNEL_GET_REG_ADDR(dev, channel, in_arb);
    reg->in_arb_priority_chn = priority;
}

/////////////////////////////////////// TX ///////////////////////////////////////////
/**
 * @brief Get 2D-DMA TX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_interrupt_status(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_int_st.val & DMA2D_LL_TX_EVENT_MASK;
}

/**
 * @brief Enable 2D-DMA TX channel interrupt
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_interrupt(dma2d_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->out_channel[channel].out_int_ena.val = dev->out_channel[channel].out_int_ena.val | (mask & DMA2D_LL_TX_EVENT_MASK);
    } else {
        dev->out_channel[channel].out_int_ena.val = dev->out_channel[channel].out_int_ena.val & ~(mask & DMA2D_LL_TX_EVENT_MASK);
    }
}

/**
 * @brief Clear 2D-DMA TX channel interrupt
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_clear_interrupt_status(dma2d_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->out_channel[channel].out_int_clr.val = (mask & DMA2D_LL_TX_EVENT_MASK);
}

/**
 * @brief Get 2D-DMA TX channel interrupt status register address
 */
static inline volatile void *dma2d_ll_tx_get_interrupt_status_reg(dma2d_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->out_channel[channel].out_int_st);
}

/**
 * @brief Enable 2D-DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_owner_check(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_check_owner_chn = enable;
}

/**
 * @brief Enable 2D-DMA TX channel EOF flag generation mode to be EOF flag is generated when data has been popped from DMA FIFO
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_eof_mode(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_eof_mode_chn = enable;
}

/**
 * @brief Enable 2D-DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_auto_write_back(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_auto_wrback_chn = enable;
}

/**
 * @brief Enable 2D-DMA TX channel page boundary wrap
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_page_bound_wrap(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_page_bound_en_chn = enable;
}

/**
 * @brief Set 2D-DMA TX channel maximum burst reading data length in bytes
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_data_burst_length(dma2d_dev_t *dev, uint32_t channel, dma2d_data_burst_length_t length)
{
    uint32_t sel;
    switch (length) {
    case DMA2D_DATA_BURST_LENGTH_8:
        sel = 0;
        break;
    case DMA2D_DATA_BURST_LENGTH_16:
        sel = 1;
        break;
    case DMA2D_DATA_BURST_LENGTH_32:
        sel = 2;
        break;
    case DMA2D_DATA_BURST_LENGTH_64:
        sel = 3;
        break;
    case DMA2D_DATA_BURST_LENGTH_128:
        sel = 4;
        break;
    default:
        // Unsupported data burst length
        abort();
    }
    dev->out_channel[channel].out_conf0.out_mem_burst_length_chn = sel;
}

/**
 * @brief Enable 2D-DMA TX channel burst reading descriptor link, disabled by default
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_descriptor_burst(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.outdscr_burst_en_chn = enable;
}

/**
 * @brief Reset 2D-DMA TX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_reset_channel(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_conf0.out_rst_chn = 1;
    dev->out_channel[channel].out_conf0.out_rst_chn = 0;
}

/**
 * @brief Check if 2D-DMA TX channel is safe to reset
 */
__attribute__((always_inline))
static inline bool dma2d_ll_tx_is_reset_avail(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_state.out_reset_avail_chn;
}

/**
 * @brief Disable 2D-DMA TX channel via a command
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_abort(dma2d_dev_t *dev, uint32_t channel, bool disable)
{
    dev->out_channel[channel].out_conf0.out_cmd_disable_chn = disable;
}

/**
 * @brief Enable 2D-DMA TX channel to obtain descriptor from IP port (here referring the PPA module)
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_dscr_port(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_dscr_port_en_chn = enable;
}

/**
 * @brief Set 2D-DMA TX channel block size in dscr-port mode
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_dscr_port_block_size(dma2d_dev_t *dev, uint32_t channel, uint32_t blk_h, uint32_t blk_v)
{
    dev->out_channel[channel].out_dscr_port_blk.out_dscr_port_blk_h_chn = blk_h;
    dev->out_channel[channel].out_dscr_port_blk.out_dscr_port_blk_v_chn = blk_v;
}

/**
 * @brief Select 2D-DMA TX channel macro block size
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_macro_block_size(dma2d_dev_t *dev, uint32_t channel, dma2d_macro_block_size_t size)
{
    uint32_t sel;
    switch (size) {
    case DMA2D_MACRO_BLOCK_SIZE_NONE:
        sel = 3;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_8:
        sel = 0;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_8_16:
        sel = 1;
        break;
    case DMA2D_MACRO_BLOCK_SIZE_16_16:
        sel = 2;
        break;
    default:
        // Unsupported macro block size
        abort();
    }
    dev->out_channel[channel].out_conf0.out_macro_block_size_chn = sel;
}

/**
 * @brief Push data into 2D-DMA TX FIFO
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_push_data(dma2d_dev_t *dev, uint32_t channel, uint32_t data)
{
    dev->out_channel[channel].out_push.outfifo_wdata_chn = data;
    dev->out_channel[channel].out_push.outfifo_push_chn = 1;
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_desc_addr(dma2d_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->out_channel[channel].out_link_addr.outlink_addr_chn = addr;
}

/**
 * @brief Start dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_start(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_link_conf.outlink_start_chn = 1;
}

/**
 * @brief Stop dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_stop(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_link_conf.outlink_stop_chn = 1;
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_restart(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_link_conf.outlink_restart_chn = 1;
}

/**
 * @brief Check if 2D-DMA TX descriptor FSM is in IDLE state
 */
__attribute__((always_inline))
static inline bool dma2d_ll_tx_is_desc_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_link_conf.outlink_park_chn;
}

/**
 * @brief Check if 2D-DMA TX FSM is in IDLE state
 */
__attribute__((always_inline))
static inline bool dma2d_ll_tx_is_fsm_idle(dma2d_dev_t *dev, uint32_t channel)
{
    return (dev->out_channel[channel].out_state.out_state_chn == 0);
}

/**
 * @brief Get TX EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_eof_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_eof_des_addr.val;
}

/**
 * @brief Get the pre-fetched TX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_prefetched_desc_addr(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_dscr.val;
}

/**
 * @brief Set priority for 2D-DMA TX channel
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_priority(dma2d_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->out_channel[channel].out_arb.out_arb_priority_chn = prio;
}

/**
 * @brief Connect 2D-DMA TX channel to a given peripheral
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_connect_to_periph(dma2d_dev_t *dev, uint32_t channel, dma2d_trigger_peripheral_t periph, int periph_id)
{
    (void)periph;
    dev->out_channel[channel].out_peri_sel.out_peri_sel_chn = periph_id;
}

/**
 * @brief Disconnect 2D-DMA TX channel from peripheral
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_disconnect_from_periph(dma2d_dev_t *dev, uint32_t channel)
{
    dev->out_channel[channel].out_peri_sel.out_peri_sel_chn = DMA2D_LL_CHANNEL_PERIPH_NO_CHOICE;
}

// REORDER FUNCTION (Only CH0 supports this feature)

/**
 * @brief Enable 2D-DMA TX channel macro block reorder function
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_enable_reorder(dma2d_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out_channel[channel].out_conf0.out_reorder_en_chn = enable;
}

// COLOR SPACE CONVERSION FUNCTION

/**
 * @brief Configure 2D-DMA TX channel color space conversion parameters
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_configure_color_space_conv(dma2d_dev_t *dev, uint32_t channel, dma2d_csc_tx_option_t csc_sel)
{
    // L1
    uint32_t input_sel = 7; // Disable CSC
    // L2
    bool proc_en = false; // Disable generic color convert module between color input & color output
    int (*table)[4] = NULL;
    // L3
    uint32_t output_sel = 2; // Output directly

    const int color_space_conv_param_rgb2yuv_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT601;
    const int color_space_conv_param_rgb2yuv_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT709;
    const int color_space_conv_param_yuv2rgb_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT601;
    const int color_space_conv_param_yuv2rgb_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT709;

    switch (csc_sel) {
    case DMA2D_CSC_TX_NONE:
        input_sel = 7;
        break;
    case DMA2D_CSC_TX_SCRAMBLE:
        input_sel = 2; // Or 3
        proc_en = false;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_RGB565:
        input_sel = 3;
        proc_en = false;
        output_sel = 0;
        break;
    case DMA2D_CSC_TX_RGB565_TO_RGB888:
        input_sel = 0;
        proc_en = false;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV444_601:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt601_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV444_709:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt709_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV422_601:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt601_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_TX_RGB888_TO_YUV422_709:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_rgb2yuv_bt709_table;
        output_sel = 1;
        break;
    case DMA2D_CSC_TX_YUV444_TO_RGB888_601:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_YUV444_TO_RGB888_709:
        input_sel = 3;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_YUV422_TO_RGB888_601:
        input_sel = 1;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt601_table;
        output_sel = 2;
        break;
    case DMA2D_CSC_TX_YUV422_TO_RGB888_709:
        input_sel = 1;
        proc_en = true;
        table = (int (*)[4])color_space_conv_param_yuv2rgb_bt709_table;
        output_sel = 2;
        break;
    default:
        // Unsupported color space conversion type
        abort();
    }

    dev->out_channel[channel].out_color_convert.out_color_input_sel_chn = input_sel;
    dev->out_channel[channel].out_color_convert.out_color_3b_proc_en_chn = proc_en;
    dev->out_channel[channel].out_color_convert.out_color_output_sel_chn = output_sel;
    if (proc_en) {
        HAL_ASSERT(table);
        typeof(dev->out_channel[channel].out_color_param_group) color_param_group;

        color_param_group.param_h.a = table[0][0];
        color_param_group.param_h.b = table[0][1];
        color_param_group.param_h.c = table[0][2];
        color_param_group.param_h.d = table[0][3];

        color_param_group.param_m.a = table[1][0];
        color_param_group.param_m.b = table[1][1];
        color_param_group.param_m.c = table[1][2];
        color_param_group.param_m.d = table[1][3];

        color_param_group.param_l.a = table[2][0];
        color_param_group.param_l.b = table[2][1];
        color_param_group.param_l.c = table[2][2];
        color_param_group.param_l.d = table[2][3];

        dev->out_channel[channel].out_color_param_group.param_h.val[0] = color_param_group.param_h.val[0];
        dev->out_channel[channel].out_color_param_group.param_h.val[1] = color_param_group.param_h.val[1];
        dev->out_channel[channel].out_color_param_group.param_m.val[0] = color_param_group.param_m.val[0];
        dev->out_channel[channel].out_color_param_group.param_m.val[1] = color_param_group.param_m.val[1];
        dev->out_channel[channel].out_color_param_group.param_l.val[0] = color_param_group.param_l.val[0];
        dev->out_channel[channel].out_color_param_group.param_l.val[1] = color_param_group.param_l.val[1];
    }
}

/**
 * @brief Configure 2D-DMA TX channel data scramble before color conversion
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_csc_pre_scramble(dma2d_dev_t *dev, uint32_t channel, dma2d_scramble_order_t order)
{
    dev->out_channel[channel].out_scramble.out_scramble_sel_pre_chn = dma2d_ll_get_scramble_order_sel(order);
}

// Arbiter

/**
 * @brief Enable 2D-DMA TX side arbiter weight configuration
 */
static inline void dma2d_ll_tx_enable_arb_weight(dma2d_dev_t *dev, bool enable)
{
    dev->out_arb_config.out_weight_en = enable;
}

/**
 * @brief Set 2D-DMA TX side arbiter period
 *
 * @param timeout_num Number of AXI clock cycles (240MHz) in one arbiter period
 */
static inline void dma2d_ll_tx_set_arb_timeout(dma2d_dev_t *dev, uint32_t timeout_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->out_arb_config, out_arb_timeout_num, timeout_num);
}

/**
 * @brief Set 2D-DMA TX channel arbiter token number
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_arb_token_num(dma2d_dev_t *dev, uint32_t channel, uint32_t token_num)
{
    dev->out_channel[channel].out_arb.out_arb_token_num_chn = token_num;
}

/**
 * @brief Get 2D-DMA TX channel arbiter token number
 */
__attribute__((always_inline))
static inline uint32_t dma2d_ll_tx_get_arb_token_num(dma2d_dev_t *dev, uint32_t channel)
{
    return dev->out_channel[channel].out_arb.out_arb_token_num_chn;
}

/**
 * @brief Set 2D-DMA TX channel arbiter priority
 */
__attribute__((always_inline))
static inline void dma2d_ll_tx_set_arb_priority(dma2d_dev_t *dev, uint32_t channel, uint32_t priority)
{
    dev->out_channel[channel].out_arb.out_arb_priority_chn = priority;
}

#ifdef __cplusplus
}
#endif
