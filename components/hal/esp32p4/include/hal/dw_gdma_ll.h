/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_assert.h"
#include "hal/misc.h"
#include "hal/dw_gdma_types.h"
#include "soc/dw_gdma_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/reg_base.h"

#define DW_GDMA_LL_GET_HW(id)          (((id) == 0) ? (&DW_GDMA) : NULL)

#define DW_GDMA_LL_GROUPS              1 // there's one DW-GDMA instance connected to the AXI bus
#define DW_GDMA_LL_CHANNELS_PER_GROUP  4 // there are 4 independent channels in the DW-GDMA

#define DW_GDMA_LL_MASTER_PORT_MIPI_DSI 0 // DW_GDMA master 0 can access DSI bridge
#define DW_GDMA_LL_MASTER_PORT_MIPI_CSI 0 // DW_GDMA master 0 can access CSI bridge
#define DW_GDMA_LL_MASTER_PORT_MEMORY   1 // DW_GDMA master 1 can only access L2MEM & ROM & MSPI Flash/PSRAM

#define DW_GDMA_LL_MAX_OUTSTANDING_REQUESTS 16 // maximum number of outstanding requests

#define DW_GDMA_LL_LINK_LIST_ALIGNMENT 64 // link list item must be aligned to 64 bytes

// Common event bitmap
#define DW_GDMA_LL_COMMON_EVENT_SLVIF_DEC_ERR                  (0x1 << 0)
#define DW_GDMA_LL_COMMON_EVENT_SLVIF_WR2RO_ERR                (0x1 << 1)
#define DW_GDMA_LL_COMMON_EVENT_SLVIF_RD2WO_ERR                (0x1 << 2)
#define DW_GDMA_LL_COMMON_EVENT_SLVIF_WRONHOLD_ERR             (0x1 << 3)
#define DW_GDMA_LL_COMMON_EVENT_SLVIF_WRPARITY_ERR             (0x1 << 7)
#define DW_GDMA_LL_COMMON_EVENT_SLVIF_UNDEFINEDREG_DEC_ERR     (0x1 << 8)
#define DW_GDMA_LL_COMMON_EVENT_MXIF1_RCH0_ECCPROT_CORRERR     (0x1 << 9)
#define DW_GDMA_LL_COMMON_EVENT_MXIF1_RCH0_ECCPROT_UNCORRERR   (0x1 << 10)
#define DW_GDMA_LL_COMMON_EVENT_MXIF1_RECCPROT_CORRERR         (0x1 << 11)
#define DW_GDMA_LL_COMMON_EVENT_MXIF1_RECCPROT_UNCORRERR       (0x1 << 12)
#define DW_GDMA_LL_COMMON_EVENT_MXIF1_BCH_ECCPROT_CORRERR      (0x1 << 13)
#define DW_GDMA_LL_COMMON_EVENT_MXIF1_BCH_ECCPROT_UNCORRERR    (0x1 << 14)
#define DW_GDMA_LL_COMMON_EVENT_MXIF2_RCH0_ECCPROT_CORRERR     (0x1 << 15)
#define DW_GDMA_LL_COMMON_EVENT_MXIF2_RCH0_ECCPROT_UNCORRERR   (0x1 << 16)
#define DW_GDMA_LL_COMMON_EVENT_MXIF2_RECCPROT_CORRERR         (0x1 << 17)
#define DW_GDMA_LL_COMMON_EVENT_MXIF2_RECCPROT_UNCORRERR       (0x1 << 18)
#define DW_GDMA_LL_COMMON_EVENT_MXIF2_BCH_ECCPROT_CORRERR      (0x1 << 19)
#define DW_GDMA_LL_COMMON_EVENT_MXIF2_BCH_ECCPROT_UNCORRERR    (0x1 << 20)

// Channel event bitmap
#define DW_GDMA_LL_CHANNEL_EVENT_BLOCK_TFR_DONE                 (0x1 << 0)
#define DW_GDMA_LL_CHANNEL_EVENT_DMA_TFR_DONE                   (0x1 << 1)
#define DW_GDMA_LL_CHANNEL_EVENT_SRC_TRANSCOMP                  (0x1 << 3)
#define DW_GDMA_LL_CHANNEL_EVENT_DST_TRANSCOMP                  (0x1 << 4)
#define DW_GDMA_LL_CHANNEL_EVENT_SRC_DEC_ERR                    (0x1 << 5)
#define DW_GDMA_LL_CHANNEL_EVENT_DST_DEC_ERR                    (0x1 << 6)
#define DW_GDMA_LL_CHANNEL_EVENT_SRC_SLV_ERR                    (0x1 << 7)
#define DW_GDMA_LL_CHANNEL_EVENT_DST_SLV_ERR                    (0x1 << 8)
#define DW_GDMA_LL_CHANNEL_EVENT_LLI_RD_DEC_ERR                 (0x1 << 9)
#define DW_GDMA_LL_CHANNEL_EVENT_LLI_WR_DEC_ERR                 (0x1 << 10)
#define DW_GDMA_LL_CHANNEL_EVENT_LLI_RD_SLV_ERR                 (0x1 << 11)
#define DW_GDMA_LL_CHANNEL_EVENT_LLI_WR_SLV_ERR                 (0x1 << 12)
#define DW_GDMA_LL_CHANNEL_EVENT_SHADOWREG_OR_LLI_INVALID_ERR   (0x1 << 13)
#define DW_GDMA_LL_CHANNEL_EVENT_LOCK_CLEARED                   (0x1 << 27)
#define DW_GDMA_LL_CHANNEL_EVENT_SRC_SUSPENDED                  (0x1 << 28)
#define DW_GDMA_LL_CHANNEL_EVENT_SUSPENDED                      (0x1 << 29)
#define DW_GDMA_LL_CHANNEL_EVENT_DISABLED                       (0x1 << 30)
#define DW_GDMA_LL_CHANNEL_EVENT_ABORTED                        (0x1 << 31)

