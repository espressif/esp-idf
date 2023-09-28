/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/assert.h"
#include "soc/dw_gdma_struct.h"
#include "soc/hp_sys_clkrst_struct.h"

#define DW_GDMA_LL_GET_HW() (&DW_GDMA)

#define DW_GDMA_LL_MASTER_PORT_MIPI_DSI 0 // DW_GDMA master 0 can access DSI bridge
#define DW_GDMA_LL_MASTER_PORT_MIPI_CSI 0 // DW_GDMA master 0 can access CSI bridge
#define DW_GDMA_LL_MASTER_PORT_MEMORY   1 // DW_GDMA master 1 can only access L2MEM & ROM & MSPI Flash/PSRAM

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DW_GDMA transfer width
 */
typedef enum {
    DW_GDMA_LL_TRANS_WIDTH_8,   /*!< Data transfer width: 8 bits */
    DW_GDMA_LL_TRANS_WIDTH_16,  /*!< Data transfer width: 16 bits */
    DW_GDMA_LL_TRANS_WIDTH_32,  /*!< Data transfer width: 32 bits */
    DW_GDMA_LL_TRANS_WIDTH_64,  /*!< Data transfer width: 64 bits */
    DW_GDMA_LL_TRANS_WIDTH_128, /*!< Data transfer width: 128 bits */
    DW_GDMA_LL_TRANS_WIDTH_256, /*!< Data transfer width: 256 bits */
    DW_GDMA_LL_TRANS_WIDTH_512, /*!< Data transfer width: 512 bits */
} dw_gdma_ll_transfer_width_t;

/**
 * @brief DW_GDMA burst items
 */
typedef enum {
    DW_GDMA_LL_BURST_ITEMS_1,    /*!< 1 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_4,    /*!< 4 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_8,    /*!< 8 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_16,   /*!< 16 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_32,   /*!< 32 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_64,   /*!< 64 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_128,  /*!< 128 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_256,  /*!< 256 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_512,  /*!< 512 data items in the burst transaction */
    DW_GDMA_LL_BURST_ITEMS_1024, /*!< 1024 data items in the burst transaction */
} dw_gdma_ll_burst_items_t;

/**
 * @brief Multi block transfer type
 */
typedef enum {
    DW_GDMA_LL_MULTI_BLOCK_CONTIGUOUS,  /*!< Contiguous */
    DW_GDMA_LL_MULTI_BLOCK_RELOAD,      /*!< Reload */
    DW_GDMA_LL_MULTI_BLOCK_SHADOW_REG,  /*!< Shadow register */
    DW_GDMA_LL_MULTI_BLOCK_LINK_LIST,   /*!< Link list */
} dw_gdma_ll_multi_block_type_t;

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

/**
 * @brief Handshake interface
 */
typedef enum {
    DW_GDMA_LL_HANDSHAKE_HW, /*!< Transaction requests are initiated by hardware */
    DW_GDMA_LL_HANDSHAKE_SW, /*!< Transaction requests are initiated by software */
} dw_gdma_ll_handshake_interface_t;

/**
 * @brief Handshake number for different peripherals
 */
typedef enum {
    DW_GDMA_LL_HW_HANDSHAKE_PERIPH_DSI, /*!< Handshake peripheral is DSI */
    DW_GDMA_LL_HW_HANDSHAKE_PERIPH_CSI, /*!< Handshake peripheral is CSI */
    DW_GDMA_LL_HW_HANDSHAKE_PERIPH_ISP, /*!< Handshake peripheral is ISP */
} dw_gdma_ll_hw_handshake_periph_t;

/**
 * @brief Channel lock level
 */
typedef enum {
    DW_GDMA_LL_LOCK_LEVEL_FULL_TRANS,  /*!< Lock over complete DMA transfer */
    DW_GDMA_LL_LOCK_LEVEL_BLOCK_TRANS, /*!< Lock over DMA block transfer */
} dw_gdma_ll_lock_level_t;

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
static inline void dw_gdma_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_gdma = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_gdma = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define dw_gdma_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; dw_gdma_ll_reset_register(__VA_ARGS__)

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
 * @brief Check if the common register interrupt is active
 *
 * @param dev Pointer to the DW_GDMA registers
 * @return True: common register interrupt is active, False: common register interrupt is inactive
 */
