/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/usb_dwc_struct.h"
#include "hal/usb_types_private.h"
#include "hal/misc.h"


/* -----------------------------------------------------------------------------
------------------------------- Global Registers -------------------------------
----------------------------------------------------------------------------- */

/*
 * Interrupt bit masks of the GINTSTS and GINTMSK registers
 */
#define USB_DWC_LL_INTR_CORE_WKUPINT        (1 << 31)
#define USB_DWC_LL_INTR_CORE_SESSREQINT     (1 << 30)
#define USB_DWC_LL_INTR_CORE_DISCONNINT     (1 << 29)
#define USB_DWC_LL_INTR_CORE_CONIDSTSCHNG   (1 << 28)
#define USB_DWC_LL_INTR_CORE_PTXFEMP        (1 << 26)
#define USB_DWC_LL_INTR_CORE_HCHINT         (1 << 25)
#define USB_DWC_LL_INTR_CORE_PRTINT         (1 << 24)
#define USB_DWC_LL_INTR_CORE_RESETDET       (1 << 23)
#define USB_DWC_LL_INTR_CORE_FETSUSP        (1 << 22)
#define USB_DWC_LL_INTR_CORE_INCOMPIP       (1 << 21)
#define USB_DWC_LL_INTR_CORE_INCOMPISOIN    (1 << 20)
#define USB_DWC_LL_INTR_CORE_OEPINT         (1 << 19)
#define USB_DWC_LL_INTR_CORE_IEPINT         (1 << 18)
#define USB_DWC_LL_INTR_CORE_EPMIS          (1 << 17)
#define USB_DWC_LL_INTR_CORE_EOPF           (1 << 15)
#define USB_DWC_LL_INTR_CORE_ISOOUTDROP     (1 << 14)
#define USB_DWC_LL_INTR_CORE_ENUMDONE       (1 << 13)
#define USB_DWC_LL_INTR_CORE_USBRST         (1 << 12)
#define USB_DWC_LL_INTR_CORE_USBSUSP        (1 << 11)
#define USB_DWC_LL_INTR_CORE_ERLYSUSP       (1 << 10)
#define USB_DWC_LL_INTR_CORE_GOUTNAKEFF     (1 << 7)
#define USB_DWC_LL_INTR_CORE_GINNAKEFF      (1 << 6)
#define USB_DWC_LL_INTR_CORE_NPTXFEMP       (1 << 5)
#define USB_DWC_LL_INTR_CORE_RXFLVL         (1 << 4)
#define USB_DWC_LL_INTR_CORE_SOF            (1 << 3)
#define USB_DWC_LL_INTR_CORE_OTGINT         (1 << 2)
#define USB_DWC_LL_INTR_CORE_MODEMIS        (1 << 1)
#define USB_DWC_LL_INTR_CORE_CURMOD         (1 << 0)

/*
 * Bit mask of interrupt generating bits of the the HPRT register. These bits
 * are ORd into the USB_DWC_LL_INTR_CORE_PRTINT interrupt.
 *
 * Note: Some fields of the HPRT are W1C (write 1 clear), this we cannot do a
 * simple read and write-back to clear the HPRT interrupt bits. Instead we need
 * a W1C mask the non-interrupt related bits
 */
#define USB_DWC_LL_HPRT_W1C_MSK             (0x2E)
#define USB_DWC_LL_HPRT_ENA_MSK             (0x04)
#define USB_DWC_LL_INTR_HPRT_PRTOVRCURRCHNG (1 << 5)
#define USB_DWC_LL_INTR_HPRT_PRTENCHNG      (1 << 3)
#define USB_DWC_LL_INTR_HPRT_PRTCONNDET     (1 << 1)

/*
 * Bit mask of channel interrupts (HCINTi and HCINTMSKi registers)
 *
 * Note: Under Scatter/Gather DMA mode, only the following interrupts can be unmasked
 * - DESC_LS_ROLL
 * - XCS_XACT_ERR (always unmasked)
 * - BNAINTR
 * - CHHLTD
 * - XFERCOMPL
 * The remaining interrupt bits will still be set (when the corresponding event occurs)
 * but will not generate an interrupt. Therefore we must proxy through the
 * USB_DWC_LL_INTR_CHAN_CHHLTD interrupt to check the other interrupt bits.
 */
