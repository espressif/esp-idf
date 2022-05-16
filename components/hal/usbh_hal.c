/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sdkconfig.h"
#include "hal/usbh_hal.h"
#include "hal/usbh_ll.h"
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
 * - USB_LL_INTR_CORE_PRTINT
 * - USB_LL_INTR_CORE_HCHINT
 * - USB_LL_INTR_CORE_DISCONNINT
 * The following PORT interrupts cannot be masked, listed LSB to MSB
 * - USBH_LL_INTR_HPRT_PRTCONNDET
 * - USBH_LL_INTR_HPRT_PRTENCHNG
 * - USBH_LL_INTR_HPRT_PRTOVRCURRCHNG
 */
#define CORE_INTRS_EN_MSK   (USB_LL_INTR_CORE_DISCONNINT)

//Interrupts that pertain to core events
#define CORE_EVENTS_INTRS_MSK (USB_LL_INTR_CORE_DISCONNINT | \
                               USB_LL_INTR_CORE_HCHINT)

//Interrupt that pertain to host port events
#define PORT_EVENTS_INTRS_MSK (USBH_LL_INTR_HPRT_PRTCONNDET | \
                               USBH_LL_INTR_HPRT_PRTENCHNG | \
                               USBH_LL_INTR_HPRT_PRTOVRCURRCHNG)

/**
 * The following channel interrupt bits are currently checked (in order LSB to MSB)
 * - USBH_LL_INTR_CHAN_XFERCOMPL
 * - USBH_LL_INTR_CHAN_CHHLTD
 * - USBH_LL_INTR_CHAN_STALL
 * - USBH_LL_INTR_CHAN_BBLEER
 * - USBH_LL_INTR_CHAN_BNAINTR
 * - USBH_LL_INTR_CHAN_XCS_XACT_ERR
 *
 * Note the following points about channel interrupts:
 * - Not all bits are unmaskable under scatter/gather
 * - Those bits proxy their interrupt through the USBH_LL_INTR_CHAN_CHHLTD bit
 * - USBH_LL_INTR_CHAN_XCS_XACT_ERR is always unmasked
 * - When USBH_LL_INTR_CHAN_BNAINTR occurs, USBH_LL_INTR_CHAN_CHHLTD will NOT.
 * - USBH_LL_INTR_CHAN_AHBERR doesn't actually ever happen on our system (i.e., ESP32-S2, ESP32-S3):
 *      - If the QTD list's starting address is an invalid address (e.g., NULL), the core will attempt to fetch that
 *        address for a transfer descriptor and probably gets all zeroes. It will interpret the zero as a bad QTD and
 *        return a USBH_LL_INTR_CHAN_BNAINTR instead.
 *      - If the QTD's buffer pointer is an invalid address, the core will attempt to read/write data to/from that
 *        invalid buffer address with NO INDICATION OF ERROR. The transfer will be acknowledged and treated as
 *        successful. Bad buffer pointers MUST BE CHECKED FROM HIGHER LAYERS INSTEAD.
 */
#define CHAN_INTRS_EN_MSK   (USBH_LL_INTR_CHAN_XFERCOMPL | \
                             USBH_LL_INTR_CHAN_CHHLTD | \
                             USBH_LL_INTR_CHAN_BNAINTR)

#define CHAN_INTRS_ERROR_MSK  (USBH_LL_INTR_CHAN_STALL | \
                               USBH_LL_INTR_CHAN_BBLEER | \
                               USBH_LL_INTR_CHAN_BNAINTR | \
                               USBH_LL_INTR_CHAN_XCS_XACT_ERR)

// -------------------------------------------------- Core (Global) ----------------------------------------------------

