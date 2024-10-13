/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32 SDIO slave register operations
// It's strange but `tx_*` regs for host->slave transfers while `rx_*` regs for slave->host transfers
// To reduce ambiguity, we call (host->slave, tx) transfers receiving and (slave->host, rx) transfers receiving

#pragma once

#include "hal/sdio_slave_types.h"
#include "hal/misc.h"
#include "soc/slc_struct.h"
#include "soc/slc_reg.h"
#include "soc/host_struct.h"
#include "soc/host_reg.h"
#include "soc/hinf_struct.h"
#include "soc/lldesc.h"
#include "soc/dport_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Get address of the only SLC registers for ESP32
#define sdio_slave_ll_get_slc(ID)   (&SLC)
/// Get address of the only HOST registers for ESP32
#define sdio_slave_ll_get_host(ID)  (&HOST)
/// Get address of the only HINF registers for ESP32
#define sdio_slave_ll_get_hinf(ID)  (&HINF)

/*
 *  SLC2 DMA Desc struct, aka sdio_slave_ll_desc_t
 *
 * --------------------------------------------------------------
 * | own | EoF | sub_sof | 5'b0   | length [11:0] | size [11:0] |
 * --------------------------------------------------------------
 * |            buf_ptr [31:0]                                  |
 * --------------------------------------------------------------
 * |            next_desc_ptr [31:0]                            |
 * --------------------------------------------------------------
 */