#define USB_DWC_LL_INTR_CHAN_DESC_LS_ROLL   (1 << 13)
#define USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR   (1 << 12)
#define USB_DWC_LL_INTR_CHAN_BNAINTR        (1 << 11)
#define USB_DWC_LL_INTR_CHAN_DATATGLERR     (1 << 10)
#define USB_DWC_LL_INTR_CHAN_FRMOVRUN       (1 << 9)
#define USB_DWC_LL_INTR_CHAN_BBLEER         (1 << 8)
#define USB_DWC_LL_INTR_CHAN_XACTERR        (1 << 7)
#define USB_DWC_LL_INTR_CHAN_NYET           (1 << 6)
#define USB_DWC_LL_INTR_CHAN_ACK            (1 << 5)
#define USB_DWC_LL_INTR_CHAN_NAK            (1 << 4)
#define USB_DWC_LL_INTR_CHAN_STALL          (1 << 3)
#define USB_DWC_LL_INTR_CHAN_AHBERR         (1 << 2)
#define USB_DWC_LL_INTR_CHAN_CHHLTD         (1 << 1)
#define USB_DWC_LL_INTR_CHAN_XFERCOMPL      (1 << 0)

/*
 * QTD (Queue Transfer Descriptor) structure used in Scatter/Gather DMA mode.
 * Each QTD describes one transfer. Scatter gather mode will automatically split
 * a transfer into multiple MPS packets. Each QTD is 64bits in size
 *
 * Note: The status information part of the QTD is interpreted differently depending
 * on IN or OUT, and ISO or non-ISO
 */
typedef struct {
    union {
        struct {
            uint32_t xfer_size: 17;
            uint32_t aqtd_offset: 6;
            uint32_t aqtd_valid: 1;
            uint32_t reserved_24: 1;
            uint32_t intr_cplt: 1;
            uint32_t eol: 1;
            uint32_t reserved_27: 1;
            uint32_t rx_status: 2;
            uint32_t reserved_30: 1;
            uint32_t active: 1;
        } in_non_iso;
        struct {
            uint32_t xfer_size: 12;
            uint32_t reserved_12_24: 13;
            uint32_t intr_cplt: 1;
            uint32_t reserved_26_27: 2;
            uint32_t rx_status: 2;
            uint32_t reserved_30: 1;
            uint32_t active: 1;
        } in_iso;
        struct {
            uint32_t xfer_size: 17;
            uint32_t reserved_17_23: 7;
            uint32_t is_setup: 1;
            uint32_t intr_cplt: 1;
            uint32_t eol: 1;
            uint32_t reserved_27: 1;
            uint32_t tx_status: 2;
            uint32_t reserved_30: 1;
            uint32_t active: 1;
        } out_non_iso;
        struct {
            uint32_t xfer_size: 12;
            uint32_t reserved_12_24: 13;
            uint32_t intr_cplt: 1;
            uint32_t eol: 1;
            uint32_t reserved_27: 1;
            uint32_t tx_status: 2;
            uint32_t reserved_30: 1;
            uint32_t active: 1;
        } out_iso;
        uint32_t buffer_status_val;
    };
    uint8_t *buffer;
} usb_dwc_ll_dma_qtd_t;


/* -----------------------------------------------------------------------------
------------------------------- Global Registers -------------------------------
----------------------------------------------------------------------------- */

// --------------------------- GAHBCFG Register --------------------------------

static inline void usb_dwc_ll_gahbcfg_en_dma_mode(usb_dwc_dev_t *hw)
{
    hw->gahbcfg_reg.dmaen = 1;
}

static inline void usb_dwc_ll_gahbcfg_en_slave_mode(usb_dwc_dev_t *hw)
{
    hw->gahbcfg_reg.dmaen = 0;
}

static inline void usb_dwc_ll_gahbcfg_set_hbstlen(usb_dwc_dev_t *hw, uint32_t burst_len)
{
    hw->gahbcfg_reg.hbstlen = burst_len;
}

static inline void usb_dwc_ll_gahbcfg_en_global_intr(usb_dwc_dev_t *hw)
{
    hw->gahbcfg_reg.glbllntrmsk = 1;
}

static inline void usb_dwc_ll_gahbcfg_dis_global_intr(usb_dwc_dev_t *hw)
{
    hw->gahbcfg_reg.glbllntrmsk = 0;
}

// --------------------------- GUSBCFG Register --------------------------------

static inline void usb_dwc_ll_gusbcfg_force_host_mode(usb_dwc_dev_t *hw)
{
    hw->gusbcfg_reg.forcehstmode = 1;
}

static inline void usb_dwc_ll_gusbcfg_dis_hnp_cap(usb_dwc_dev_t *hw)
{
    hw->gusbcfg_reg.hnpcap = 0;
}

static inline void usb_dwc_ll_gusbcfg_dis_srp_cap(usb_dwc_dev_t *hw)
{
    hw->gusbcfg_reg.srpcap = 0;
}

// --------------------------- GRSTCTL Register --------------------------------

static inline bool usb_dwc_ll_grstctl_is_ahb_idle(usb_dwc_dev_t *hw)
{
    return hw->grstctl_reg.ahbidle;
}

