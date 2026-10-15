/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h> // For PRIu32
#include <string.h> // For memset()
#include <stdlib.h> // For abort()
#include "soc/soc_caps.h"
#include "soc/chip_revision.h"
#include "soc/usb_periph.h"
#include "hal/usb_dwc_hal.h"
#include "hal/usb_dwc_ll.h"
#include "hal/efuse_hal.h"
#include "hal/assert.h"
#include "hal/log.h"

HAL_LOG_ATTR_TAG(USB_HAL_TAG, "usb_dwc_hal");

// ------------------------------------------------ Macros and Types ---------------------------------------------------

// ---------------------- Constants ------------------------

#define BENDPOINTADDRESS_NUM_MSK     0x0F   //Endpoint number mask of the bEndpointAddress field of an endpoint descriptor
#define BENDPOINTADDRESS_DIR_MSK     0x80   //Endpoint direction mask of the bEndpointAddress field of an endpoint descriptor

// Core register IDs supported by this driver: v4.00a and v4.30a
#define CORE_REG_GSNPSID_4_00a    0x4F54400A
#define CORE_REG_GSNPSID_4_30a    0x4F54430A

// -------------------- Configurable -----------------------

/**
 * The following core interrupts will be enabled (listed LSB to MSB). Some of these
 * interrupts are enabled later than others.
 * - USB_DWC_LL_INTR_CORE_PRTINT
 * - USB_DWC_LL_INTR_CORE_HCHINT
 * - USB_DWC_LL_INTR_CORE_DISCONNINT
 * The following PORT interrupts cannot be masked, listed LSB to MSB
 * - USB_DWC_LL_INTR_HPRT_PRTCONNDET
 * - USB_DWC_LL_INTR_HPRT_PRTENCHNG
 * - USB_DWC_LL_INTR_HPRT_PRTOVRCURRCHNG
 */
#define CORE_INTRS_EN_MSK   (USB_DWC_LL_INTR_CORE_DISCONNINT)

//Interrupts that pertain to core events
#define CORE_EVENTS_INTRS_MSK (USB_DWC_LL_INTR_CORE_DISCONNINT | \
                               USB_DWC_LL_INTR_CORE_HCHINT | \
                               USB_DWC_LL_INTR_CORE_WKUPINT)

//Interrupt that pertain to host port events
#define PORT_EVENTS_INTRS_MSK (USB_DWC_LL_INTR_HPRT_PRTCONNDET | \
                               USB_DWC_LL_INTR_HPRT_PRTENCHNG | \
                               USB_DWC_LL_INTR_HPRT_PRTOVRCURRCHNG)

/**
 * Channel interrupts are proxied through USB_DWC_LL_INTR_CHAN_CHHLTD: in Buffer DMA mode only CHHLTD is unmasked
 * (XFERCOMPL and BNA are additionally unmasked in Scatter/Gather DMA mode), and the remaining status bits -
 * including XFERCOMPL in Buffer DMA mode - are read from HCINT once the channel halts.
 *
 * The channel error bits that are checked after a halt depend on the DMA mode (see CHAN_INTRS_ERROR_MSK_*):
 * - Both DMA modes:        USB_DWC_LL_INTR_CHAN_STALL, USB_DWC_LL_INTR_CHAN_BBLEER
 * - Scatter/Gather only:   USB_DWC_LL_INTR_CHAN_BNAINTR, USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR (bit 12)
 * - Buffer DMA only:       USB_DWC_LL_INTR_CHAN_XACTERR (bit 7)
 *
 * Note the following points about channel interrupts:
 * - Not all bits are unmaskable under scatter/gather; those bits proxy their interrupt through CHHLTD.
 * - When USB_DWC_LL_INTR_CHAN_BNAINTR occurs, USB_DWC_LL_INTR_CHAN_CHHLTD will NOT.
 * - The transaction error is reported on a different HCINT bit per DMA mode: the excessive (3 consecutive)
 *   transaction error XCS_XACT_ERR (bit 12) in Scatter/Gather DMA mode (the core retries in hardware and reports
 *   only once the retries are exhausted), and the per-transaction XactErr (bit 7) in Buffer DMA mode (the core
 *   does NOT retry, so software must count these and retry the channel). XCS_XACT_ERR is reserved in Buffer DMA
 *   mode, and XactErr's interrupt is masked by the core in Scatter/Gather DMA mode. Both map to the single HAL
 *   error USB_DWC_HAL_CHAN_ERROR_XACT_ERR.
 * - USB_DWC_LL_INTR_CHAN_AHBERR doesn't actually ever happen on our system (i.e., ESP32-S2, ESP32-S3):
 *      - If the QTD list's starting address is an invalid address (e.g., NULL), the core will attempt to fetch that
 *        address for a transfer descriptor and probably gets all zeroes. It will interpret the zero as a bad QTD and
 *        return a USB_DWC_LL_INTR_CHAN_BNAINTR instead.
 *      - If the QTD's buffer pointer is an invalid address, the core will attempt to read/write data to/from that
 *        invalid buffer address with NO INDICATION OF ERROR. The transfer will be acknowledged and treated as
 *        successful. Bad buffer pointers MUST BE CHECKED FROM HIGHER LAYERS INSTEAD.
 *
 * USB_DWC_LL_INTR_CHAN_BNAINTR is only unmasked in Scatter/Gather DMA mode: BNA is generated when the core
 * fetches a transfer descriptor that is not ready, and there are no descriptors in Buffer DMA mode, so BNA can
 * never occur there.
 *
 * The following channel interrupts are read alongside the transaction error (XCS_XACT_ERR in Scatter/Gather DMA
 * mode, XactErr in Buffer DMA mode) to feed the Buffer DMA error path (see chan_obj->flags.xact_err_rst_seen):
 * - Bulk/control OUT: USB_DWC_LL_INTR_CHAN_NAK, USB_DWC_LL_INTR_CHAN_NYET, USB_DWC_LL_INTR_CHAN_ACK
 * - Bulk/control IN:  USB_DWC_LL_INTR_CHAN_NAK, USB_DWC_LL_INTR_CHAN_ACK, USB_DWC_LL_INTR_CHAN_DATATGLERR
 */

/* Scatter/Gather DMA and Buffer DMA interrupt base mask: Only Channel-halt interrupt is common for both modes */
#define CHAN_INTRS_EN_MSK_BASE    (USB_DWC_LL_INTR_CHAN_CHHLTD)

/*
 * Buffer DMA: only CHHLTD is unmasked. XferCompl is deliberately NOT unmasked. Doing so, it would raise
 * a second, redundant interrupt per transfer whose only side effect is an early read-and-clear of HCINT.
 * Completion is instead confirmed from the XferCompl status bit, which the core latches in HCINT regardless
 * of the mask and is therefore still present on the CHHLTD pass.
 */

/* Buffer DMA interrupt mask*/
#define CHAN_INTRS_EN_MSK_BUFFER  (CHAN_INTRS_EN_MSK_BASE)

/* Scatter/Gather DMA interrupt mask */
#define CHAN_INTRS_EN_MSK_SG      (CHAN_INTRS_EN_MSK_BASE | \
                                   USB_DWC_LL_INTR_CHAN_XFERCOMPL | \
                                   USB_DWC_LL_INTR_CHAN_BNAINTR)

