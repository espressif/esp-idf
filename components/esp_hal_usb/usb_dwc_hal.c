/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h> // For memset()
#include <stdlib.h> // For abort()
#include "soc/soc_caps.h"
#include "soc/chip_revision.h"
#include "soc/usb_periph.h"
#include "hal/usb_dwc_hal.h"
#include "hal/usb_dwc_ll.h"
#include "hal/efuse_hal.h"
#include "hal/assert.h"

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
 * The following channel interrupt bits are currently checked (in order LSB to MSB)
 * - USB_DWC_LL_INTR_CHAN_XFERCOMPL
 * - USB_DWC_LL_INTR_CHAN_CHHLTD
 * - USB_DWC_LL_INTR_CHAN_STALL
 * - USB_DWC_LL_INTR_CHAN_BBLEER
 * - USB_DWC_LL_INTR_CHAN_BNAINTR
 * - USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR
 *
 * Note the following points about channel interrupts:
 * - Not all bits are unmaskable under scatter/gather
 * - Those bits proxy their interrupt through the USB_DWC_LL_INTR_CHAN_CHHLTD bit
 * - USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR is always unmasked
 * - When USB_DWC_LL_INTR_CHAN_BNAINTR occurs, USB_DWC_LL_INTR_CHAN_CHHLTD will NOT.
 * - USB_DWC_LL_INTR_CHAN_AHBERR doesn't actually ever happen on our system (i.e., ESP32-S2, ESP32-S3):
 *      - If the QTD list's starting address is an invalid address (e.g., NULL), the core will attempt to fetch that
 *        address for a transfer descriptor and probably gets all zeroes. It will interpret the zero as a bad QTD and
 *        return a USB_DWC_LL_INTR_CHAN_BNAINTR instead.
 *      - If the QTD's buffer pointer is an invalid address, the core will attempt to read/write data to/from that
 *        invalid buffer address with NO INDICATION OF ERROR. The transfer will be acknowledged and treated as
 *        successful. Bad buffer pointers MUST BE CHECKED FROM HIGHER LAYERS INSTEAD.
 */
#define CHAN_INTRS_EN_MSK   (USB_DWC_LL_INTR_CHAN_XFERCOMPL | \
                             USB_DWC_LL_INTR_CHAN_CHHLTD | \
                             USB_DWC_LL_INTR_CHAN_BNAINTR)

#define CHAN_INTRS_ERROR_MSK  (USB_DWC_LL_INTR_CHAN_STALL | \
                               USB_DWC_LL_INTR_CHAN_BBLEER | \
                               USB_DWC_LL_INTR_CHAN_BNAINTR | \
                               USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR)

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
#if SOC_IS(ESP32P4)
    /*
     * ESP32P4-specific initialization: Clear USB PHY suspend state set during system boot.
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
#endif // SOC_IS(ESP32P4)
    usb_dwc_ll_gahbcfg_set_hbstlen(hal->dev, hbstlen);  //Set AHB burst mode
    //GUSBCFG register
    usb_dwc_ll_gusbcfg_dis_hnp_cap(hal->dev);       //Disable HNP
    usb_dwc_ll_gusbcfg_dis_srp_cap(hal->dev);       //Disable SRP

    // If this USB-DWC supports HS PHY, use it
    if (hal->constant_config.hsphy_type != 0) {
        usb_dwc_ll_gusbcfg_set_timeout_cal(hal->dev, 5); // 5 PHY clocks for our HS PHY
        usb_dwc_ll_gusbcfg_set_utmi_phy(hal->dev);
    }
    //Enable interruts
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, 0xFFFFFFFF);     //Mask all interrupts first
    usb_dwc_ll_gintmsk_en_intrs(hal->dev, CORE_INTRS_EN_MSK);   //Unmask global interrupts
    usb_dwc_ll_gintsts_read_and_clear_intrs(hal->dev);   //Clear interrupts
    usb_dwc_ll_gahbcfg_en_global_intr(hal->dev);        //Enable interrupt signal
    //Enable host mode
    usb_dwc_ll_gusbcfg_force_host_mode(hal->dev);
}

void usb_dwc_hal_init(usb_dwc_hal_context_t *hal, int port_id)
{
    // Check if a peripheral is alive by reading the core ID registers
    HAL_ASSERT(port_id < SOC_USB_OTG_PERIPH_NUM);
    usb_dwc_dev_t *dev = USB_DWC_LL_GET_HW(port_id);
    uint32_t core_id = usb_dwc_ll_gsnpsid_get_id(dev);
    HAL_ASSERT(core_id == CORE_REG_GSNPSID_4_00a || core_id == CORE_REG_GSNPSID_4_30a);
    (void) core_id;     //Suppress unused variable warning if asserts are disabled

    // Initialize HAL context
    memset(hal, 0, sizeof(usb_dwc_hal_context_t));
    hal->dev = dev;

    // Save constant configuration of this USB-DWC instance
    /*
    * EPINFO_CTL is located at the end of FIFO, its size is fixed in HW.
    * The reserved size is always the worst-case, which is device mode that requires 4 locations per EP direction (including EP0).
    * Here we just read the FIFO size from HW register, to avoid any ambivalence
    */
    hal->constant_config.fifo_size = usb_dwc_ll_ghwcfg_get_fifo_depth(dev);
    hal->constant_config.hsphy_type = usb_dwc_ll_ghwcfg_get_hsphy_type(dev);
    hal->constant_config.chan_num_total = usb_dwc_ll_ghwcfg_get_channel_num(dev);

    set_defaults(hal);
}

