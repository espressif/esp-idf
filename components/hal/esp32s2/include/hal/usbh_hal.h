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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
NOTE: Thread safety is the responsibility fo the HAL user. All USB Host HAL
      functions should be called from critical sections unless specified otherwise
*/

#include <stdlib.h>
#include <stddef.h>
#include "soc/usbh_struct.h"
#include "soc/usb_wrap_struct.h"
#include "hal/usbh_ll.h"
#include "hal/usb_types.h"

/* -----------------------------------------------------------------------------
------------------------------- Macros and Types -------------------------------
----------------------------------------------------------------------------- */

// ---------------------------- Constants/Configs ------------------------------

#define USBH_HAL_DMA_MEM_ALIGN              512
#define USBH_HAL_NUM_CHAN                   8
#define USBH_HAL_XFER_DESC_SIZE             (sizeof(usbh_ll_dma_qtd_t))

// ------------------------------- HAL States ----------------------------------

/**
 * @brief Channel states
 *
 */
typedef enum {
    USBH_HAL_CHAN_STATE_HALTED = 0,         /**< The channel is halted. No transfer descriptor list is being executed */
    USBH_HAL_CHAN_STATE_ACTIVE,             /**< The channel is active. A transfer descriptor list is being executed */
    USBH_HAL_CHAN_STATE_ERROR,              /**< The channel is in the error state */
} usbh_hal_chan_state_t;

// ------------------------------- HAL Events ----------------------------------

/**
 * @brief Host port HAL events
 */
typedef enum {
    USBH_HAL_PORT_EVENT_NONE,               /**< No event occurred, or could not decode interrupt */
    USBH_HAL_PORT_EVENT_CHAN,               /**< A channel event has occurred. Call the the channel event handler instead */
    USBH_HAL_PORT_EVENT_CONN,               /**< The host port has detected a connection */
    USBH_HAL_PORT_EVENT_DISCONN,            /**< The host port has been disconnected */
    USBH_HAL_PORT_EVENT_ENABLED,            /**< The host port has been enabled (i.e., connected to a device that has been reset. Started sending SOFs) */
    USBH_HAL_PORT_EVENT_DISABLED,           /**< The host port has been disabled (no more SOFs). Could be due to disable/reset request, or a port error (e.g. port babble condition. See 11.8.1 of USB2.0 spec) */
    USBH_HAL_PORT_EVENT_OVRCUR,             /**< The host port has encountered an overcurrent condition */
    USBH_HAL_PORT_EVENT_OVRCUR_CLR,         /**< The host port has been cleared of the overcurrent condition */
} usbh_hal_port_event_t;

/**
 * @brief Channel events
 */
typedef enum {
    USBH_HAL_CHAN_EVENT_SLOT_DONE,          /**< The channel has completed execution of an entire transfer descriptor list. Channel is now halted */
    USBH_HAL_CHAN_EVENT_SLOT_HALT,          /**< The channel as completed execution of a single transfer descriptor in a list. Channel is now halted */
    USBH_HAL_CHAN_EVENT_ERROR,              /**< The channel has encountered an error. Channel is now halted. */
    USBH_HAL_CHAN_EVENT_HALT_REQ,           /**< The channel has been successfully halted as requested */
} usbh_hal_chan_event_t;

// ------------------------------- HAL Errors ----------------------------------

/**
 * @brief Channel errors
 */
typedef enum {
    USBH_HAL_CHAN_ERROR_XCS_XACT = 0,       /**< Excessive (three consecutive) transaction errors (e.g., no response, bad CRC etc */
    USBH_HAL_CHAN_ERROR_BNA,                /**< Buffer Not Available error (i.e., transfer slot is unfilled */
    USBH_HAL_CHAN_ERROR_PKT_BBL,            /**< Packet babbler error (packet exceeded MPS) */
    USBH_HAL_CHAN_ERROR_STALL,              /**< STALL response received */
} usbh_hal_chan_error_t;