/*
 * Error bits checked after a channel halt. The transaction-error bit differs by DMA mode: XCS_XACT_ERR (bit 12)
 * is Scatter/Gather DMA only (reserved in Buffer DMA mode), while XactErr (bit 7) is the Buffer DMA transaction
 * error (its interrupt is masked by the core in Scatter/Gather DMA mode). STALL and BblErr apply to both modes.
 */
#define CHAN_INTRS_ERROR_MSK_BASE    (USB_DWC_LL_INTR_CHAN_STALL | \
                                      USB_DWC_LL_INTR_CHAN_BBLEER)

#define CHAN_INTRS_ERROR_MSK_SG      (CHAN_INTRS_ERROR_MSK_BASE | \
                                      USB_DWC_LL_INTR_CHAN_BNAINTR | \
                                      USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR)

#define CHAN_INTRS_ERROR_MSK_BUFFER  (CHAN_INTRS_ERROR_MSK_BASE | \
                                      USB_DWC_LL_INTR_CHAN_XACTERR)

// -------------------------------------------------- Core (Global) ----------------------------------------------------

static void set_defaults(usb_dwc_hal_context_t *hal)
{
    //GAHBCFG register
    usb_dwc_ll_gahbcfg_en_dma_mode(hal->dev);
    int hbstlen = 0;    //Use AHB burst SINGLE by default
#if SOC_IS(ESP32S2)
    /*
    Hardware errata workaround for the ESP32-S2 ECO0 (see ESP32-S2 Errata Document section 4.0 for full details).

    ESP32-S2 ECO0 has a hardware errata where the AHB bus arbiter may generate incorrect arbitration signals leading to
    the DWC_OTG corrupting the DMA transfers of other peripherals (or vice versa) on the same bus. The peripherals that
    share the same bus with DWC_OTG include I2C and SPI (see ESP32-S2 Errata Document for more details). To workaround
    this, the DWC_OTG's AHB should use INCR mode to prevent change of arbitration during a burst operation, thus
    avoiding this errata.

    Note: Setting AHB burst to INCR increases the likeliness of DMA underruns on other peripherals sharing the same bus
    arbiter as the DWC_OTG (e.g., I2C and SPI) as change of arbitration during the burst operation is not permitted.
    Users should keep this limitation in mind when the DWC_OTG transfers large data payloads (e.g., 512 MPS transfers)
    while this workaround is enabled.
    */
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 100)) {
        hbstlen = 1;    //Set AHB burst to INCR to workaround hardware errata
    }
#endif // SOC_IS(ESP32S2)
#if SOC_IS(ESP32P4) || SOC_IS(ESP32S31)
    /*
     * ESP32P4/ESP32S31-specific initialization: Clear USB PHY suspend state set during system boot.
     *
     * During system initialization (see clk_gate_ll.h:periph_ll_clk_gate_set_default), the USB PHY
     * is forced into suspend mode before disabling clocks to prevent USB leakage current and ensure
     * proper power management.
     *
     * When initializing the USB DWC HAL, we need to restore the USB PHY to normal operation by:
     *   1. Clearing GOTGCTL.BvalidOvEn (disable override, allow hardware to detect session validity)
     *   2. Clearing PCGCCTL.StopPclk (resume PHY clock for normal operation)
     */
    usb_dwc_ll_enable_bvalid_override(hal->dev, false);
    usb_dwc_ll_set_stoppclk(hal->dev, false);
#endif // SOC_IS(ESP32P4) || SOC_IS(ESP32S31)
    usb_dwc_ll_gahbcfg_set_hbstlen(hal->dev, hbstlen);  //Set AHB burst mode

    //GUSBCFG register
    bool hnp_cap, srp_cap;
    usb_dwc_ll_ghwcfg_get_hnp_srp_cap(hal->dev, &hnp_cap, &srp_cap);

    // On targets where the USB controller is HNP capable, the data lines pull-downs are controlled by the USB controller.
    // Enabling HNP capability will also enable the data line pull-downs in deep-sleep mode eliminating leakage current.
    usb_dwc_ll_gusbcfg_set_hnp_cap(hal->dev, hnp_cap);
    usb_dwc_ll_gusbcfg_set_srp_cap(hal->dev, false);     //Disable SRP

    // If this USB-DWC supports HS PHY, use it
    if (hal->constant_config.hsphy_type != 0) {
        usb_dwc_ll_gusbcfg_set_timeout_cal(hal->dev, 5); // 5 PHY clocks for our HS PHY
        usb_dwc_ll_gusbcfg_set_utmi_phy(hal->dev);
    }
    //Enable interrupts
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, 0xFFFFFFFF);     //Mask all interrupts first
    usb_dwc_ll_gintmsk_en_intrs(hal->dev, CORE_INTRS_EN_MSK);   //Unmask global interrupts
    usb_dwc_ll_gintsts_read_and_clear_intrs(hal->dev);   //Clear interrupts
    usb_dwc_ll_gahbcfg_en_global_intr(hal->dev);        //Enable interrupt signal
    //Enable host mode
    usb_dwc_ll_gusbcfg_force_host_mode(hal->dev);

#if SOC_IS(ESP32P4)
    if (hnp_cap && hal->constant_config.hsphy_type != 0 && !ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 300)) {
        // ESP32-P4 HW rev < 3.0, HS DWC + UTMI: no GPIO-matrix OTG sense for this instance; when HNPCap is enabled, anchor the A-host session
        usb_dwc_ll_gotgctl_anchor_internal_utmi_a_host(hal->dev, true);
    }
#endif // SOC_IS(ESP32P4)
}

void usb_dwc_hal_init_with_config(usb_dwc_hal_context_t *hal, int port_id, const usb_dwc_hal_config_t *config)
{
    // Check if a peripheral is alive by reading the core ID registers
    HAL_ASSERT(port_id < SOC_USB_OTG_PERIPH_NUM);
    // NULL config means default configuration: Scatter/Gather (descriptor) DMA mode
    const usb_dwc_hal_dma_mode_t dma_mode = (config) ? config->dma_mode : USB_DWC_HAL_DMA_MODE_DESC;
    HAL_ASSERT(dma_mode == USB_DWC_HAL_DMA_MODE_DESC || dma_mode == USB_DWC_HAL_DMA_MODE_BUFFER);
    usb_dwc_dev_t *dev = USB_DWC_LL_GET_HW(port_id);
    uint32_t core_id = usb_dwc_ll_gsnpsid_get_id(dev);
    HAL_ASSERT(core_id == CORE_REG_GSNPSID_4_00a || core_id == CORE_REG_GSNPSID_4_30a);
    (void) core_id;     //Suppress unused variable warning if asserts are disabled

    // Initialize HAL context
    memset(hal, 0, sizeof(usb_dwc_hal_context_t));
    hal->dev = dev;
    hal->constant_config.dma_mode = dma_mode;

    // Save constant configuration of this USB-DWC instance
    /*
    * EPINFO_CTL is located at the end of FIFO, its size is fixed in HW.
    * The reserved size is always the worst-case, which is device mode that requires 4 locations per EP direction (including EP0).
    * Here we just read the FIFO size from HW register, to avoid any ambivalence
    */
    hal->constant_config.fifo_size = usb_dwc_ll_ghwcfg_get_fifo_depth(dev);
    hal->constant_config.hsphy_type = usb_dwc_ll_ghwcfg_get_hsphy_type(dev);
    hal->constant_config.chan_num_total = usb_dwc_ll_ghwcfg_get_channel_num(dev);
    hal->constant_config.max_size_byte_limit = (1U << usb_dwc_ll_ghwcfg_get_xfer_size_width(dev)) - 1;
    hal->constant_config.max_size_packet_limit = (1U << usb_dwc_ll_ghwcfg_get_packet_size_width(dev)) - 1;

    // Check passed configuration flags
    if (config) {
        if (config->flags & USB_DWC_HAL_CONFIG_FLAG_FSLS_ONLY) {
            hal->constant_config.flags.fsls_only = 1;
        }
    }

    set_defaults(hal);
    HAL_EARLY_LOGD(USB_HAL_TAG, "Hal inited for port number %d and %s DMA mode", port_id, ((dma_mode == USB_DWC_HAL_DMA_MODE_DESC) ? ("descriptor") : ("buffer")));
}

