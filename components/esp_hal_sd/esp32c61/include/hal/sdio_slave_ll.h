/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for SDIO slave register operations
// It's strange but `tx_*` regs for host->slave transfers while `rx_*` regs for slave->host transfers
// To reduce ambiguity, we call (host->slave, tx) transfers receiving and (slave->host, rx) transfers receiving

#pragma once

#include <sys/queue.h>
#include <stdbool.h>
#include "hal/sdio_slave_types.h"
#include "hal/misc.h"
#include "soc/sdio_slc_struct.h"
#include "soc/sdio_slc_reg.h"
#include "soc/sdio_slc_host_struct.h"
#include "soc/sdio_slc_host_reg.h"
#include "soc/sdio_hinf_struct.h"
#include "soc/pcr_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Get address of the only SLC registers
#define sdio_slave_ll_get_slc(ID)   (&SLC)
/// Get address of the only HOST registers
#define sdio_slave_ll_get_host(ID)  (&HOST)
/// Get address of the only HINF registers
#define sdio_slave_ll_get_hinf(ID)  (&HINF)
/// Get the mask of the interrupt status.
#define sdio_slave_ll_intr_status_mask  (0xff | SDIO_SLC0_RX_DONE_INT_ST | SDIO_SLC0_RX_EOF_INT_ST | SDIO_SLC0_TX_DONE_INT_ST)

/*
 *  SLC2 DMA Desc struct, aka sdio_slave_ll_desc_t
 *
 * --------------------------------------------------------------
 * | own | EoF | sub_sof | 1'b0   | length [13:0] | size [13:0] |
 * --------------------------------------------------------------
 * |            buf_ptr [31:0]                                  |
 * --------------------------------------------------------------
 * |            next_desc_ptr [31:0]                            |
 * --------------------------------------------------------------
 */

/* this bitfield is start from the LSB!!! */
typedef struct sdio_slave_ll_desc_s {
    volatile uint32_t size  : 14,
             length: 14,
             offset: 1, /* starting from bit28, h/w reserved 1bit, s/w use it as offset in buffer */
             sosf  : 1, /* start of sub-frame */
             eof   : 1, /* end of frame */
             owner : 1; /* hw or sw */
    volatile const uint8_t *buf;       /* point to buffer data */
    union {
        volatile uint32_t empty;
        STAILQ_ENTRY(sdio_slave_ll_desc_s) qe;  /* pointing to the next desc */
    };
} sdio_slave_ll_desc_t;

/// Mask of general purpose interrupts sending from the host.
typedef enum {
    SDIO_SLAVE_LL_SLVINT_0 = BIT(0),    ///< General purpose interrupt bit 0.
    SDIO_SLAVE_LL_SLVINT_1 = BIT(1),
    SDIO_SLAVE_LL_SLVINT_2 = BIT(2),
    SDIO_SLAVE_LL_SLVINT_3 = BIT(3),
    SDIO_SLAVE_LL_SLVINT_4 = BIT(4),
    SDIO_SLAVE_LL_SLVINT_5 = BIT(5),
    SDIO_SLAVE_LL_SLVINT_6 = BIT(6),
    SDIO_SLAVE_LL_SLVINT_7 = BIT(7),
} sdio_slave_ll_slvint_t;

/**
 * @brief Enable the bus clock for the SDIO slave module
 *
 * @param enable true to enable, false to disable
 */
static inline void sdio_slave_ll_enable_bus_clock(bool enable)
{
    PCR.sdio_slave_conf.sdio_slave_clk_en = enable;
}

/**
 * @brief Reset the SDIO slave module
 */
static inline void sdio_slave_ll_reset_register(void)
{
    PCR.sdio_slave_conf.sdio_slave_rst_en = 1;
    PCR.sdio_slave_conf.sdio_slave_rst_en = 0;
}

/**
 * Initialize the hardware.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_init(slc_dev_t *slc)
{
    slc->slc_slc0int_ena.val = 0;

    slc->slc_conf0.slc0_rx_auto_wrback = 1;
    slc->slc_conf0.slc0_token_auto_clr = 0;
    slc->slc_conf0.slc0_rx_loop_test = 0;
    slc->slc_conf0.slc0_tx_loop_test = 0;

    slc->slc_conf1.slc0_rx_stitch_en = 0;
    slc->slc_conf1.slc0_tx_stitch_en = 0;
    slc->slc_conf1.slc0_len_auto_clr = 0;

    slc->slc_rx_dscr_conf.slc0_token_no_replace = 1;
}

/**
 * Set the timing for the communication
 *
 * @param host Address of the host registers
 * @param timing Timing configuration to set
 */