#define DW_GDMA_LL_CHANNEL_EVENT_MASK(chan)                     (1 << (chan))
#define DW_GDMA_LL_COMMON_EVENT_MASK                            (1 << 16)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handshake number for different peripherals
 */
typedef enum {
    DW_GDMA_LL_HW_HANDSHAKE_PERIPH_DSI, /*!< Handshake peripheral is DSI */
    DW_GDMA_LL_HW_HANDSHAKE_PERIPH_CSI, /*!< Handshake peripheral is CSI */
    DW_GDMA_LL_HW_HANDSHAKE_PERIPH_ISP, /*!< Handshake peripheral is ISP */
} dw_gdma_ll_hw_handshake_periph_t;

/**
 * @brief Enable the bus clock for the DMA module
 */
static inline void dw_gdma_ll_enable_bus_clock(int group_id, bool enable)
{
    (void) group_id;
    HP_SYS_CLKRST.soc_clk_ctrl0.reg_gdma_cpu_clk_en = enable;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_gdma_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define dw_gdma_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; dw_gdma_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the DMA module
 */
static inline void _dw_gdma_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_gdma = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_gdma = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define dw_gdma_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; _dw_gdma_ll_reset_register(__VA_ARGS__)

/**
 * @brief Check if the bus clock is enabled for the DMA module
 */
__attribute__((always_inline))
static inline bool dw_gdma_ll_is_bus_clock_enabled(int group_id)
{
    (void) group_id;
    return HP_SYS_CLKRST.soc_clk_ctrl1.reg_gdma_sys_clk_en && HP_SYS_CLKRST.soc_clk_ctrl0.reg_gdma_cpu_clk_en;
}

/**
 * @brief Reset the DMA controller by software
 *
 * @param dev Pointer to the DW_GDMA registers
 */
static inline void dw_gdma_ll_reset(dw_gdma_dev_t *dev)
{
    dev->reset0.dmac_rst = 1;
    while (dev->reset0.dmac_rst);
}

/**
 * @brief Enable the DMA controller
 *
 * @note If disable the controller while any channel is active, the controller won't stop
 *       until all activity on all channels are terminated.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_enable_controller(dw_gdma_dev_t *dev, bool en)
{
    dev->cfg0.dmac_en = en;
}

/**
 * @brief Enable the interrupt generation globally
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_enable_intr_global(dw_gdma_dev_t *dev, bool en)
{
    dev->cfg0.int_en = en;
}

/**
 * @brief Get the address of the interrupt status register
 *
 * @param dev Pointer to the DW_GDMA registers
 * @return Address of the interrupt status register
 */
static inline volatile void *dw_gdma_ll_get_intr_status_reg(dw_gdma_dev_t *dev)
{
    return &dev->int_st0;
}

/**
 * @brief Clear the common interrupt by mask
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param mask Mask of the interrupt to clear
 */
static inline void dw_gdma_ll_clear_common_intr(dw_gdma_dev_t *dev, uint32_t mask)
{
    dev->common_int_clr0.val = mask;
}

/**
 * @brief Enable the generation of common interrupt
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param mask Mask of the interrupt to enable
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_enable_common_intr_generation(dw_gdma_dev_t *dev, uint32_t mask, bool en)
{
    if (en) {
        dev->common_int_st_ena0.val |= mask;
    } else {
        dev->common_int_st_ena0.val &= ~mask;
    }
}

/**
 * @brief Enable the propagation of common interrupt to the CPU
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param mask Mask of the interrupt to enable
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_enable_common_intr_propagation(dw_gdma_dev_t *dev, uint32_t mask, bool en)
{
    if (en) {
        dev->common_int_sig_ena0.val |= mask;
    } else {
        dev->common_int_sig_ena0.val &= ~mask;
    }
}

/**
 * @brief Get the common interrupt status
 *
 * @note This register captures Slave interface access errors
 *
 * @param dev Pointer to the DW_GDMA registers
 * @return Mask of the common interrupt status
 */
static inline uint32_t dw_gdma_ll_get_common_intr_status(dw_gdma_dev_t *dev)
{
    return dev->common_int_st0.val;
}

/**
 * @brief Enable the generation of the channel interrupt
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param mask Mask of the interrupt to enable
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_channel_enable_intr_generation(dw_gdma_dev_t *dev, uint8_t channel, uint32_t mask, bool en)
{
    if (en) {
        dev->ch[channel].int_st_ena0.val |= mask;
    } else {
        dev->ch[channel].int_st_ena0.val &= ~mask;
    }
}

/**
 * @brief Enable the propagation of channel interrupt to the CPU
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param mask Mask of the interrupt to enable
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_channel_enable_intr_propagation(dw_gdma_dev_t *dev, uint8_t channel, uint32_t mask, bool en)
{
    if (en) {
        dev->ch[channel].int_sig_ena0.val |= mask;
    } else {
        dev->ch[channel].int_sig_ena0.val &= ~mask;
    }
}

/**
 * @brief Get the channel interrupt status
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return Mask of the channel interrupt status
 */
__attribute__((always_inline))
static inline uint32_t dw_gdma_ll_channel_get_intr_status(dw_gdma_dev_t *dev, uint8_t channel)
{
    return dev->ch[channel].int_st0.val;
}

/**
 * @brief Clear the channel interrupt by mask
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param mask Mask of the interrupt to clear
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_clear_intr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t mask)
{
    dev->ch[channel].int_clr0.val = mask;
}

/**
 * @brief Enable the DMA channel to start the transfer
 *
 * @note When the DMA transfer finished, the channel will be disabled automatically
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_channel_enable(dw_gdma_dev_t *dev, uint8_t channel, bool en)
{
    if (en) {
        dev->chen0.val = 0x101 << channel;
    } else {
        dev->chen0.val = 0x100 << channel;
    }
}

/**
 * @brief Suspend the DMA channel
 *
 * @note There is no guarantee that the current dma transaction will complete
 * @note The suspend bit is cleared when the channel is disabled
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en True to enter suspend mode, false to exit suspend mode
 */
static inline void dw_gdma_ll_channel_suspend(dw_gdma_dev_t *dev, uint8_t channel, bool en)
{
    if (en) {
        dev->chen0.val = 0x1010000 << channel;
    } else {
        dev->chen0.val = 0x1000000 << channel;
    }
}

/**
 * @brief Abort the DMA channel
 *
 * @note Abort should only be used in situations where a particular channel hangs due to no response.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_abort(dw_gdma_dev_t *dev, uint8_t channel)
{
    // the abort bit clears itself after the abort is done
    dev->chen1.val = 0x101 << channel;
    while (dev->chen1.val & (0x101 << channel));
}

/**
 * @brief Set the source address of the DMA transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param src_addr Source address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_src_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t src_addr)
{
    dev->ch[channel].sar0.sar0 = src_addr;
}

/**
 * @brief Set the destination address of the DMA transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param dst_addr Destination address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_dst_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t dst_addr)
{
    dev->ch[channel].dar0.dar0 = dst_addr;
}

/**
 * @brief Set the number of data to be transferred
 *
 * @note "transfer width" * "transfer block size" = the total bytes in one block transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param sz Number of transfer items
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_trans_block_size(dw_gdma_dev_t *dev, uint8_t channel, uint32_t sz)
{
    dev->ch[channel].block_ts0.block_ts = sz - 1;
}

/**
 * @brief Set the source master port based on the memory address
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param mem_addr Memory address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_src_master_port(dw_gdma_dev_t *dev, uint8_t channel, intptr_t mem_addr)
{
    if (mem_addr == MIPI_CSI_BRG_MEM_BASE) {
        dev->ch[channel].ctl0.sms = DW_GDMA_LL_MASTER_PORT_MIPI_CSI;
    } else {
        dev->ch[channel].ctl0.sms = DW_GDMA_LL_MASTER_PORT_MEMORY;
    }
}

/**
 * @brief Set the destination master port based on the memory address
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param mem_addr Memory address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_dst_master_port(dw_gdma_dev_t *dev, uint8_t channel, intptr_t mem_addr)
{
    if (mem_addr == MIPI_DSI_BRG_MEM_BASE) {
        dev->ch[channel].ctl0.dms = DW_GDMA_LL_MASTER_PORT_MIPI_DSI;
    } else {
        dev->ch[channel].ctl0.dms = DW_GDMA_LL_MASTER_PORT_MEMORY;
    }
}

/**
 * @brief Enable the source address burst mode
 *
 * @note Increase the source address by the data width after each transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param mode Address burst mode
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_src_burst_mode(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_burst_mode_t mode)
{
    dev->ch[channel].ctl0.sinc = mode;
}

/**
 * @brief Enable the destination address burst mode
 *
 * @note Increase the destination address by the data width after each transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param mode Address burst mode
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_dst_burst_mode(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_burst_mode_t mode)
{
    dev->ch[channel].ctl0.dinc = mode;
}

/**
 * @brief Set the transfer width of the source data
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param width Transfer width
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_src_trans_width(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_transfer_width_t width)
{
    dev->ch[channel].ctl0.src_tr_width = width;
}

/**
 * @brief Set the transfer width of the destination data
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param width Transfer width
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_dst_trans_width(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_transfer_width_t width)
{
    dev->ch[channel].ctl0.dst_tr_width = width;
}

/**
 * @brief Set the number of data items that can be transferred in a single burst transaction for the source master port
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param items Number of data items
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_src_burst_items(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_burst_items_t items)
{
    dev->ch[channel].ctl0.src_msize = items;
}

/**
 * @brief Set the number of data items that can be transferred in a single burst transaction for the destination master port
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param items Number of data items
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_dst_burst_items(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_burst_items_t items)
{
    dev->ch[channel].ctl0.dst_msize = items;
}

/**
 * @brief Set the source burst length
 *
 * @note This controls how many times the DMA controller will ask for data from the source device in a single burst transaction.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param len Burst length
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_src_burst_len(dw_gdma_dev_t *dev, uint8_t channel, uint8_t len)
{
    dev->ch[channel].ctl1.arlen_en = len > 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ch[channel].ctl1, arlen, len);
}

/**
 * @brief Set the destination burst length
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param len Burst length
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_dst_burst_len(dw_gdma_dev_t *dev, uint8_t channel, uint8_t len)
{
    dev->ch[channel].ctl1.awlen_en = len > 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->ch[channel].ctl1, awlen, len);
}

/**
 * @brief Set block markers
 *
 * @note This is only valid for `DW_GDMA_BLOCK_TRANSFER_SHADOW` transfer type
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en_intr True to generate an interrupt when the block transfer is done, false to disable
 * @param is_last True to mark the block transfer as the last one
 * @param is_valid True to mark the block transfer as valid
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_set_block_markers(dw_gdma_dev_t *dev, uint8_t channel, bool en_intr, bool is_last, bool is_valid)
{
    dmac_chn_ctl1_reg_t ctrl1;
    ctrl1.val = dev->ch[channel].ctl1.val;
    ctrl1.shadowreg_or_lli_last = is_last;
    ctrl1.ioc_blktfr = en_intr;
    ctrl1.shadowreg_or_lli_valid = is_valid;
    dev->ch[channel].ctl1.val = ctrl1.val;
}

/**
 * @brief Whether to enable the status write back for the source peripheral
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en True to enable write back, false to disable
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_enable_src_periph_status_write_back(dw_gdma_dev_t *dev, uint8_t channel, bool en)
{
    dev->ch[channel].ctl1.src_stat_en = en;
}

/**
 * @brief Whether to enable the status write back for the destination peripheral
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en True to enable write back, false to disable
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_enable_dst_periph_status_write_back(dw_gdma_dev_t *dev, uint8_t channel, bool en)
{
    dev->ch[channel].ctl1.dst_stat_en = en;
}

/**
 * @brief Set the multi block transfer type for the source peripheral
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param type Multi block transfer type
 */
static inline void dw_gdma_ll_channel_set_src_multi_block_type(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_block_transfer_type_t type)
{
    dev->ch[channel].cfg0.src_multblk_type = type;
}

/**
 * @brief Set the multi block transfer type for the destination peripheral
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param type Multi block transfer type
 */
static inline void dw_gdma_ll_channel_set_dst_multi_block_type(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_block_transfer_type_t type)
{
    dev->ch[channel].cfg0.dst_multblk_type = type;
}

/**
 * @brief Transfer type and flow control
 */
typedef enum {
    DW_GDMA_LL_FLOW_M2M_DMAC, /*!< Flow: memory to memory, controller: DMA engine */
    DW_GDMA_LL_FLOW_M2P_DMAC, /*!< Flow: memory to peripheral, controller: DMA engine */
    DW_GDMA_LL_FLOW_P2M_DMAC, /*!< Flow: peripheral to memory, controller: DMA engine */
    DW_GDMA_LL_FLOW_P2P_DMAC, /*!< Flow: peripheral to peripheral, controller: DMA engine */
    DW_GDMA_LL_FLOW_P2M_SRC,  /*!< Flow: peripheral to memory, controller: source peripheral */
    DW_GDMA_LL_FLOW_P2P_SRC,  /*!< Flow: peripheral to peripheral, controller: source peripheral */
    DW_GDMA_LL_FLOW_M2P_DST,  /*!< Flow: memory to peripheral, controller: destination peripheral */
    DW_GDMA_LL_FLOW_P2P_DST,  /*!< Flow: peripheral to peripheral, controller: destination peripheral */
} dw_gdma_ll_trans_flow_t;

#define _MAKE_GDMA_FLOW_CTRL_CODE(src, dst, con) ((src) << 3 | (dst) << 2 | (con))

/**
 * @brief Set transfer flow controller
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param src_role Source target role
 * @param dst_role Destination target role
 * @param controller Flow controller
 */
static inline void dw_gdma_ll_channel_set_trans_flow(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_role_t src_role, dw_gdma_role_t dst_role, dw_gdma_flow_controller_t controller)
{
    bool src_is_perih = (src_role != DW_GDMA_ROLE_MEM);
    bool dst_is_perih = (dst_role != DW_GDMA_ROLE_MEM);
    uint32_t fc_code = _MAKE_GDMA_FLOW_CTRL_CODE(src_is_perih, dst_is_perih, controller);
    switch (fc_code) {
    case _MAKE_GDMA_FLOW_CTRL_CODE(0, 0, DW_GDMA_FLOW_CTRL_SELF):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_M2M_DMAC;
        break;
    case _MAKE_GDMA_FLOW_CTRL_CODE(0, 1, DW_GDMA_FLOW_CTRL_SELF):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_M2P_DMAC;
        break;
    case _MAKE_GDMA_FLOW_CTRL_CODE(1, 0, DW_GDMA_FLOW_CTRL_SELF):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_P2M_DMAC;
        break;
    case _MAKE_GDMA_FLOW_CTRL_CODE(1, 1, DW_GDMA_FLOW_CTRL_SELF):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_P2P_DMAC;
        break;
    case _MAKE_GDMA_FLOW_CTRL_CODE(1, 0, DW_GDMA_FLOW_CTRL_SRC):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_P2M_SRC;
        break;
    case _MAKE_GDMA_FLOW_CTRL_CODE(1, 1, DW_GDMA_FLOW_CTRL_SRC):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_P2P_SRC;
        break;
    case _MAKE_GDMA_FLOW_CTRL_CODE(0, 1, DW_GDMA_FLOW_CTRL_DST):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_M2P_DST;
        break;
    case _MAKE_GDMA_FLOW_CTRL_CODE(1, 1, DW_GDMA_FLOW_CTRL_DST):
        dev->ch[channel].cfg1.tt_fc = DW_GDMA_LL_FLOW_P2P_DST;
        break;
    default:
        abort();
        break;
    }
}

/**
 * @brief Set the handshaking interface for source requests
 *
 * @note If source peripheral is memory, this configuration is ignored.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param hs Handshaking interface
 */
static inline void dw_gdma_ll_channel_set_src_handshake_interface(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_handshake_type_t hs)
{
    dev->ch[channel].cfg1.hs_sel_src = hs;
}

/**
 * @brief Set the handshaking interface for destination requests
 *
 * @note If destination peripheral is memory, this configuration is ignored.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param hs Handshaking interface
 */
static inline void dw_gdma_ll_channel_set_dst_handshake_interface(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_handshake_type_t hs)
{
    dev->ch[channel].cfg1.hs_sel_dst = hs;
}

/**
 * @brief Set the source handshaking peripheral
 *
 * @note Only valid for hardware handshake interface
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param periph Peripheral ID
 */
static inline void dw_gdma_ll_channel_set_src_handshake_periph(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_role_t periph)
{
    switch (periph) {
    case DW_GDMA_ROLE_PERIPH_DSI:
        dev->ch[channel].cfg1.src_per = DW_GDMA_LL_HW_HANDSHAKE_PERIPH_DSI;
        break;
    case DW_GDMA_ROLE_PERIPH_CSI:
        dev->ch[channel].cfg1.src_per = DW_GDMA_LL_HW_HANDSHAKE_PERIPH_CSI;
        break;
    case DW_GDMA_ROLE_PERIPH_ISP:
        dev->ch[channel].cfg1.src_per = DW_GDMA_LL_HW_HANDSHAKE_PERIPH_ISP;
        break;
    default:
        abort();
        break;
    }
}

/**
 * @brief Set the destination handshaking peripheral
 *
 * @note Only valid for hardware handshake interface
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param periph Peripheral ID
 */
static inline void dw_gdma_ll_channel_set_dst_handshake_periph(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_role_t periph)
{
    switch (periph) {
    case DW_GDMA_ROLE_PERIPH_DSI:
        dev->ch[channel].cfg1.dst_per = DW_GDMA_LL_HW_HANDSHAKE_PERIPH_DSI;
        break;
    case DW_GDMA_ROLE_PERIPH_CSI:
        dev->ch[channel].cfg1.dst_per = DW_GDMA_LL_HW_HANDSHAKE_PERIPH_CSI;
        break;
    case DW_GDMA_ROLE_PERIPH_ISP:
        dev->ch[channel].cfg1.dst_per = DW_GDMA_LL_HW_HANDSHAKE_PERIPH_ISP;
        break;
    default:
        abort();
        break;
    }
}

/**
 * @brief Set channel priority
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param priority Priority number, bigger number means higher priority
 */
static inline void dw_gdma_ll_channel_set_priority(dw_gdma_dev_t *dev, uint8_t channel, uint32_t priority)
{
    dev->ch[channel].cfg1.ch_prior = priority;
}

/**
 * @brief Lock the DMA channel, so the channel can have exclusive access to the bus
 *
 * @note channel locking is supported only for M2M transfer at Block transfer and DMA transfer levels
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param lock_level At which level the lock is applied
 */
static inline void dw_gdma_ll_channel_lock(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_lock_level_t lock_level)
{
    dev->ch[channel].cfg1.lock_ch_l = lock_level;
    dev->ch[channel].cfg1.lock_ch = 1;
}

/**
 * @brief Unlock the DMA channel
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 */
static inline void dw_gdma_ll_channel_unlock(dw_gdma_dev_t *dev, uint8_t channel)
{
    dev->ch[channel].cfg1.lock_ch = 0;
}

/**
 * @brief Set the outstanding request limit for the source peripheral
 *
 * @note The actual number of outstanding requests are limited by the capabilities of the source and destination interfaces.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param limit Outstanding request limit
 */
static inline void dw_gdma_ll_channel_set_src_outstanding_limit(dw_gdma_dev_t *dev, uint8_t channel, uint32_t limit)
{
    dev->ch[channel].cfg1.src_osr_lmt = limit - 1;
}

/**
 * @brief Set the outstanding request limit for the destination peripheral
 *
 * @note The actual number of outstanding requests are limited by the capabilities of the source and destination interfaces.
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param limit Outstanding request limit
 */
static inline void dw_gdma_ll_channel_set_dst_outstanding_limit(dw_gdma_dev_t *dev, uint8_t channel, uint32_t limit)
{
    dev->ch[channel].cfg1.dst_osr_lmt = limit - 1;
}

/**
 * @brief Set the address of the first link list item
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param addr Address of the first link list item, it must be aligned 64 bytes
 */
static inline void dw_gdma_ll_channel_set_link_list_head_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t addr)
{
    dev->ch[channel].llp0.loc0 = addr >> 6;
    dev->ch[channel].llp1.val = 0;
}