void usb_dwc_hal_deinit(usb_dwc_hal_context_t *hal)
{
    //Disable and clear global interrupt
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, 0xFFFFFFFF); //Disable all interrupts
    usb_dwc_ll_gintsts_read_and_clear_intrs(hal->dev);   //Clear interrupts
    usb_dwc_ll_gahbcfg_dis_global_intr(hal->dev);       //Disable interrupt signal
    hal->dev = NULL;
    HAL_EARLY_LOGD(USB_HAL_TAG, "HAL deinited");
}

void usb_dwc_hal_core_soft_reset(usb_dwc_hal_context_t *hal)
{
    usb_dwc_ll_grstctl_core_soft_reset(hal->dev);
    while (!usb_dwc_ll_grstctl_is_ahb_idle(hal->dev)) {
        ;   // Wait until AHB Master bus is idle before doing any other operations
    }

    // Set the default bits in USB-DWC registers
    set_defaults(hal);

    // Clear all the flags and channels
    hal->periodic_frame_list = NULL;
    hal->flags.val = 0;
    hal->channels.num_allocated = 0;
    hal->channels.chan_pend_intrs_msk = 0;
    if (hal->channels.hdls) {
        for (int i = 0; i < hal->constant_config.chan_num_total; i++) {
            hal->channels.hdls[i] = NULL;
        }
    }
    HAL_EARLY_LOGD(USB_HAL_TAG, "Core soft reset done");
}

bool usb_dwc_hal_fifo_config_is_valid(const usb_dwc_hal_context_t *hal, const usb_dwc_hal_fifo_config_t *config)
{
    if (!hal || !config) {
        return false;
    }
    uint32_t used_lines = config->rx_fifo_lines + config->nptx_fifo_lines + config->ptx_fifo_lines;
    return (used_lines <= hal->constant_config.fifo_size);
}

void usb_dwc_hal_set_fifo_config(usb_dwc_hal_context_t *hal, const usb_dwc_hal_fifo_config_t *config)
{
    // Check internal HAL state
    HAL_ASSERT(hal != NULL);
    HAL_ASSERT(hal->channels.hdls != NULL);

    // Validate provided config
    HAL_ASSERT(config != NULL);

    // Check if configuration exceeds available FIFO memory
    HAL_ASSERT(usb_dwc_hal_fifo_config_is_valid(hal, config));

    // Ensure no active channels (must only be called before USB install completes)
    for (int i = 0; i < hal->constant_config.chan_num_total; i++) {
        if (hal->channels.hdls[i] != NULL) {
            HAL_ASSERT(!hal->channels.hdls[i]->flags.active);
        }
    }

    // Program FIFO size registers
    usb_dwc_ll_grxfsiz_set_fifo_size(hal->dev, config->rx_fifo_lines);// Set RX FIFO size (GRXFSIZ)
    // Set Non-Periodic TX FIFO (GNPTXFSIZ)
    // Offset = RX FIFO lines
    usb_dwc_ll_gnptxfsiz_set_fifo_size(hal->dev, config->rx_fifo_lines, config->nptx_fifo_lines);
    // Set Periodic TX FIFO (HPTXFSIZ)
    // Offset = RX + NPTX
    usb_dwc_ll_hptxfsiz_set_ptx_fifo_size(hal->dev,
                                          config->rx_fifo_lines + config->nptx_fifo_lines,
                                          config->ptx_fifo_lines);

    // Flush all FIFOs
    usb_dwc_ll_grstctl_flush_nptx_fifo(hal->dev);
    usb_dwc_ll_grstctl_flush_ptx_fifo(hal->dev);
    usb_dwc_ll_grstctl_flush_rx_fifo(hal->dev);

    // Save configuration to HAL context
    hal->fifo_config = *config;
    hal->flags.fifo_sizes_set = 1;
    HAL_EARLY_LOGD(USB_HAL_TAG, "FIFO sizes set: rx=%"PRIu32" nptx=%"PRIu32" ptx=%"PRIu32" lines",
                   config->rx_fifo_lines, config->nptx_fifo_lines, config->ptx_fifo_lines);
}

void usb_dwc_hal_get_mps_limits(usb_dwc_hal_context_t *hal, usb_hal_fifo_mps_limits_t *mps_limits)
{
    HAL_ASSERT(hal && mps_limits);
    HAL_ASSERT(hal->flags.fifo_sizes_set);

    const usb_dwc_hal_fifo_config_t *fifo_config = &(hal->fifo_config);
    mps_limits->in_mps = (fifo_config->rx_fifo_lines - USB_DWC_HAL_RX_FIFO_STATUS_LINES) * 4;
    mps_limits->non_periodic_out_mps = fifo_config->nptx_fifo_lines * 4;
    mps_limits->periodic_out_mps = fifo_config->ptx_fifo_lines * 4;
}

