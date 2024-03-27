/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32P4 eMAC register operations

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/eth_types.h"
#include "soc/emac_dma_struct.h"
#include "soc/emac_mac_struct.h"

#include "soc/hp_system_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lp_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Register configuration */
#define EMAC_LL_INTERFRAME_GAP_96BIT (0)
#define EMAC_LL_INTERFRAME_GAP_88BIT (1)
#define EMAC_LL_INTERFRAME_GAP_80BIT (2)
#define EMAC_LL_INTERFRAME_GAP_72BIT (3)
#define EMAC_LL_INTERFRAME_GAP_64BIT (4)
#define EMAC_LL_INTERFRAME_GAP_56BIT (5)
#define EMAC_LL_INTERFRAME_GAP_48BIT (6)
#define EMAC_LL_INTERFRAME_GAP_40BIT (7)

#define EMAC_LL_BACKOFF_LIMIT_10 (0)
#define EMAC_LL_BACKOFF_LIMIT_8 (1)
#define EMAC_LL_BACKOFF_LIMIT_4 (2)
#define EMAC_LL_BACKOFF_LIMIT_1 (3)

#define EMAC_LL_PREAMBLE_LENGTH_7 (0)
#define EMAC_LL_PREAMBLE_LENGTH_5 (1)
#define EMAC_LL_PREAMBLE_LENGTH_3 (2)

#define EMAC_LL_SOURCE_ADDR_FILTER_DISABLE (0)
#define EMAC_LL_SOURCE_ADDR_FILTER_NORMAL (2)
#define EMAC_LL_SOURCE_ADDR_FILTER_INVERSE (3)

#define EMAC_LL_CONTROL_FRAME_BLOCKALL (0)
#define EMAC_LL_CONTROL_FRAME_FORWARDALL_PAUSE (1)
#define EMAC_LL_CONTROL_FRAME_FORWARDALL (2)
#define EMAC_LL_CONTROL_FRAME_FORWARDFILT (3)

#define EMAC_LL_PAUSE_TIME 0x1648

#define EMAC_LL_PAUSE_LOW_THRESHOLD_MINUS_4 (0)
#define EMAC_LL_PAUSE_LOW_THRESHOLD_MINUS_28 (1)
#define EMAC_LL_PAUSE_LOW_THRESHOLD_MINUS_144 (2)
#define EMAC_LL_PAUSE_LOW_THRESHOLD_MINUS_256 (3)

#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_64 (0)
#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_128 (1)
#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_192 (2)
#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_256 (3)
#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_40 (4)
#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_32 (5)
#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_24 (6)
#define EMAC_LL_TRANSMIT_THRESHOLD_CONTROL_16 (7)

#define EMAC_LL_RECEIVE_THRESHOLD_CONTROL_64 (0)
#define EMAC_LL_RECEIVE_THRESHOLD_CONTROL_32 (1)
#define EMAC_LL_RECEIVE_THRESHOLD_CONTROL_96 (2)
#define EMAC_LL_RECEIVE_THRESHOLD_CONTROL_128 (3)

#define EMAC_LL_DMA_BURST_LENGTH_1BEAT (1)
#define EMAC_LL_DMA_BURST_LENGTH_2BEAT (2)
#define EMAC_LL_DMA_BURST_LENGTH_4BEAT (4)
#define EMAC_LL_DMA_BURST_LENGTH_8BEAT (8)
#define EMAC_LL_DMA_BURST_LENGTH_16BEAT (16)
#define EMAC_LL_DMA_BURST_LENGTH_32BEAT (32)

#define EMAC_LL_DMA_ARBITRATION_ROUNDROBIN_RXTX_1_1 (0)
#define EMAC_LL_DMA_ARBITRATION_ROUNDROBIN_RXTX_2_1 (1)
#define EMAC_LL_DMA_ARBITRATION_ROUNDROBIN_RXTX_3_1 (2)
#define EMAC_LL_DMA_ARBITRATION_ROUNDROBIN_RXTX_4_1 (3)