/**
 * @brief Get the current link list item address
 *
 * @note When the DMA detects an invalid block, this function can tell which link list item is invalid
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return Address of the current link list item
 */
__attribute__((always_inline))
static inline intptr_t dw_gdma_ll_channel_get_current_link_list_item_addr(dw_gdma_dev_t *dev, uint8_t channel)
{
    return (intptr_t)dev->ch[channel].llp0.loc0 << 6;
}

/**
 * @brief Set the master port of the memory which holds the link list
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param port Master port
 */
static inline void dw_gdma_ll_channel_set_link_list_master_port(dw_gdma_dev_t *dev, uint8_t channel, uint32_t port)
{
    dev->ch[channel].llp0.lms = port;
}

/**
 * @brief Get the total number of data that got transferred
 *
 * @note for normal transfer, this value is the same as the value of `dw_gdma_ll_channel_set_trans_block_size`
 * @note if any error occurs, the transfer might be terminated early, this function returns actual data transferred without error.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return Total number of data that got transferred
 */
static inline uint32_t dw_gdma_ll_channel_get_trans_amount(dw_gdma_dev_t *dev, uint8_t channel)
{
    return dev->ch[channel].status0.cmpltd_blk_tfr_size;
}

/**
 * @brief Get the total number of data left in the channel FIFO after completing the current block transfer
 *
 * @note for normal transfer completion without errors, this function should always return 0
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return Total number of data left in the channel FIFO
 */