// ----------------------- Transfer Descriptor Related -------------------------

/**
 * @brief Flags used to describe the type of transfer descriptor to fill
 */
#define USBH_HAL_XFER_DESC_FLAG_IN          0x01
#define USBH_HAL_XFER_DESC_FLAG_SETUP       0x02
#define USBH_HAL_XFER_DESC_FLAG_NULL        0x04
#define USBH_HAL_XFER_DESC_FLAG_HALT        0x08

/**
 * @brief Status value of a transfer descriptor
 *
 * A transfer descriptor's status remains unexecuted until the entire transfer
 * descriptor completes (either successfully or an error). Therefore, if a
 * channel halt is requested before a transfer descriptor completes, the
 * transfer descriptoor remains unexecuted.
 */
#define USBH_HAL_XFER_DESC_STS_SUCCESS      USBH_LL_QTD_STATUS_SUCCESS
#define USBH_HAL_XFER_DESC_STS_PKTERR       USBH_LL_QTD_STATUS_PKTERR
#define USBH_HAL_XFER_DESC_STS_BUFFER_ERR   USBH_LL_QTD_STATUS_BUFFER
#define USBH_HAL_XFER_DESC_STS_NOT_EXECUTED USBH_LL_QTD_STATUS_NOT_EXECUTED

// ------------------------------ Object Types ---------------------------------

/**
 * @brief Endpoint characteristics structure
 */
typedef struct {
    union {
        struct {
            usb_xfer_type_t type: 2;        /**< The type of endpoint */
            uint32_t bEndpointAddress: 8;   /**< Endpoint address (containing endpoint number and direction) */
            uint32_t mps: 11;               /**< Maximum Packet Size */
            uint32_t dev_addr: 8;           /**< Device Address */
            uint32_t ls_via_fs_hub: 1;      /**< The endpoint is on a LS device that is routed through an FS hub.
                                                 Setting this bit will lead to the addition of the PREamble packet */
            uint32_t reserved2: 2;
        };
        uint32_t val;
    };
} usbh_hal_ep_char_t;

/**
 * @brief Channel object
 */
typedef struct {
    //Channel control, status, and information
    union {
        struct {
            uint32_t active: 1;             /**< The channel is enabled */
            uint32_t halt_requested: 1;     /**< A halt has been requested */
            uint32_t error_pending: 1;      /**< The channel is waiting for the error to be handled */
            uint32_t chan_idx: 4;           /**< The index number of the channel */
            uint32_t reserved25: 25;
        };
        uint32_t val;
    } flags;                                /**< Flags regarding channel's status and information */
    usb_host_chan_regs_t *regs;             /**< Pointer to the channel's register set */
    usbh_hal_chan_error_t error;            /**< The last error that occurred on the channel */
    void *chan_ctx;                         /**< Context variable for the owner of the channel */
    //Transfer Descriptor List Slot
    struct {
        union {
            struct {
                uint32_t slot_acquired: 1;  /**< The transfer descriptor list slot has been acquired */
                uint32_t reserved7: 7;
                uint32_t cur_qtd_idx: 8;    /**< Index of the first QTD in chain of QTDs being executed */
                uint32_t qtd_list_len: 8;   /**< Length of QTD list in number of QTDs */
                uint32_t reserved8: 8;
            };
            uint32_t val;
        } flags;
        void *owner_ctx;                    /**< Context variable for the owner of the slot */
        usbh_ll_dma_qtd_t *xfer_desc_list;  /**< Pointer to transfer descriptor list */
    } slot;
} usbh_hal_chan_t;

/**
 * @brief HAL context structure
 */
