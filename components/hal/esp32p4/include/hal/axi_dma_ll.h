/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/hal_utils.h"
#include "hal/gdma_types.h"
#include "hal/gdma_ll.h"
#include "soc/axi_dma_struct.h"
#include "soc/axi_dma_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXI_DMA_LL_GET_HW(id) (((id) == 0) ? (&AXI_DMA) : NULL)

// any "dummy" peripheral ID can be used for M2M mode
#define AXI_DMA_LL_M2M_FREE_PERIPH_ID_MASK (0xFFC0)
#define AXI_DMA_LL_INVALID_PERIPH_ID       (0x3F)

///////////////////////////////////// Common /////////////////////////////////////////
/**
 * @brief Force enable register clock
 */
static inline void axi_dma_ll_force_enable_reg_clock(axi_dma_dev_t *dev, bool enable)
{
    dev->misc_conf.clk_en = enable;
}

/**
 * @brief Disable priority arbitration
 *
 * @param dev DMA register base address
 * @param dis True to disable, false to enable
 */
static inline void axi_dma_ll_disable_prio_arb(axi_dma_dev_t *dev, bool dis)
{
    dev->misc_conf.arb_pri_dis = dis;
}

/**
 * @brief Reset DMA FSM (Read and Write)
 *
 * @param dev DMA register base address
 */
static inline void axi_dma_ll_reset_fsm(axi_dma_dev_t *dev)
{
    dev->misc_conf.axim_rst_rd_inter = 1;
    dev->misc_conf.axim_rst_rd_inter = 0;
    dev->misc_conf.axim_rst_wr_inter = 1;
    dev->misc_conf.axim_rst_wr_inter = 0;
}

/**
 * @brief Preset valid memory range for AXI-DMA
 *
 * @param dev DMA register base address
 */
static inline void axi_dma_ll_set_default_memory_range(axi_dma_dev_t *dev)
{
    // AXI-DMA can access L2MEM, L2ROM, MSPI Flash, MSPI PSRAM
    dev->intr_mem_start_addr.val = 0x4FC00000;
    dev->intr_mem_end_addr.val = 0x4FFC0000;
    dev->extr_mem_start_addr.val = 0x40000000;
    dev->extr_mem_end_addr.val = 0x4C000000;
}

///////////////////////////////////// RX /////////////////////////////////////////
/**
 * @brief Get DMA RX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t axi_dma_ll_rx_get_interrupt_status(axi_dma_dev_t *dev, uint32_t channel, bool raw)
{
    if (raw) {
        return dev->in[channel].intr.raw.val;
    } else {
        return dev->in[channel].intr.st.val;
    }
}

/**
 * @brief Enable DMA RX channel interrupt
 */
static inline void axi_dma_ll_rx_enable_interrupt(axi_dma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->in[channel].intr.ena.val |= mask;
    } else {
        dev->in[channel].intr.ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA RX channel interrupt
 */
__attribute__((always_inline))
static inline void axi_dma_ll_rx_clear_interrupt_status(axi_dma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->in[channel].intr.clr.val = mask;
}

/**
 * @brief Get DMA RX channel interrupt status register address
 */
static inline volatile void *axi_dma_ll_rx_get_interrupt_status_reg(axi_dma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->in[channel].intr.st);
}

/**
 * @brief Enable DMA RX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void axi_dma_ll_rx_enable_owner_check(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in[channel].conf.in_conf1.in_check_owner_chn = enable;
}

/**
 * @brief Enable DMA RX channel burst reading data, always enabled
 */
static inline void axi_dma_ll_rx_enable_data_burst(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
}

/**
 * @brief Enable DMA RX channel burst reading descriptor link, disabled by default
 */
static inline void axi_dma_ll_rx_enable_descriptor_burst(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in[channel].conf.in_conf0.indscr_burst_en_chn = enable;
}

/**
 * @brief Set the RX channel burst size
 */
static inline void axi_dma_ll_rx_set_burst_size(axi_dma_dev_t *dev, uint32_t channel, uint32_t sz)
{
    HAL_ASSERT(sz >= 8 && sz <= 128);
    int ctz = __builtin_ctz(sz);
    dev->in[channel].conf.in_conf0.in_burst_size_sel_chn = ctz - 3;
}