static inline uint32_t dw_gdma_ll_channel_get_fifo_remain(dw_gdma_dev_t *dev, uint8_t channel)
{
    return dev->ch[channel].status1.data_left_in_fifo;
}

/**
 * @brief Resume the multi-block transfer
 *
 * @note This function is only valid for linked list or shadow register based multi-block transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_channel_resume_multi_block_transfer(dw_gdma_dev_t *dev, uint8_t channel)
{
    // this register is write-only, we can't do read-modify-write
    dev->ch[channel].blk_tfr_resumereq0.val = 0x01;
}

/**
 * @brief Set the address where to fetch the status of the source peripheral
 *
 * @note Status of the source peripheral can be read by `dw_gdma_ll_channel_get_src_periph_status`
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param addr Address to fetch the status
 */
static inline void dw_gdma_ll_channel_set_src_periph_status_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t addr)
{
    dev->ch[channel].sstatar0.sstatar0 = addr;
}

/**
 * @brief Set the address where to fetch the status of the destination peripheral
 *
 * @note Status of the destination peripheral can be read by `dw_gdma_ll_channel_get_dst_periph_status`
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param addr Address to fetch the status
 */
static inline void dw_gdma_ll_channel_set_dst_periph_status_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t addr)
{
    dev->ch[channel].dstatar0.dstatar0 = addr;
}

