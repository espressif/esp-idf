/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I3C register operations

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lpperi_struct.h"
#include "hal/misc.h"
#include "soc/i3c_mst_mem_struct.h"
#include "soc/i3c_mst_struct.h"
#include "soc/i3c_mst_reg.h"
#include "hal/i3c_master_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I3C_LL_GET_HW(num)          (((num) == 0) ? (&I3C_MST) : NULL)
#define I3C_LL_MASTER_EVENT_INTR    (I3C_MST_TRANSFER_COMPLETE_INT_ENA_M | I3C_MST_COMMAND_DONE_INT_ENA_M | I3C_MST_TX_DATA_BUF_THLD_INT_ENA_M | I3C_MST_RX_DATA_BUF_THLD_INT_ENA_M)
#define I3C_LL_MASTER_FIFO_LENGTH            (128)
#define I3C_LL_MASTER_TRANSMIT_EVENT_INTR    (I3C_MST_TX_DATA_BUF_THLD_INT_ENA_M | I3C_MST_TRANSFER_COMPLETE_INT_ENA_M | I3C_MST_COMMAND_DONE_INT_ENA_M)
#define I3C_LL_MASTER_RECEIVE_EVENT_INTR     (I3C_MST_RX_DATA_BUF_THLD_INT_ENA_M | I3C_MST_TRANSFER_COMPLETE_INT_ENA_M | I3C_MST_COMMAND_DONE_INT_ENA_M)

typedef enum {
    I3C_MASTER_LL_FIFO_WM_LENGTH_2 = 0x0,
    I3C_MASTER_LL_FIFO_WM_LENGTH_4 = 0x1,
    I3C_MASTER_LL_FIFO_WM_LENGTH_8 = 0x2,
    I3C_MASTER_LL_FIFO_WM_LENGTH_16 = 0x3,
    I3C_MASTER_LL_FIFO_WM_LENGTH_31 = 0x4,
} i3c_master_ll_fifo_wm_enum_t;

/**
 * @brief Set the clock source for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param src_clk Clock source, (1) for PLL_F160M, (0) for XTAL
 */