/**
 * @brief Reset DMA RX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void axi_dma_ll_rx_reset_channel(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].conf.in_conf0.in_rst_chn = 1;
    dev->in[channel].conf.in_conf0.in_rst_chn = 0;
}

/**
 * @brief Pop data from DMA RX FIFO
 */
static inline uint32_t axi_dma_ll_rx_pop_data(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].conf.in_pop.infifo_pop_chn = 1;
    return dev->in[channel].conf.in_pop.infifo_rdata_chn;
}

/**
 * @brief Set the descriptor link base address for RX channel
 */
__attribute__((always_inline))
static inline void axi_dma_ll_rx_set_desc_addr(axi_dma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->in[channel].conf.in_link2.inlink_addr_chn = addr;
}

/**
 * @brief Start dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void axi_dma_ll_rx_start(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].conf.in_link1.inlink_start_chn = 1;
}

/**
 * @brief Stop dealing with RX descriptors
 */
__attribute__((always_inline))
static inline void axi_dma_ll_rx_stop(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].conf.in_link1.inlink_stop_chn = 1;
}

/**
 * @brief Abort the RX channel, stop the undergoing transfer immediately
 */
__attribute__((always_inline))
static inline void axi_dma_ll_rx_abort(axi_dma_dev_t *dev, uint32_t channel, bool abort)
{
    dev->in[channel].conf.in_conf0.in_cmd_disable_chn = abort;
}

/**
 * @brief Restart a new inlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void axi_dma_ll_rx_restart(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].conf.in_link1.inlink_restart_chn = 1;
}

/**
 * @brief Enable DMA RX to return the address of current descriptor when receives error
 */
static inline void axi_dma_ll_rx_enable_auto_return(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in[channel].conf.in_link1.inlink_auto_ret_chn = enable;
}

/**
 * @brief Check if DMA RX descriptor FSM is in IDLE state
 */
static inline bool axi_dma_ll_rx_is_desc_fsm_idle(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->in[channel].conf.in_link1.inlink_park_chn;
}

/**
 * @brief Get RX success EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t axi_dma_ll_rx_get_success_eof_desc_addr(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->in[channel].conf.in_suc_eof_des_addr.val;
}

/**
 * @brief Get RX error EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t axi_dma_ll_rx_get_error_eof_desc_addr(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->in[channel].conf.in_err_eof_des_addr.val;
}

/**
 * @brief Get the pre-fetched RX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t axi_dma_ll_rx_get_prefetched_desc_addr(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->in[channel].conf.in_dscr.val;
}

/**
 * @brief Set priority for DMA RX channel
 */
static inline void axi_dma_ll_rx_set_priority(axi_dma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->in[channel].conf.in_pri.rx_pri_chn = prio;
}

/**
 * @brief Connect DMA RX channel to a given peripheral
 */
static inline void axi_dma_ll_rx_connect_to_periph(axi_dma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    dev->in[channel].conf.in_peri_sel.peri_in_sel_chn = periph_id;
    dev->in[channel].conf.in_conf0.mem_trans_en_chn = (periph == GDMA_TRIG_PERIPH_M2M);
}

/**
 * @brief Disconnect DMA RX channel from peripheral
 */
static inline void axi_dma_ll_rx_disconnect_from_periph(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].conf.in_peri_sel.peri_in_sel_chn = GDMA_LL_INVALID_PERIPH_ID;
    dev->in[channel].conf.in_conf0.mem_trans_en_chn = false;
}

/**
 * @brief Whether to enable the ETM subsystem for RX channel
 *
 * @note When ETM_EN is 1, only ETM tasks can be used to configure the transfer direction and enable the channel.
 */
static inline void axi_dma_ll_rx_enable_etm_task(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in[channel].conf.in_conf0.in_etm_en_chn = enable;
}

/**
 * @brief Whether to enable access to ecc or aes memory
 */
static inline void axi_dma_ll_rx_enable_ext_mem_ecc_aes_access(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->in[channel].conf.in_conf0.in_ecc_aes_en_chn = enable;
}

/**
 * @brief Return if the channel is ready to be reset
 */
__attribute__((always_inline))
static inline bool axi_dma_ll_rx_is_reset_avail(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->in_reset_avail_chn[channel].in_reset_avail_chn;
}