/* PTP register bits */
#define EMAC_LL_DMAPTPRXDESC_PTPMT_SYNC 0x00000100U                      /* SYNC message (all clock types) */
#define EMAC_LL_DMAPTPRXDESC_PTPMT_FOLLOWUP 0x00000200U                  /* FollowUp message (all clock types) */
#define EMAC_LL_DMAPTPRXDESC_PTPMT_DELAYREQ 0x00000300U                  /* DelayReq message (all clock types) */
#define EMAC_LL_DMAPTPRXDESC_PTPMT_DELAYRESP 0x00000400U                 /* DelayResp message (all clock types) */
#define EMAC_LL_DMAPTPRXDESC_PTPMT_PDELAYREQ_ANNOUNCE 0x00000500U        /* PdelayReq message (peer-to-peer transparent clock) or Announce message (Ordinary or Boundary clock) */
#define EMAC_LL_DMAPTPRXDESC_PTPMT_PDELAYRESP_MANAG 0x00000600U          /* PdelayResp message (peer-to-peer transparent clock) or Management message (Ordinary or Boundary clock)  */
#define EMAC_LL_DMAPTPRXDESC_PTPMT_PDELAYRESPFOLLOWUP_SIGNAL 0x00000700U /* PdelayRespFollowUp message (peer-to-peer transparent clock) or Signaling message (Ordinary or Boundary clock) */

#define EMAC_LL_DMAPTPRXDESC_IPPT_UDP 0x00000001U  /* UDP payload encapsulated in the IP datagram */
#define EMAC_LL_DMAPTPRXDESC_IPPT_TCP 0x00000002U  /* TCP payload encapsulated in the IP datagram */
#define EMAC_LL_DMAPTPRXDESC_IPPT_ICMP 0x00000003U /* ICMP payload encapsulated in the IP datagram */

#define EMAC_LL_DMADESC_OWNER_CPU  (0)
#define EMAC_LL_DMADESC_OWNER_DMA  (1)

/* Interrupt flags (referring to dmastatus register in emac_dma_struct.h) */
#define EMAC_LL_DMA_TRANSMIT_FINISH_INTR               0x00000001U
#define EMAC_LL_DMA_TRANSMIT_STOP_INTR                 0x00000002U
#define EMAC_LL_DMA_TRANSMIT_BUFF_UNAVAILABLE_INTR     0x00000004U
#define EMAC_LL_DMA_TRANSMIT_TIMEOUT_INTR              0x00000008U
#define EMAC_LL_DMA_RECEIVE_OVERFLOW_INTR              0x00000010U
#define EMAC_LL_DMA_TRANSMIT_UNDERFLOW_INTR            0x00000020U
#define EMAC_LL_DMA_RECEIVE_FINISH_INTR                0x00000040U
#define EMAC_LL_DMA_RECEIVE_BUFF_UNAVAILABLE_INTR      0x00000080U
#define EMAC_LL_DMA_RECEIVE_STOP_INTR                  0x00000100U
#define EMAC_LL_DMA_RECEIVE_TIMEOUT_INTR               0x00000200U
#define EMAC_LL_DMA_TRANSMIT_FIRST_BYTE_INTR           0x00000400U
#define EMAC_LL_DMA_FATAL_BUS_ERROR_INRT               0x00001000U
#define EMAC_LL_DMA_RECEIVE_FIRST_BYTE_INTR            0x00002000U
#define EMAC_LL_DMA_ABNORMAL_INTR_SUMMARY              0x00004000U
#define EMAC_LL_DMA_NORMAL_INTR_SUMMARY                0x00008000U
#define EMAC_LL_DMA_POWER_MANAGE_INTR                  0x10000000U
#define EMAC_LL_DMA_TIMESTAMP_TRIGGER_INTR             0x20000000U