typedef struct {
    //Context
    usbh_dev_t *dev;                            /**< Pointer to base address of DWC_OTG registers */
    usb_wrap_dev_t *wrap_dev;                   /**< Pointer to base address of USB Wrapper registers */
    //Host Port related
    union {
        struct {
            uint32_t dbnc_lock_enabled: 1;  /**< Debounce lock enabled */
            uint32_t reserved31: 31;
        };
        uint32_t val;
    } flags;
    //Channel related
    struct {
        int num_allocd;                             /**< Number of channels currently allocated */
        uint32_t chan_pend_intrs_msk;               /**< Bit mask of channels with pending interrupts */
        usbh_hal_chan_t *hdls[USBH_HAL_NUM_CHAN];   /**< Handles of each channel. Set to NULL if channel has not been allocated */
    } channels;
} usbh_hal_context_t;

/* -----------------------------------------------------------------------------
--------------------------------- Core (Global) --------------------------------
----------------------------------------------------------------------------- */

/**
 * @brief Initialize the HAL context and check if DWC_OTG is alive
 *
 * Entry:
 * - The peripheral must have been reset and clock un-gated
 * - GPIO pins configured
 * - Interrupt allocated but DISABLED (in case of an unknown interupt state)
 * Exit:
 * - Checks to see if DWC_OTG is alive, and if HW version/config is correct
 * - HAl context initialized
 * - Sets default values to some global and OTG registers (GAHBCFG and GUSBCFG)
 * - Umask global interrupt signal
 * - Put DWC_OTG into host mode. Require 25ms delay before this takes effect.
 * - State -> USBH_HAL_PORT_STATE_OTG
 * - Interrupts cleared. Users can now enable their ISR
 *
 * @param[inout] hal Context of the HAL layer
 */
void usbh_hal_init(usbh_hal_context_t *hal);

/**
 * @brief Deinitialize the HAL context
 *
 * Entry:
 * - All channels should be properly disabled, and any pending events handled
 * Exit:
 * - DWC_OTG global interrupt disabled
 * - HAL context deinitialized
 *
 * @param hal Context of the HAL layer
 */
void usbh_hal_deinit(usbh_hal_context_t *hal);

/**
 * @brief Issue a soft reset to the controller
 *
 * This should be called when the host port encounters an error event or has
 * been disconnected. Before calling this, users are responsible for safely
 * freeing all channels as a soft reset will wipe all host port nd channel
 * registers.
 *
 * This function will result in the host port being put back into same state as
 * after calling usbh_hal_init().
 *
 * @note This has nothing to do with a USB bus reset. It simply resets the peripheral
 *
 * @param hal Context of the HAL layer
 */
void usbh_hal_core_soft_reset(usbh_hal_context_t *hal);

/* -----------------------------------------------------------------------------
---------------------------------- Host Port ----------------------------------
----------------------------------------------------------------------------- */

// ---------------------------- Host Port Control ------------------------------

/**
 * @brief Enable the host port's interrupt allowing port and channel events to occur
 *
 * @param hal Context of the HAL layer
 */
static inline void usbh_hal_port_start(usbh_hal_context_t *hal)
{
    //Configure Host related interrupts
    usbh_ll_haintmsk_dis_chan_intr(hal->dev, 0xFFFFFFFF);   //Disable interrupts for all channels
    usb_ll_en_intrs(hal->dev, USB_LL_INTR_CORE_PRTINT | USB_LL_INTR_CORE_HCHINT);
}

/**
 * @brief Disable the host port's interrupt preventing any further port or channel events
 *
 * @param hal Context of the HAL layer
 */
static inline void usbh_hal_port_stop(usbh_hal_context_t *hal)
{
    //Disable Host port and channel interrupts
    usb_ll_dis_intrs(hal->dev, USB_LL_INTR_CORE_PRTINT | USB_LL_INTR_CORE_HCHINT);
}

/**
 * @brief Toggle the host port's power
 *
 * @param hal Context of the HAL layer
 * @param power_on Whether to power ON or OFF the port
 */
static inline void usbh_hal_port_toggle_power(usbh_hal_context_t *hal, bool power_on)
{
    if (power_on) {
        usbh_ll_hprt_en_pwr(hal->dev);
    } else {
        usbh_ll_hprt_dis_pwr(hal->dev);
    }
}