/* this bitfield is start from the LSB!!! */
typedef struct sdio_slave_ll_desc_s {
    volatile uint32_t size  : 12,
             length: 12,
             offset: 5, /* starting from bit24, h/w reserved 5bit, s/w use it as offset in buffer */
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
static inline void _sdio_slave_ll_enable_bus_clock(bool enable)
{
    uint32_t reg_val = DPORT_READ_PERI_REG(DPORT_WIFI_CLK_EN_REG);
    reg_val &= ~DPORT_WIFI_CLK_SDIOSLAVE_EN;
    reg_val |= enable << 4;
    DPORT_WRITE_PERI_REG(DPORT_WIFI_CLK_EN_REG, reg_val);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdio_slave_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _sdio_slave_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the SDIO slave module
 */
static inline void _sdio_slave_ll_reset_register(void)
{
    DPORT_WRITE_PERI_REG(DPORT_CORE_RST_EN_REG, DPORT_SDIO_RST);
    DPORT_WRITE_PERI_REG(DPORT_CORE_RST_EN_REG, 0);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdio_slave_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; _sdio_slave_ll_reset_register(__VA_ARGS__)

/**
 * Initialize the hardware.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_init(slc_dev_t *slc)
{
    slc->slc0_int_ena.val = 0;

    slc->conf0.slc0_rx_auto_wrback = 1;
    slc->conf0.slc0_token_auto_clr = 0;
    slc->conf0.slc0_rx_loop_test = 0;
    slc->conf0.slc0_tx_loop_test = 0;

    slc->conf1.slc0_rx_stitch_en = 0;
    slc->conf1.slc0_tx_stitch_en = 0;
    slc->conf1.slc0_len_auto_clr = 0;

    slc->rx_dscr_conf.slc0_token_no_replace = 1;
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
        host->conf.frc_sdio20 = 0x1f;
        host->conf.frc_sdio11 = 0;
        host->conf.frc_pos_samp = 0x1f;
        host->conf.frc_neg_samp = 0;
        break;
    case SDIO_SLAVE_TIMING_PSEND_NSAMPLE:
        host->conf.frc_sdio20 = 0x1f;
        host->conf.frc_sdio11 = 0;
        host->conf.frc_pos_samp = 0;
        host->conf.frc_neg_samp = 0x1f;
        break;
    case SDIO_SLAVE_TIMING_NSEND_PSAMPLE:
        host->conf.frc_sdio20 = 0;
        host->conf.frc_sdio11 = 0x1f;
        host->conf.frc_pos_samp = 0x1f;
        host->conf.frc_neg_samp = 0;
        break;
    case SDIO_SLAVE_TIMING_NSEND_NSAMPLE:
        host->conf.frc_sdio20 = 0;
        host->conf.frc_sdio11 = 0x1f;
        host->conf.frc_pos_samp = 0;
        host->conf.frc_neg_samp = 0x1f;
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
    slc->conf0.slc0_rx_rst = 1;
    slc->conf0.slc0_rx_rst = 0;
}

/**
 * Start the sending DMA with the given descriptor.
 *
 * @param slc Address of the SLC registers
 * @param desc Descriptor to send
 */
static inline void sdio_slave_ll_send_start(slc_dev_t *slc, const sdio_slave_ll_desc_t *desc)
{
    slc->slc0_rx_link.addr = (uint32_t)desc;
    slc->slc0_rx_link.start = 1;
}

/**
 * Write the PKT_LEN register to be written by the host to a certain value.
 *
 * @param slc Address of the SLC registers
 * @param len Length to write
 */
static inline void sdio_slave_ll_send_write_len(slc_dev_t *slc, uint32_t len)
{
    slc->slc0_len_conf.val = FIELD_TO_VALUE2(SLC_SLC0_LEN_WDATA, len) | FIELD_TO_VALUE2(SLC_SLC0_LEN_WR, 1);
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
    return host->pkt_len.reg_slc0_len;
}

/**
 * Enable the rx_done interrupt. (sending)
 *
 * @param slc Address of the SLC registers
 * @param ena true if enable, otherwise false.
 */
static inline void sdio_slave_ll_send_part_done_intr_ena(slc_dev_t *slc, bool ena)
{
    slc->slc0_int_ena.rx_done = (ena ? 1 : 0);
}

/**
 * Clear the rx_done interrupt. (sending)
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_send_part_done_clear(slc_dev_t *slc)
{
    slc->slc0_int_clr.rx_done = 1;
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
    return slc->slc0_int_raw.rx_done;
}

/**
 * Stop the sending DMA.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_send_stop(slc_dev_t *slc)
{
    slc->slc0_rx_link.stop = 1;
}

/**
 * Enable the sending interrupt (rx_eof).
 *
 * @param slc Address of the SLC registers
 * @param ena true to enable, false to disable
 */
static inline void sdio_slave_ll_send_intr_ena(slc_dev_t *slc, bool ena)
{
    slc->slc0_int_ena.rx_eof = (ena ? 1 : 0);
}

/**
 * Clear the sending interrupt (rx_eof).
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_send_intr_clr(slc_dev_t *slc)
{
    slc->slc0_int_clr.rx_eof = 1;
}

/**
 * Check whether the sending is done.
 *
 * @param slc Address of the SLC registers
 * @return true if done, otherwise false
 */
static inline bool sdio_slave_ll_send_done(slc_dev_t *slc)
{
    return slc->slc0_int_st.rx_eof != 0;
}

/**
 * Clear the host interrupt indicating the slave having packet to be read.
 *
 * @param host Address of the host registers
 */
static inline void sdio_slave_ll_send_hostint_clr(host_dev_t *host)
{
    host->slc0_int_clr.rx_new_packet = 1;
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
    slc->slc0_int_ena.tx_done = (ena ? 1 : 0);
}

/**
 * Start receiving DMA with the given descriptor.
 *
 * @param slc Address of the SLC registers
 * @param desc Descriptor of the receiving buffer.
 */
static inline void sdio_slave_ll_recv_start(slc_dev_t *slc, sdio_slave_ll_desc_t *desc)
{
    slc->slc0_tx_link.addr = (uint32_t)desc;
    slc->slc0_tx_link.start = 1;
}

/**
 * Increase the receiving buffer counter by 1.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_size_inc(slc_dev_t *slc)
{
    // fields wdata and inc_more should be written by the same instruction.
    slc->slc0_token1.val = FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_WDATA, 1) | FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_INC_MORE, 1);
}

/**
 * Reset the receiving buffer.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_size_reset(slc_dev_t *slc)
{
    slc->slc0_token1.val = FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_WDATA, 0) | FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_WR, 1);
}

/**
 * Check whether there is a receiving finished event.
 *
 * @param slc Address of the SLC registers
 * @return
 */
static inline bool sdio_slave_ll_recv_done(slc_dev_t *slc)
{
    return slc->slc0_int_raw.tx_done != 0;
}

/**
 * Clear the receiving finished interrupt.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_done_clear(slc_dev_t *slc)
{
    slc->slc0_int_clr.tx_done = 1;
}

/**
 * Restart the DMA. Call after you modified the next pointer of the tail descriptor to the appended
 * descriptor.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_restart(slc_dev_t *slc)
{
    slc->slc0_tx_link.restart = 1;
}

/**
 * Reset the receiving DMA.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_reset(slc_dev_t *slc)
{
    slc->conf0.slc0_tx_rst = 1;
    slc->conf0.slc0_tx_rst = 0;
}

/**
 * Stop the receiving DMA.
 *
 * @param slc Address of the SLC registers
 */
static inline void sdio_slave_ll_recv_stop(slc_dev_t *slc)
{
    slc->slc0_tx_link.stop = 1;
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
    return (intptr_t) & (host->conf_w0) + pos + (pos > 23 ? 4 : 0) + (pos > 31 ? 12 : 0);
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
    return (sdio_slave_hostint_t)host->slc0_func1_int_ena.val;
}

/**
 * Set the interrupt enable bits for the host.
 *
 * @param host Address of the host registers
 * @param mask Mask of interrupts to enable
 */
static inline void sdio_slave_ll_host_set_intena(host_dev_t *host, const sdio_slave_hostint_t *mask)
{
    host->slc0_func1_int_ena.val = (*mask);
}

/**
 * Clear the interrupt bits for the host.
 * @param host Address of the host registers
 * @param mask Mask of interrupts to clear.
 */
static inline void sdio_slave_ll_host_intr_clear(host_dev_t *host, const sdio_slave_hostint_t *mask)
{
    host->slc0_int_clr.val = (*mask);
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(slc->intvec_tohost, slc0_intvec, *mask);
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
    slc->slc0_int_ena.val = (slc->slc0_int_ena.val & (~0xff)) | ((*mask) & 0xff);
}

/**
 * Fetch the slave interrupts (send from host) and clear them.
 *
 * @param slc Address of the SLC registers
 * @param out_slv_int Output of the slave interrupts fetched and cleared.
 */
static inline void sdio_slave_ll_slvint_fetch_clear(slc_dev_t *slc, sdio_slave_ll_slvint_t *out_slv_int)
{
    sdio_slave_ll_slvint_t slv_int = (sdio_slave_ll_slvint_t)(slc->slc0_int_st.val & 0xff);
    *out_slv_int = slv_int;
    slc->slc0_int_clr.val = slv_int;
}

#ifdef __cplusplus
}
#endif