///////////////////////////////////// TX /////////////////////////////////////////
/**
 * @brief Get DMA TX channel interrupt status word
 */
__attribute__((always_inline))
static inline uint32_t axi_dma_ll_tx_get_interrupt_status(axi_dma_dev_t *dev, uint32_t channel, bool raw)
{
    if (raw) {
        return dev->out[channel].intr.raw.val;
    } else {
        return dev->out[channel].intr.st.val;
    }
}

/**
 * @brief Enable DMA TX channel interrupt
 */
static inline void axi_dma_ll_tx_enable_interrupt(axi_dma_dev_t *dev, uint32_t channel, uint32_t mask, bool enable)
{
    if (enable) {
        dev->out[channel].intr.ena.val |= mask;
    } else {
        dev->out[channel].intr.ena.val &= ~mask;
    }
}

/**
 * @brief Clear DMA TX channel interrupt
 */
__attribute__((always_inline))
static inline void axi_dma_ll_tx_clear_interrupt_status(axi_dma_dev_t *dev, uint32_t channel, uint32_t mask)
{
    dev->out[channel].intr.clr.val = mask;
}

/**
 * @brief Get DMA TX channel interrupt status register address
 */
static inline volatile void *axi_dma_ll_tx_get_interrupt_status_reg(axi_dma_dev_t *dev, uint32_t channel)
{
    return (volatile void *)(&dev->out[channel].intr.st);
}

/**
 * @brief Enable DMA TX channel to check the owner bit in the descriptor, disabled by default
 */
static inline void axi_dma_ll_tx_enable_owner_check(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out[channel].conf.out_conf1.out_check_owner_chn = enable;
}

/**
 * @brief Enable DMA TX channel burst sending data, always enabled
 */
static inline void axi_dma_ll_tx_enable_data_burst(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
}

/**
 * @brief Enable DMA TX channel burst reading descriptor link, disabled by default
 */
static inline void axi_dma_ll_tx_enable_descriptor_burst(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out[channel].conf.out_conf0.outdscr_burst_en_chn = enable;
}

/**
 * @brief Set the TX channel burst size
 */
static inline void axi_dma_ll_tx_set_burst_size(axi_dma_dev_t *dev, uint32_t channel, uint32_t sz)
{
    HAL_ASSERT(sz >= 8 && sz <= 128);
    int ctz = __builtin_ctz(sz);
    dev->out[channel].conf.out_conf0.out_burst_size_sel_chn = ctz - 3;
}

/**
 * @brief Set TX channel EOF mode
 */
static inline void axi_dma_ll_tx_set_eof_mode(axi_dma_dev_t *dev, uint32_t channel, uint32_t mode)
{
    dev->out[channel].conf.out_conf0.out_eof_mode_chn = mode;
}

/**
 * @brief Enable DMA TX channel automatic write results back to descriptor after all data has been sent out, disabled by default
 */
static inline void axi_dma_ll_tx_enable_auto_write_back(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out[channel].conf.out_conf0.out_auto_wrback_chn = enable;
}

/**
 * @brief Reset DMA TX channel FSM and FIFO pointer
 */
__attribute__((always_inline))
static inline void axi_dma_ll_tx_reset_channel(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->out[channel].conf.out_conf0.out_rst_chn = 1;
    dev->out[channel].conf.out_conf0.out_rst_chn = 0;
}

/**
 * @brief Push data into DMA TX FIFO
 */
static inline void axi_dma_ll_tx_push_data(axi_dma_dev_t *dev, uint32_t channel, uint32_t data)
{
    dev->out[channel].conf.out_push.outfifo_wdata_chn = data;
    dev->out[channel].conf.out_push.outfifo_push_chn = 1;
}

/**
 * @brief Set the descriptor link base address for TX channel
 */
__attribute__((always_inline))
static inline void axi_dma_ll_tx_set_desc_addr(axi_dma_dev_t *dev, uint32_t channel, uint32_t addr)
{
    dev->out[channel].conf.out_link2.outlink_addr_chn = addr;
}

/**
 * @brief Start dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void axi_dma_ll_tx_start(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->out[channel].conf.out_link1.outlink_start_chn = 1;
}

/**
 * @brief Stop dealing with TX descriptors
 */
__attribute__((always_inline))
static inline void axi_dma_ll_tx_stop(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->out[channel].conf.out_link1.outlink_stop_chn = 1;
}