static inline void sdio_slave_ll_set_timing(host_dev_t *host, sdio_slave_timing_t timing)
{
    switch (timing) {
    case SDIO_SLAVE_TIMING_PSEND_PSAMPLE:
        host->slc_host_conf.slchost_frc_sdio20 = 0x1f;
        host->slc_host_conf.slchost_frc_sdio11 = 0;
        host->slc_host_conf.slchost_frc_pos_samp = 0x1f;
        host->slc_host_conf.slchost_frc_neg_samp = 0;
        break;
    case SDIO_SLAVE_TIMING_PSEND_NSAMPLE:
        host->slc_host_conf.slchost_frc_sdio20 = 0x1f;
        host->slc_host_conf.slchost_frc_sdio11 = 0;
        host->slc_host_conf.slchost_frc_pos_samp = 0;
        host->slc_host_conf.slchost_frc_neg_samp = 0x1f;
        break;
    case SDIO_SLAVE_TIMING_NSEND_PSAMPLE:
        host->slc_host_conf.slchost_frc_sdio20 = 0;
        host->slc_host_conf.slchost_frc_sdio11 = 0x1f;
        host->slc_host_conf.slchost_frc_pos_samp = 0x1f;
        host->slc_host_conf.slchost_frc_neg_samp = 0;
        break;
    case SDIO_SLAVE_TIMING_NSEND_NSAMPLE:
        host->slc_host_conf.slchost_frc_sdio20 = 0;
        host->slc_host_conf.slchost_frc_sdio11 = 0x1f;
        host->slc_host_conf.slchost_frc_pos_samp = 0;
        host->slc_host_conf.slchost_frc_neg_samp = 0x1f;
        break;
    }
}

/**
 * Set the CCCR, SDIO and Physical Layer version
 */
static inline void sdio_slave_ll_init_version(hinf_dev_t *hinf)
{
    hinf->cfg_data1.sdio_ver = 0x232;
}

/**
 * Set the HS supported bit to be read by the host.
 *
 * @param hinf Address of the hinf registers
 * @param hs true if supported, otherwise false.
 */
static inline void sdio_slave_ll_enable_hs(hinf_dev_t *hinf, bool hs)
{
    if (hs) {
        hinf->cfg_data1.highspeed_enable = 1;
    } else {
        hinf->cfg_data1.highspeed_enable = 0;
    }
}

/**
 * Set the IO Ready bit to be read by the host.
 *
 * @param hinf Address of the hinf registers
 * @param ready true if ready, otherwise false.
 */
static inline void sdio_slave_ll_set_ioready(hinf_dev_t *hinf, bool ready)
{
    hinf->cfg_data1.sdio_ioready1 = (ready ? 1 : 0);   //set IO ready to 1 to stop host from using
}

/*---------------------------------------------------------------------------
 *                  Send
 *--------------------------------------------------------------------------*/
/**
 * Reset the sending DMA.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_send_reset(slc_dev_t *slc)
{
    //reset to flush previous packets
    slc->slc_conf0.slc0_rx_rst = 1;
    slc->slc_conf0.slc0_rx_rst = 0;
}

/**
 * Start the sending DMA with the given descriptor.
 *
 * @param slc Address of the SLC registers
 * @param desc Descriptor to send
 */
static inline void sdio_slave_ll_send_start(slc_dev_t *slc, const sdio_slave_ll_desc_t *desc)
{
    slc->slc_slc0rx_link_addr.slc0_rxlink_addr = (uint32_t)desc;
    slc->slc_slc0rx_link.slc0_rxlink_start = 1;
}

/**
 * Write the PKT_LEN register to be written by the host to a certain value.
 *
 * @param slc Address of the SLC registers
 * @param len Length to write
 */
static inline void sdio_slave_ll_send_write_len(slc_dev_t *slc, uint32_t len)
{
    slc->slc_slc0_len_conf.val = FIELD_TO_VALUE2(SDIO_SLC0_LEN_WDATA, len) | FIELD_TO_VALUE2(SDIO_SLC0_LEN_WR, 1);
}

/**
 * Read the value of PKT_LEN register. The register may keep the same until read
 * by the host.
 *
 * @param host Address of the host registers
 * @return The value of PKT_LEN register.
 */
static inline uint32_t sdio_slave_ll_send_read_len(host_dev_t *host)
{
    return host->slc_host_pkt_len.slchost_hostslchost_slc0_len;
}

/**
 * Enable the rx_done interrupt. (sending)
 *
 * @param slc Address of the SLC registers
 * @param ena true if enable, otherwise false.
 */
static inline void sdio_slave_ll_send_part_done_intr_ena(slc_dev_t *slc, bool ena)
{
    slc->slc_slc0int_ena.slc0_rx_done_int_ena = (ena ? 1 : 0);
}