static inline bool dw_gdma_ll_is_common_intr_active(dw_gdma_dev_t *dev)
{
    return dev->int_st0.commonreg_intstat;
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
static inline uint32_t dw_gdma_ll_channel_get_inr_status(dw_gdma_dev_t *dev, uint8_t channel)
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
static inline void dw_gdma_ll_channel_abort(dw_gdma_dev_t *dev, uint8_t channel)
{
    // the abort bit clears itself after the abort is done
    dev->chen1.val = 0x101 << channel;
}

/**
 * @brief Check if the DMA channel interrupt is active
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return True: channel interrupt is active, False: channel interrupt is inactive
 */
static inline bool dw_gdma_ll_channel_is_interrupt_active(dw_gdma_dev_t *dev, uint8_t channel)
{
    return dev->int_st0.val & (1 << channel);
}

/**
 * @brief Set the source address of the DMA transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param src_addr Source address
 */
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
static inline void dw_gdma_ll_channel_set_dst_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t dst_addr)
{
    dev->ch[channel].dar0.dar0 = dst_addr;
}

/**
 * @brief Set the number of data to be transferred
 *
 * @note data_transfer_width * item_amount determins the total bytes in one block transfer.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param item_numbers Number of transfer items
 */
static inline void dw_gdma_ll_channel_set_trans_amount(dw_gdma_dev_t *dev, uint8_t channel, uint32_t item_numbers)
{
    dev->ch[channel].block_ts0.block_ts = item_numbers - 1;
}

/**
 * @brief Set the source master port
 *
 * @note The choice of master port depends on the location of the source data.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param port Source master port
 */
static inline void dw_gdma_ll_channel_set_src_master_port(dw_gdma_dev_t *dev, uint8_t channel, uint32_t port)
{
    dev->ch[channel].ctl0.sms = port;
}

/**
 * @brief Set the destination master port
 *
 * @note The choice of master port depends on the location of the destination data.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param port Destination master port
 */
static inline void dw_gdma_ll_channel_set_dst_master_port(dw_gdma_dev_t *dev, uint8_t channel, uint32_t port)
{
    dev->ch[channel].ctl0.dms = port;
}

/**
 * @brief Enable the source address increment
 *
 * @note Increase the source address by the data width after each transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_channel_enable_src_addr_increment(dw_gdma_dev_t *dev, uint8_t channel, bool en)
{
    dev->ch[channel].ctl0.sinc = !en;
}

/**
 * @brief Enable the destination address increment
 *
 * @note Increase the destination address by the data width after each transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_channel_enable_dst_addr_increment(dw_gdma_dev_t *dev, uint8_t channel, bool en)
{
    dev->ch[channel].ctl0.dinc = !en;
}

/**
 * @brief Set the transfer width of the source data
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param width Transfer width
 */
static inline void dw_gdma_ll_channel_set_src_trans_width(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_transfer_width_t width)
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
static inline void dw_gdma_ll_channel_set_dst_trans_width(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_transfer_width_t width)
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
static inline void dw_gdma_ll_channel_set_src_burst_items(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_burst_items_t items)
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
static inline void dw_gdma_ll_channel_set_dst_burst_items(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_burst_items_t items)
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
static inline void dw_gdma_ll_channel_set_src_burst_len(dw_gdma_dev_t *dev, uint8_t channel, uint32_t len)
{
    dev->ch[channel].ctl1.arlen_en = 1;
    dev->ch[channel].ctl1.arlen = len - 1;
}

/**
 * @brief Set the destination burst length
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param len Burst length
 */
static inline void dw_gdma_ll_channel_set_dst_burst_len(dw_gdma_dev_t *dev, uint8_t channel, uint32_t len)
{
    dev->ch[channel].ctl1.awlen_en = 1;
    dev->ch[channel].ctl1.awlen = len - 1;
}

/**
 * @brief Enable to generate an interrupt when the block transfer is done
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param en True to enable, false to disable
 */
static inline void dw_gdma_ll_channel_enable_intr_block_trans_done(dw_gdma_dev_t *dev, uint8_t channel, bool en)
{
    dev->ch[channel].ctl1.ioc_blktfr = en;
}