size_t usb_dwc_hal_get_xfer_size_limit(usb_dwc_hal_context_t *hal, uint16_t mps)
{
    HAL_ASSERT(hal);
    HAL_ASSERT(mps > 0);
    size_t max_xfer_size;

    /*
     * The limiting hardware field depends on the DMA mode. Per the DWC_otg databook Section 5.4.41
     * (Table 5-47), the HCTSIZi register is dual-purpose:
     *   - Buffer DMA: XferSize holds the transfer byte count (width OTG_TRANS_COUNT_WIDTH -> max_size_byte_limit)
     *     and PktCnt holds the packet count (width OTG_PACKET_COUNT_WIDTH -> max_size_packet_limit).
     *   - Scatter/Gather DMA: the same bits instead carry NTD (Number of Transfer Descriptors) and
     *     SCHED_INFO; HCTSIZ does NOT hold a byte transfer size at all. There the per-transfer byte
     *     count lives in the qTD descriptor's "Total bytes to transfer" field (programming guide
     *     Section 6), which is 17 bits wide (0..128K-1) for non-isochronous channels.
     */
    if (hal->constant_config.dma_mode == USB_DWC_HAL_DMA_MODE_BUFFER) {
        /*
         * Buffer DMA: a single channel activation programs the transfer into HCTSIZ, so it is bounded
         * by whichever of XferSize (bytes) or PktCnt (packets) saturates first (databook Section 5.4.41).
         */
        const size_t byte_limit   = hal->constant_config.max_size_byte_limit;
        const size_t packet_limit = (size_t)mps * hal->constant_config.max_size_packet_limit;
        max_xfer_size = (packet_limit < byte_limit) ? packet_limit : byte_limit;
    } else {
        /*
         * Scatter/Gather DMA: a bulk/control transfer is described by a single non-isochronous qTD,
         * whose "Total bytes to transfer" field is USB_DWC_LL_QTD_NON_ISO_MAX_XFER_SIZE bytes wide
         * (programming guide Section 6). Periodic transfers are split across the qTD list and are
         * bounded by the list length instead, which the HCD enforces when it fills the descriptor list.
         */
        max_xfer_size = USB_DWC_LL_QTD_NON_ISO_MAX_XFER_SIZE;
    }

    /*
     * Floor to a whole number of maximum-sized packets. The Buffer DMA IN path rounds the programmed
     * transfer size up to a multiple of MPS (see usb_dwc_hal_chan_activate_buffer()), so the largest
     * accepted request must leave room for that rounding; an MPS-aligned limit guarantees both the
     * rounded XferSize and the resulting PktCnt stay within their hardware fields.
     */
    max_xfer_size -= (max_xfer_size % mps);
    return max_xfer_size;
}

// ---------------------------------------------------- Host Port ------------------------------------------------------

static inline void debounce_lock_enable(usb_dwc_hal_context_t *hal)
{
    //Disable the hprt (connection) and disconnection interrupts to prevent repeated triggerings
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, USB_DWC_LL_INTR_CORE_PRTINT | USB_DWC_LL_INTR_CORE_DISCONNINT);
    hal->flags.dbnc_lock_enabled = 1;
}

void usb_dwc_hal_port_toggle_reset(usb_dwc_hal_context_t *hal, bool enable)
{
    // If the core is connected to HS PHY but configuration is FS/LS only
    if (enable && hal->constant_config.hsphy_type && hal->constant_config.flags.fsls_only) {
        usb_dwc_ll_hcfg_set_fsls_supp_only(hal->dev);
    }
    usb_dwc_ll_hprt_set_port_reset(hal->dev, enable);
    HAL_EARLY_LOGV(USB_HAL_TAG, "Port reset %s", enable ? "asserted" : "released");
}

void usb_dwc_hal_port_enable(usb_dwc_hal_context_t *hal)
{
    // Select DMA mode
    if (hal->constant_config.dma_mode == USB_DWC_HAL_DMA_MODE_DESC) {
        usb_dwc_ll_hcfg_en_scatt_gatt_dma(hal->dev); // Enable Scatter-Gather DMA mode
        usb_dwc_ll_hcfg_dis_perio_sched(hal->dev);   // Disable Periodic Scheduler (for now)
        HAL_EARLY_LOGD(USB_HAL_TAG, "Descriptor DMA selected");
    } else {
        usb_dwc_ll_hcfg_en_buffer_dma(hal->dev);     // Enable Buffer DMA mode
        HAL_EARLY_LOGD(USB_HAL_TAG, "Buffer DMA selected");
    }

    // Configure PHY clock: Only for USB-DWC with FSLS PHY
    if (hal->constant_config.hsphy_type == 0) {
        usb_dwc_ll_hcfg_set_fsls_phy_clock(hal->dev);
        usb_dwc_ll_hfir_set_frame_interval(hal->dev);
    }
    HAL_EARLY_LOGD(USB_HAL_TAG, "Port enabled");
}

// ----------------------------------------------------- Channel -------------------------------------------------------

// ----------------- Channel Allocation --------------------

bool usb_dwc_hal_chan_alloc(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj, void *chan_ctx)
{
    HAL_ASSERT(hal->channels.hdls);
    HAL_ASSERT(hal->flags.fifo_sizes_set);  //FIFO sizes should be set before attempting to allocate a channel
    //Attempt to allocate channel
    if (hal->channels.num_allocated == hal->constant_config.chan_num_total) {
        return false;    //Out of free channels
    }
    int chan_idx = -1;
    for (int i = 0; i < hal->constant_config.chan_num_total; i++) {
        if (hal->channels.hdls[i] == NULL) {
            hal->channels.hdls[i] = chan_obj;
            chan_idx = i;
            hal->channels.num_allocated++;
            break;
        }
    }
    HAL_ASSERT(chan_idx != -1);
    //Initialize channel object
    memset(chan_obj, 0, sizeof(usb_dwc_hal_chan_t));
    chan_obj->flags.chan_idx = chan_idx;
    chan_obj->regs = usb_dwc_ll_chan_get_regs(hal->dev, chan_idx);
    chan_obj->chan_ctx = chan_ctx;
    //Note: EP characteristics configured separately
    //Clean and unmask the channel's interrupt
    usb_dwc_ll_hcint_read_and_clear_intrs(chan_obj->regs);            //Clear the interrupt bits for that channel
    usb_dwc_ll_haintmsk_en_chan_intr(hal->dev, 1 << chan_obj->flags.chan_idx);
    //Unmask interrupts for this channel. BNA is a Scatter/Gather DMA only condition (descriptor fetch)
    const uint32_t chan_intrs_en_msk = (hal->constant_config.dma_mode == USB_DWC_HAL_DMA_MODE_DESC) ? CHAN_INTRS_EN_MSK_SG : CHAN_INTRS_EN_MSK_BUFFER;
    usb_dwc_ll_hcintmsk_set_intr_mask(chan_obj->regs, chan_intrs_en_msk);
    // Set default channel transfer parameters
    if (hal->constant_config.dma_mode == USB_DWC_HAL_DMA_MODE_DESC) {
        usb_dwc_ll_hctsiz_init_desc_dma(chan_obj->regs);
    } else {
        usb_dwc_ll_hctsiz_init_buffer_dma(chan_obj->regs);
    }
    HAL_EARLY_LOGD(USB_HAL_TAG, "Channel %d allocated", chan_idx);
    return true;
}

void usb_dwc_hal_chan_free(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj)
{
    HAL_ASSERT(hal->channels.hdls);
    if (chan_obj->type == USB_DWC_XFER_TYPE_INTR || chan_obj->type == USB_DWC_XFER_TYPE_ISOCHRONOUS) {
        //Unschedule this channel
        for (int i = 0; i < hal->frame_list_len; i++) {
            hal->periodic_frame_list[i] &= ~(1 << chan_obj->flags.chan_idx);
        }
    }
    //Can only free a channel when in the disabled state and descriptor list released
    HAL_ASSERT(!chan_obj->flags.active);
    //Disable channel's interrupt
    usb_dwc_ll_haintmsk_dis_chan_intr(hal->dev, 1 << chan_obj->flags.chan_idx);
    //Deallocate channel
    hal->channels.hdls[chan_obj->flags.chan_idx] = NULL;
    hal->channels.num_allocated--;
    HAL_ASSERT(hal->channels.num_allocated >= 0);
    HAL_EARLY_LOGD(USB_HAL_TAG, "Channel %d freed", chan_obj->flags.chan_idx);
}