/* Interrupt enable (referring to dmain_en register in emac_dma_struct.h) */
#define EMAC_LL_INTR_TRANSMIT_ENABLE                    0x00000001U
#define EMAC_LL_INTR_TRANSMIT_STOP_ENABLE               0x00000002U
#define EMAC_LL_INTR_TRANSMIT_BUFF_UNAVAILABLE_ENABLE   0x00000004U
#define EMAC_LL_INTR_TRANSMIT_TIMEOUT_ENABLE            0x00000008U
#define EMAC_LL_INTR_OVERFLOW_ENABLE                    0x00000010U
#define EMAC_LL_INTR_UNDERFLOW_ENABLE                   0x00000020U
#define EMAC_LL_INTR_RECEIVE_ENABLE                     0x00000040U
#define EMAC_LL_INTR_RECEIVE_BUFF_UNAVAILABLE_ENABLE    0x00000080U
#define EMAC_LL_INTR_RECEIVE_STOP_ENABLE                0x00000100U
#define EMAC_LL_INTR_RECEIVE_TIMEOUT_ENABLE             0x00000200U
#define EMAC_LL_INTR_TRANSMIT_FIRST_BYTE_ENABLE         0x00000400U
#define EMAC_LL_INTR_FATAL_BUS_ERR_ENABLE               0x00002000U
#define EMAC_LL_INTR_RECEIVE_FIRST_BYTE_ENABLE          0x00004000U
#define EMAC_LL_INTR_ABNORMAL_SUMMARY_ENABLE            0x00008000U
#define EMAC_LL_INTR_NORMAL_SUMMARY_ENABLE              0x00010000U

/* Enable needed interrupts (recv/recv_buf_unavailabal/normal must be enabled to make eth work) */
#define EMAC_LL_CONFIG_ENABLE_INTR_MASK    (EMAC_LL_INTR_RECEIVE_ENABLE | EMAC_LL_INTR_NORMAL_SUMMARY_ENABLE)

/************** Start of mac regs operation ********************/
/* emacgmiiaddr */
static inline void emac_ll_set_csr_clock_division(emac_mac_dev_t *mac_regs, uint32_t div_mode)
{
    mac_regs->emacgmiiaddr.miicsrclk = div_mode;
}

static inline bool emac_ll_is_mii_busy(emac_mac_dev_t *mac_regs)
{
    return mac_regs->emacgmiiaddr.miibusy ? true : false;
}

static inline void emac_ll_set_phy_addr(emac_mac_dev_t *mac_regs, uint32_t addr)
{
    mac_regs->emacgmiiaddr.miidev = addr;
}

static inline void emac_ll_set_phy_reg(emac_mac_dev_t *mac_regs, uint32_t reg)
{
    mac_regs->emacgmiiaddr.miireg = reg;
}

static inline void emac_ll_write_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->emacgmiiaddr.miiwrite = enable;
}

static inline void emac_ll_set_busy(emac_mac_dev_t *mac_regs, bool busy)
{
    mac_regs->emacgmiiaddr.miibusy = busy ? 1 : 0;
}

/* gmacconfig */
static inline void emac_ll_watchdog_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.watchdog = !enable;
}

static inline void emac_ll_jabber_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.jabber = !enable;
}

static inline void emac_ll_set_inter_frame_gap(emac_mac_dev_t *mac_regs, uint32_t gap)
{
    mac_regs->gmacconfig.interframegap = gap;
}

static inline void emac_ll_carrier_sense_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.disablecrs = !enable;
}

static inline void emac_ll_set_port_speed(emac_mac_dev_t *mac_regs, eth_speed_t speed)
{
    if (speed == ETH_SPEED_10M || speed == ETH_SPEED_100M) {
        mac_regs->gmacconfig.mii = 1;   // 10_100MBPS
        mac_regs->gmacconfig.fespeed = speed;
    } else {
        mac_regs->gmacconfig.mii = 0;   // 1000MBPS
    }
}