/**
 * @brief Toggle reset signal on the bus
 *
 * The reset signal should be held for at least 10ms
 * Entry:
 * - Host port detects a device connection or Host port is already enabled
 * Exit:
 * - On release of the reset signal, a USBH_HAL_PORT_EVENT_ENABLED will be generated
 *
 * @note If the host port is already enabled, then issuing a reset will cause
 *       it be disabled and generate a USBH_HAL_PORT_EVENT_DISABLED event. The
 *       host port will not be enabled until the reset signal is released (thus
 *       generating the USBH_HAL_PORT_EVENT_ENABLED event)
 *
 * @param hal Context of the HAL layer
 * @param enable Enable/disable reset signal
 */
static inline void usbh_hal_port_toggle_reset(usbh_hal_context_t *hal, bool enable)
{
    assert(hal->channels.num_allocd == 0);  //Cannot reset if there are still allocated channels
    usbh_ll_hprt_set_port_reset(hal->dev, enable);
}

/**
 * @brief Enable the host port
 *
 * Entry:
 * - Host port enabled event triggered following a reset
 * Exit:
 * - Host port enabled to operate in scatter/gather DMA mode
 * - DMA fifo sizes configured
 *
 * @param hal Context of the HAL layer
 */
void usbh_hal_port_enable(usbh_hal_context_t *hal);

/**
 * @brief Disable the host port
 *
 * Exit:
 * - Host port disabled event triggered
 *
 * @param hal Context of the HAL layer
 */
static inline void usbh_hal_port_disable(usbh_hal_context_t *hal)
{
    usbh_ll_hprt_port_dis(hal->dev);
}

/**
 * @brief Suspend the host port
 *
 * @param hal Context of the HAL layers
 */
static inline void usbh_hal_port_suspend(usbh_hal_context_t *hal)
{
    usbh_ll_hprt_set_port_suspend(hal->dev);
}

/**
 * @brief Toggle resume signal on the bus
 *
 * Hosts should hold the resume signal for at least 20ms
 *
 * @note If a remote wakeup event occurs, the resume signal is driven
 *       and cleared automatically.
 *
 * @param hal Context of the HAL layer
 * @param enable Enable/disable resume signal
 */
static inline void usbh_hal_port_toggle_resume(usbh_hal_context_t *hal, bool enable)
{
    if (enable) {
        usbh_ll_hprt_set_port_resume(hal->dev);
    } else {
        usbh_ll_hprt_clr_port_resume(hal->dev);
    }
}

/**
 * @brief Check whether the resume signal is being driven
 *
 * If a remote wakeup event occurs, the core will automatically drive and clear
 * the resume signal for the required amount of time. Call this function to
 * check whether the resume signal has completed.
 *
 * @param hal Context of the HAL layer
 * @return true Resume signal is still being driven
 * @return false Resume signal is no longer driven
 */
static inline bool usbh_hal_port_check_resume(usbh_hal_context_t *hal)
{
    return usbh_ll_hprt_get_port_resume(hal->dev);
}

// -------------------------- Host Port Status/State ---------------------------

/**
 * @brief Check if a device is currently connected to the host port
 *
 * This function is intended to be called after one of the following events
 * followed by an adequate debounce delay
 * - USBH_HAL_PORT_EVENT_CONN
 * - USBH_HAL_PORT_EVENT_DISCONN
 *
 * @note No other connection/disconnection event will occur again until the
 *       debounce lock is disabled via usbh_hal_disable_debounce_lock()
 *
 * @param hal Context of the HAL layer
 * @return true A device is connected to the host port
 * @return false A device is not connected to the host port
 */
static inline bool usbh_hal_port_check_if_connected(usbh_hal_context_t *hal)
{
    return usbh_ll_hprt_get_conn_status(hal->dev);
}

/**
 * @brief Check the speed (LS/FS) of the device connected to the host port
 *
 * @note This function should only be called after confirming that a device is
 *       connected to the host port
 *
 * @param hal Context of the HAL layer
 * @return usb_speed_t Speed of the connected device
 */