// ---------------- Channel Configuration ------------------

void usb_dwc_hal_chan_set_ep_char(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj, usb_dwc_hal_ep_char_t *ep_char)
{
    //Cannot change ep_char whilst channel is still active or in error
    HAL_ASSERT(!chan_obj->flags.active);
    //Set the endpoint characteristics of the pipe
    usb_dwc_ll_hcchar_init(chan_obj->regs,
                           ep_char->dev_addr,
                           ep_char->bEndpointAddress & BENDPOINTADDRESS_NUM_MSK,
                           ep_char->mps,
                           ep_char->type,
                           ep_char->bEndpointAddress & BENDPOINTADDRESS_DIR_MSK,
                           ep_char->ls_via_fs_hub);
    /*
     * TODO: For periodic (isochronous/interrupt) endpoints, MC/EC is the number of transactions per
     * microframe (1..3 for high-bandwidth endpoints), and the DWC_otg programming guide (§6.5.1,
     * §6.5.2) states that leaving it at 0 yields undefined behaviour. This applies to BOTH DMA modes:
     *   - Buffer DMA: add MC/EC programming here once interrupt/isochronous Buffer DMA support is added.
     *   - Scatter/Gather (descriptor) DMA: periodic channels also require a non-zero MC/EC, but it is
     *     currently never programmed on the SG path, so periodic SG channels run with MC/EC = 0.
     * Note that SG asynchronous (bulk/control) transfers do not use MC/EC at all (guide §6.3, §6.4):
     * they are driven entirely by the qTD descriptor list, which is why only Buffer DMA bulk/control
     * is handled below.
     */
    // Program Multi-count/Error-count
    if (hal->constant_config.dma_mode == USB_DWC_HAL_DMA_MODE_BUFFER &&
            (ep_char->type == USB_DWC_XFER_TYPE_CTRL || ep_char->type == USB_DWC_XFER_TYPE_BULK)) {
        usb_dwc_ll_hcchar_set_multi_count(chan_obj->regs, 1);
    }
    //Save channel type, MPS, and direction
    chan_obj->type = ep_char->type;
    chan_obj->mps = ep_char->mps;
    chan_obj->flags.is_in = (ep_char->bEndpointAddress & BENDPOINTADDRESS_DIR_MSK) ? 1 : 0;
    //If this is a periodic endpoint/channel, set its schedule in the frame list
    if (ep_char->type == USB_DWC_XFER_TYPE_ISOCHRONOUS || ep_char->type == USB_DWC_XFER_TYPE_INTR) {
        unsigned int interval_frame_list = ep_char->periodic.interval;
        unsigned int offset_frame_list = ep_char->periodic.offset;
        // Periodic Frame List works with USB frames. For HS endpoints we must divide interval[microframes] by 8 to get interval[frames]
        if (ep_char->periodic.is_hs) {
            interval_frame_list /= 8;
            offset_frame_list /= 8;
        }
        // Interval in Periodic Frame List must be power of 2.
        // This is not a HW restriction. It is just a lot easier to schedule channels like this.
        if (interval_frame_list >= (int)hal->frame_list_len) { // Upper limits is Periodic Frame List length
            interval_frame_list = (int)hal->frame_list_len;
        } else if (interval_frame_list >= 32) {
            interval_frame_list = 32;
        } else if (interval_frame_list >= 16) {
            interval_frame_list = 16;
        } else if (interval_frame_list >= 8) {
            interval_frame_list = 8;
        } else if (interval_frame_list >= 4) {
            interval_frame_list = 4;
        } else if (interval_frame_list >= 2) {
            interval_frame_list = 2;
        } else {                                           // Lower limit is 1
            interval_frame_list = 1;
        }
        // Schedule the channel in the frame list
        for (int i = 0; i < hal->frame_list_len; i += interval_frame_list) {
            int index = (offset_frame_list + i) % hal->frame_list_len;
            hal->periodic_frame_list[index] |= 1 << chan_obj->flags.chan_idx;
        }
        // For HS endpoints we must write to sched_info field of HCTSIZ register to schedule microframes
        // For FS endpoints sched_info is always 0xFF
        // LS endpoints do not support periodic transfers
        unsigned int tokens_per_frame = 0;
        if (ep_char->periodic.is_hs) {
            if (ep_char->periodic.interval >= 8) {
                tokens_per_frame = 1; // 1 token every 8 microframes
            } else if (ep_char->periodic.interval >= 4) {
                tokens_per_frame = 2; // 1 token every 4 microframes
            } else if (ep_char->periodic.interval >= 2) {
                tokens_per_frame = 4; // 1 token every 2 microframes
            } else {
                tokens_per_frame = 8; // 1 token every microframe
            }
        } else {
            tokens_per_frame = 8;
        }
        usb_dwc_ll_hctsiz_set_sched_info(chan_obj->regs, tokens_per_frame, ep_char->periodic.offset);
    }
    HAL_EARLY_LOGV(USB_HAL_TAG, "Channel %d EP char set: dev=%d ep=0x%02x mps=%d type=%d",
                   chan_obj->flags.chan_idx, ep_char->dev_addr, ep_char->bEndpointAddress, ep_char->mps, ep_char->type);
}

// ------------------- Channel Control ---------------------

/**
 * @brief Calculate the number of packets required for a transfer
 *
 * Zero-length transfer still counts as one packet (a single zero-length packet)
 *
 * IN  transfer:    Transfer length must be a multiple of the endpoint's MPS
 *                  Packet count is then computed as transfer length / MPS
 * OUT transfer:    Packet count is then computed as ceil(transfer length / MPS)
 *
 * @param[in] xfer_len      Transfer length in bytes (can be 0)
 * @param[in] mps           Maximum packet size of the endpoint in bytes
 * @param[in] is_in         Transfer direction in = true, out = false
 * @return Number of packets (always >= 1)
 */
static int calc_pkt_count(const int xfer_len, int mps, bool is_in)
{
    if (xfer_len == 0) {
        return 1;                // Zero-length transfer is still one (zero-length) packet on the USB Bus
    }

    /*
     * DWC_otg programming guide, "Initializing Channels when the DWC_otg Host Core is in Buffer DMA"
     * (§3.4, step 5): "Program the Transfer Size field so that the channel’s transfer size is a multiple of the maximum packet size"
     *
     * Note: The guide additionally adjusts the MPS up to the DWORD boundary for non-DWORD-aligned MPS values
     * (only possible for interrupt and isochronous endpoints). This is deliberately not done here: Control and
     * bulk endpoint MPS values are always DWORD-aligned per the USB specification, and periodic transfers are
     * activated per (micro)frame packet.
     */
    int pkt_cnt = 0;
    if (is_in) {
        // The programmed transfer size must be a multiple of the endpoint's MPS
        HAL_ASSERT((xfer_len % mps) == 0);              // Defensive programming: transfer length must be a multiple of the MPS (ensured in the HCD)
        pkt_cnt = xfer_len / mps;                       // Above check ensures non-remainder division here
    } else {
        pkt_cnt = (xfer_len + mps - 1) / mps;           // Ceiling division: ceil(xfer_len / mps) using integer math only
    }
    HAL_ASSERT(pkt_cnt >= 1);                           // The above packet count calculation must result in a non-zero packet count
    return pkt_cnt;
}