static inline bool usb_dwc_ll_grstctl_is_dma_req_in_progress(usb_dwc_dev_t *hw)
{
    return hw->grstctl_reg.dmareq;
}

static inline void usb_dwc_ll_grstctl_flush_nptx_fifo(usb_dwc_dev_t *hw)
{
    hw->grstctl_reg.txfnum = 0;     //Set the TX FIFO number to 0 to select the non-periodic TX FIFO
    hw->grstctl_reg.txfflsh = 1;    //Flush the selected TX FIFO
    //Wait for the flushing to complete
    while (hw->grstctl_reg.txfflsh) {
        ;
    }
}

static inline void usb_dwc_ll_grstctl_flush_ptx_fifo(usb_dwc_dev_t *hw)
{
    hw->grstctl_reg.txfnum = 1;     //Set the TX FIFO number to 1 to select the periodic TX FIFO
    hw->grstctl_reg.txfflsh = 1;    //FLush the select TX FIFO
    //Wait for the flushing to complete
    while (hw->grstctl_reg.txfflsh) {
        ;
    }
}

static inline void usb_dwc_ll_grstctl_flush_rx_fifo(usb_dwc_dev_t *hw)
{
    hw->grstctl_reg.rxfflsh = 1;
    //Wait for the flushing to complete
    while (hw->grstctl_reg.rxfflsh) {
        ;
    }
}

static inline void usb_dwc_ll_grstctl_reset_frame_counter(usb_dwc_dev_t *hw)
{
    hw->grstctl_reg.frmcntrrst = 1;
}

static inline void usb_dwc_ll_grstctl_core_soft_reset(usb_dwc_dev_t *hw)
{
    hw->grstctl_reg.csftrst = 1;
}

static inline bool usb_dwc_ll_grstctl_is_core_soft_reset_in_progress(usb_dwc_dev_t *hw)
{
    return hw->grstctl_reg.csftrst;
}

// --------------------------- GINTSTS Register --------------------------------

/**
 * @brief Reads and clears the global interrupt register
 *
 * @param hw Start address of the DWC_OTG registers
 * @return uint32_t Mask of interrupts
 */
static inline uint32_t usb_dwc_ll_gintsts_read_and_clear_intrs(usb_dwc_dev_t *hw)
{
    usb_dwc_gintsts_reg_t gintsts;
    gintsts.val = hw->gintsts_reg.val;
    hw->gintsts_reg.val = gintsts.val;  //Write back to clear
    return gintsts.val;
}

/**
 * @brief Clear specific interrupts
 *
 * @param hw Start address of the DWC_OTG registers
 * @param intr_msk Mask of interrupts to clear
 */
static inline void usb_dwc_ll_gintsts_clear_intrs(usb_dwc_dev_t *hw, uint32_t intr_msk)
{
    //All GINTSTS fields are either W1C or read only. So safe to write directly
    hw->gintsts_reg.val = intr_msk;
}

// --------------------------- GINTMSK Register --------------------------------

static inline void usb_dwc_ll_gintmsk_en_intrs(usb_dwc_dev_t *hw, uint32_t intr_mask)
{
    hw->gintmsk_reg.val |= intr_mask;
}

static inline void usb_dwc_ll_gintmsk_dis_intrs(usb_dwc_dev_t *hw, uint32_t intr_mask)
{
    hw->gintmsk_reg.val &= ~intr_mask;
}

// --------------------------- GRXFSIZ Register --------------------------------

static inline void usb_dwc_ll_grxfsiz_set_fifo_size(usb_dwc_dev_t *hw, uint32_t num_lines)
{
    //Set size in words
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->grxfsiz_reg, rxfdep, num_lines);
}

// -------------------------- GNPTXFSIZ Register -------------------------------

static inline void usb_dwc_ll_gnptxfsiz_set_fifo_size(usb_dwc_dev_t *hw, uint32_t addr, uint32_t num_lines)
{
    usb_dwc_gnptxfsiz_reg_t gnptxfsiz;
    gnptxfsiz.val = hw->gnptxfsiz_reg.val;
    HAL_FORCE_MODIFY_U32_REG_FIELD(gnptxfsiz, nptxfstaddr, addr);
    HAL_FORCE_MODIFY_U32_REG_FIELD(gnptxfsiz, nptxfdep, num_lines);
    hw->gnptxfsiz_reg.val = gnptxfsiz.val;
}

// --------------------------- GSNPSID Register --------------------------------

static inline uint32_t usb_dwc_ll_gsnpsid_get_id(usb_dwc_dev_t *hw)
{
    return hw->gsnpsid_reg.val;
}

// --------------------------- GHWCFGx Register --------------------------------