static inline usb_speed_t usbh_hal_port_get_conn_speed(usbh_hal_context_t *hal)
{
    return usbh_ll_hprt_get_speed(hal->dev);
}

/**
 * @brief Disable the debounce lock
 *
 * This function should be called after calling usbh_hal_port_check_if_connected()
 * and will allow connection/disconnection events to occur again. Any pending
 * connection or disconenction interrupts are cleared.
 *
 * @param hal Context of the HAL layer
 */
static inline void usbh_hal_disable_debounce_lock(usbh_hal_context_t *hal)
{
    hal->flags.dbnc_lock_enabled = 0;
    //Clear Conenction and disconenction interrupt in case it triggered again
    usb_ll_intr_clear(hal->dev, USB_LL_INTR_CORE_DISCONNINT);
    usbh_ll_hprt_intr_clear(hal->dev, USBH_LL_INTR_HPRT_PRTENCHNG);
    //Reenable the hprt (connection) and disconnection interrupts
    usb_ll_en_intrs(hal->dev, USB_LL_INTR_CORE_PRTINT | USB_LL_INTR_CORE_DISCONNINT);
}

/* -----------------------------------------------------------------------------
----------------------------------- Channel ------------------------------------
------------------------------------------------------------------------------*/

// --------------------------- Channel Allocation ------------------------------

/**
 * @brief Allocate a channel
 *
 * @param[in] hal Context of the HAL layer
 * @param[inout] chan_obj Empty channel object
 * @param[in] chan_ctx Context variable for the allocator of the channel
 * @return true Channel successfully allocated
 * @return false Failed to allocate channel
 */
bool usbh_hal_chan_alloc(usbh_hal_context_t *hal, usbh_hal_chan_t *chan_obj, void *chan_ctx);

/**
 * @brief Free a channel
 *
 * @param[in] hal Context of the HAL layer
 * @param[in] chan_obj Channel object
 */
void usbh_hal_chan_free(usbh_hal_context_t *hal, usbh_hal_chan_t *chan_obj);

/**
 * @brief Get the context variable of the channel
 *
 * @param[in] chan_obj Channel object
 * @return void* The context variable of the channel
 */
static inline void *usbh_hal_chan_get_context(usbh_hal_chan_t *chan_obj)
{
    return chan_obj->chan_ctx;
}

// ---------------------------- Channel Control --------------------------------

/**
 * @brief Get the current state of a channel
 *
 * @param chan_obj Channel object
 * @return usbh_hal_chan_state_t State of the channel
 */
static inline usbh_hal_chan_state_t usbh_hal_chan_get_state(usbh_hal_chan_t *chan_obj)
{
    if (chan_obj->flags.error_pending) {
        return USBH_HAL_CHAN_STATE_ERROR;
    } else if (chan_obj->flags.active) {
        return USBH_HAL_CHAN_STATE_ACTIVE;
    } else {
        return USBH_HAL_CHAN_STATE_HALTED;
    }
}

/**
 * @brief Set the endpoint information for a particular channel
 *
 * This should be called when a channel switches target from one EP to another
 *
 * @note the channel must be in the disabled state in order to change its EP
 *       information
 *
 * @param chan_obj Channel object
 * @param ep_char Endpoint characteristics
 */
void usbh_hal_chan_set_ep_char(usbh_hal_chan_t *chan_obj, usbh_hal_ep_char_t *ep_char);

/**
 * @brief Set the direction of the channel
 *
 * This is a convenience function to flip the direction of a channel without
 * needing to reconfigure all of the channel's EP info. This is used primarily
 * for control transfers.
 *
 * @note This function should only be called when the channel is in the disabled
 *       state or is halted from a USBH_HAL_CHAN_EVENT_SLOT_HALT event
 *
 * @param chan_obj Channel object
 * @param is_in Whether the direction is IN
 */