void usb_dwc_hal_deinit(usb_dwc_hal_context_t *hal)
{
    //Disable and clear global interrupt
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, 0xFFFFFFFF); //Disable all interrupts
    usb_dwc_ll_gintsts_read_and_clear_intrs(hal->dev);   //Clear interrupts
    usb_dwc_ll_gahbcfg_dis_global_intr(hal->dev);       //Disable interrupt signal
    hal->dev = NULL;
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
}

void usb_dwc_hal_get_mps_limits(usb_dwc_hal_context_t *hal, usb_hal_fifo_mps_limits_t *mps_limits)
{
    HAL_ASSERT(hal && mps_limits);
    HAL_ASSERT(hal->flags.fifo_sizes_set);

    const usb_dwc_hal_fifo_config_t *fifo_config = &(hal->fifo_config);
    mps_limits->in_mps = (fifo_config->rx_fifo_lines - 2) * 4; // Two lines are reserved for status quadlets internally by USB_DWC
    mps_limits->non_periodic_out_mps = fifo_config->nptx_fifo_lines * 4;
    mps_limits->periodic_out_mps = fifo_config->ptx_fifo_lines * 4;
}

// ---------------------------------------------------- Host Port ------------------------------------------------------

static inline void debounce_lock_enable(usb_dwc_hal_context_t *hal)
{
    //Disable the hprt (connection) and disconnection interrupts to prevent repeated triggerings
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, USB_DWC_LL_INTR_CORE_PRTINT | USB_DWC_LL_INTR_CORE_DISCONNINT);
    hal->flags.dbnc_lock_enabled = 1;
}

void usb_dwc_hal_port_enable(usb_dwc_hal_context_t *hal)
{
    // Host Configuration
    usb_dwc_ll_hcfg_en_scatt_gatt_dma(hal->dev); // Enable Scatther-Gather DMA mode
    usb_dwc_ll_hcfg_dis_perio_sched(hal->dev);   // Disable Periodic Scheduler (for now)

    // Configure PHY clock: Only for USB-DWC with FSLS PHY
    if (hal->constant_config.hsphy_type == 0) {
        usb_dwc_ll_hcfg_set_fsls_phy_clock(hal->dev);
        usb_dwc_ll_hfir_set_frame_interval(hal->dev);
    }
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
    usb_dwc_ll_hcintmsk_set_intr_mask(chan_obj->regs, CHAN_INTRS_EN_MSK);  //Unmask interrupts for this channel
    usb_dwc_ll_hctsiz_init(chan_obj->regs);
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
    //Save channel type
    chan_obj->type = ep_char->type;
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
}

// ------------------- Channel Control ---------------------

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

usb_dwc_hal_chan_event_t usb_dwc_hal_chan_decode_intr(usb_dwc_hal_chan_t *chan_obj)
{
    uint32_t chan_intrs = usb_dwc_ll_hcint_read_and_clear_intrs(chan_obj->regs);
    usb_dwc_hal_chan_event_t chan_event;
    //Note: We don't assert on (chan_obj->flags.active) here as it could have been already cleared by usb_dwc_hal_chan_request_halt()

    /*
    Note: Do not change order of checks as some events take precedence over others.
    Errors > Channel Halt Request > Transfer completed
    */
    if (chan_intrs & CHAN_INTRS_ERROR_MSK) {    //Note: Errors are uncommon, so we check against the entire interrupt mask to reduce frequency of entering this call path
        HAL_ASSERT(chan_intrs & USB_DWC_LL_INTR_CHAN_CHHLTD);  //An error should have halted the channel
        //Store the error in hal context
        usb_dwc_hal_chan_error_t error;
        if (chan_intrs & USB_DWC_LL_INTR_CHAN_STALL) {
            error = USB_DWC_HAL_CHAN_ERROR_STALL;
        } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_BBLEER) {
            error = USB_DWC_HAL_CHAN_ERROR_PKT_BBL;
        } else if (chan_intrs & USB_DWC_LL_INTR_CHAN_BNAINTR) {
            error = USB_DWC_HAL_CHAN_ERROR_BNA;
        } else {    //USB_DWC_LL_INTR_CHAN_XCS_XACT_ERR
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