/**
 * @brief Get the hardware configuration regiters of the DWC_OTG controller
 *
 * The hardware configuraiton regitsers are read only and indicate the various
 * features of the DWC_OTG core.
 *
 * @param hw Start address of the DWC_OTG registers
 * @param[out] ghwcfg1 Hardware configuration registesr 1
 * @param[out] ghwcfg2 Hardware configuration registesr 2
 * @param[out] ghwcfg3 Hardware configuration registesr 3
 * @param[out] ghwcfg4 Hardware configuration registesr 4
 */
static inline void usb_dwc_ll_ghwcfg_get_hw_config(usb_dwc_dev_t *hw, uint32_t *ghwcfg1, uint32_t *ghwcfg2, uint32_t *ghwcfg3, uint32_t *ghwcfg4)
{
    *ghwcfg1 = hw->ghwcfg1_reg.val;
    *ghwcfg2 = hw->ghwcfg2_reg.val;
    *ghwcfg3 = hw->ghwcfg3_reg.val;
    *ghwcfg4 = hw->ghwcfg4_reg.val;
}

// --------------------------- HPTXFSIZ Register -------------------------------

static inline void usb_dwc_ll_hptxfsiz_set_ptx_fifo_size(usb_dwc_dev_t *hw, uint32_t addr, uint32_t num_lines)
{
    usb_dwc_hptxfsiz_reg_t hptxfsiz;
    hptxfsiz.val = hw->hptxfsiz_reg.val;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hptxfsiz, ptxfstaddr, addr);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hptxfsiz, ptxfsize, num_lines);
    hw->hptxfsiz_reg.val = hptxfsiz.val;
}

/* -----------------------------------------------------------------------------
-------------------------------- Host Registers --------------------------------
----------------------------------------------------------------------------- */

// ----------------------------- HCFG Register ---------------------------------

static inline void usb_dwc_ll_hcfg_en_perio_sched(usb_dwc_dev_t *hw)
{
    hw->hcfg_reg.perschedena = 1;
}

static inline void usb_dwc_ll_hcfg_dis_perio_sched(usb_dwc_dev_t *hw)
{
    hw->hcfg_reg.perschedena = 0;
}

/**
 * Sets the length of the frame list
 *
 * @param num_entires Number of entires in the frame list
 */
static inline void usb_dwc_ll_hcfg_set_num_frame_list_entries(usb_dwc_dev_t *hw, usb_hal_frame_list_len_t num_entries)
{
    uint32_t frlisten;
    switch (num_entries) {
        case USB_HAL_FRAME_LIST_LEN_8:
            frlisten = 0;
            break;
        case USB_HAL_FRAME_LIST_LEN_16:
            frlisten = 1;
            break;
        case USB_HAL_FRAME_LIST_LEN_32:
            frlisten = 2;
            break;
        default: //USB_HAL_FRAME_LIST_LEN_64
            frlisten = 3;
            break;
    }
    hw->hcfg_reg.frlisten = frlisten;
}

static inline void usb_dwc_ll_hcfg_en_scatt_gatt_dma(usb_dwc_dev_t *hw)
{
    hw->hcfg_reg.descdma = 1;
}

static inline void usb_dwc_ll_hcfg_set_fsls_supp_only(usb_dwc_dev_t *hw)
{
    hw->hcfg_reg.fslssupp = 1;
}

static inline void usb_dwc_ll_hcfg_set_fsls_pclk_sel(usb_dwc_dev_t *hw)
{
    hw->hcfg_reg.fslspclksel = 1;
}

/**
 * @brief Sets some default values to HCFG to operate in Host mode with scatter/gather DMA
 *
 * @param hw Start address of the DWC_OTG registers
 * @param speed Speed to initialize the host port at
 */
static inline void usb_dwc_ll_hcfg_set_defaults(usb_dwc_dev_t *hw, usb_priv_speed_t speed)
{
    hw->hcfg_reg.descdma = 1;   //Enable scatt/gatt
    hw->hcfg_reg.fslssupp = 1;  //FS/LS support only
    /*
    Indicate to the OTG core what speed the PHY clock is at
    Note: It seems like our PHY has an implicit 8 divider applied when in LS mode,
          so the values of FSLSPclkSel and FrInt have to be adjusted accordingly.
    */
    hw->hcfg_reg.fslspclksel = (speed == USB_PRIV_SPEED_FULL) ? 1 : 2;  //PHY clock on esp32-sx for FS/LS-only
    hw->hcfg_reg.perschedena = 0;   //Disable perio sched
}

// ----------------------------- HFIR Register ---------------------------------