/**
 * @brief Get the status of the source peripheral
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return Status of the source peripheral
 */
static inline uint32_t dw_gdma_ll_channel_get_src_periph_status(dw_gdma_dev_t *dev, uint8_t channel)
{
    return dev->ch[channel].sstat0.val;
}

/**
 * @brief Get the status of the destination peripheral
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return Status of the destination peripheral
 */
static inline uint32_t dw_gdma_ll_channel_get_dst_periph_status(dw_gdma_dev_t *dev, uint8_t channel)
{
    return dev->ch[channel].dstat0.val;
}

/**
 * @brief Type of DW-DMA link list item
 */
struct dw_gdma_link_list_item_t {
    dmac_chn_sar0_reg_t sar_lo;   /*!< Source address low 32 bits */
    dmac_chn_sar1_reg_t sar_hi;   /*!< Source address high 32 bits */
    dmac_chn_dar0_reg_t dar_lo;   /*!< Destination address low 32 bits */
    dmac_chn_dar1_reg_t dar_hi;   /*!< Destination address high 32 bits */
    dmac_chn_block_ts0_reg_t block_ts_lo; /*!< Block transfer size, specify the number of data items to be transferred in a block */
    uint32_t reserved_14;
    dmac_chn_llp0_reg_t llp_lo;   /*!< Pointer to the next link list item low 32 bits. Set to zero to indicate the end of the list */
    dmac_chn_llp1_reg_t llp_hi;   /*!< Pointer to the next link list item high 32 bits. Set to zero to indicate the end of the list */
    dmac_chn_ctl0_reg_t ctrl_lo;  /*!< Control word low 32 bits */
    dmac_chn_ctl1_reg_t ctrl_hi;  /*!< Control word high 32 bits */
    dmac_chn_sstat0_reg_t sstat;  /*!< Status of the source peripheral */
    dmac_chn_dstat0_reg_t dstat;  /*!< Status of the destination peripheral */
    dmac_chn_status0_reg_t status_lo; /*!< Channel status low 32 bits */
    dmac_chn_status1_reg_t status_hi; /*!< Channel status high 32 bits */
    uint32_t reserved_38;
    uint32_t reserved_3c;
} __attribute__((aligned(DW_GDMA_LL_LINK_LIST_ALIGNMENT)));