static void set_defaults(usbh_hal_context_t *hal)
{
    //GAHBCFG register
    usb_ll_en_dma_mode(hal->dev);
#ifdef CONFIG_IDF_TARGET_ESP32S2
    usb_ll_set_hbstlen(hal->dev, 1);    //Use INCR AHB burst. See the ESP32-S2 and later chip ERRATA.
#elif CONFIG_IDF_TARGET_ESP32S3
    usb_ll_set_hbstlen(hal->dev, 0);    //Do not use USB burst INCR mode for the ESP32-S3, to avoid interference with other peripherals.
#endif
    //GUSBCFG register
    usb_ll_dis_hnp_cap(hal->dev);       //Disable HNP
    usb_ll_dis_srp_cap(hal->dev);       //Disable SRP
    //Enable interruts
    usb_ll_dis_intrs(hal->dev, 0xFFFFFFFF);     //Mask all interrupts first
    usb_ll_en_intrs(hal->dev, CORE_INTRS_EN_MSK);   //Unmask global interrupts
    usb_ll_intr_read_and_clear(hal->dev);   //Clear interrupts
    usb_ll_en_global_intr(hal->dev);        //Enable interrupt signal
    //Enable host mode
    usb_ll_set_host_mode(hal->dev);
}

void usbh_hal_init(usbh_hal_context_t *hal)
{
    //Check if a peripheral is alive by reading the core ID registers
    usbh_dev_t *dev = &USBH;
    uint32_t core_id = usb_ll_get_controller_core_id(dev);
    HAL_ASSERT(core_id == CORE_REG_GSNPSID);
    (void) core_id;     //Suppress unused variable warning if asserts are disabled
    //Initialize HAL context
    memset(hal, 0, sizeof(usbh_hal_context_t));
    hal->dev = dev;
    set_defaults(hal);
}

void usbh_hal_deinit(usbh_hal_context_t *hal)
{
    //Disable and clear global interrupt
    usb_ll_dis_intrs(hal->dev, 0xFFFFFFFF); //Disable all interrupts
    usb_ll_intr_read_and_clear(hal->dev);   //Clear interrupts
    usb_ll_dis_global_intr(hal->dev);       //Disable interrupt signal
    hal->dev = NULL;
}

void usbh_hal_core_soft_reset(usbh_hal_context_t *hal)
{
    usb_ll_core_soft_reset(hal->dev);
    while (usb_ll_check_core_soft_reset(hal->dev)) {
        ;   //Wait until core reset is done
    }
    while (!usb_ll_check_ahb_idle(hal->dev)) {
        ;   //Wait until AHB Master bus is idle before doing any other operations
    }
    //Set the default bits
    set_defaults(hal);
    //Clear all the flags and channels
    hal->periodic_frame_list = NULL;
    hal->flags.val = 0;
    hal->channels.num_allocd = 0;
    hal->channels.chan_pend_intrs_msk = 0;
    memset(hal->channels.hdls, 0, sizeof(usbh_hal_chan_t *) * USBH_HAL_NUM_CHAN);
}

void usbh_hal_set_fifo_size(usbh_hal_context_t *hal, const usbh_hal_fifo_config_t *fifo_config)
{
    HAL_ASSERT((fifo_config->rx_fifo_lines + fifo_config->nptx_fifo_lines + fifo_config->ptx_fifo_lines) <= USBH_HAL_FIFO_TOTAL_USABLE_LINES);
    //Check that none of the channels are active
    for (int i = 0; i < USBH_HAL_NUM_CHAN; i++) {
        if (hal->channels.hdls[i] != NULL) {
            HAL_ASSERT(!hal->channels.hdls[i]->flags.active);
        }
    }
    //Set the new FIFO lengths
    usb_ll_set_rx_fifo_size(hal->dev, fifo_config->rx_fifo_lines);
    usb_ll_set_nptx_fifo_size(hal->dev, fifo_config->rx_fifo_lines, fifo_config->nptx_fifo_lines);
    usbh_ll_set_ptx_fifo_size(hal->dev, fifo_config->rx_fifo_lines + fifo_config->nptx_fifo_lines, fifo_config->ptx_fifo_lines);
    //Flush the FIFOs
    usb_ll_flush_nptx_fifo(hal->dev);
    usb_ll_flush_ptx_fifo(hal->dev);
    usb_ll_flush_rx_fifo(hal->dev);
    hal->flags.fifo_sizes_set = 1;
}