static inline void usb_dwc_ll_hfir_set_defaults(usb_dwc_dev_t *hw, usb_priv_speed_t speed)
{
    usb_dwc_hfir_reg_t hfir;
    hfir.val = hw->hfir_reg.val;
    hfir.hfirrldctrl = 0;       //Disable dynamic loading
    /*
    Set frame interval to be equal to 1ms
    Note: It seems like our PHY has an implicit 8 divider applied when in LS mode,
          so the values of FSLSPclkSel and FrInt have to be adjusted accordingly.
    */
    hfir.frint = (speed == USB_PRIV_SPEED_FULL) ? 48000 : 6000; //esp32-sx targets only support FS or LS
    hw->hfir_reg.val = hfir.val;
}

// ----------------------------- HFNUM Register --------------------------------

static inline uint32_t usb_dwc_ll_hfnum_get_frame_time_rem(usb_dwc_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->hfnum_reg, frrem);
}

static inline uint32_t usb_dwc_ll_hfnum_get_frame_num(usb_dwc_dev_t *hw)
{
    return hw->hfnum_reg.frnum;
}

// ---------------------------- HPTXSTS Register -------------------------------

static inline uint32_t usb_dwc_ll_hptxsts_get_ptxq_top(usb_dwc_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->hptxsts_reg, ptxqtop);
}

static inline uint32_t usb_dwc_ll_hptxsts_get_ptxq_space_avail(usb_dwc_dev_t *hw)
{
    return hw->hptxsts_reg.ptxqspcavail;
}

static inline uint32_t usb_dwc_ll_ptxsts_get_ptxf_space_avail(usb_dwc_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->hptxsts_reg, ptxfspcavail);
}

// ----------------------------- HAINT Register --------------------------------

static inline uint32_t usb_dwc_ll_haint_get_chan_intrs(usb_dwc_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->haint_reg, haint);
}

// --------------------------- HAINTMSK Register -------------------------------

static inline void usb_dwc_ll_haintmsk_en_chan_intr(usb_dwc_dev_t *hw, uint32_t mask)
{

    hw->haintmsk_reg.val |= mask;
}

static inline void usb_dwc_ll_haintmsk_dis_chan_intr(usb_dwc_dev_t *hw, uint32_t mask)
{
    hw->haintmsk_reg.val &= ~mask;
}

// --------------------------- HFLBAddr Register -------------------------------

/**
 * @brief Set the base address of the scheduling frame list
 *
 * @note For some reason, this address must be 512 bytes aligned or else a bunch of frames will not be scheduled when
 *       the frame list rolls over. However, according to the databook, there is no mention of the HFLBAddr needing to
 *       be aligned.
 *
 * @param hw Start address of the DWC_OTG registers
 * @param addr Base address of the scheduling frame list
 */
static inline void usb_dwc_ll_hflbaddr_set_base_addr(usb_dwc_dev_t *hw, uint32_t addr)
{
    hw->hflbaddr_reg.hflbaddr = addr;
}

/**
 * @brief Get the base address of the scheduling frame list
 *
 * @param hw Start address of the DWC_OTG registers
 * @return uint32_t Base address of the scheduling frame list
 */
static inline uint32_t usb_dwc_ll_hflbaddr_get_base_addr(usb_dwc_dev_t *hw)
{
    return hw->hflbaddr_reg.hflbaddr;
}

// ----------------------------- HPRT Register ---------------------------------

static inline usb_priv_speed_t usb_dwc_ll_hprt_get_speed(usb_dwc_dev_t *hw)
{
    usb_priv_speed_t speed;
    //esp32-s2 and esp32-s3 only support FS or LS
    switch (hw->hprt_reg.prtspd) {
        case 1:
            speed = USB_PRIV_SPEED_FULL;
            break;
        default:
            speed = USB_PRIV_SPEED_LOW;
            break;
    }
    return speed;
}

static inline uint32_t usb_dwc_ll_hprt_get_test_ctl(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prttstctl;
}

static inline void usb_dwc_ll_hprt_set_test_ctl(usb_dwc_dev_t *hw, uint32_t test_mode)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prttstctl = test_mode;
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_W1C_MSK);
}

static inline void usb_dwc_ll_hprt_en_pwr(usb_dwc_dev_t *hw)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prtpwr = 1;
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_W1C_MSK);
}

static inline void usb_dwc_ll_hprt_dis_pwr(usb_dwc_dev_t *hw)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prtpwr = 0;
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_W1C_MSK);
}

static inline uint32_t usb_dwc_ll_hprt_get_pwr_line_status(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prtlnsts;
}

static inline void usb_dwc_ll_hprt_set_port_reset(usb_dwc_dev_t *hw, bool reset)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prtrst = reset;
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_W1C_MSK);
}

static inline bool usb_dwc_ll_hprt_get_port_reset(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prtrst;
}