static inline void usbh_hal_chan_set_dir(usbh_hal_chan_t *chan_obj, bool is_in)
{
    //Cannot change direction whilst channel is still active or in error
    assert(!chan_obj->flags.active && !chan_obj->flags.error_pending);
    usbh_ll_chan_set_dir(chan_obj->regs, is_in);
}

/**
 * @brief Set the next Packet ID of the channel (e.g., DATA0/DATA1)
 *
 * This should be called when a channel switches target from one EP to another
 * or when change stages for a control transfer
 *
 * @note The channel should only be called when the channel is in the
 *       halted state.
 *
 * @param chan_obj Channel object
 * @param pid PID of the next DATA packet (DATA0 or DATA1)
 */
static inline void usbh_hal_chan_set_pid(usbh_hal_chan_t *chan_obj, int pid)
{
    //Cannot change pid whilst channel is still active or in error
    assert(!chan_obj->flags.active && !chan_obj->flags.error_pending);
    //Update channel object and set the register
    usbh_ll_chan_set_pid(chan_obj->regs, pid);
}

/**
 * @brief Get the next PID of a channel
 *
 * Returns the next PID (DATA0 or DATA1) of the channel. This function should be
 * used when the next PID of a pipe needs to be saved (e.g., when switching pipes
 * on a channel)
 *
 * @param chan_obj Channel object
 * @return uint32_t Starting PID of the next transfer (DATA0 or DATA1)
 */
static inline uint32_t usbh_hal_chan_get_pid(usbh_hal_chan_t *chan_obj)
{
    assert(!chan_obj->flags.active && !chan_obj->flags.error_pending);
    return usbh_ll_chan_get_pid(chan_obj->regs);
}

/**
 * @brief Get a channel's error
 *
 * @param chan_obj Channel object
 * @return usbh_hal_chan_error_t The type of error the channel has encountered
 */
static inline usbh_hal_chan_error_t usbh_hal_chan_get_error(usbh_hal_chan_t *chan_obj)
{
    assert(chan_obj->flags.error_pending);
    return chan_obj->error;
}

/**
 * @brief Clear a channel of it's error
 *
 * @param chan_obj Channel object
 */
static inline void usbh_hal_chan_clear_error(usbh_hal_chan_t *chan_obj)
{
    //Can only clear error when an error has occurred
    assert(chan_obj->flags.error_pending);
    chan_obj->flags.error_pending = 0;
}

/* -----------------------------------------------------------------------------
-------------------------- Transfer Descriptor List ----------------------------
------------------------------------------------------------------------------*/

/**
 * @brief Fill a single entry in a transfer descriptor list
 *
 * - A single entry corresponds to a USB transfer in a particular direction
 *   (e.g., a BULK OUT).
 * - The channel will automatically split the transfer into multiple MPS sized
 *   packets of the endpoint.
 * - For multi direction transfers (such as the various stages of a control transfer),
 *   the direction and PID of channel must be managed manually. Set the
 *   USBH_HAL_XFER_DESC_FLAG_HALT flag to halt on each entry to flip the direction
 *   and PID of the channel.
 * - For IN transfer entries, set the USBH_HAL_XFER_DESC_FLAG_IN. The transfer
 *   size must also be an integer multiple of the endpoint's MPS
 *
 * @note The USBH_HAL_XFER_DESC_FLAG_HALT must be set on the last descriptor of
 *       the list so that an interrupt is generated at the end of the list
 * @note The USBH_HAL_XFER_DESC_FLAG_HALT can be set on every descriptor if users
 *       prefer to manually step through the list (such as change EP directions in between)
 * @note Critical section is not required for this function
 *
 * @param xfer_desc_list Transfer descriptor list
 * @param xfer_desc_idx Transfer descriptor index
 * @param xfer_data_buff Transfer data buffer
 * @param xfer_len Transfer length
 * @param flags Transfer flags
 */