static inline void emac_ll_recv_own_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.rxown = !enable;
}

static inline void emac_ll_loopback_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.loopback = enable;
}

static inline void emac_ll_set_duplex(emac_mac_dev_t *mac_regs, eth_duplex_t duplex)
{
    mac_regs->gmacconfig.duplex = duplex;
}

static inline void emac_ll_checksum_offload_mode(emac_mac_dev_t *mac_regs, eth_checksum_t mode)
{
    mac_regs->gmacconfig.rxipcoffload = mode;
}

static inline void emac_ll_retry_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.retry = !enable;
}

static inline void emac_ll_auto_pad_crc_strip_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.padcrcstrip = enable;
}

static inline void emac_ll_set_back_off_limit(emac_mac_dev_t *mac_regs, uint32_t limit)
{
    mac_regs->gmacconfig.backofflimit = limit;
}

static inline void emac_ll_deferral_check_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.deferralcheck = enable;
}

static inline void emac_ll_set_preamble_length(emac_mac_dev_t *mac_regs, uint32_t len)
{
    mac_regs->gmacconfig.pltf = len;
}

static inline void emac_ll_transmit_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.tx = enable;
}

static inline void emac_ll_receive_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacconfig.rx = enable;
}

/* gmacff */
static inline void emac_ll_receive_all_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacff.receive_all = enable;
}

static inline void emac_ll_set_src_addr_filter(emac_mac_dev_t *mac_regs, uint32_t filter)
{
    mac_regs->gmacff.safe = filter;
}

static inline void emac_ll_sa_inverse_filter_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacff.saif = enable;
}

static inline void emac_ll_set_pass_ctrl_frame_mode(emac_mac_dev_t *mac_regs, uint32_t mode)
{
    mac_regs->gmacff.pcf = mode;
}

static inline void emac_ll_broadcast_frame_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacff.dbf = !enable;
}

static inline void emac_ll_pass_all_multicast_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacff.pam = enable;
}

static inline void emac_ll_da_inverse_filter_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacff.daif = enable;
}

static inline void emac_ll_promiscuous_mode_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacff.pmode = enable;
}

/* gmacfc */
static inline void emac_ll_set_pause_time(emac_mac_dev_t *mac_regs, uint32_t time)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mac_regs->gmacfc, pause_time, time);
}

static inline void emac_ll_zero_quanta_pause_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacfc.dzpq = !enable;
}

static inline void emac_ll_set_pause_low_threshold(emac_mac_dev_t *mac_regs, uint32_t threshold)
{
    mac_regs->gmacfc.plt = threshold;
}

static inline void emac_ll_unicast_pause_frame_detect_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacfc.upfd = enable;
}

static inline void emac_ll_receive_flow_ctrl_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacfc.rfce = enable;
}

static inline void emac_ll_transmit_flow_ctrl_enable(emac_mac_dev_t *mac_regs, bool enable)
{
    mac_regs->gmacfc.tfce = enable;
}

static inline void emac_ll_clear(emac_mac_dev_t *mac_regs)
{
    mac_regs->gmacfc.val = 0;
}

/* emacdebug */
static inline uint32_t emac_ll_transmit_frame_ctrl_status(emac_mac_dev_t *mac_regs)
{
    return mac_regs->emacdebug.mactfcs;
}

static inline uint32_t emac_ll_receive_read_ctrl_state(emac_mac_dev_t *mac_regs)
{
    return mac_regs->emacdebug.mtlrfrcs;
}

static inline uint32_t emac_ll_read_debug_reg(emac_mac_dev_t *mac_regs)
{
    return mac_regs->emacdebug.val;
}

/* emacmiidata */
static inline void emac_ll_set_phy_data(emac_mac_dev_t *mac_regs, uint32_t data)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mac_regs->emacmiidata, mii_data, data);
}

