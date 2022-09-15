/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sdkconfig.h"
#include "hal/usb_dwc_hal.h"
#include "hal/usb_dwc_ll.h"
#include "hal/assert.h"

// ------------------------------------------------ Macros and Types ---------------------------------------------------

// ---------------------- Constants ------------------------

#define BENDPOINTADDRESS_NUM_MSK     0x0F   //Endpoint number mask of the bEndpointAddress field of an endpoint descriptor
#define BENDPOINTADDRESS_DIR_MSK     0x80   //Endpoint direction mask of the bEndpointAddress field of an endpoint descriptor

#define CORE_REG_GSNPSID    0x4F54400A
#define CORE_REG_GHWCFG1    0x00000000
#define CORE_REG_GHWCFG2    0x224DD930
#define CORE_REG_GHWCFG3    0x00C804B5
#define CORE_REG_GHWCFG4    0xD3F0A030

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
                               USB_DWC_LL_INTR_CORE_HCHINT)

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
#ifdef CONFIG_IDF_TARGET_ESP32S2
    usb_dwc_ll_gahbcfg_set_hbstlen(hal->dev, 1);    //Use INCR AHB burst. See the ESP32-S2 and later chip ERRATA.
#elif CONFIG_IDF_TARGET_ESP32S3
    usb_dwc_ll_gahbcfg_set_hbstlen(hal->dev, 0);    //Do not use USB burst INCR mode for the ESP32-S3, to avoid interference with other peripherals.
#endif
    //GUSBCFG register
    usb_dwc_ll_gusbcfg_dis_hnp_cap(hal->dev);       //Disable HNP
    usb_dwc_ll_gusbcfg_dis_srp_cap(hal->dev);       //Disable SRP
    //Enable interruts
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, 0xFFFFFFFF);     //Mask all interrupts first
    usb_dwc_ll_gintmsk_en_intrs(hal->dev, CORE_INTRS_EN_MSK);   //Unmask global interrupts
    usb_dwc_ll_gintsts_read_and_clear_intrs(hal->dev);   //Clear interrupts
    usb_dwc_ll_gahbcfg_en_global_intr(hal->dev);        //Enable interrupt signal
    //Enable host mode
    usb_dwc_ll_gusbcfg_force_host_mode(hal->dev);
}

void usb_dwc_hal_init(usb_dwc_hal_context_t *hal)
{
    //Check if a peripheral is alive by reading the core ID registers
    usb_dwc_dev_t *dev = &USB_DWC;
    uint32_t core_id = usb_dwc_ll_gsnpsid_get_id(dev);
    HAL_ASSERT(core_id == CORE_REG_GSNPSID);
    (void) core_id;     //Suppress unused variable warning if asserts are disabled
    //Initialize HAL context
    memset(hal, 0, sizeof(usb_dwc_hal_context_t));
    hal->dev = dev;
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
    while (usb_dwc_ll_grstctl_is_core_soft_reset_in_progress(hal->dev)) {
        ;   //Wait until core reset is done
    }
    while (!usb_dwc_ll_grstctl_is_ahb_idle(hal->dev)) {
        ;   //Wait until AHB Master bus is idle before doing any other operations
    }
    //Set the default bits
    set_defaults(hal);
    //Clear all the flags and channels
    hal->periodic_frame_list = NULL;
    hal->flags.val = 0;
    hal->channels.num_allocd = 0;
    hal->channels.chan_pend_intrs_msk = 0;
    memset(hal->channels.hdls, 0, sizeof(usb_dwc_hal_chan_t *) * USB_DWC_HAL_NUM_CHAN);
}

void usb_dwc_hal_set_fifo_size(usb_dwc_hal_context_t *hal, const usb_dwc_hal_fifo_config_t *fifo_config)
{
    HAL_ASSERT((fifo_config->rx_fifo_lines + fifo_config->nptx_fifo_lines + fifo_config->ptx_fifo_lines) <= USB_DWC_HAL_FIFO_TOTAL_USABLE_LINES);
    //Check that none of the channels are active
    for (int i = 0; i < USB_DWC_HAL_NUM_CHAN; i++) {
        if (hal->channels.hdls[i] != NULL) {
            HAL_ASSERT(!hal->channels.hdls[i]->flags.active);
        }
    }
    //Set the new FIFO lengths
    usb_dwc_ll_grxfsiz_set_fifo_size(hal->dev, fifo_config->rx_fifo_lines);
    usb_dwc_ll_gnptxfsiz_set_fifo_size(hal->dev, fifo_config->rx_fifo_lines, fifo_config->nptx_fifo_lines);
    usb_dwc_ll_hptxfsiz_set_ptx_fifo_size(hal->dev, fifo_config->rx_fifo_lines + fifo_config->nptx_fifo_lines, fifo_config->ptx_fifo_lines);
    //Flush the FIFOs
    usb_dwc_ll_grstctl_flush_nptx_fifo(hal->dev);
    usb_dwc_ll_grstctl_flush_ptx_fifo(hal->dev);
    usb_dwc_ll_grstctl_flush_rx_fifo(hal->dev);
    hal->flags.fifo_sizes_set = 1;
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
    usb_priv_speed_t speed = usb_dwc_ll_hprt_get_speed(hal->dev);
    //Host Configuration
    usb_dwc_ll_hcfg_set_defaults(hal->dev, speed);
    //Configure HFIR
    usb_dwc_ll_hfir_set_defaults(hal->dev, speed);
}