void usb_dwc_hal_chan_activate_buffer(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj, const usb_dwc_hal_buffer_xfer_t *xfer)
{
    const int mps = chan_obj->mps;                  // Cached from the endpoint characteristics
    const bool is_in = chan_obj->flags.is_in;       // Cached from the endpoint characteristics
    const int xfer_len = xfer->len;                 // Transfer length must not change (as it's the length the application reserved for the data read/written from USB Bust to memory)
    uint8_t *xfer_addr = xfer->buf;

    HAL_ASSERT(!chan_obj->flags.active);            // Channel must not be already active
    HAL_ASSERT(xfer != NULL);                       // Transfer structure can't be null
    HAL_ASSERT(mps > 0);                            // EP characteristics (with MPS) must be set before activation
    HAL_ASSERT(xfer_addr != NULL);                  // Transfer buffer must be a valid pointer
    HAL_ASSERT(((uintptr_t)xfer_addr & 0x3) == 0);  // The core's DMA always accesses the buffer in DWORDs

    const int pkt_cnt = calc_pkt_count(xfer_len, mps, is_in);

    // Defensive programming: the HCD validates the transfer size and max packet count before activating,
    // They must fit the HCTSIZ.XferSize, HCTSIZ.PktCnt fields, databook Section 5.4.41
    HAL_ASSERT(xfer_len <= hal->constant_config.max_size_byte_limit);       // OTG_TRANS_COUNT_WIDTH
    HAL_ASSERT(pkt_cnt <= hal->constant_config.max_size_packet_limit);      // OTG_PACKET_COUNT_WIDTH

    const bool set_doping = false;
    const uint32_t hctsiz_pid = usb_dwc_ll_pid_to_reg(xfer->pid);
    usb_dwc_ll_hcdma_set_buff_addr(chan_obj->regs, xfer->buf);
    usb_dwc_ll_hctsiz_set_buffer_xfer(chan_obj->regs, (uint32_t)xfer_len, (uint32_t)pkt_cnt, hctsiz_pid, set_doping);

    chan_obj->buffer_dma.xfer_len = xfer_len;       // Save the transfer length to calculate buffer result's actual_len
    chan_obj->buffer_dma.xfer_addr = xfer_addr;     // Save the buffer address to rewind HCDMA to the start of the failed packet on a XactErr retry
    chan_obj->buffer_dma.xfer_pkt_cnt = pkt_cnt;    // Save the packet count to compute successfully transferred packets on a XactErr retry
    usb_dwc_ll_hcchar_enable_chan(chan_obj->regs);  // Enable the channel
    chan_obj->flags.active = 1;

    HAL_EARLY_LOGV(USB_HAL_TAG, "Channel %d activated (buffer DMA): len=%d xfersize=%d pkt=%d pid=%d buff=%p",
                   chan_obj->flags.chan_idx, xfer->len, xfer_len, pkt_cnt, xfer->pid, xfer->buf);
}

void usb_dwc_hal_chan_get_buffer_result(usb_dwc_hal_chan_t *chan_obj, int *actual_len)
{
    HAL_ASSERT(actual_len != NULL);
    if (chan_obj->flags.is_in) {
        const int rem = (int)usb_dwc_ll_hctsiz_get_rem_xfersize(chan_obj->regs);  // Bytes the core did not fill (remaining HCTSIZ.XferSize)
        HAL_ASSERT(rem <= chan_obj->buffer_dma.xfer_len);                         // Remaining can never exceed the programmed buffer size
        *actual_len = chan_obj->buffer_dma.xfer_len - rem;                        // Bytes received = programmed buffer size - remaining
    } else {
        *actual_len = chan_obj->buffer_dma.xfer_len;                              // OUT completes fully on XferCompl, so all programmed bytes were sent
    }
}

void usb_dwc_hal_chan_retry_buffer(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj)
{
    //Can only retry a channel that has halted (e.g., due to a transaction error)
    HAL_ASSERT(!chan_obj->flags.active);
    /*
     * The byte offset of the failed packet must be derived from the PACKET count, not the residual byte count.
     * The DWC_otg databook (§5.4.41) guarantees that HCTSIZ.PktCnt is decremented only "on every successful
     * transmission or reception of a packet", but makes no such guarantee for HCTSIZ.XferSize.
     *
     * For an OUT channel the core prefetches packet data into the TxFIFO as soon as space is available (programming guide §5.1.2.1);
     * that prefetched data is flushed on a XactErr, yet XferSize may already have been decremented for bytes
     * that never reached the device. Using (programmed - XferSize) as the transferred count would over-count,
     * rewind HCDMA past the failed packet, and skip data - corrupting the transfer.
     * PktCnt is immune to this, so:
     *     done_pkt_cnt = start_pkt_count - HCTSIZ.PktCnt
     *     done_bytes   = done_pkt_cnt * MPS
     *
     * The channel is then fully "Re-initialized" as the programming guide requires - "Rewind Buffer
     * Pointers" is paired with "Re-initialize Channel" in §5.1.1.2 / §5.1.2.3, and §3.4 (steps 4-7)
     * defines re-initialization as reprogramming HCTSIZ (transfer size, packet count, PID) plus HCCHAR.
     */
    const bool port_is_hs = (usb_dwc_hal_port_get_conn_speed(hal) == USB_DWC_SPEED_HIGH);

    // Packets still outstanding. PktCnt decrements only on success, so this is reliable (databook §5.4.41).
    const uint32_t rem_pkt_cnt = usb_dwc_ll_hctsiz_get_pkt_cnt(chan_obj->regs);
    HAL_ASSERT(rem_pkt_cnt <= (uint32_t)chan_obj->buffer_dma.xfer_pkt_cnt);
    const uint32_t done_pkt_cnt = (uint32_t)chan_obj->buffer_dma.xfer_pkt_cnt - rem_pkt_cnt;

    // Byte offset of the failed packet. Every packet successfully transferred before it is a full MPS
    // (a short packet can only be the last one, which either completes the transfer or is the failing packet itself),
    // so the offset is exactly done_pkt_cnt * MPS.
    const uint32_t done_bytes = done_pkt_cnt * chan_obj->mps;
    HAL_ASSERT(done_bytes <= (uint32_t)chan_obj->buffer_dma.xfer_len);
    const uint32_t rem_bytes = (uint32_t)chan_obj->buffer_dma.xfer_len - done_bytes;

    // Re-initialize the channel: reprogram HCTSIZ (remaining size, count) preserving the core's next-expected PID,
    // arm PING for HS OUT, rewind HCDMA to the failed packet, then re-enable.
    const uint32_t pid_reg = usb_dwc_ll_hctsiz_get_pid_reg(chan_obj->regs);              // Preserve the next expected PID across re-init
    usb_dwc_ll_hctsiz_set_buffer_xfer(chan_obj->regs, rem_bytes, rem_pkt_cnt, pid_reg, false);

    // PING only for High-Speed OUT after XactErr; §5.1.2.3. On HS-capable targets the LL setter also gates to OUT;
    // on FS-only targets the setter is a no-op stub (PING does not exist there).
    usb_dwc_ll_hctsiz_set_dopng(chan_obj->regs, port_is_hs);
    usb_dwc_ll_hcdma_set_buff_addr(chan_obj->regs, chan_obj->buffer_dma.xfer_addr + done_bytes); // Rewind HCDMA to the failed packet (do NOT read HCDMAB: reserved in Buffer DMA)
    usb_dwc_ll_hcchar_enable_chan(chan_obj->regs);                                               // Re-enable the channel to resume the transfer
    chan_obj->flags.active = 1;                                                                  // Mark the channel active again
    HAL_EARLY_LOGV(USB_HAL_TAG, "Channel %d retrying (buffer DMA): done=%d rem=%d/%dpkt",
                   chan_obj->flags.chan_idx, (int)done_bytes, (int)rem_bytes, (int)rem_pkt_cnt);
}