// ---------------------------------------------------- Host Port ------------------------------------------------------

static inline void debounce_lock_enable(usbh_hal_context_t *hal)
{
    //Disable the hprt (connection) and disconnection interrupts to prevent repeated triggerings
    usb_ll_dis_intrs(hal->dev, USB_LL_INTR_CORE_PRTINT | USB_LL_INTR_CORE_DISCONNINT);
    hal->flags.dbnc_lock_enabled = 1;
}

void usbh_hal_port_enable(usbh_hal_context_t *hal)
{
    usb_priv_speed_t speed = usbh_ll_hprt_get_speed(hal->dev);
    //Host Configuration
    usbh_ll_hcfg_set_defaults(hal->dev, speed);
    //Configure HFIR
    usbh_ll_hfir_set_defaults(hal->dev, speed);
}

// ----------------------------------------------------- Channel -------------------------------------------------------

// ----------------- Channel Allocation --------------------

bool usbh_hal_chan_alloc(usbh_hal_context_t *hal, usbh_hal_chan_t *chan_obj, void *chan_ctx)
{
    HAL_ASSERT(hal->flags.fifo_sizes_set);  //FIFO sizes should be set befor attempting to allocate a channel
    //Attempt to allocate channel
    if (hal->channels.num_allocd == USBH_HAL_NUM_CHAN) {
        return false;    //Out of free channels
    }
    int chan_idx = -1;
    for (int i = 0; i < USBH_HAL_NUM_CHAN; i++) {
        if (hal->channels.hdls[i] == NULL) {
            hal->channels.hdls[i] = chan_obj;
            chan_idx = i;
            hal->channels.num_allocd++;
            break;
        }
    }
    HAL_ASSERT(chan_idx != -1);
    //Initialize channel object
    memset(chan_obj, 0, sizeof(usbh_hal_chan_t));
    chan_obj->flags.chan_idx = chan_idx;
    chan_obj->regs = usbh_ll_get_chan_regs(hal->dev, chan_idx);
    chan_obj->chan_ctx = chan_ctx;
    //Note: EP characteristics configured separately
    //Clean and unmask the channel's interrupt
    usbh_ll_chan_intr_read_and_clear(chan_obj->regs);            //Clear the interrupt bits for that channel
    usbh_ll_haintmsk_en_chan_intr(hal->dev, 1 << chan_obj->flags.chan_idx);
    usbh_ll_chan_set_intr_mask(chan_obj->regs, CHAN_INTRS_EN_MSK);  //Unmask interrupts for this channel
    usbh_ll_chan_set_pid(chan_obj->regs, 0);        //Set the initial PID to zero
    usbh_ll_chan_hctsiz_init(chan_obj->regs);       //Set the non changing parts of the HCTSIZ registers (e.g., do_ping and sched info)
    return true;
}

void usbh_hal_chan_free(usbh_hal_context_t *hal, usbh_hal_chan_t *chan_obj)
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
    usbh_ll_haintmsk_dis_chan_intr(hal->dev, 1 << chan_obj->flags.chan_idx);
    //Deallocate channel
    hal->channels.hdls[chan_obj->flags.chan_idx] = NULL;
    hal->channels.num_allocd--;
    HAL_ASSERT(hal->channels.num_allocd >= 0);
}

// ---------------- Channel Configuration ------------------