// ----------------------------------------------------- Channel -------------------------------------------------------

// ----------------- Channel Allocation --------------------

bool usb_dwc_hal_chan_alloc(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj, void *chan_ctx)
{
    HAL_ASSERT(hal->flags.fifo_sizes_set);  //FIFO sizes should be set befor attempting to allocate a channel
    //Attempt to allocate channel
    if (hal->channels.num_allocd == USB_DWC_HAL_NUM_CHAN) {
        return false;    //Out of free channels
    }
    int chan_idx = -1;
    for (int i = 0; i < USB_DWC_HAL_NUM_CHAN; i++) {
        if (hal->channels.hdls[i] == NULL) {
            hal->channels.hdls[i] = chan_obj;
            chan_idx = i;
            hal->channels.num_allocd++;
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
    usb_dwc_ll_hctsiz_set_pid(chan_obj->regs, 0);        //Set the initial PID to zero
    usb_dwc_ll_hctsiz_init(chan_obj->regs);       //Set the non changing parts of the HCTSIZ registers (e.g., do_ping and sched info)
    return true;
}

void usb_dwc_hal_chan_free(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj)
{
    if (chan_obj->type == USB_PRIV_XFER_TYPE_INTR || chan_obj->type == USB_PRIV_XFER_TYPE_ISOCHRONOUS) {
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
    hal->channels.num_allocd--;
    HAL_ASSERT(hal->channels.num_allocd >= 0);
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
    if (ep_char->type == USB_PRIV_XFER_TYPE_ISOCHRONOUS || ep_char->type == USB_PRIV_XFER_TYPE_INTR) {
        HAL_ASSERT((int)ep_char->periodic.interval <= (int)hal->frame_list_len);    //Interval cannot exceed the length of the frame list
        //Find the effective offset in the frame list (in case the phase_offset_frames > interval)
        int offset = ep_char->periodic.phase_offset_frames % ep_char->periodic.interval;
        //Schedule the channel in the frame list
        for (int i = offset; i < hal->frame_list_len; i+= ep_char->periodic.interval) {
            hal->periodic_frame_list[i] |= 1 << chan_obj->flags.chan_idx;
        }
    }
}

// ------------------- Channel Control ---------------------

void usb_dwc_hal_chan_activate(usb_dwc_hal_chan_t *chan_obj, void *xfer_desc_list, int desc_list_len, int start_idx)
{
    //Cannot activate a channel that has already been enabled or is pending error handling
    HAL_ASSERT(!chan_obj->flags.active);
    //Set start address of the QTD list and starting QTD index
    usb_dwc_ll_hcdma_set_qtd_list_addr(chan_obj->regs, xfer_desc_list, start_idx);
    usb_dwc_ll_hctsiz_set_qtd_list_len(chan_obj->regs, desc_list_len);
    usb_dwc_ll_hcchar_enable_chan(chan_obj->regs); //Start the channel
    chan_obj->flags.active = 1;
}

bool usb_dwc_hal_chan_request_halt(usb_dwc_hal_chan_t *chan_obj)
{
    //Cannot request halt on a channel that is pending error handling
    if (usb_dwc_ll_hcchar_chan_is_enabled(chan_obj->regs)) {
        //If the register indicates that the channel is still active, the active flag must have been previously set
        HAL_ASSERT(chan_obj->flags.active);
        usb_dwc_ll_hcchar_disable_chan(chan_obj->regs);
        chan_obj->flags.halt_requested = 1;
        return false;
    } else {
        //We just clear the active flag here as it could still be set (if we have a pending channel interrupt)
        chan_obj->flags.active = 0;
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
        abort();    //Should never reach this point
    }
    return chan_event;
}