static inline void usbh_hal_xfer_desc_fill(void *xfer_desc_list, int xfer_desc_idx, uint8_t *xfer_data_buff, int xfer_len, uint32_t flags)
{
    //Check if the channel should be halted on completion of this xfer descriptor
    bool halt_on_xfer_cplt = flags & USBH_HAL_XFER_DESC_FLAG_HALT;
    usbh_ll_dma_qtd_t *qtd_list = (usbh_ll_dma_qtd_t *)xfer_desc_list;
    if (flags & USBH_HAL_XFER_DESC_FLAG_NULL) {
        usbh_ll_set_qtd_null(&qtd_list[xfer_desc_idx]);
    } else if (flags & USBH_HAL_XFER_DESC_FLAG_IN) {
        usbh_ll_set_qtd_in(&qtd_list[xfer_desc_idx], xfer_data_buff, xfer_len, halt_on_xfer_cplt);
    } else {
        usbh_ll_set_qtd_out(&qtd_list[xfer_desc_idx], xfer_data_buff, xfer_len, halt_on_xfer_cplt, (flags & USBH_HAL_XFER_DESC_FLAG_SETUP));
    }
}

/**
 * @brief Parse a transfer decriptors results
 *
 * @param xfer_desc_list Transfer descriptor list
 * @param xfer_desc_idx Transfer descriptor index
 * @param[out] xfer_rem_len Remaining length of the transfer in bytes
 * @param[out] xfer_status Status of the transfer
 *
 * @note Critical section is not required for this function
 */
static inline void usbh_hal_xfer_desc_parse(void *xfer_desc_list, int xfer_desc_idx, int *xfer_rem_len, int *xfer_status)
{
    usbh_ll_dma_qtd_t *qtd_list = (usbh_ll_dma_qtd_t *)xfer_desc_list;
    usbh_ll_get_qtd_status(&qtd_list[xfer_desc_idx], xfer_rem_len, xfer_status);
}

/* -----------------------------------------------------------------------------
-------------------------------- Channel Slot ----------------------------------
------------------------------------------------------------------------------*/

/**
 * @brief Acquire a slot
 *
 * Acquiring a channel's transfer descriptor list slot will cause a give ownership
 * of the channel to the acquirer. The transfer descriptor list to be executed
 * when the channel is activated.
 *
 * @param chan_obj Channel object
 * @param xfer_desc_list A filled transfer descriptor list
 * @param desc_list_len Length of the descriptor list
 * @param owner_ctx Context variable of the owner
 */
static inline void usbh_hal_chan_slot_acquire(usbh_hal_chan_t *chan_obj, void *xfer_desc_list, int desc_list_len, void *owner_ctx)
{
    assert(!chan_obj->slot.flags.slot_acquired);
    chan_obj->slot.xfer_desc_list = (usbh_ll_dma_qtd_t *)xfer_desc_list;
    chan_obj->slot.owner_ctx = owner_ctx;
    chan_obj->slot.flags.cur_qtd_idx = 0;   //Start from the first descriptor
    chan_obj->slot.flags.qtd_list_len = desc_list_len;
    chan_obj->slot.flags.slot_acquired = 1;
    //Store the descriptor list length in the HCTSIZ register. Address of desc list is set when channel is activated
    usbh_ll_chan_set_qtd_list_len(chan_obj->regs, desc_list_len);
}

/**
 * @brief Get current owner of a slot
 *
 * This function reqturns a slot's context variable that was set when the slot
 * was acquired
 *
 * @param chan_obj Channel object
 * @return void* Context variable of the owner of the slot
 */
static inline void *usbh_hal_chan_slot_get_owner(usbh_hal_chan_t *chan_obj)
{
    assert(chan_obj->slot.flags.slot_acquired);
    return chan_obj->slot.owner_ctx;
}

/**
 * @brief Release a slot
 *
 * @note This should only be called after confirming that the transfer descriptor
 *       list has completed execution.
 * @note Users should parse the completed transfer descriptor list to check the
 *       results of each transfer.
 *
 * @param[in] chan_obj Channel object
 * @param[out] xfer_desc_list A completed transfer descriptor list
 * @param[out] desc_list_len Length of the descriptor list
 */