static inline uint32_t emac_ll_get_phy_data(emac_mac_dev_t *mac_regs)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mac_regs->emacmiidata, mii_data);
}

/* emacaddr0 */
static inline void emac_ll_set_addr(emac_mac_dev_t *mac_regs, const uint8_t *addr)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mac_regs->emacaddr0high, address0_hi, (addr[5] << 8) | addr[4]);
    mac_regs->emacaddr0low = (addr[3] << 24) | (addr[2] << 16) | (addr[1] << 8) | (addr[0]);
}
/*************** End of mac regs operation *********************/



/************** Start of dma regs operation ********************/
/* dmabusmode */
static inline void emac_ll_reset(emac_dma_dev_t *dma_regs)
{
    dma_regs->dmabusmode.sw_rst = 1;
}

static inline bool emac_ll_is_reset_done(emac_dma_dev_t *dma_regs)
{
    return dma_regs->dmabusmode.sw_rst ? false : true;
}

/* dmarxbaseaddr / dmatxbaseaddr */
static inline void emac_ll_set_rx_desc_addr(emac_dma_dev_t *dma_regs, uint32_t addr)
{
    dma_regs->dmarxbaseaddr = addr;
}

static inline void emac_ll_set_tx_desc_addr(emac_dma_dev_t *dma_regs, uint32_t addr)
{
    dma_regs->dmatxbaseaddr = addr;
}

/* dmaoperation_mode */
static inline void emac_ll_drop_tcp_err_frame_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.dis_drop_tcpip_err_fram = !enable;
}

static inline void emac_ll_recv_store_forward_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.rx_store_forward = enable;
}

static inline void emac_ll_flush_recv_frame_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.dis_flush_recv_frames = !enable;
}

static inline void emac_ll_trans_store_forward_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.tx_str_fwd = enable;
}

static inline void emac_ll_flush_trans_fifo_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.flush_tx_fifo = enable;
}

static inline bool emac_ll_get_flush_trans_fifo(emac_dma_dev_t *dma_regs)
{
    return dma_regs->dmaoperation_mode.flush_tx_fifo;
}

static inline void emac_ll_set_transmit_threshold(emac_dma_dev_t *dma_regs, uint32_t threshold)
{
    dma_regs->dmaoperation_mode.tx_thresh_ctrl = threshold;
}

static inline void emac_ll_start_stop_dma_transmit(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.start_stop_transmission_command = enable;
}

static inline void emac_ll_forward_err_frame_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.fwd_err_frame = enable;
}

static inline void emac_ll_forward_undersized_good_frame_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.fwd_under_gf = enable;
}

static inline void emac_ll_set_recv_threshold(emac_dma_dev_t *dma_regs, uint32_t threshold)
{
    dma_regs->dmaoperation_mode.rx_thresh_ctrl = threshold;
}

static inline void emac_ll_opt_second_frame_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.opt_second_frame = enable;
}

static inline void emac_ll_start_stop_dma_receive(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmaoperation_mode.start_stop_rx = enable;
}

/* dmabusmode */
static inline void emac_ll_mixed_burst_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmabusmode.dmamixedburst = enable;
}

static inline void emac_ll_addr_align_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmabusmode.dmaaddralibea = enable;
}

static inline void emac_ll_use_separate_pbl_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmabusmode.use_sep_pbl = enable;
}

static inline void emac_ll_set_rx_dma_pbl(emac_dma_dev_t *dma_regs, uint32_t pbl)
{
    dma_regs->dmabusmode.rx_dma_pbl = pbl;
}