void usbh_hal_chan_set_ep_char(usbh_hal_context_t *hal, usbh_hal_chan_t *chan_obj, usbh_hal_ep_char_t *ep_char)
{
    //Cannot change ep_char whilst channel is still active or in error
    HAL_ASSERT(!chan_obj->flags.active);
    //Set the endpoint characteristics of the pipe
    usbh_ll_chan_hcchar_init(chan_obj->regs,
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

void usbh_hal_chan_activate(usbh_hal_chan_t *chan_obj, void *xfer_desc_list, int desc_list_len, int start_idx)
{
    //Cannot activate a channel that has already been enabled or is pending error handling
    HAL_ASSERT(!chan_obj->flags.active);
    //Set start address of the QTD list and starting QTD index
    usbh_ll_chan_set_dma_addr_non_iso(chan_obj->regs, xfer_desc_list, start_idx);
    usbh_ll_chan_set_qtd_list_len(chan_obj->regs, desc_list_len);
    usbh_ll_chan_start(chan_obj->regs); //Start the channel
    chan_obj->flags.active = 1;
}

bool usbh_hal_chan_request_halt(usbh_hal_chan_t *chan_obj)
{
    //Cannot request halt on a channel that is pending error handling
    if (usbh_ll_chan_is_active(chan_obj->regs)) {
        //If the register indicates that the channel is still active, the active flag must have been previously set
        HAL_ASSERT(chan_obj->flags.active);
        usbh_ll_chan_halt(chan_obj->regs);
        chan_obj->flags.halt_requested = 1;
        return false;
    } else {
        //We just clear the active flag here as it could still be set (if we have a pending channel interrupt)
        chan_obj->flags.active = 0;
        return true;
    }
}

// ------------------------------------------------- Event Handling ----------------------------------------------------

usbh_hal_port_event_t usbh_hal_decode_intr(usbh_hal_context_t *hal)
{
    uint32_t intrs_core = usb_ll_intr_read_and_clear(hal->dev);  //Read and clear core interrupts
    uint32_t intrs_port = 0;
    if (intrs_core & USB_LL_INTR_CORE_PRTINT) {
        //There are host port interrupts. Read and clear those as well.
        intrs_port = usbh_ll_hprt_intr_read_and_clear(hal->dev);
    }
    //Note: Do not change order of checks. Regressing events (e.g. enable -> disabled, connected -> connected)
    //always take precedence. ENABLED < DISABLED < CONN < DISCONN < OVRCUR
    usbh_hal_port_event_t event = USBH_HAL_PORT_EVENT_NONE;

    //Check if this is a core or port event
    if ((intrs_core & CORE_EVENTS_INTRS_MSK) || (intrs_port & PORT_EVENTS_INTRS_MSK)) {
        //Do not change the order of the following checks. Some events/interrupts take precedence over others
        if (intrs_core & USB_LL_INTR_CORE_DISCONNINT) {
            event = USBH_HAL_PORT_EVENT_DISCONN;
            debounce_lock_enable(hal);
            //Mask the port connection and disconnection interrupts to prevent repeated triggering
        } else if (intrs_port & USBH_LL_INTR_HPRT_PRTOVRCURRCHNG) {
            //Check if this is an overcurrent or an overcurrent cleared
            if (usbh_ll_hprt_get_port_overcur(hal->dev)) {
                event = USBH_HAL_PORT_EVENT_OVRCUR;
            } else {
                event = USBH_HAL_PORT_EVENT_OVRCUR_CLR;
            }
        } else if (intrs_port & USBH_LL_INTR_HPRT_PRTENCHNG) {
            if (usbh_ll_hprt_get_port_en(hal->dev)) {   //Host port was enabled
                event = USBH_HAL_PORT_EVENT_ENABLED;
            } else {    //Host port has been disabled
                event = USBH_HAL_PORT_EVENT_DISABLED;
            }
        } else if (intrs_port & USBH_LL_INTR_HPRT_PRTCONNDET && !hal->flags.dbnc_lock_enabled) {
            event = USBH_HAL_PORT_EVENT_CONN;
            debounce_lock_enable(hal);
        }
    }
    //Port events always take precedence over channel events
    if (event == USBH_HAL_PORT_EVENT_NONE && (intrs_core & USB_LL_INTR_CORE_HCHINT)) {
        //One or more channels have pending interrupts. Store the mask of those channels
        hal->channels.chan_pend_intrs_msk = usbh_ll_get_chan_intrs_msk(hal->dev);
        event = USBH_HAL_PORT_EVENT_CHAN;
    }

    return event;
}

usbh_hal_chan_t *usbh_hal_get_chan_pending_intr(usbh_hal_context_t *hal)
{
    int chan_num = __builtin_ffs(hal->channels.chan_pend_intrs_msk);
    if (chan_num) {
        hal->channels.chan_pend_intrs_msk &= ~(1 << (chan_num - 1));      //Clear the pending bit for that channel
        return hal->channels.hdls[chan_num - 1];
    } else {
        return NULL;
    }
}

usbh_hal_chan_event_t usbh_hal_chan_decode_intr(usbh_hal_chan_t *chan_obj)
{
    uint32_t chan_intrs = usbh_ll_chan_intr_read_and_clear(chan_obj->regs);
    usbh_hal_chan_event_t chan_event;
    //Note: We don't assert on (chan_obj->flags.active) here as it could have been already cleared by usbh_hal_chan_request_halt()

    if (chan_intrs & CHAN_INTRS_ERROR_MSK) {    //Note: Errors are uncommon, so we check against the entire interrupt mask to reduce frequency of entering this call path
        HAL_ASSERT(chan_intrs & USBH_LL_INTR_CHAN_CHHLTD);  //An error should have halted the channel
        //Store the error in hal context
        usbh_hal_chan_error_t error;
        if (chan_intrs & USBH_LL_INTR_CHAN_STALL) {
            error = USBH_HAL_CHAN_ERROR_STALL;
        } else if (chan_intrs & USBH_LL_INTR_CHAN_BBLEER) {
            error = USBH_HAL_CHAN_ERROR_PKT_BBL;
        } else if (chan_intrs & USBH_LL_INTR_CHAN_BNAINTR) {
            error = USBH_HAL_CHAN_ERROR_BNA;
        } else {    //USBH_LL_INTR_CHAN_XCS_XACT_ERR
            error = USBH_HAL_CHAN_ERROR_XCS_XACT;
        }
        //Update flags
        chan_obj->error = error;
        chan_obj->flags.active = 0;
        //Save the error to be handled later
        chan_event = USBH_HAL_CHAN_EVENT_ERROR;
    } else if (chan_intrs & USBH_LL_INTR_CHAN_CHHLTD) {
        if (chan_obj->flags.halt_requested) {
            chan_obj->flags.halt_requested = 0;
            chan_event = USBH_HAL_CHAN_EVENT_HALT_REQ;
        } else {
            //Must have been halted due to QTD HOC
            chan_event = USBH_HAL_CHAN_EVENT_CPLT;
        }
        chan_obj->flags.active = 0;
    } else if (chan_intrs & USBH_LL_INTR_CHAN_XFERCOMPL) {
        /*
        A transfer complete interrupt WITHOUT the channel halting only occurs when receiving a short interrupt IN packet
        and the underlying QTD does not have the HOC bit set. This signifies the last packet of the Interrupt transfer
        as all interrupt packets must MPS sized except the last.
        */
        //The channel isn't halted yet, so we need to halt it manually to stop the execution of the next QTD/packet
        usbh_ll_chan_halt(chan_obj->regs);
        /*
        After setting the halt bit, this will generate another channel halted interrupt. We treat this interrupt as
        a NONE event, then cycle back with the channel halted interrupt to handle the CPLT event.
        */
        chan_event = USBH_HAL_CHAN_EVENT_NONE;
    } else {
        abort();    //Should never reach this point
    }
    return chan_event;
}