/**
 * Clear the rx_done interrupt. (sending)
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_send_part_done_clear(slc_dev_t *slc)
{
    slc->slc_slc0int_clr.slc0_rx_done_int_clr = 1;
}

/**
 * Check whether the hardware is ready for the SW to use rx_done to invoke
 * the ISR.
 *
 * @param slc Address of the SLC registers
 * @return true if ready, otherwise false.
 */
static inline bool sdio_slave_ll_send_invoker_ready(slc_dev_t *slc)
{
    return slc->slc_slc0int_raw.slc0_rx_done_int_raw;
}

/**
 * Stop the sending DMA.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_send_stop(slc_dev_t *slc)
{
    slc->slc_slc0rx_link.slc0_rxlink_stop = 1;
}

/**
 * Enable the sending interrupt (rx_eof).
 *
 * @param slc Address of the SLC registers
 * @param ena true to enable, false to disable
 */
static inline void sdio_slave_ll_send_intr_ena(slc_dev_t *slc, bool ena)
{
    slc->slc_slc0int_ena.slc0_rx_eof_int_ena = (ena ? 1 : 0);
}

/**
 * Clear the sending interrupt (rx_eof).
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_send_intr_clr(slc_dev_t *slc)
{
    slc->slc_slc0int_clr.slc0_rx_eof_int_clr = 1;
}

/**
 * Check whether the sending is done.
 *
 * @param slc Address of the SLC registers
 * @return true if done, otherwise false
 */
static inline bool sdio_slave_ll_send_done(slc_dev_t *slc)
{
    return slc->slc_slc0int_st.slc0_rx_eof_int_st != 0;
}

/**
 * Clear the host interrupt indicating the slave having packet to be read.
 *
 * @param host Address of the host registers
 */
static inline void sdio_slave_ll_send_hostint_clr(host_dev_t *host)
{
    host->slc_host_slc0host_int_clr.slchost_slc0_rx_new_packet_int_clr = 1;
}

/*---------------------------------------------------------------------------
 *                  Receive
 *--------------------------------------------------------------------------*/
/**
 * Enable the receiving interrupt.
 *
 * @param slc Address of the SLC registers
 * @param ena
 */
static inline void sdio_slave_ll_recv_intr_ena(slc_dev_t *slc, bool ena)
{
    slc->slc_slc0int_ena.slc0_tx_done_int_ena = (ena ? 1 : 0);
}

/**
 * Start receiving DMA with the given descriptor.
 *
 * @param slc Address of the SLC registers
 * @param desc Descriptor of the receiving buffer.
 */
static inline void sdio_slave_ll_recv_start(slc_dev_t *slc, sdio_slave_ll_desc_t *desc)
{
    slc->slc_slc0tx_link_addr.slc0_txlink_addr = (uint32_t)desc;
    slc->slc_slc0tx_link.slc0_txlink_start = 1;
}

/**
 * Increase the receiving buffer counter by 1.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_size_inc(slc_dev_t *slc)
{
    // fields wdata and inc_more should be written by the same instruction.
    slc->slc_slc0token1.val = FIELD_TO_VALUE2(SDIO_SLC0_TOKEN1_WDATA, 1) | FIELD_TO_VALUE2(SDIO_SLC0_TOKEN1_INC_MORE, 1);
}

/**
 * Reset the receiving buffer.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_size_reset(slc_dev_t *slc)
{
    slc->slc_slc0token1.val = FIELD_TO_VALUE2(SDIO_SLC0_TOKEN1_WDATA, 0) | FIELD_TO_VALUE2(SDIO_SLC0_TOKEN1_WR, 1);
}

/**
 * Check whether there is a receiving finished event.
 *
 * @param slc Address of the SLC registers
 * @return
 */
static inline bool sdio_slave_ll_recv_done(slc_dev_t *slc)
{
    return slc->slc_slc0int_raw.slc0_tx_done_int_raw != 0;
}

/**
 * Clear the receiving finished interrupt.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_done_clear(slc_dev_t *slc)
{
    slc->slc_slc0int_clr.slc0_tx_done_int_clr = 1;
}

/**
 * Restart the DMA. Call after you modified the next pointer of the tail descriptor to the appended
 * descriptor.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_restart(slc_dev_t *slc)
{
    slc->slc_slc0tx_link.slc0_txlink_restart = 1;
}

/**
 * Reset the receiving DMA.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_reset(slc_dev_t *slc)
{
    slc->slc_conf0.slc0_tx_rst = 1;
    slc->slc_conf0.slc0_tx_rst = 0;
}

/**
 * Stop the receiving DMA.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_stop(slc_dev_t *slc)
{
    slc->slc_slc0tx_link.slc0_txlink_stop = 1;
}

/*---------------------------------------------------------------------------
 *                  Host
 *--------------------------------------------------------------------------*/
