// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "hal/usbh_hal.h"
#include "hal/usbh_ll.h"

/* -----------------------------------------------------------------------------
------------------------------- Macros and Types -------------------------------
----------------------------------------------------------------------------- */

// -------------------------------- Constants ----------------------------------

#define CORE_REG_GSNPSID    0x4F54400A
#define CORE_REG_GHWCFG1    0x00000000
#define CORE_REG_GHWCFG2    0x224DD930
#define CORE_REG_GHWCFG3    0x00C804B5
#define CORE_REG_GHWCFG4    0xD3F0A030

// ------------------------------ Configurable ---------------------------------

#define CHAN_MAX_SLOTS      16

/*
FIFO lengths configured as follows:

RXFIFO (Receive FIFO)
 - Recommended: (((LPS/4) + 2) * NUM_PACKETS)  + (NUM_CHAN * 2) + (NUM_BULK_CTRL * 1)
 - Actual: Assume (LPS = 64), (NUM_CHAN = 8), (NUM_BULK_CTRL = 8):
NPTXFIFO (Non-periodic TX FIFO)
 - Recommended: (((LPS/4) + 2) * 2) Fit two largest packet sizes (and each packets overhead info)
 - Actual: Assume LPS is 64 (is the MPS for CTRL/BULK/INTR in FS)
PTXFIFO (Periodic TX FIFO)
 - Recommended: ((LPS/4) + 2) * NUM_PACKETS
 - Actual: Assume a single LPS of 64 (quarter of ISO MPS), then 2 packets worth of overhead
REGFIFO (Register storage)
 - Recommended: 4 * NUM_CHAN
 - Actual: Assume NUM_CHAN is 8
*/
#define HW_FIFO_LEN         256
#define RX_FIFO_LEN         92
#define NPTX_FIFO_LEN       36
#define PTX_FIFO_LEN        72
#define REG_FIFO_LEN        32
_Static_assert((RX_FIFO_LEN + NPTX_FIFO_LEN + PTX_FIFO_LEN + REG_FIFO_LEN) <= HW_FIFO_LEN, "Sum of FIFO lengths not equal to HW_FIFO_LEN");

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
 * - USBH_LL_INTR_CHAN_AHBERR
 * - USBH_LL_INTR_CHAN_STALL
 * - USBH_LL_INTR_CHAN_BBLEER
 * - USBH_LL_INTR_CHAN_BNAINTR
 * - USBH_LL_INTR_CHAN_XCS_XACT_ERR
 *
 * - Not all bits are unmaskable under scatter/gather
 * - Those bits proxy their interrupt through the USBH_LL_INTR_CHAN_CHHLTD bit
 * - USBH_LL_INTR_CHAN_XCS_XACT_ERR is always unmasked
 */
#define CHAN_INTRS_EN_MSK   (USBH_LL_INTR_CHAN_XFERCOMPL | \
                             USBH_LL_INTR_CHAN_CHHLTD | \
                             USBH_LL_INTR_CHAN_BNAINTR)

#define CHAN_INTRS_ERROR_MSK  (USBH_LL_INTR_CHAN_AHBERR | \
                               USBH_LL_INTR_CHAN_STALL | \
                               USBH_LL_INTR_CHAN_BBLEER | \
                               USBH_LL_INTR_CHAN_BNAINTR | \
                               USBH_LL_INTR_CHAN_XCS_XACT_ERR)

/* -----------------------------------------------------------------------------
--------------------------------- Core (Global) --------------------------------
----------------------------------------------------------------------------- */

// ---------------------------- Private Functions ------------------------------