static inline void usb_dwc_ll_hprt_set_port_suspend(usb_dwc_dev_t *hw)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prtsusp = 1;
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_W1C_MSK);
}

static inline bool usb_dwc_ll_hprt_get_port_suspend(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prtsusp;
}

static inline void usb_dwc_ll_hprt_set_port_resume(usb_dwc_dev_t *hw)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prtres = 1;
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_W1C_MSK);
}

static inline void usb_dwc_ll_hprt_clr_port_resume(usb_dwc_dev_t *hw)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prtres = 0;
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_W1C_MSK);
}

static inline bool usb_dwc_ll_hprt_get_port_resume(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prtres;
}

static inline bool usb_dwc_ll_hprt_get_port_overcur(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prtovrcurract;
}

static inline bool usb_dwc_ll_hprt_get_port_en(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prtena;
}

static inline void usb_dwc_ll_hprt_port_dis(usb_dwc_dev_t *hw)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hprt.prtena = 1;        //W1C to disable
    //we want to W1C ENA but not W1C the interrupt bits
    hw->hprt_reg.val = hprt.val & ((~USB_DWC_LL_HPRT_W1C_MSK) | USB_DWC_LL_HPRT_ENA_MSK);
}

static inline bool usb_dwc_ll_hprt_get_conn_status(usb_dwc_dev_t *hw)
{
    return hw->hprt_reg.prtconnsts;
}

static inline uint32_t usb_dwc_ll_hprt_intr_read_and_clear(usb_dwc_dev_t *hw)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    //We want to W1C the interrupt bits but not that ENA
    hw->hprt_reg.val = hprt.val & (~USB_DWC_LL_HPRT_ENA_MSK);
    //Return only the interrupt bits
    return (hprt.val & (USB_DWC_LL_HPRT_W1C_MSK & ~(USB_DWC_LL_HPRT_ENA_MSK)));
}

static inline void usb_dwc_ll_hprt_intr_clear(usb_dwc_dev_t *hw, uint32_t intr_mask)
{
    usb_dwc_hprt_reg_t hprt;
    hprt.val = hw->hprt_reg.val;
    hw->hprt_reg.val = ((hprt.val & ~USB_DWC_LL_HPRT_ENA_MSK) & ~USB_DWC_LL_HPRT_W1C_MSK) | intr_mask;
}

//Per Channel registers

// --------------------------- HCCHARi Register --------------------------------

static inline void usb_dwc_ll_hcchar_enable_chan(volatile usb_dwc_host_chan_regs_t *chan)
{
    chan->hcchar_reg.chena = 1;
}

static inline bool usb_dwc_ll_hcchar_chan_is_enabled(volatile usb_dwc_host_chan_regs_t *chan)
{
    return chan->hcchar_reg.chena;
}

static inline void usb_dwc_ll_hcchar_disable_chan(volatile usb_dwc_host_chan_regs_t *chan)
{
    chan->hcchar_reg.chdis = 1;
}

static inline void usb_dwc_ll_hcchar_set_odd_frame(volatile usb_dwc_host_chan_regs_t *chan)
{
    chan->hcchar_reg.oddfrm = 1;
}

static inline void usb_dwc_ll_hcchar_set_even_frame(volatile usb_dwc_host_chan_regs_t *chan)
{
    chan->hcchar_reg.oddfrm = 0;
}

static inline void usb_dwc_ll_hcchar_set_dev_addr(volatile usb_dwc_host_chan_regs_t *chan, uint32_t addr)
{
    chan->hcchar_reg.devaddr = addr;
}

static inline void usb_dwc_ll_hcchar_set_ep_type(volatile usb_dwc_host_chan_regs_t *chan, usb_priv_xfer_type_t type)
{
    uint32_t ep_type;
    switch (type) {
        case USB_PRIV_XFER_TYPE_CTRL:
            ep_type = 0;
            break;
        case USB_PRIV_XFER_TYPE_ISOCHRONOUS:
            ep_type = 1;
            break;
        case USB_PRIV_XFER_TYPE_BULK:
            ep_type = 2;
            break;
        default:    //USB_PRIV_XFER_TYPE_INTR
            ep_type = 3;
            break;
    }
    chan->hcchar_reg.eptype = ep_type;
}

//Indicates whether channel is commuunicating with a LS device connected via a FS hub. Setting this bit to 1 will cause
//each packet to be preceded by a PREamble packet
static inline void usb_dwc_ll_hcchar_set_lspddev(volatile usb_dwc_host_chan_regs_t *chan, bool is_ls)
{
    chan->hcchar_reg.lspddev = is_ls;
}

static inline void usb_dwc_ll_hcchar_set_dir(volatile usb_dwc_host_chan_regs_t *chan, bool is_in)
{
    chan->hcchar_reg.epdir = is_in;
}