typedef struct dw_gdma_link_list_item_t dw_gdma_link_list_item_t;
ESP_STATIC_ASSERT(sizeof(dw_gdma_link_list_item_t) == DW_GDMA_LL_LINK_LIST_ALIGNMENT, "Invalid size of dw_gdma_link_list_item_t structure");

/**
 * @brief Set the transfer width of the source data
 *
 * @param lli Link list item
 * @param width Transfer width
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_src_trans_width(dw_gdma_link_list_item_t *lli, dw_gdma_transfer_width_t width)
{
    lli->ctrl_lo.src_tr_width = width;
}

/**
 * @brief Set the transfer width of the destination data
 *
 * @param lli Link list item
 * @param width Transfer width
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_dst_trans_width(dw_gdma_link_list_item_t *lli, dw_gdma_transfer_width_t width)
{
    lli->ctrl_lo.dst_tr_width = width;
}

/**
 * @brief Set the source master port based on the memory address
 *
 * @param lli Link list item
 * @param mem_addr Memory address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_src_master_port(dw_gdma_link_list_item_t *lli, intptr_t mem_addr)
{
    if (mem_addr == MIPI_CSI_BRG_MEM_BASE) {
        lli->ctrl_lo.sms = DW_GDMA_LL_MASTER_PORT_MIPI_CSI;
    } else {
        lli->ctrl_lo.sms = DW_GDMA_LL_MASTER_PORT_MEMORY;
    }
}

/**
 * @brief Set the destination master port based on the memory address
 *
 * @param lli Link list item
 * @param mem_addr Memory address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_dst_master_port(dw_gdma_link_list_item_t *lli, intptr_t mem_addr)
{
    if (mem_addr == MIPI_DSI_BRG_MEM_BASE) {
        lli->ctrl_lo.dms = DW_GDMA_LL_MASTER_PORT_MIPI_DSI;
    } else {
        lli->ctrl_lo.dms = DW_GDMA_LL_MASTER_PORT_MEMORY;
    }
}

/**
 * @brief Set the source address of the DMA transfer
 *
 * @param lli Link list item
 * @param src_addr Source address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_src_addr(dw_gdma_link_list_item_t *lli, uint32_t src_addr)
{
    lli->sar_lo.sar0 = src_addr;
}

/**
 * @brief Set the destination address of the DMA transfer
 *
 * @param lli Link list item
 * @param dst_addr Destination address
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_dst_addr(dw_gdma_link_list_item_t *lli, uint32_t dst_addr)
{
    lli->dar_lo.dar0 = dst_addr;
}

/**
 * @brief Set the number of data to be transferred
 *
 * @note "transfer width" * "transfer size" = the total bytes in one block transfer
 *
 * @param lli Link list item
 * @param sz Number of transfer items
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_trans_block_size(dw_gdma_link_list_item_t *lli, uint32_t sz)
{
    lli->block_ts_lo.block_ts = sz - 1;
}

/**
 * @brief Enable the source address burst mode
 *
 * @note Increase the source address by the data width after each transfer
 *
 * @param lli Link list item
 * @param mode Address burst mode
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_src_burst_mode(dw_gdma_link_list_item_t *lli, dw_gdma_burst_mode_t mode)
{
    lli->ctrl_lo.sinc = mode;
}

/**
 * @brief Enable the destination address burst mode
 *
 * @note Increase the destination address by the data width after each transfer
 *
 * @param lli Link list item
 * @param mode Address burst mode
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_dst_burst_mode(dw_gdma_link_list_item_t *lli, dw_gdma_burst_mode_t mode)
{
    lli->ctrl_lo.dinc = mode;
}

/**
 * @brief Set the number of data items that can be transferred in a single burst transaction for the source master port
 *
 * @param lli Link list item
 * @param items Number of data items
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_src_burst_items(dw_gdma_link_list_item_t *lli, dw_gdma_burst_items_t items)
{
    lli->ctrl_lo.src_msize = items;
}

/**
 * @brief Set the number of data items that can be transferred in a single burst transaction for the destination master port
 *
 * @param lli Link list item
 * @param items Number of data items
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_dst_burst_items(dw_gdma_link_list_item_t *lli, dw_gdma_burst_items_t items)
{
    lli->ctrl_lo.dst_msize = items;
}

/**
 * @brief Set the source burst length
 *
 * @note This controls how many times the DMA controller will ask for data from the source device in a single burst transaction.
 *
 * @param lli Link list item
 * @param len Burst length
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_src_burst_len(dw_gdma_link_list_item_t *lli, uint8_t len)
{
    lli->ctrl_hi.arlen_en = len > 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(lli->ctrl_hi, arlen, len);
}

/**
 * @brief Set the destination burst length
 *
 * @param lli Link list item
 * @param len Burst length
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_dst_burst_len(dw_gdma_link_list_item_t *lli, uint8_t len)
{
    lli->ctrl_hi.awlen_en = len > 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(lli->ctrl_hi, awlen, len);
}

/**
 * @brief Set block markers
 *
 * @note This is only valid for `DW_GDMA_BLOCK_TRANSFER_SHADOW` transfer type
 *
 * @param lli Link list item
 * @param en_intr True to generate an interrupt when the block transfer is done, false to disable
 * @param is_last True to mark the block transfer as the last one
 * @param is_valid True to mark the block transfer as valid
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_set_block_markers(dw_gdma_link_list_item_t *lli, bool en_intr, bool is_last, bool is_valid)
{
    lli->ctrl_hi.ioc_blktfr = en_intr;
    lli->ctrl_hi.shadowreg_or_lli_last = is_last;
    lli->ctrl_hi.shadowreg_or_lli_valid = is_valid;
}

/**
 * @brief Whether to enable the status write back for the source peripheral
 *
 * @param lli Link list item
 * @param en True to enable write back, false to disable
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_enable_src_periph_status_write_back(dw_gdma_link_list_item_t *lli, bool en)
{
    lli->ctrl_hi.src_stat_en = en;
}

/**
 * @brief Whether to enable the status write back for the destination peripheral
 *
 * @param lli Link list item
 * @param en True to enable write back, false to disable
 */