/**
 * @brief Abort the TX channel, stop the undergoing transfer immediately
 */
__attribute__((always_inline))
static inline void axi_dma_ll_tx_abort(axi_dma_dev_t *dev, uint32_t channel, bool abort)
{
    dev->out[channel].conf.out_conf0.out_cmd_disable_chn = abort;
}

/**
 * @brief Restart a new outlink right after the last descriptor
 */
__attribute__((always_inline))
static inline void axi_dma_ll_tx_restart(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->out[channel].conf.out_link1.outlink_restart_chn = 1;
}

/**
 * @brief Check if DMA TX descriptor FSM is in IDLE state
 */
static inline bool axi_dma_ll_tx_is_desc_fsm_idle(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->out[channel].conf.out_link1.outlink_park_chn;
}

/**
 * @brief Get TX EOF descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t axi_dma_ll_tx_get_eof_desc_addr(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->out[channel].conf.out_eof_des_addr.val;
}

/**
 * @brief Get the pre-fetched TX descriptor's address
 */
__attribute__((always_inline))
static inline uint32_t axi_dma_ll_tx_get_prefetched_desc_addr(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->out[channel].conf.out_dscr.val;
}

/**
 * @brief Set priority for DMA TX channel
 */
static inline void axi_dma_ll_tx_set_priority(axi_dma_dev_t *dev, uint32_t channel, uint32_t prio)
{
    dev->out[channel].conf.out_pri.tx_pri_chn = prio;
}

/**
 * @brief Connect DMA TX channel to a given peripheral
 */
static inline void axi_dma_ll_tx_connect_to_periph(axi_dma_dev_t *dev, uint32_t channel, gdma_trigger_peripheral_t periph, int periph_id)
{
    (void)periph;
    dev->out[channel].conf.out_peri_sel.peri_out_sel_chn = periph_id;
}

/**
 * @brief Disconnect DMA TX channel from peripheral
 */
static inline void axi_dma_ll_tx_disconnect_from_periph(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->out[channel].conf.out_peri_sel.peri_out_sel_chn = GDMA_LL_INVALID_PERIPH_ID;
}

/**
 * @brief Whether to enable the ETM subsystem for TX channel
 *
 * @note When ETM_EN is 1, only ETM tasks can be used to configure the transfer direction and enable the channel.
 */
static inline void axi_dma_ll_tx_enable_etm_task(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out[channel].conf.out_conf0.out_etm_en_chn = enable;
}

/**
 * @brief Whether to enable access to ecc or aes memory
 */
static inline void axi_dma_ll_tx_enable_ext_mem_ecc_aes_access(axi_dma_dev_t *dev, uint32_t channel, bool enable)
{
    dev->out[channel].conf.out_conf0.out_ecc_aes_en_chn = enable;
}

/**
 * @brief Return if the channel is ready to be reset
 */
__attribute__((always_inline))
static inline bool axi_dma_ll_tx_is_reset_avail(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->out_reset_avail_chn[channel].out_reset_avail_chn;
}

///////////////////////////////////// CRC-TX /////////////////////////////////////////

/**
 * @brief Clear the CRC result for the TX channel
 */
static inline void axi_dma_ll_tx_crc_clear(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->out[channel].crc.out_crc_clear.out_crc_clear_chn_reg = 1;
    dev->out[channel].crc.out_crc_clear.out_crc_clear_chn_reg = 0;
}

/**
 * @brief Set CRC width for TX channel
 */
static inline void axi_dma_ll_tx_crc_set_width(axi_dma_dev_t *dev, uint32_t channel, uint32_t width)
{
    HAL_ASSERT(width <= 16);
    dev->out[channel].crc.tx_crc_width.tx_crc_width_chn = (width - 1) / 8;
}

/**
 * @brief Set CRC initial value for TX channel
 */
static inline void axi_dma_ll_tx_crc_set_init_value(axi_dma_dev_t *dev, uint32_t channel, uint32_t value)
{
    dev->out[channel].crc.out_crc_init_data.out_crc_init_data_chn = value;
}

/**
 * @brief Get CRC result for TX channel
 */
static inline uint32_t axi_dma_ll_tx_crc_get_result(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->out[channel].crc.out_crc_final_result.out_crc_final_result_chn;
}