static inline void usb_dwc_ll_hcchar_set_ep_num(volatile usb_dwc_host_chan_regs_t *chan, uint32_t num)
{
    chan->hcchar_reg.epnum = num;
}

static inline void usb_dwc_ll_hcchar_set_mps(volatile usb_dwc_host_chan_regs_t *chan, uint32_t mps)
{
    chan->hcchar_reg.mps = mps;
}

static inline void usb_dwc_ll_hcchar_init(volatile usb_dwc_host_chan_regs_t *chan, int dev_addr, int ep_num, int mps, usb_priv_xfer_type_t type, bool is_in, bool is_ls)
{
    //Sets all persistent fields of the channel over its lifetimez
    usb_dwc_ll_hcchar_set_dev_addr(chan, dev_addr);
    usb_dwc_ll_hcchar_set_ep_type(chan, type);
    usb_dwc_ll_hcchar_set_lspddev(chan, is_ls);
    usb_dwc_ll_hcchar_set_dir(chan, is_in);
    usb_dwc_ll_hcchar_set_ep_num(chan, ep_num);
    usb_dwc_ll_hcchar_set_mps(chan, mps);
}

// ---------------------------- HCINTi Register --------------------------------

static inline uint32_t usb_dwc_ll_hcint_read_and_clear_intrs(volatile usb_dwc_host_chan_regs_t *chan)
{
    usb_dwc_hcint_reg_t hcint;
    hcint.val = chan->hcint_reg.val;
    chan->hcint_reg.val = hcint.val;
    return hcint.val;
}

// --------------------------- HCINTMSKi Register ------------------------------

static inline void usb_dwc_ll_hcintmsk_set_intr_mask(volatile usb_dwc_host_chan_regs_t *chan, uint32_t mask)
{
    chan->hcintmsk_reg.val = mask;
}

// ---------------------------- HCTSIZi Register -------------------------------

static inline void usb_dwc_ll_hctsiz_set_pid(volatile usb_dwc_host_chan_regs_t *chan, uint32_t data_pid)
{
    if (data_pid == 0) {
        chan->hctsiz_reg.pid = 0;
    } else {
        chan->hctsiz_reg.pid = 2;
    }
}

static inline uint32_t usb_dwc_ll_hctsiz_get_pid(volatile usb_dwc_host_chan_regs_t *chan)
{
    if (chan->hctsiz_reg.pid == 0) {
        return 0;   //DATA0
    } else {
        return 1;   //DATA1
    }
}

static inline void usb_dwc_ll_hctsiz_set_qtd_list_len(volatile usb_dwc_host_chan_regs_t *chan, int qtd_list_len)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(chan->hctsiz_reg, ntd, qtd_list_len - 1);    //Set the length of the descriptor list
}

static inline void usb_dwc_ll_hctsiz_init(volatile usb_dwc_host_chan_regs_t *chan)
{
    chan->hctsiz_reg.dopng = 0;         //Don't do ping
    HAL_FORCE_MODIFY_U32_REG_FIELD(chan->hctsiz_reg, sched_info, 0xFF); //Schedinfo is always 0xFF for fullspeed. Not used in Bulk/Ctrl channels
}

// ---------------------------- HCDMAi Register --------------------------------

static inline void usb_dwc_ll_hcdma_set_qtd_list_addr(volatile usb_dwc_host_chan_regs_t *chan, void *dmaaddr, uint32_t qtd_idx)
{
    //Set HCDMAi
    chan->hcdma_reg.val = 0;
    chan->hcdma_reg.non_iso.dmaaddr = (((uint32_t)dmaaddr) >> 9) & 0x7FFFFF;  //MSB of 512 byte aligned address
    chan->hcdma_reg.non_iso.ctd = qtd_idx;
}

static inline int usb_dwc_ll_hcdam_get_cur_qtd_idx(usb_dwc_host_chan_regs_t *chan)
{
    return chan->hcdma_reg.non_iso.ctd;
}

// ---------------------------- HCDMABi Register -------------------------------

static inline void *usb_dwc_ll_hcdmab_get_buff_addr(volatile usb_dwc_host_chan_regs_t *chan)
{
    return (void *)chan->hcdmab_reg.hcdmab;
}

/* -----------------------------------------------------------------------------
---------------------------- Scatter/Gather DMA QTDs ---------------------------
----------------------------------------------------------------------------- */

// ---------------------------- Helper Functions -------------------------------

/**
 * @brief Get the base address of a channel's register based on the channel's index
 *
 * @param dev Start address of the DWC_OTG registers
 * @param chan_idx The channel's index
 * @return usb_dwc_host_chan_regs_t* Pointer to channel's registers
 */