static void set_defaults(usbh_hal_context_t *hal)
{
    usbh_ll_internal_phy_conf(hal->wrap_dev);   //Enable and configure internal PHY
    //GAHBCFG register
    usb_ll_en_dma_mode(hal->dev);
    usb_ll_set_hbstlen(hal->dev, 0);    //INCR16 AHB burst length
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

// ---------------------------- Public Functions -------------------------------

void usbh_hal_init(usbh_hal_context_t *hal)
{
    //Check if a peripheral is alive by reading the core ID registers
    usbh_dev_t *dev = &USBH;
    uint32_t core_id = usb_ll_get_controller_core_id(dev);
    assert(core_id == CORE_REG_GSNPSID);
    //Initialize HAL context
    memset(hal, 0, sizeof(usbh_hal_context_t));
    hal->dev = dev;
    hal->wrap_dev = &USB_WRAP;
    set_defaults(hal);
}

void usbh_hal_deinit(usbh_hal_context_t *hal)
{
    //Disable and clear global interrupt
    usb_ll_dis_intrs(hal->dev, 0xFFFFFFFF); //Disable all interrupts
    usb_ll_intr_read_and_clear(hal->dev);   //Clear interrupts
    usb_ll_dis_global_intr(hal->dev);       //Disable interrupt signal
    hal->dev = NULL;
    hal->wrap_dev = NULL;
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
    //Clear all the flags
    hal->flags.val = 0;
}

/* -----------------------------------------------------------------------------
---------------------------------- Host Port ----------------------------------
----------------------------------------------------------------------------- */

static inline void debounce_lock_enable(usbh_hal_context_t *hal)
{
    //Disable the hprt (connection) and disconnection interrupts to prevent repeated triggerings
    usb_ll_dis_intrs(hal->dev, USB_LL_INTR_CORE_PRTINT | USB_LL_INTR_CORE_DISCONNINT);
    hal->flags.dbnc_lock_enabled = 1;
}

void usbh_hal_port_enable(usbh_hal_context_t *hal)
{
    //Host Configuration
    usbh_ll_hcfg_set_defaults(hal->dev);
    //Todo: Set frame list entries and ena per sched
    //Configure HFIR
    usbh_ll_hfir_set_defaults(hal->dev);
    //Config FIFO sizes
    usb_ll_set_rx_fifo_size(hal->dev, RX_FIFO_LEN);
    usb_ll_set_nptx_fifo_size(hal->dev, RX_FIFO_LEN, NPTX_FIFO_LEN);
    usbh_ll_set_ptx_fifo_size(hal->dev, RX_FIFO_LEN + NPTX_FIFO_LEN, PTX_FIFO_LEN);
}

/* -----------------------------------------------------------------------------
----------------------------------- Channel ------------------------------------
------------------------------------------------------------------------------*/

// --------------------------- Channel Allocation ------------------------------

//Allocate a channel
bool usbh_hal_chan_alloc(usbh_hal_context_t *hal, usbh_hal_chan_t *chan_obj, void *chan_ctx)
{
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
    assert(chan_idx != -1);
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
    usbh_ll_chan_set_pid(chan_obj->regs, 0);      //Set the initial PID to zero
    usbh_ll_chan_hctsiz_init(chan_obj->regs);       //Set the non changing parts of the HCTSIZ registers (e.g., do_ping and sched info)
    return true;
}

//Returns object memory
void usbh_hal_chan_free(usbh_hal_context_t *hal, usbh_hal_chan_t *chan_obj)
{
    //Can only free a channel when in the disabled state and descriptor list released
    assert(!chan_obj->slot.flags.slot_acquired
                      && !chan_obj->flags.active
                      && !chan_obj->flags.error_pending);
    //Deallocate channel
    hal->channels.hdls[chan_obj->flags.chan_idx] = NULL;
    hal->channels.num_allocd--;
    assert(hal->channels.num_allocd >= 0);
}

// ---------------------------- Channel Control --------------------------------

void usbh_hal_chan_set_ep_char(usbh_hal_chan_t *chan_obj, usbh_hal_ep_char_t *ep_char)
{
    //Cannot change ep_char whilst channel is still active or in error
    assert(!chan_obj->flags.active && !chan_obj->flags.error_pending);
    //Set the endpoint characteristics of the pipe
    usbh_ll_chan_hcchar_init(chan_obj->regs,
                             ep_char->dev_addr,
                             ep_char->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK,
                             ep_char->mps,
                             ep_char->type,
                             ep_char->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK);
}

/* -----------------------------------------------------------------------------
------------------------------- Transfers Slots --------------------------------
------------------------------------------------------------------------------*/

void usbh_hal_chan_activate(usbh_hal_chan_t *chan_obj, int num_to_skip)
{
    //Cannot enable a channel that has already been enabled or is pending error handling
    assert(!chan_obj->flags.active && !chan_obj->flags.error_pending);
    assert(chan_obj->slot.flags.slot_acquired);
    //Update the descriptor list index and check if it's within bounds
    chan_obj->slot.flags.cur_qtd_idx += num_to_skip;
    assert(chan_obj->slot.flags.cur_qtd_idx < chan_obj->slot.flags.qtd_list_len);
    chan_obj->flags.active = 1;

    //Set start address of the QTD list and starting QTD index
    usbh_ll_chan_set_dma_addr_non_iso(chan_obj->regs, chan_obj->slot.xfer_desc_list, chan_obj->slot.flags.cur_qtd_idx);
    //Start the channel
    usbh_ll_chan_start(chan_obj->regs);
}

bool usbh_hal_chan_request_halt(usbh_hal_chan_t *chan_obj)
{
    //Cannot request halt on a channel that is pending error handling
    assert(!chan_obj->flags.error_pending);
    if (usbh_ll_chan_is_active(chan_obj->regs)) {
        usbh_ll_chan_halt(chan_obj->regs);
        chan_obj->flags.halt_requested = 1;
        return false;
    }
    return true;
}

/* -----------------------------------------------------------------------------
-------------------------------- Event Handling --------------------------------
----------------------------------------------------------------------------- */

usbh_hal_port_event_t usbh_hal_decode_intr(usbh_hal_context_t *hal)
{
    uint32_t intrs_core = usb_ll_intr_read_and_clear(hal->dev);  //Read and clear core interrupts
    uint32_t intrs_port = 0;
    if (intrs_core & USB_LL_INTR_CORE_PRTINT) {
        //There are host port interrupts. Read and clear those as well.
        intrs_port = usbh_ll_hprt_intr_read_and_clear(hal->dev);
    }
    //Note: Do not change order of checks. Regressing events (e.g. enable -> disabled, connected -> connected)
    //always take precendance. ENABLED < DISABLED < CONN < DISCONN < OVRCUR
    usbh_hal_port_event_t event = -1;

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
    if (intrs_core & USB_LL_INTR_CORE_HCHINT) {
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
    //Currently, all cases where channel interrupts occur will also halt the channel
    assert(chan_intrs & USBH_LL_INTR_CHAN_CHHLTD);
    chan_obj->flags.active = 0;
    //Note: Do not change the current checking order of checks. Certain interrupts (e.g., errors) have precedence over others
    if (chan_intrs & CHAN_INTRS_ERROR_MSK) {  //One of the error interrupts has occurred.
        //Note: Errors are uncommon, so we check against the entire interrupt mask to reduce frequency of entering this call path
        //Store the error in hal context
        usbh_hal_chan_error_t error;
        if (chan_intrs & USBH_LL_INTR_CHAN_AHBERR) {
            error = USBH_HAL_CHAN_ERROR_AHB;
        } else if (chan_intrs & USBH_LL_INTR_CHAN_STALL) {
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
        chan_obj->flags.error_pending = 1;
        //Save the error to be handled later
        chan_event = USBH_HAL_CHAN_EVENT_ERROR;
    } else if (chan_obj->flags.halt_requested) {    //A halt was previously requested and has not been fulfilled
        chan_obj->flags.halt_requested = 0;
        chan_event = USBH_HAL_CHAN_EVENT_HALT_REQ;
    } else if (chan_intrs & USBH_LL_INTR_CHAN_XFERCOMPL) {
        int cur_qtd_idx = usbh_ll_chan_get_ctd(chan_obj->regs);
        //Store current qtd index
        chan_obj->slot.flags.cur_qtd_idx = cur_qtd_idx;
        if (cur_qtd_idx == 0) {
            //If the transfer descriptor list has completed, the CTD index should be 0 (wrapped around)
            chan_event = USBH_HAL_CHAN_EVENT_SLOT_DONE;
        } else {
            chan_event = USBH_HAL_CHAN_EVENT_SLOT_HALT;
        }
    } else {
        //Channel halted suddenly (i.e,, a disconnect)
        chan_event = USBH_HAL_CHAN_EVENT_SUDDEN_HLT;
    }
    return chan_event;
}