static inline void emac_ll_set_prog_burst_len(emac_dma_dev_t *dma_regs, eth_mac_dma_burst_len_t dma_burst_len)
{
    dma_regs->dmabusmode.prog_burst_len =   dma_burst_len == ETH_DMA_BURST_LEN_1 ? EMAC_LL_DMA_BURST_LENGTH_1BEAT :
                                            dma_burst_len == ETH_DMA_BURST_LEN_2 ? EMAC_LL_DMA_BURST_LENGTH_2BEAT :
                                            dma_burst_len == ETH_DMA_BURST_LEN_4 ? EMAC_LL_DMA_BURST_LENGTH_4BEAT :
                                            dma_burst_len == ETH_DMA_BURST_LEN_8 ? EMAC_LL_DMA_BURST_LENGTH_8BEAT :
                                            dma_burst_len == ETH_DMA_BURST_LEN_16 ? EMAC_LL_DMA_BURST_LENGTH_16BEAT :
                                            EMAC_LL_DMA_BURST_LENGTH_32BEAT;
}

static inline void emac_ll_enhance_desc_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmabusmode.alt_desc_size = enable;
}

static inline void emac_ll_set_desc_skip_len(emac_dma_dev_t *dma_regs, uint32_t len)
{
    dma_regs->dmabusmode.desc_skip_len = len;
}

static inline void emac_ll_fixed_arbitration_enable(emac_dma_dev_t *dma_regs, bool enable)
{
    dma_regs->dmabusmode.dma_arb_sch = enable;
}

static inline void emac_ll_set_priority_ratio(emac_dma_dev_t *dma_regs, uint32_t ratio)
{
    dma_regs->dmabusmode.pri_ratio = ratio;
}

/* dmain_en */
static inline void emac_ll_enable_all_intr(emac_dma_dev_t *dma_regs)
{
    dma_regs->dmain_en.val = 0xFFFFFFFF;
}

static inline void emac_ll_disable_all_intr(emac_dma_dev_t *dma_regs)
{
    dma_regs->dmain_en.val = 0x00000000;
}

static inline void emac_ll_enable_corresponding_intr(emac_dma_dev_t *dma_regs, uint32_t mask)
{
    dma_regs->dmain_en.val |= mask;
}

static inline void emac_ll_disable_corresponding_intr(emac_dma_dev_t *dma_regs, uint32_t mask)
{
    dma_regs->dmain_en.val &= ~mask;
}

static inline uint32_t emac_ll_get_intr_enable_status(emac_dma_dev_t *dma_regs)
{
    return dma_regs->dmain_en.val;
}

/* dmastatus */
__attribute__((always_inline)) static inline uint32_t emac_ll_get_intr_status(emac_dma_dev_t *dma_regs)
{
    return dma_regs->dmastatus.val;
}

__attribute__((always_inline)) static inline void emac_ll_clear_corresponding_intr(emac_dma_dev_t *dma_regs, uint32_t bits)
{
    dma_regs->dmastatus.val = bits;
}

__attribute__((always_inline)) static inline void emac_ll_clear_all_pending_intr(emac_dma_dev_t *dma_regs)
{
    dma_regs->dmastatus.val = 0xFFFFFFFF;
}


/* dmatxpolldemand / dmarxpolldemand */
static inline void emac_ll_transmit_poll_demand(emac_dma_dev_t *dma_regs, uint32_t val)
{
    dma_regs->dmatxpolldemand = val;
}
static inline void emac_ll_receive_poll_demand(emac_dma_dev_t *dma_regs, uint32_t val)
{
    dma_regs->dmarxpolldemand = val;
}

/*************** End of dma regs operation *********************/


/**
 * @brief Enable the bus clock for the EMAC module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void emac_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_emac_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define emac_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; emac_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the EMAC module
 *
 * @param group_id Group ID
 */