void usb_dwc_hal_chan_activate(usb_dwc_hal_chan_t *chan_obj, void *xfer_desc_list, int desc_list_len, int start_idx)
{
    // Cannot activate a channel that has already been enabled or is pending error handling
    HAL_ASSERT(!chan_obj->flags.active);
    // Make sure that PING is not enabled from previous transaction
    usb_dwc_ll_hctsiz_set_dopng(chan_obj->regs, false);
    // Set start address of the QTD list and starting QTD index
    usb_dwc_ll_hcdma_set_qtd_list_addr(chan_obj->regs, xfer_desc_list, start_idx);
    usb_dwc_ll_hctsiz_set_qtd_list_len(chan_obj->regs, desc_list_len);
    usb_dwc_ll_hcchar_enable_chan(chan_obj->regs); // Start the channel
    chan_obj->flags.active = 1;
    HAL_EARLY_LOGV(USB_HAL_TAG, "Channel %d activated (SG DMA)", chan_obj->flags.chan_idx);
}

bool usb_dwc_hal_chan_request_halt(usb_dwc_hal_chan_t *chan_obj)
{
    if (chan_obj->flags.active) {
        /*
        Request a halt so long as the channel's active flag is set.
        - If the underlying hardware channel is already halted but the channel is pending interrupt handling,
        disabling the channel will have no effect (i.e., no channel interrupt is generated).
        - If the underlying channel is currently active, disabling the channel will trigger a channel interrupt.

        Regardless, setting the "halt_requested" should cause "usb_dwc_hal_chan_decode_intr()" to report the
        USB_DWC_HAL_CHAN_EVENT_HALT_REQ event when channel interrupt is handled (pending or triggered).
        */
        usb_dwc_ll_hcchar_disable_chan(chan_obj->regs);
        chan_obj->flags.halt_requested = 1;
        return false;
    } else {
        //Channel was never active to begin with, simply return true
        return true;
    }
}

// ------------------------------------------------- Event Handling ----------------------------------------------------

usb_dwc_hal_port_event_t usb_dwc_hal_decode_intr(usb_dwc_hal_context_t *hal)
{
    uint32_t intrs_core = usb_dwc_ll_gintsts_read_and_clear_intrs(hal->dev);  //Read and clear core interrupts
    uint32_t intrs_port = 0;
    if (intrs_core & USB_DWC_LL_INTR_CORE_PRTINT) {
        //There are host port interrupts. Read and clear those as well.
        intrs_port = usb_dwc_ll_hprt_intr_read_and_clear(hal->dev);
    }
    //Note: Do not change order of checks. Regressing events (e.g. enable -> disabled, connected -> connected)
    //always take precedence. ENABLED < DISABLED < CONN < DISCONN < OVRCUR
    usb_dwc_hal_port_event_t event = USB_DWC_HAL_PORT_EVENT_NONE;

    //Check if this is a core or port event
    if ((intrs_core & CORE_EVENTS_INTRS_MSK) || (intrs_port & PORT_EVENTS_INTRS_MSK)) {
        //Do not change the order of the following checks. Some events/interrupts take precedence over others
        if (intrs_core & USB_DWC_LL_INTR_CORE_DISCONNINT) {
            event = USB_DWC_HAL_PORT_EVENT_DISCONN;
            debounce_lock_enable(hal);
            //Mask the port connection and disconnection interrupts to prevent repeated triggering
        } else if (intrs_port & USB_DWC_LL_INTR_HPRT_PRTOVRCURRCHNG) {
            //Check if this is an overcurrent or an overcurrent cleared
            if (usb_dwc_ll_hprt_get_port_overcur(hal->dev)) {
                event = USB_DWC_HAL_PORT_EVENT_OVRCUR;
            } else {
                event = USB_DWC_HAL_PORT_EVENT_OVRCUR_CLR;
            }
        } else if (intrs_port & USB_DWC_LL_INTR_HPRT_PRTENCHNG) {
            if (usb_dwc_ll_hprt_get_port_en(hal->dev)) {   //Host port was enabled
                event = USB_DWC_HAL_PORT_EVENT_ENABLED;
            } else {    //Host port has been disabled
                event = USB_DWC_HAL_PORT_EVENT_DISABLED;
            }
        } else if (intrs_port & USB_DWC_LL_INTR_HPRT_PRTCONNDET && !hal->flags.dbnc_lock_enabled) {
            event = USB_DWC_HAL_PORT_EVENT_CONN;
            debounce_lock_enable(hal);
        } else if (intrs_core & USB_DWC_LL_INTR_CORE_WKUPINT) {
            event = USB_DWC_HAL_PORT_EVENT_REMOTE_WAKEUP;   // Remote wakeup was generated from device
        }
    }
    //Port events always take precedence over channel events
    if (event == USB_DWC_HAL_PORT_EVENT_NONE && (intrs_core & USB_DWC_LL_INTR_CORE_HCHINT)) {
        //One or more channels have pending interrupts. Store the mask of those channels
        hal->channels.chan_pend_intrs_msk = usb_dwc_ll_haint_get_chan_intrs(hal->dev);
        event = USB_DWC_HAL_PORT_EVENT_CHAN;
    }

    return event;
}

usb_dwc_hal_chan_t *usb_dwc_hal_get_chan_pending_intr(usb_dwc_hal_context_t *hal)
{
    HAL_ASSERT(hal->channels.hdls);
    int chan_num = __builtin_ffs(hal->channels.chan_pend_intrs_msk);
    if (chan_num) {
        hal->channels.chan_pend_intrs_msk &= ~(1 << (chan_num - 1));      //Clear the pending bit for that channel
        return hal->channels.hdls[chan_num - 1];
    } else {
        return NULL;
    }
}