__attribute__((always_inline))
static inline void dw_gdma_ll_lli_enable_dst_periph_status_write_back(dw_gdma_link_list_item_t *lli, bool en)
{
    lli->ctrl_hi.dst_stat_en = en;
}

/**
 * @brief Get the status of the source peripheral
 *
 * @param lli Link list item
 * @return Status of the source peripheral
 */
static inline uint32_t dw_gdma_ll_lli_get_src_periph_status(dw_gdma_link_list_item_t *lli)
{
    return lli->sstat.val;
}

/**
 * @brief Get the status of the destination peripheral
 *
 * @param lli Link list item
 * @return Status of the destination peripheral
 */
static inline uint32_t dw_gdma_ll_lli_get_dst_periph_status(dw_gdma_link_list_item_t *lli)
{
    return lli->dstat.val;
}

/**
 * @brief Set the master port of the memory which holds the link list
 *
 * @param lli Link list item
 * @param port Master port
 */
static inline void dw_gdma_ll_lli_set_link_list_master_port(dw_gdma_link_list_item_t *lli, uint32_t port)
{
    lli->llp_lo.lms = port;
}

/**
 * @brief Set the address of the next link list item
 *
 * @param lli Link list item
 * @param addr Address of the next link list item, it must be aligned 64 bytes
 */
static inline void dw_gdma_ll_lli_set_next_item_addr(dw_gdma_link_list_item_t *lli, uint32_t addr)
{
    lli->llp_lo.loc0 = addr >> 6;
    lli->llp_hi.val = 0;
}

/**
 * @brief Get the total number of data that got transferred
 *
 * @note for normal transfer, this value is the same as the value of `dw_gdma_ll_lli_set_trans_block_size`
 * @note if any error occurs, the transfer might be terminated early, this function returns actual data transferred without error.
 *
 * @param lli Link list item
 * @return Total number of data that got transferred
 */
static inline uint32_t dw_gdma_ll_lli_get_trans_amount(dw_gdma_link_list_item_t *lli)
{
    return lli->status_lo.cmpltd_blk_tfr_size;
}

/**
 * @brief Get the total number of data left in the channel FIFO after completing the current block transfer
 *
 * @param lli Link list item
 * @return Total number of data left in the channel FIFO
 */
static inline uint32_t dw_gdma_ll_lli_get_fifo_remain(dw_gdma_link_list_item_t *lli)
{
    return lli->status_hi.data_left_in_fifo;
}

#ifdef __cplusplus
}
#endif