static inline void emac_ll_reset_register(int group_id)
{
    (void)group_id;
    LP_AON_CLKRST.hp_sdmmc_emac_rst_ctrl.rst_en_emac = 1;
    LP_AON_CLKRST.hp_sdmmc_emac_rst_ctrl.rst_en_emac = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define emac_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; emac_ll_reset_register(__VA_ARGS__)

static inline eth_data_interface_t emac_ll_get_phy_intf(void *ext_regs)
{
    if (HP_SYSTEM.sys_gmac_ctrl0.sys_phy_intf_sel == 0x04) {
        return EMAC_DATA_INTERFACE_RMII;
    }
    return EMAC_DATA_INTERFACE_MII;
}

static inline void emac_ll_clock_enable_mii(void *ext_regs)
{
    HP_SYSTEM.sys_gmac_ctrl0.sys_phy_intf_sel = 0x0;

    HP_SYS_CLKRST.peri_clk_ctrl00.reg_pad_emac_ref_clk_en = 0;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rmii_clk_en = 0;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rmii_clk_src_sel = 0;

    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rx_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rx_clk_src_sel = 1; // 0-pad_emac_txrx_clk, 1-pad_emac_rx_clk
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_rx_clk_div_num = 0; // 25MHz

    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_tx_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_tx_clk_src_sel = 1; // 0-pad_emac_txrx_clk, 1-pad_emac_tx_clk
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_tx_clk_div_num = 0; // 25MHz

    LP_AON_CLKRST.hp_clk_ctrl.hp_pad_emac_tx_clk_en = 1;
    LP_AON_CLKRST.hp_clk_ctrl.hp_pad_emac_rx_clk_en = 1;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define emac_ll_clock_enable_mii(...) (void)__DECLARE_RCC_ATOMIC_ENV; emac_ll_clock_enable_mii(__VA_ARGS__)

static inline void emac_ll_clock_enable_rmii_input(void *ext_regs)
{
    HP_SYSTEM.sys_gmac_ctrl0.sys_phy_intf_sel = 0x4; // set RMII

    HP_SYS_CLKRST.peri_clk_ctrl00.reg_pad_emac_ref_clk_en = 0;

    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rmii_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rmii_clk_src_sel = 0; // 0-pad_emac_txrx_clk, 1-pad_emac_rx_clk, 2-pad_emac_tx_clk

    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rx_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_emac_rx_clk_src_sel = 0; // 0-pad_emac_txrx_clk, 1-pad_emac_rx_clk
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_rx_clk_div_num = 1; // set default divider

    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_tx_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_tx_clk_src_sel = 0; // 0-pad_emac_txrx_clk, 1-pad_emac_tx_clk
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_tx_clk_div_num = 1; // set default divider

    LP_AON_CLKRST.hp_clk_ctrl.hp_pad_emac_tx_clk_en = 0;
    LP_AON_CLKRST.hp_clk_ctrl.hp_pad_emac_rx_clk_en = 0;
    LP_AON_CLKRST.hp_clk_ctrl.hp_pad_emac_txrx_clk_en = 1;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define emac_ll_clock_enable_rmii_input(...) (void)__DECLARE_RCC_ATOMIC_ENV; emac_ll_clock_enable_rmii_input(__VA_ARGS__)

static inline void emac_ll_clock_rmii_rx_tx_div(void *ext_regs, int div)
{
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_rx_clk_div_num = div;
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_emac_tx_clk_div_num = div;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define emac_ll_clock_rmii_rx_tx_div(...) (void)__DECLARE_RCC_ATOMIC_ENV; emac_ll_clock_rmii_rx_tx_div(__VA_ARGS__)

static inline void emac_ll_clock_enable_rmii_output(void *ext_regs)
{
    HP_SYSTEM.sys_gmac_ctrl0.sys_phy_intf_sel = 0x4; // set RMII

    HP_SYS_CLKRST.peri_clk_ctrl00.reg_pad_emac_ref_clk_en = 1;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define emac_ll_clock_enable_rmii_output(...) (void)__DECLARE_RCC_ATOMIC_ENV; emac_ll_clock_enable_rmii_output(__VA_ARGS__)

static inline void emac_ll_pause_frame_enable(void *ext_regs, bool enable)
{
    HP_SYSTEM.sys_gmac_ctrl0.sys_phy_intf_sel = enable;
}

#ifdef __cplusplus
}
#endif