/**
 * @brief Set the multi block transfer type for the source peripheral
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param type Multi block transfer type
 */
static inline void dw_gdma_ll_channel_set_src_multi_block_type(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_multi_block_type_t type)
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
static inline void dw_gdma_ll_channel_set_dst_multi_block_type(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_multi_block_type_t type)
{
    dev->ch[channel].cfg0.dst_multblk_type = type;
}

/**
 * @brief Set the unique ID for the source peripheral
 *
 * @note This ID is related to Out-of-order transaction
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param uid Unique ID
 */
static inline void dw_gdma_ll_channel_set_src_uid(dw_gdma_dev_t *dev, uint8_t channel, uint32_t uid)
{
    dev->ch[channel].cfg0.rd_uid = uid;
}

/**
 * @brief Set the unique ID for the destination peripheral
 *
 * @note This ID is related to Out-of-order transaction
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param uid Unique ID
 */
static inline void dw_gdma_ll_channel_set_dst_uid(dw_gdma_dev_t *dev, uint8_t channel, uint32_t uid)
{
    dev->ch[channel].cfg0.wr_uid = uid;
}

/**
 * @brief Set transfer type and flow control
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param flow Transfer flow control
 */
static inline void dw_gdma_ll_channel_set_trans_flow(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_trans_flow_t flow)
{
    dev->ch[channel].cfg1.tt_fc = flow;
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
static inline void dw_gdma_ll_channel_set_src_handshake_interface(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_handshake_interface_t hs)
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
static inline void dw_gdma_ll_channel_set_dst_handshake_interface(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_handshake_interface_t hs)
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
static inline void dw_gdma_ll_channel_set_src_handshake_periph(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_hw_handshake_periph_t periph)
{
    dev->ch[channel].cfg1.src_per = periph;
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
static inline void dw_gdma_ll_channel_set_dst_handshake_periph(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_hw_handshake_periph_t periph)
{
    dev->ch[channel].cfg1.dst_per = periph;
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
static inline void dw_gdma_ll_channel_lock(dw_gdma_dev_t *dev, uint8_t channel, dw_gdma_ll_lock_level_t lock_level)
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
 * @param addr Address of the first link list item, it must be aligned 64
 */
static inline void dw_gdma_ll_channel_set_link_list_head_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t addr)
{
    dev->ch[channel].llp0.loc0 = addr >> 6;
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
 * @brief Get the total number of data that transferred for the previous block transfer.
 *
 * @note for normal transfer, this value is the same as the value of `dw_gdma_ll_channel_set_trans_amount`
 * @note if any error occurs, the transfer might be terminated early, this function returns actual data transferred without error.
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @return Total number of data that transferred for the previous block transfer
 */
static inline uint32_t dw_gdma_ll_channel_get_trans_amount(dw_gdma_dev_t *dev, uint8_t channel)
{
    return dev->ch[channel].status0.cmpltd_blk_tfr_size;
}

/**
 * @brief Resume the multi-block transfer
 *
 * @note This function is only valid for linked list or shadow register based multi-block transfer
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 */
static inline void dw_gdma_ll_channel_resume_multi_block_transfer(dw_gdma_dev_t *dev, uint8_t channel)
{
    // this register is write-only, we can't do read-modify-write
    dev->ch[channel].blk_tfr_resumereq0.val = 0x01;
}

/**
 * @brief Set the address to fetch the source status of the DMA channel
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param addr Address to fetch the source status of the DMA channel
 */
static inline void dw_gdma_ll_channel_set_src_status_fetch_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t addr)
{
    dev->ch[channel].sstatar0.sstatar0 = addr;
}

/**
 * @brief Set the address to fetch the destination status of the DMA channel
 *
 * @param dev Pointer to the DW_GDMA registers
 * @param channel Channel number
 * @param addr Address to fetch the destination status of the DMA channel
 */
static inline void dw_gdma_ll_channel_set_dst_status_fetch_addr(dw_gdma_dev_t *dev, uint8_t channel, uint32_t addr)
{
    dev->ch[channel].dstatar0.dstatar0 = addr;
}

#ifdef __cplusplus
}
#endif