/**
 * @brief Latch the CRC configuration to the hardware, TX channel
 */
static inline void axi_dma_ll_tx_crc_latch_config(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->out[channel].crc.tx_crc_width.tx_crc_latch_flag_chn = 1;
    dev->out[channel].crc.tx_crc_width.tx_crc_latch_flag_chn = 0;
}

/**
 * @brief Set the lfsr and data mask that used by the Parallel CRC calculation formula for a given CRC bit, TX channel
 */
static inline void axi_dma_ll_tx_crc_set_lfsr_data_mask(axi_dma_dev_t *dev, uint32_t channel, uint32_t crc_bit,
                                                        uint32_t lfsr_mask, uint32_t data_mask, bool reverse_data_mask)
{
    dev->out[channel].crc.tx_crc_en_addr.tx_crc_en_addr_chn = crc_bit;
    dev->out[channel].crc.tx_crc_en_wr_data.tx_crc_en_wr_data_chn = lfsr_mask;
    dev->out[channel].crc.tx_crc_data_en_addr.tx_crc_data_en_addr_chn = crc_bit;
    if (reverse_data_mask) {
        // "& 0xff" because the hardware only support 8-bit data
        data_mask = hal_utils_bitwise_reverse8(data_mask & 0xFF);
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->out[channel].crc.tx_crc_data_en_wr_data, tx_crc_data_en_wr_data_chn, data_mask);
}

///////////////////////////////////// CRC-RX /////////////////////////////////////////

/**
 * @brief Clear the CRC result for the RX channel
 */
static inline void axi_dma_ll_rx_crc_clear(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].crc.in_crc_clear.in_crc_clear_chn_reg = 1;
    dev->in[channel].crc.in_crc_clear.in_crc_clear_chn_reg = 0;
}

/**
 * @brief Set CRC width for RX channel
 */
static inline void axi_dma_ll_rx_crc_set_width(axi_dma_dev_t *dev, uint32_t channel, uint32_t width)
{
    HAL_ASSERT(width <= 16);
    dev->in[channel].crc.rx_crc_width.rx_crc_width_chn = (width - 1) / 8;
}

/**
 * @brief Set CRC initial value for RX channel
 */
static inline void axi_dma_ll_rx_crc_set_init_value(axi_dma_dev_t *dev, uint32_t channel, uint32_t value)
{
    dev->in[channel].crc.in_crc_init_data.in_crc_init_data_chn = value;
}

/**
 * @brief Get CRC result for RX channel
 */
static inline uint32_t axi_dma_ll_rx_crc_get_result(axi_dma_dev_t *dev, uint32_t channel)
{
    return dev->in[channel].crc.in_crc_final_result.in_crc_final_result_chn;
}

/**
 * @brief Latch the CRC configuration to the hardware, RX channel
 */
static inline void axi_dma_ll_rx_crc_latch_config(axi_dma_dev_t *dev, uint32_t channel)
{
    dev->in[channel].crc.rx_crc_width.rx_crc_latch_flag_chn = 1;
    dev->in[channel].crc.rx_crc_width.rx_crc_latch_flag_chn = 0;
}

/**
 * @brief Set the lfsr and data mask that used by the Parallel CRC calculation formula for a given CRC bit, RX channel
 */
static inline void axi_dma_ll_rx_crc_set_lfsr_data_mask(axi_dma_dev_t *dev, uint32_t channel, uint32_t crc_bit,
                                                        uint32_t lfsr_mask, uint32_t data_mask, bool reverse_data_mask)
{
    dev->in[channel].crc.rx_crc_en_addr.rx_crc_en_addr_chn = crc_bit;
    dev->in[channel].crc.rx_crc_en_wr_data.rx_crc_en_wr_data_chn = lfsr_mask;
    dev->in[channel].crc.rx_crc_data_en_addr.rx_crc_data_en_addr_chn = crc_bit;
    if (reverse_data_mask) {
        // "& 0xff" because the hardware only support 8-bit data
        data_mask = hal_utils_bitwise_reverse8(data_mask & 0xFF);
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->in[channel].crc.rx_crc_data_en_wr_data, rx_crc_data_en_wr_data_chn, data_mask);
}

#ifdef __cplusplus
}
#endif
