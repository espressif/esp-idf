/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for I3C slave register operations

#include <stdbool.h>
#include <stddef.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "hal/misc.h"
#include "soc/i3c_slv_struct.h"
#include "soc/i3c_slv_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I3C_SLAVE_LL_EVENT_INTR        (I3C_SLV_RXPEND_ENA_M | I3C_SLV_TXSEND_ENA_M)


/**
 * @brief Enable or disable I3C slave bus clock
 *
 * @param hw I3C slave hardware pointer (currently unused)
 * @param enable true: enable clock, false: disable clock
 */
static inline void i3c_slave_ll_enable_bus_clock(i3c_slv_dev_t *hw, bool enable)
{
    (void)hw; // Suppress unused parameter warning
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_i3c_slv_apb_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_slave_ll_enable_bus_clock(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_slave_ll_enable_bus_clock(__VA_ARGS__);} while(0)

/**
 * @brief Reset I3C slave registers
 *
 * Perform register reset by setting reset signal to 1 then 0
 *
 * @param hw I3C slave hardware pointer (currently unused)
 */
static inline void i3c_slave_ll_reset_register(i3c_slv_dev_t *hw)
{
    (void)hw; // Suppress unused parameter warning
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_i3cslv = 1;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_i3cslv = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define i3c_slave_ll_reset_register(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; i3c_slave_ll_reset_register(__VA_ARGS__);} while(0)

/**
 * @brief Configure I3C slave static address
 *
 * @param hw I3C slave hardware pointer
 * @param address 7-bit static address to set
 */
static inline void i3c_slave_ll_config_static_address(i3c_slv_dev_t *hw, uint8_t address)
{
    hw->config.saddr = address;
}


/**
 * @brief Get pointer to I3C slave interrupt status register
 *
 * @param dev I3C slave hardware pointer
 * @return volatile void* Pointer to interrupt masked status register
 */
static inline volatile void *i3c_slave_ll_get_interrupt_status_reg(i3c_slv_dev_t *dev)
{
    return &dev->intmasked;
}


/**
 * @brief Clear I3C slave interrupt flags
 *
 * @param dev I3C slave hardware pointer
 * @param mask Interrupt bit mask to clear
 */
static inline void i3c_slave_ll_clear_intr_mask(i3c_slv_dev_t *dev, uint32_t mask)
{
    dev->intclr.val = mask;
}

/**
 * @brief Enable I3C slave interrupt mask
 *
 * @param dev I3C slave hardware pointer
 * @param mask Interrupt bit mask to enable
 */
static inline void i3c_slave_ll_enable_intr_mask(i3c_slv_dev_t *dev, uint32_t mask)
{
    dev->intset.val |= mask;
}

/**
 * @brief Disable I3C slave interrupt mask
 *
 * @param dev I3C slave hardware pointer
 * @param mask Interrupt bit mask to disable
 */
static inline void i3c_slave_ll_disable_intr_mask(i3c_slv_dev_t *dev, uint32_t mask)
{
    dev->intset.val &= (~mask);
}

/**
 * @brief Get current I3C slave interrupt mask status
 *
 * @param dev I3C slave hardware pointer
 * @param intr_status Pointer to store interrupt status
 */
static inline void i3c_slave_ll_get_intr_mask(i3c_slv_dev_t *dev, uint32_t *intr_status)
{
    *intr_status = dev->intmasked.val;
}

/**
 * @brief Write data to I3C slave device
 *
 * Write data to transmit FIFO byte by byte
 *
 * @param dev I3C slave hardware pointer
 * @param data Pointer to data to write
 * @param length Length of data to write (bytes)
 */
static inline void i3c_slave_ll_write_data(i3c_slv_dev_t *dev, const uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        dev->wdatab.val = data[i];
    }
}

/**
 * @brief Get data length in I3C slave receive FIFO
 *
 * @param dev I3C slave hardware pointer
 * @return size_t Number of bytes available to read in receive FIFO
 */
static inline size_t i3c_slave_ll_get_read_data_length(i3c_slv_dev_t *dev)
{
    return dev->datactrl.rxcount;
}

/**
 * @brief Read data from I3C slave device
 *
 * Read data from receive FIFO byte by byte
 *
 * @param dev I3C slave hardware pointer
 * @param data Buffer to store read data
 * @param length Length of data to read (bytes)
 */
static inline void i3c_slave_ll_read_data(i3c_slv_dev_t *dev, uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        data[i] = dev->rdarab.data0;
    }
}

/**
 * @brief Unlock or lock I3C slave registers
 *
 * @param dev I3C slave hardware pointer
 * @param unlock true: unlock registers to allow modification, false: lock registers to prevent modification
 */
static inline void i3c_slave_ll_unlock_register(i3c_slv_dev_t *dev, bool unlock)
{
    dev->datactrl.unlock = unlock;
}

/**
 * @brief Set I3C slave transmit FIFO trigger level
 *
 * When transmit FIFO contains fewer bytes than this level, interrupt or DMA request will be triggered
 *
 * @param dev I3C slave hardware pointer
 * @param threshold Transmit FIFO trigger level (0-3)
 */
static inline void i3c_slave_ll_set_transmit_fifo_threshold(i3c_slv_dev_t *dev, uint8_t threshold)
{
    dev->datactrl.txtrig = threshold;
}

/**
 * @brief Set I3C slave receive FIFO trigger level
 *
 * When receive FIFO reaches this level, interrupt or DMA request will be triggered
 *
 * @param dev I3C slave hardware pointer
 * @param threshold Receive FIFO trigger level (0-3)
 */
static inline void i3c_slave_ll_set_receive_fifo_threshold(i3c_slv_dev_t *dev, uint8_t threshold)
{
    dev->datactrl.rxtrig = threshold;
}

#ifdef __cplusplus
}
#endif