static inline void i3c_master_ll_set_source_clk(i3c_mst_dev_t *hw, i3c_master_clock_source_t src_clk)
{
    // src_clk : (1) for PLL_F160M, (0) for XTAL
    if (hw == &I3C_MST) {
        HP_SYS_CLKRST.peri_clk_ctrl119.reg_i3c_mst_clk_src_sel = (src_clk == I3C_MASTER_CLK_SRC_PLL_F160M) ? 1 : 0;
    } else {
        HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_set_source_clk(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_set_source_clk(__VA_ARGS__);} while(0)

/**
 * @brief Set the device address table for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param addr_table Device address table
 * @param device_number Number of devices
 */
static inline void i3c_master_ll_set_address_device_table(i3c_mst_dev_t *hw, i3c_master_address_table_t *addr_table, size_t device_number)
{
    for (int i = 0; i < device_number; i++) {
        I3C_MST_MEM.dev_addr_table[i].val = addr_table[i].dat.val;
    }
}

/**
 * @brief Enable or disable the bus clock for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param enable Whether to enable the bus clock
 */
static inline void i3c_master_ll_enable_bus_clock(i3c_mst_dev_t *hw, bool enable)
{
    if (hw == &I3C_MST) {
        HP_SYS_CLKRST.soc_clk_ctrl2.reg_i3c_mst_apb_clk_en = enable;
    } else {
        HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_enable_bus_clock(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_enable_bus_clock(__VA_ARGS__);} while(0)

/**
 * @brief Enable or disable the clock for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param enable Whether to enable the clock
 */
static inline void i3c_master_ll_enable_controller_clock(i3c_mst_dev_t *hw, bool enable)
{
    if (hw == &I3C_MST) {
        HP_SYS_CLKRST.peri_clk_ctrl119.reg_i3c_mst_clk_en = enable;
    } else {
        HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_enable_controller_clock(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_enable_controller_clock(__VA_ARGS__);} while(0)

/**
 * @brief Reset the I3C master registers
 *
 * @param hw I3C master hardware instance
 */
static inline void i3c_master_ll_reset_register(i3c_mst_dev_t *hw)
{
    if (hw == &I3C_MST) {
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_i3cmst = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_i3cmst = 0;
    } else {
        HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_master_ll_reset_register(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_master_ll_reset_register(__VA_ARGS__);} while(0)

/**
 * @brief Set the command table for the I3C master
 *
 * @param hw I3C master hardware instance
 * @param command_buf Command buffer
 * @param command_num Number of commands
 */
static inline void i3c_master_ll_set_command(i3c_mst_dev_t *hw, i3c_master_command_table_t *command_buf, size_t command_num)
{
    for (int i = 0; i < command_num; i++) {
        I3C_MST_MEM.command_buf_port.reg_command = command_buf[i].cmd_l.val;
        I3C_MST_MEM.command_buf_port.reg_command = command_buf[i].cmd_h.val;
    }
}

/**
 * @brief Start a transaction on the I3C master
 *
 * @param hw I3C master hardware instance
 */
static inline void i3c_master_ll_start_transaction(i3c_mst_dev_t *hw)
{
    hw->device_ctrl.reg_trans_start = 1;
    hw->device_ctrl.reg_trans_start = 0;
}

/**
 * @brief Set the open-drain timing for I3C master
 *
 * @param hw I3C master hardware instance
 * @param clock_source_freq Clock source frequency
 * @param scl_freq SCL frequency
 */
static inline void i3c_master_ll_set_i3c_open_drain_timing(i3c_mst_dev_t *hw, uint32_t clock_source_freq, uint32_t scl_freq)
{
    uint32_t period_cnt = clock_source_freq / scl_freq / 2;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_od_time, reg_i3c_mst_od_high_period, (period_cnt - 1));
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_od_time, reg_i3c_mst_od_low_period, (period_cnt - 1));
}

/**
 * @brief Set the fast mode timing for I2C master
 *
 * @param hw I3C master hardware instance
 * @param clock_source_freq Clock source frequency
 * @param scl_freq SCL frequency
 */
static inline void i3c_master_ll_set_i2c_fast_mode_timing(i3c_mst_dev_t *hw, uint32_t clock_source_freq, uint32_t scl_freq)
{
    uint32_t period_cnt = clock_source_freq / scl_freq / 2;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i2c_fm_time, reg_i2c_fm_high_period, (period_cnt - 1));
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i2c_fm_time, reg_i2c_fm_low_period, (period_cnt - 1));
}

/**
 * @brief Set the push-pull timing for I3C master
 *
 * @param hw I3C master hardware instance
 * @param clock_source_freq Clock source frequency
 * @param scl_freq SCL frequency
 */
static inline void i3c_master_ll_set_i3c_push_pull_timing(i3c_mst_dev_t *hw, uint32_t clock_source_freq, uint32_t scl_freq)
{
    uint32_t period_cnt = clock_source_freq / scl_freq / 2;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_pp_time, reg_i3c_mst_pp_high_period, (period_cnt - 1));
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->scl_i3c_mst_pp_time, reg_i3c_mst_pp_low_period, (period_cnt - 1));
}

/**
 * @brief Set the restart setup time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_restart_setup_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_rstart_setup.reg_scl_rstart_setup_time = (time_ns + clock_source_period_ns) / clock_source_period_ns;
}

/**
 * @brief Set the start hold time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_start_hold_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_start_hold.reg_scl_start_hold_time = time_ns / clock_source_period_ns + 1;
}

/**
 * @brief Set the stop hold time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_stop_hold_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_stop_hold.reg_scl_stop_hold_time = time_ns / clock_source_period_ns + 1;
}

/**
 * @brief Set the stop setup time for I3C master
 *
 * @param hw I3C master hardware instance
 * @param time_ns Time in nanoseconds
 * @param clock_source_period_ns Clock source period in nanoseconds
 */
static inline void i3c_master_ll_set_stop_setup_time(i3c_mst_dev_t *hw, uint32_t time_ns, uint32_t clock_source_period_ns)
{
    hw->scl_stop_setup.reg_scl_stop_setup_time = (time_ns + clock_source_period_ns) / clock_source_period_ns;
}

/**
 * @brief Get the data byte count from the I3C master
 *
 * @param hw I3C master hardware instance
 * @param count Pointer to store the data byte count
 */
static inline void i3c_master_ll_get_data_byte_count(i3c_mst_dev_t *hw, uint32_t *count)
{
    *count = I3C_MST.present_state1.data_byte_cnt;
}

/**
 * @brief Read data from the RX port of the I3C master
 *
 * @param hw I3C master hardware instance
 * @param data Pointer to store the read data
 * @param length Length of data to read
 */
static inline void i3c_master_ll_read_rx_port(i3c_mst_dev_t *hw, uint8_t *data, size_t length)
{
    uint32_t data_word = 0;
    int x = 0;
    uint32_t *data_u32 = (uint32_t *)data;
    for (x = 0; x < (int)length / 4; x++) {
        data_u32[x] = I3C_MST_MEM.rx_data_port.rx_data_port;
    }
    if (length % 4) {
        data_word = I3C_MST_MEM.rx_data_port.rx_data_port;
        for (int i = 0; i < length % 4; i++) {
            data[x * 4 + i] = (data_word >> 8 * i) & 0xFF;
        }
    }
}

/**
 * @brief Write data to the TX port of the I3C master
 *
 * @param hw I3C master hardware instance
 * @param data Pointer to the data to write
 * @param length Length of data to write
 */
__attribute__((always_inline))
static inline void i3c_master_ll_write_tx_port(i3c_mst_dev_t *hw, const uint8_t *data, size_t length)
{
    int x = 0;
    uint32_t data_word = 0;
    const uint32_t *data_u32 = (const uint32_t *)data;
    for (x = 0; x < (int)length / 4; x++) {
        I3C_MST_MEM.tx_data_port.reg_tx_data_port = data_u32[x];
    }

    if (length % 4) {
        data_word = 0;
        for (int i = 0; i < length % 4; i++) {
            data_word |= (data[x * 4 + i] << 8 * i);
        }
        I3C_MST_MEM.tx_data_port.reg_tx_data_port = data_word;
    }
}

/**
 * @brief Get the interrupt status register of the I3C master
 *
 * @param dev I3C master hardware instance
 * @return Pointer to the interrupt status register
 */
static inline volatile void *i3c_master_ll_get_interrupt_status_reg(i3c_mst_dev_t *dev)
{
    return &dev->int_st;
}

/**
 * @brief Clear the interrupt mask for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param mask Interrupt mask to clear
 */
__attribute__((always_inline))
static inline void i3c_master_ll_clear_intr_mask(i3c_mst_dev_t *dev, uint32_t mask)
{
    dev->int_clr.val = mask;
}

/**
 * @brief Enable the interrupt mask for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param mask Interrupt mask to enable
 */
__attribute__((always_inline))
static inline void i3c_master_ll_enable_intr_mask(i3c_mst_dev_t *dev, uint32_t mask)
{
    dev->int_st_ena.val |= mask;
}

/**
 * @brief Disable the interrupt mask for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param mask Interrupt mask to disable
 */
__attribute__((always_inline))
static inline void i3c_master_ll_disable_intr_mask(i3c_mst_dev_t *dev, uint32_t mask)
{
    dev->int_st_ena.val &= (~mask);
}

/**
 * @brief Get the interrupt mask status for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param intr_status Pointer to store the interrupt status
 */
__attribute__((always_inline))
static inline void i3c_master_ll_get_intr_mask(i3c_mst_dev_t *dev, uint32_t *intr_status)
{
    *intr_status = dev->int_st.val;
}

/**
 * @brief Get the TX FIFO empty count for the I3C master
 *
 * @param dev I3C master hardware instance
 * @return TX FIFO empty count
 */
static inline uint8_t i3c_master_ll_get_tx_fifo_empty_count(i3c_mst_dev_t *dev)
{
    return dev->data_buffer_status_level.tx_data_buf_empty_cnt;
}

/**
 * @brief Get the RX FIFO full count for the I3C master
 *
 * @param dev I3C master hardware instance
 * @return RX FIFO full count
 */
static inline uint8_t i3c_master_ll_get_rx_fifo_full_count(i3c_mst_dev_t *dev)
{
    return dev->data_buffer_status_level.rx_data_buf_cnt;
}

/**
 * @brief Set the TX data FIFO watermark threshold for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param fifo_wm FIFO watermark threshold
 */
static inline void i3c_master_ll_set_tx_data_fifo_wm_threshold(i3c_mst_dev_t *dev, i3c_master_ll_fifo_wm_enum_t fifo_wm)
{
    dev->data_buffer_thld_ctrl.reg_tx_data_buf_thld = fifo_wm;
}

/**
 * @brief Set the RX data FIFO watermark threshold for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param fifo_wm FIFO watermark threshold
 */
static inline void i3c_master_ll_set_rx_data_fifo_wm_threshold(i3c_mst_dev_t *dev, i3c_master_ll_fifo_wm_enum_t fifo_wm)
{
    dev->data_buffer_thld_ctrl.reg_rx_data_buf_thld = fifo_wm;
}

/**
 * @brief Enable or disable TX by DMA for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param enable Whether to enable TX by DMA
 */
static inline void i3c_master_ll_enable_tx_by_dma(i3c_mst_dev_t *dev, bool enable)
{
    dev->device_ctrl.reg_dma_tx_en = enable;
}

/**
 * @brief Enable or disable RX by DMA for the I3C master
 *
 * @param dev I3C master hardware instance
 * @param enable Whether to enable RX by DMA
 */
static inline void i3c_master_ll_enable_rx_by_dma(i3c_mst_dev_t *dev, bool enable)
{
    dev->device_ctrl.reg_dma_rx_en = enable;
}

/**
 * @brief Get the response buffer value from the I3C master
 *
 * @param dev I3C master hardware instance
 * @return Response buffer value
 */
static inline uint32_t i3c_master_ll_get_response_buffer_value(i3c_mst_dev_t *dev)
{
    return I3C_MST_MEM.response_buf_port.response;
}

#ifdef __cplusplus
}
#endif