static inline usb_dwc_host_chan_regs_t *usb_dwc_ll_chan_get_regs(usb_dwc_dev_t *dev, int chan_idx)
{
    return &dev->host_chans[chan_idx];
}

// ------------------------------ QTD related ----------------------------------

#define USB_DWC_LL_QTD_STATUS_SUCCESS      0x0     //If QTD was processed, it indicates the data was transmitted/received successfully
#define USB_DWC_LL_QTD_STATUS_PKTERR       0x1     //Data trasnmitted/received with errors (CRC/Timeout/Stuff/False EOP/Excessive NAK).
//Note: 0x2 is reserved
#define USB_DWC_LL_QTD_STATUS_BUFFER       0x3     //AHB error occurred.
#define USB_DWC_LL_QTD_STATUS_NOT_EXECUTED 0x4     //QTD as never processed

/**
 * @brief Set a QTD for a non isochronous IN transfer
 *
 * @param qtd Pointer to the QTD
 * @param data_buff Pointer to buffer containing the data to transfer
 * @param xfer_len Number of bytes in transfer. Setting 0 will do a zero length IN transfer.
 *                 Non zero length must be mulitple of the endpoint's MPS.
 * @param hoc Halt on complete (will generate an interrupt and halt the channel)
 */
static inline void usb_dwc_ll_qtd_set_in(usb_dwc_ll_dma_qtd_t *qtd, uint8_t *data_buff, int xfer_len, bool hoc)
{
    qtd->buffer = data_buff;        //Set pointer to data buffer
    qtd->buffer_status_val = 0;     //Reset all flags to zero
    qtd->in_non_iso.xfer_size = xfer_len;
    if (hoc) {
        qtd->in_non_iso.intr_cplt = 1;  //We need to set this to distinguish between a halt due to a QTD
        qtd->in_non_iso.eol = 1;        //Used to halt the channel at this qtd
    }
    qtd->in_non_iso.active = 1;
}

/**
 * @brief Set a QTD for a non isochronous OUT transfer
 *
 * @param qtd Poitner to the QTD
 * @param data_buff Pointer to buffer containing the data to transfer
 * @param xfer_len Number of bytes to transfer. Setting 0 will do a zero length transfer.
 *                 For ctrl setup packets, this should be set to 8.
 * @param hoc Halt on complete (will generate an interrupt)
 * @param is_setup Indicates whether this is a control transfer setup packet or a normal OUT Data transfer.
 *                 (As per the USB protocol, setup packets cannot be STALLd or NAKd by the device)
 */
static inline void usb_dwc_ll_qtd_set_out(usb_dwc_ll_dma_qtd_t *qtd, uint8_t *data_buff, int xfer_len, bool hoc, bool is_setup)
{
    qtd->buffer = data_buff;        //Set pointer to data buffer
    qtd->buffer_status_val = 0;     //Reset all flags to zero
    qtd->out_non_iso.xfer_size = xfer_len;
    if (is_setup) {
        qtd->out_non_iso.is_setup = 1;
    }
    if (hoc) {
        qtd->in_non_iso.intr_cplt = 1;  //We need to set this to distinguish between a halt due to a QTD
        qtd->in_non_iso.eol = 1;        //Used to halt the channel at this qtd
    }
    qtd->out_non_iso.active = 1;
}

/**
 * @brief Set a QTD as NULL
 *
 * This sets the QTD to a value of 0. This is only useful when you need to insert
 * blank QTDs into a list of QTDs
 *
 * @param qtd Pointer to the QTD
 */
static inline void usb_dwc_ll_qtd_set_null(usb_dwc_ll_dma_qtd_t *qtd)
{
    qtd->buffer = NULL;
    qtd->buffer_status_val = 0;     //Disable qtd by clearing it to zero. Used by interrupt/isoc as an unscheudled frame
}

/**
 * @brief Get the status of a QTD
 *
 * When a channel get's halted, call this to check whether each QTD was executed successfully
 *
 * @param qtd Poitner to the QTD
 * @param[out] rem_len Number of bytes ramining in the QTD
 * @param[out] status Status of the QTD
 */
static inline void usb_dwc_ll_qtd_get_status(usb_dwc_ll_dma_qtd_t *qtd, int *rem_len, int *status)
{
    //Status is the same regardless of IN or OUT
    if (qtd->in_non_iso.active) {
        //QTD was never processed
        *status = USB_DWC_LL_QTD_STATUS_NOT_EXECUTED;
    } else {
        *status = qtd->in_non_iso.rx_status;
    }
    *rem_len = qtd->in_non_iso.xfer_size;
    //Clear the QTD just for safety
    qtd->buffer_status_val = 0;
}

#ifdef __cplusplus
}
#endif