static inline void usbh_hal_chan_slot_release(usbh_hal_chan_t *chan_obj, void **xfer_desc_list, int *desc_list_len)
{
    assert(chan_obj->slot.flags.slot_acquired);
    *xfer_desc_list = (void *)chan_obj->slot.xfer_desc_list;
    *desc_list_len = chan_obj->slot.flags.qtd_list_len;
    chan_obj->slot.flags.slot_acquired = 0;
}

/**
 * @brief Activate a channel
 *
 * Activating a channel will cause it to start executing the transfer descriptor
 * list in its slot starting from its next descriptor index. When a transfer
 * descriptor completes execution and has the HALT flag set, an event will be
 * generated.
 *
 * @param chan_obj Channel object
 * @param num_to_skip Number of transfer descriptors to skip over
 */
void usbh_hal_chan_activate(usbh_hal_chan_t *chan_obj, int num_to_skip);

/**
 * @brief Get next transfer descriptor index
 *
 * This function returns the index of the next descriptor that will be executed
 * in the transfer descriptor list.
 *
 * @param chan_obj Channel object
 * @return int Descriptor index
 */
static inline int usbh_hal_chan_get_next_desc_index(usbh_hal_chan_t *chan_obj)
{
    return chan_obj->slot.flags.cur_qtd_idx;
}

/**
 * @brief Request to halt a channel
 *
 * This function should be called in order to halt a channel. If the channel is
 * already halted, this function will return true. If the channel is still
 * active, this function will return false and users must wait for the
 * USBH_HAL_CHAN_EVENT_HALT_REQ event before treating the channel as halted.
 *
 * @note When a transfer is in progress (i.e., the channel is active) and a halt
 *       is requested, the channel will halt after the next USB packet is completed.
 *       If the transfer has more pending packets, the transfer will just be
 *       marked as USBH_HAL_XFER_DESC_STS_NOT_EXECUTED.
 *
 * @param chan_obj Channel object
 * @return true The channel is already halted
 * @return false The halt was requested, wait for USBH_HAL_CHAN_EVENT_HALT_REQ
 */
bool usbh_hal_chan_slot_request_halt(usbh_hal_chan_t *chan_obj);

/* -----------------------------------------------------------------------------
-------------------------------- Event Handling --------------------------------
----------------------------------------------------------------------------- */

/**
 * @brief Decode global and host port interrupts
 *
 * - Reads and clears global and host port interrupt registers
 * - Decodes the interrupt bits to determine what host port event occurred
 *
 * @note This should be the first interrupt decode function to be run
 *
 * @param hal Context of the HAL layer
 * @return usbh_hal_port_event_t Host port event
 */
usbh_hal_port_event_t usbh_hal_decode_intr(usbh_hal_context_t *hal);

/**
 * @brief Gets the next channel with a pending interrupt
 *
 * If no channel is pending an interrupt, this function will return NULL. If one
 * or more channels are pending an interrupt, this function returns one of the
 * channel's objects. Call this function repeatedly until it returns NULL.
 *
 * @note If a channel error event occurs, or a Slot halt/done event occurs, the
 *       channel is immediately halted and no further channel interrupt or errors
 *       can occur until it is reactivated.
 *
 * @param hal Context of the HAL layer
 * @return usbh_hal_chan_t* Channel object. NULL if no channel are pending an interrupt.
 */
usbh_hal_chan_t *usbh_hal_get_chan_pending_intr(usbh_hal_context_t *hal);

/**
 * @brief Decode a particular channel's interrupt
 *
 * - Reads and clears the interrupt register of the channel
 * - Returns the corresponding event for that channel
 *
 * @param chan_obj Channel object
 * @return usbh_hal_chan_event_t Channel event
 */
usbh_hal_chan_event_t usbh_hal_chan_decode_intr(usbh_hal_chan_t *chan_obj);

#ifdef __cplusplus
}
#endif