usb_dwc_hal_chan_event_t usb_dwc_hal_chan_decode_intr_desc_dma(usb_dwc_hal_chan_t *chan_obj)
{
    uint32_t chan_intrs = usb_dwc_ll_hcint_read_and_clear_intrs(chan_obj->regs);
    usb_dwc_hal_chan_event_t chan_event;
    //Note: We don't assert on (chan_obj->flags.active) here as it could have been already cleared by usb_dwc_hal_chan_request_halt()

    /*
    Note: Do not change order of checks as some events take precedence over others.
    Errors > Channel Halt Request > Transfer completed
    */
    if (chan_intrs & CHAN_INTRS_ERROR_MSK_SG) {    //Note: Errors are uncommon, so we check against the entire interrupt mask to reduce frequency of entering this call path
        //Store the error in hal context
        usb_dwc_hal_chan_error_t error;
        if (chan_intrs & USB_DWC_LL_INTR_CHAN_STALL) {
            error = USB_DWC_HAL_CHAN_ERROR_STALL;
        } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_BBLEER) {
            error = USB_DWC_HAL_CHAN_ERROR_PKT_BBL;
        } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_BNAINTR) {
            error = USB_DWC_HAL_CHAN_ERROR_BNA;
        } else {    //USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR
            // The last unmasked error is XCS_XACT_ERR, the error bits and the error handling for the XACT error differs
            // between the DMA modes, we propagate different error codes to the HCD depending on the DMA mode.
            // HCD then handles the error based on the DMA mode
            error = USB_DWC_HAL_CHAN_ERROR_XCS_XACT;
        }
        //Update flags
        chan_obj->error = error;
        chan_obj->flags.active = 0;
        //Save the error to be handled later
        chan_event = USB_DWC_HAL_CHAN_EVENT_ERROR;
    } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_CHHLTD) {
        if (chan_obj->flags.halt_requested) {
            chan_obj->flags.halt_requested = 0;
            chan_event = USB_DWC_HAL_CHAN_EVENT_HALT_REQ;
        } else {
            //Must have been halted due to QTD HOC
            chan_event = USB_DWC_HAL_CHAN_EVENT_CPLT;
        }
        chan_obj->flags.active = 0;
    } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_XFERCOMPL) {
        /*
        A transfer complete interrupt WITHOUT the channel halting only occurs when receiving a short interrupt IN packet
        and the underlying QTD does not have the HOC bit set. This signifies the last packet of the Interrupt transfer
        as all interrupt packets must MPS sized except the last.
        */
        //The channel isn't halted yet, so we need to halt it manually to stop the execution of the next QTD/packet
        usb_dwc_ll_hcchar_disable_chan(chan_obj->regs);
        /*
        After setting the halt bit, this will generate another channel halted interrupt. We treat this interrupt as
        a NONE event, then cycle back with the channel halted interrupt to handle the CPLT event.
        */
        chan_event = USB_DWC_HAL_CHAN_EVENT_NONE;
    } else {
        abort();
    }
    return chan_event;
}

usb_dwc_hal_chan_event_t usb_dwc_hal_chan_decode_intr_buffer_dma(usb_dwc_hal_chan_t *chan_obj)
{
    uint32_t chan_intrs = usb_dwc_ll_hcint_read_and_clear_intrs(chan_obj->regs);
    usb_dwc_hal_chan_event_t chan_event;
    //Note: We don't assert on (chan_obj->flags.active) here as it could have been already cleared by usb_dwc_hal_chan_request_halt()

    /*
    Note: Do not change order of checks as some events take precedence over others.
    Errors > Channel Halt Request > Transfer completed
    */
    //The transaction-error bit differs by DMA mode (XCS_XACT_ERR for Scatter/Gather, XactErr for Buffer DMA)
    if (chan_intrs & CHAN_INTRS_ERROR_MSK_BUFFER) {    //Note: Errors are uncommon, so we check against the entire interrupt mask to reduce frequency of entering this call path
        //Store the error in hal context
        usb_dwc_hal_chan_error_t error;
        if (chan_intrs & USB_DWC_LL_INTR_CHAN_STALL) {
            error = USB_DWC_HAL_CHAN_ERROR_STALL;
        } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_BBLEER) {
            error = USB_DWC_HAL_CHAN_ERROR_PKT_BBL;
        } else {    // USB_DWC_LL_INTR_CHAN_XACTERR
            // The last unmasked error is XactErr, the error bits and the error handling for the XACT error differs
            // between the DMA modes, we propagate different error codes to the HCD depending on the DMA mode.
            // HCD then handles the error based on the DMA mode
            error = USB_DWC_HAL_CHAN_ERROR_XACT_ERR;
            /*
             * In Buffer DMA mode, some non-error HCINT status bits indicate that the current XactErr is NOT
             * consecutive with the previous one, so the HCD must reset its software error streak instead of
             * incrementing it:
             * - Bulk/control OUT (§5.1.2.3): ACK/NAK/NYET
             * - Bulk/control IN  (§5.1.1.2): ACK/NAK/DataTglErr
             *
             * These bits can accumulate while only CHHLTD is unmasked, so record them here when the halted
             * channel is finally serviced.
             */
            bool xact_err_rst_seen = false;
            const bool is_in = chan_obj->regs->hcchar_reg.epdir;
            xact_err_rst_seen = (chan_intrs & (USB_DWC_LL_INTR_CHAN_ACK | USB_DWC_LL_INTR_CHAN_NAK)) != 0;
            if (is_in) {
                const bool non_periodic = (chan_obj->type == USB_DWC_XFER_TYPE_CTRL || chan_obj->type == USB_DWC_XFER_TYPE_BULK);
                if (non_periodic && (chan_intrs & USB_DWC_LL_INTR_CHAN_DATATGLERR)) {
                    xact_err_rst_seen = true;
                }
            } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_NYET) {
                xact_err_rst_seen = true;
            }
            chan_obj->flags.xact_err_rst_seen = xact_err_rst_seen;
        }
        //Update flags
        chan_obj->error = error;
        chan_obj->flags.active = 0;
        //Save the error to be handled later
        chan_event = USB_DWC_HAL_CHAN_EVENT_ERROR;
    } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_CHHLTD) {
        if (chan_obj->flags.halt_requested) {
            chan_obj->flags.halt_requested = 0;
            chan_event = USB_DWC_HAL_CHAN_EVENT_HALT_REQ;
        } else {
            /*
             * Halted with no error bit and no halt request: the transfer has completed. In Buffer DMA
             * mode, confirm completion against the XferCompl status bit - per the guide's ISR model
             * (§5.1.1.2 / §5.1.2.3), a bulk/control channel never halts without either XferCompl or an error bit
             */
            HAL_ASSERT(chan_intrs & USB_DWC_LL_INTR_CHAN_XFERCOMPL);
            chan_event = USB_DWC_HAL_CHAN_EVENT_CPLT;
        }
        chan_obj->flags.active = 0;
    } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_XFERCOMPL) {
        // Not expected in Buffer DMA: XferCompl is masked so it never raises an interrupt on its own, and
        // whenever this handler runs the channel has already halted (handled on the CHHLTD branch above).
        // Nothing to do here but fall through to the pending CHHLTD.
        chan_event = USB_DWC_HAL_CHAN_EVENT_NONE;
    } else {
        abort();
    }
    return chan_event;
}