/**
 * Get the address of the shared general purpose register. Internal.
 *
 * @param host Address of the host registers
 * @param pos Position of the register, 0-63 except 24-27.
 * @return address of the register.
 */
static inline intptr_t sdio_slave_ll_host_get_w_reg(host_dev_t *host, int pos)
{
    return (intptr_t) & (host->slc_host_conf_w0) + pos + (pos > 23 ? 4 : 0) + (pos > 31 ? 12 : 0);
}

/**
 * Get the value of the shared general purpose register.
 *
 * @param host Address of the host registers
 * @param pos Position of the register, 0-63, except 24-27.
 * @return value of the register.
 */
static inline uint8_t sdio_slave_ll_host_get_reg(host_dev_t *host, int pos)
{
    return *(uint8_t *)sdio_slave_ll_host_get_w_reg(host, pos);
}

/**
 * Set the value of the shared general purpose register.
 *
 * @param host Address of the host registers
 * @param pos Position of the register, 0-63, except 24-27.
 * @param reg Value to set.
 */
static inline void sdio_slave_ll_host_set_reg(host_dev_t *host, int pos, uint8_t reg)
{
    uint32_t *addr = (uint32_t *)(sdio_slave_ll_host_get_w_reg(host, pos) & (~3));
    uint32_t shift = (pos % 4) * 8;
    *addr &= ~(0xff << shift);
    *addr |= ((uint32_t)reg << shift);
}

/**
 * Get the interrupt enable bits for the host.
 *
 * @param host Address of the host registers
 * @return Enabled interrupts
 */
static inline sdio_slave_hostint_t sdio_slave_ll_host_get_intena(host_dev_t *host)
{
    return (sdio_slave_hostint_t)host->slc_host_slc0host_func1_int_ena.val;
}

/**
 * Set the interrupt enable bits for the host.
 *
 * @param host Address of the host registers
 * @param mask Mask of interrupts to enable
 */
static inline void sdio_slave_ll_host_set_intena(host_dev_t *host, const sdio_slave_hostint_t *mask)
{
    host->slc_host_slc0host_func1_int_ena.val = (*mask);
}

/**
 * Clear the interrupt bits for the host.
 * @param host Address of the host registers
 * @param mask Mask of interrupts to clear.
 */
static inline void sdio_slave_ll_host_intr_clear(host_dev_t *host, const sdio_slave_hostint_t *mask)
{
    host->slc_host_slc0host_int_clr.val = (*mask);
}

/**
 * Send general purpose interrupts to the host.
 * @param slc Address of the SLC registers
 * @param mask Mask of interrupts to seend to host
 */
static inline void sdio_slave_ll_host_send_int(slc_dev_t *slc, const sdio_slave_hostint_t *mask)
{
    //use registers in SLC to trigger, rather than write HOST registers directly
    //other interrupts than tohost interrupts are not supported yet
    HAL_FORCE_MODIFY_U32_REG_FIELD(slc->slc_slcintvec_tohost, slc0_tohost_intvec, *mask);
}

/**
 * Enable some of the slave interrupts (send from host)
 *
 * @param slc Address of the SLC registers
 * @param mask Mask of interrupts to enable, all those set to 0 will be disabled.
 */
static inline void sdio_slave_ll_slvint_set_ena(slc_dev_t *slc, const sdio_slave_ll_slvint_t *mask)
{
    //other interrupts are not enabled
    slc->slc_slc0int_ena.val = (slc->slc_slc0int_ena.val & (~0xff)) | ((*mask) & 0xff);
}

/**
 * Fetch the slave interrupts (send from host) and clear them.
 *
 * @param slc Address of the SLC registers
 * @param out_slv_int Output of the slave interrupts fetched and cleared.
 */
static inline void sdio_slave_ll_slvint_fetch_clear(slc_dev_t *slc, sdio_slave_ll_slvint_t *out_slv_int)
{
    sdio_slave_ll_slvint_t slv_int = (sdio_slave_ll_slvint_t)(slc->slc_slc0int_st.val & 0xff);
    *out_slv_int = slv_int;
    slc->slc_slc0int_clr.val = slv_int;
}

/**
 * Get the address of the interrupt status register.
 *
 * @param slc Address of the SLC registers
 * @return Address of the interrupt status register
 */
static inline volatile void* sdio_slave_ll_get_intr_status_reg(slc_dev_t *slc)
{
    return &slc->slc_slc0int_st.val;
}

#ifdef __cplusplus
}
#endif
