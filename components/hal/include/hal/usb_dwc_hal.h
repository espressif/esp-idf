/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
/*
This header is shared across all targets. Resolve to an empty header for targets
that don't support USB OTG.
*/
#if SOC_USB_OTG_SUPPORTED
#include <stdint.h>
#include <stdbool.h>
#include "hal/usb_dwc_ll.h"
#include "hal/usb_dwc_types.h"
#include "hal/assert.h"
#endif // SOC_USB_OTG_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_OTG_SUPPORTED

// ------------------------------------------------ Macros and Types ---------------------------------------------------

// ----------------------- Configs -------------------------

/**
 * @brief Possible FIFO biases
 */
typedef enum {
    USB_HAL_FIFO_BIAS_DEFAULT,           /**< Default (balanced) FIFO sizes */
    USB_HAL_FIFO_BIAS_RX,                /**< Bigger RX FIFO for IN transfers */
    USB_HAL_FIFO_BIAS_PTX,               /**< Bigger periodic TX FIFO for ISOC OUT transfers */
} usb_hal_fifo_bias_t;

/**
 * @brief MPS limits based on FIFO configuration
 *
 * In bytes
 *
 * The resulting values depend on
 * 1. FIFO total size (chip specific)
 * 2. Set FIFO bias
 */
typedef struct {
    unsigned int in_mps;                 /**< Maximum packet size of IN packet */
    unsigned int non_periodic_out_mps;   /**< Maximum packet size of BULK and CTRL OUT packets */
    unsigned int periodic_out_mps;       /**< Maximum packet size of INTR and ISOC OUT packets */
} usb_hal_fifo_mps_limits_t;

/**
 * @brief FIFO size configuration structure
 */
typedef struct {
    uint32_t rx_fifo_lines;                 /**< Size of the RX FIFO in terms the number of FIFO lines */
    uint32_t nptx_fifo_lines;               /**< Size of the Non-periodic FIFO in terms the number of FIFO lines */
    uint32_t ptx_fifo_lines;                /**< Size of the Periodic FIFO in terms the number of FIFO lines */
} usb_dwc_hal_fifo_config_t;

// --------------------- HAL Events ------------------------

/**
 * @brief Host port HAL events
 */
typedef enum {
    USB_DWC_HAL_PORT_EVENT_NONE,            /**< No event occurred, or could not decode interrupt */
    USB_DWC_HAL_PORT_EVENT_CHAN,            /**< A channel event has occurred. Call the the channel event handler instead */
    USB_DWC_HAL_PORT_EVENT_CONN,            /**< The host port has detected a connection */
    USB_DWC_HAL_PORT_EVENT_DISCONN,         /**< The host port has been disconnected */
    USB_DWC_HAL_PORT_EVENT_ENABLED,         /**< The host port has been enabled (i.e., connected to a device that has been reset. Started sending SOFs) */
    USB_DWC_HAL_PORT_EVENT_DISABLED,        /**< The host port has been disabled (no more SOFs). Could be due to disable/reset request, or a port error (e.g. port babble condition. See 11.8.1 of USB2.0 spec) */
    USB_DWC_HAL_PORT_EVENT_OVRCUR,          /**< The host port has encountered an overcurrent condition */
    USB_DWC_HAL_PORT_EVENT_OVRCUR_CLR,      /**< The host port has been cleared of the overcurrent condition */
} usb_dwc_hal_port_event_t;

/**
 * @brief Channel events
 */
typedef enum {
    USB_DWC_HAL_CHAN_EVENT_CPLT,            /**< The channel has completed execution of a transfer descriptor that had the USB_DWC_HAL_XFER_DESC_FLAG_HOC flag set. Channel is now halted */
    USB_DWC_HAL_CHAN_EVENT_ERROR,           /**< The channel has encountered an error. Channel is now halted. */
    USB_DWC_HAL_CHAN_EVENT_HALT_REQ,        /**< The channel has been successfully halted as requested */
    USB_DWC_HAL_CHAN_EVENT_NONE,            /**< No event (interrupt ran for internal processing) */
} usb_dwc_hal_chan_event_t;

// --------------------- HAL Errors ------------------------

/**
 * @brief Channel errors
 */
typedef enum {
    USB_DWC_HAL_CHAN_ERROR_XCS_XACT = 0,    /**< Excessive (three consecutive) transaction errors (e.g., no response, bad CRC etc */
    USB_DWC_HAL_CHAN_ERROR_BNA,             /**< Buffer Not Available error (i.e., An inactive transfer descriptor was fetched by the channel) */
    USB_DWC_HAL_CHAN_ERROR_PKT_BBL,         /**< Packet babbler error (packet exceeded MPS) */
    USB_DWC_HAL_CHAN_ERROR_STALL,           /**< STALL response received */
} usb_dwc_hal_chan_error_t;

// ------------- Transfer Descriptor Related ---------------

/**
 * @brief Flags used to describe the type of transfer descriptor to fill
 */
#define USB_DWC_HAL_XFER_DESC_FLAG_IN       0x01    /**< Indicates this transfer descriptor is of the IN direction */
#define USB_DWC_HAL_XFER_DESC_FLAG_SETUP    0x02    /**< Indicates this transfer descriptor is an OUT setup */
#define USB_DWC_HAL_XFER_DESC_FLAG_HOC      0x04    /**< Indicates that the channel will be halted after this transfer descriptor completes */

/**
 * @brief Status value of a transfer descriptor
 *
 * A transfer descriptor's status remains unexecuted until the entire transfer descriptor completes (either successfully
 * or an error). Therefore, if a channel halt is requested before a transfer descriptor completes, the transfer
 * descriptor remains unexecuted.
 */
#define USB_DWC_HAL_XFER_DESC_STS_SUCCESS       USB_DWC_LL_QTD_STATUS_SUCCESS
#define USB_DWC_HAL_XFER_DESC_STS_PKTERR        USB_DWC_LL_QTD_STATUS_PKTERR
#define USB_DWC_HAL_XFER_DESC_STS_BUFFER_ERR    USB_DWC_LL_QTD_STATUS_BUFFER
#define USB_DWC_HAL_XFER_DESC_STS_NOT_EXECUTED  USB_DWC_LL_QTD_STATUS_NOT_EXECUTED

// -------------------- Object Types -----------------------

/**
 * @brief Endpoint characteristics structure
 */
typedef struct {
    union {
        struct {
            usb_dwc_xfer_type_t type: 2;    /**< The type of endpoint */
            uint32_t bEndpointAddress: 8;   /**< Endpoint address (containing endpoint number and direction) */
            uint32_t mps: 11;               /**< Maximum Packet Size */
            uint32_t dev_addr: 8;           /**< Device Address */
            uint32_t ls_via_fs_hub: 1;      /**< The endpoint is on a LS device that is routed through an FS hub.
                                                 Setting this bit will lead to the addition of the PREamble packet */
            uint32_t reserved2: 2;
        };
        uint32_t val;
    };
    struct {
        unsigned int interval;              /**< The interval of the endpoint in frames (FS) or microframes (HS) */
        uint32_t phase_offset_frames;       /**< Phase offset in number of frames */
    } periodic;     /**< Characteristic for periodic (interrupt/isochronous) endpoints only */
} usb_dwc_hal_ep_char_t;

/**
 * @brief Channel object
 */
typedef struct {
    //Channel control, status, and information
    union {
        struct {
            uint32_t active: 1;             /**< Debugging bit to indicate whether channel is enabled */
            uint32_t halt_requested: 1;     /**< A halt has been requested */
            uint32_t reserved: 2;
            uint32_t chan_idx: 4;           /**< The index number of the channel */
            uint32_t reserved24: 24;
        };
        uint32_t val;
    } flags;                                /**< Flags regarding channel's status and information */
    usb_dwc_host_chan_regs_t *regs;         /**< Pointer to the channel's register set */
    usb_dwc_hal_chan_error_t error;         /**< The last error that occurred on the channel */
    usb_dwc_xfer_type_t type;               /**< The transfer type of the channel */
    void *chan_ctx;                         /**< Context variable for the owner of the channel */
} usb_dwc_hal_chan_t;

/**
 * @brief HAL context structure
 */
typedef struct {
    //Context
    usb_dwc_dev_t *dev;                            /**< Pointer to base address of DWC_OTG registers */
    //Host Port related
    uint32_t *periodic_frame_list;                 /**< Pointer to scheduling frame list */
    usb_hal_frame_list_len_t frame_list_len;       /**< Length of the periodic scheduling frame list */
    //FIFO related
    usb_dwc_hal_fifo_config_t fifo_config;         /**< FIFO sizes configuration */
    union {
        struct {
            uint32_t dbnc_lock_enabled: 1;      /**< Debounce lock enabled */
            uint32_t fifo_sizes_set: 1;         /**< Whether the FIFO sizes have been set or not */
            uint32_t periodic_sched_enabled: 1; /**< Periodic scheduling (for interrupt and isochronous transfers) is enabled */
            uint32_t reserved: 5;
            uint32_t reserved24: 24;
        };
        uint32_t val;
    } flags;
    //Channel related
    struct {
        int num_allocd;                             /**< Number of channels currently allocated */
        uint32_t chan_pend_intrs_msk;               /**< Bit mask of channels with pending interrupts */
        usb_dwc_hal_chan_t *hdls[OTG_NUM_HOST_CHAN];    /**< Handles of each channel. Set to NULL if channel has not been allocated */
    } channels;
} usb_dwc_hal_context_t;

// -------------------------------------------------- Core (Global) ----------------------------------------------------

/**
 * @brief Initialize the HAL context and check if DWC_OTG is alive
 *
 * Entry:
 * - The peripheral must have been reset and clock un-gated
 * - The USB PHY (internal or external) and associated GPIOs must already be configured
 * - GPIO pins configured
 * - Interrupt allocated but DISABLED (in case of an unknown interupt state)
 * Exit:
 * - Checks to see if DWC_OTG is alive, and if HW version/config is correct
 * - HAl context initialized
 * - Sets default values to some global and OTG registers (GAHBCFG and GUSBCFG)
 * - Umask global interrupt signal
 * - Put DWC_OTG into host mode. Require 25ms delay before this takes effect.
 * - State -> USB_DWC_HAL_PORT_STATE_OTG
 * - Interrupts cleared. Users can now enable their ISR
 *
 * @param[inout] hal Context of the HAL layer
 */
void usb_dwc_hal_init(usb_dwc_hal_context_t *hal);

/**
 * @brief Deinitialize the HAL context
 *
 * Entry:
 * - All channels must be properly disabled, and any pending events handled
 * Exit:
 * - DWC_OTG global interrupt disabled
 * - HAL context deinitialized
 *
 * @param hal Context of the HAL layer
 */
void usb_dwc_hal_deinit(usb_dwc_hal_context_t *hal);

/**
 * @brief Issue a soft reset to the controller
 *
 * This should be called when the host port encounters an error event or has been disconnected. Before calling this,
 * users are responsible for safely freeing all channels as a soft reset will wipe all host port and channel registers.
 * This function will result in the host port being put back into same state as after calling usb_dwc_hal_init().
 *
 * @note This has nothing to do with a USB bus reset. It simply resets the peripheral
 *
 * @param hal Context of the HAL layer
 */
void usb_dwc_hal_core_soft_reset(usb_dwc_hal_context_t *hal);

/**
 * @brief Set FIFO bias
 *
 * This function will set the sizes of each of the FIFOs (RX FIFO, Non-periodic TX FIFO, Periodic TX FIFO) and must be
 * called at least once before allocating the channel. Based on the type of endpoints (and the endpoints' MPS), there
 * may be situations where this function may need to be called again to resize the FIFOs. If resizing FIFOs dynamically,
 * it is the user's responsibility to ensure there are no active channels when this function is called.
 *
 * @note After a port reset, the FIFO size registers will reset to their default values, so this function must be called
 *       again post reset.
 *
 * @param[in] hal       Context of the HAL layer
 * @param[in] fifo_bias FIFO bias configuration
 */
void usb_dwc_hal_set_fifo_bias(usb_dwc_hal_context_t *hal, const usb_hal_fifo_bias_t fifo_bias);

/**
 * @brief Get MPS limits
 *
 * @param[in]  hal        Context of the HAL layer
 * @param[out] mps_limits MPS limits
 */
void usb_dwc_hal_get_mps_limits(usb_dwc_hal_context_t *hal, usb_hal_fifo_mps_limits_t *mps_limits);

// ---------------------------------------------------- Host Port ------------------------------------------------------

// ------------------ Host Port Control --------------------

/**
 * @brief Initialize the host port
 *
 * - Will enable the host port's interrupts allowing port and channel events to occur
 *
 * @param hal Context of the HAL layer
 */
static inline void usb_dwc_hal_port_init(usb_dwc_hal_context_t *hal)
{
    //Configure Host related interrupts
    usb_dwc_ll_haintmsk_dis_chan_intr(hal->dev, 0xFFFFFFFF);   //Disable interrupts for all channels
    usb_dwc_ll_gintmsk_en_intrs(hal->dev, USB_DWC_LL_INTR_CORE_PRTINT | USB_DWC_LL_INTR_CORE_HCHINT);
}

/**
 * @brief Deinitialize the host port
 *
 * - Will disable the host port's interrupts preventing further port aand channel events from ocurring
 *
 * @param hal Context of the HAL layer
 */
static inline void usb_dwc_hal_port_deinit(usb_dwc_hal_context_t *hal)
{
    //Disable Host port and channel interrupts
    usb_dwc_ll_gintmsk_dis_intrs(hal->dev, USB_DWC_LL_INTR_CORE_PRTINT | USB_DWC_LL_INTR_CORE_HCHINT);
}

/**
 * @brief Toggle the host port's power
 *
 * @param hal Context of the HAL layer
 * @param power_on Whether to power ON or OFF the port
 */
static inline void usb_dwc_hal_port_toggle_power(usb_dwc_hal_context_t *hal, bool power_on)
{
    if (power_on) {
        usb_dwc_ll_hprt_en_pwr(hal->dev);
    } else {
        usb_dwc_ll_hprt_dis_pwr(hal->dev);
    }
}

/**
 * @brief Toggle reset signal on the bus
 *
 * The reset signal should be held for at least 10ms
 * Entry:
 * - Host port detects a device connection or Host port is already enabled
 * Exit:
 * - On release of the reset signal, a USB_DWC_HAL_PORT_EVENT_ENABLED will be generated
 *
 * @note If the host port is already enabled, then issuing a reset will cause it be disabled and generate a
 *       USB_DWC_HAL_PORT_EVENT_DISABLED event. The host port will not be enabled until the reset signal is released (thus
 *       generating the USB_DWC_HAL_PORT_EVENT_ENABLED event)
 *
 * @param hal Context of the HAL layer
 * @param enable Enable/disable reset signal
 */
static inline void usb_dwc_hal_port_toggle_reset(usb_dwc_hal_context_t *hal, bool enable)
{
    HAL_ASSERT(hal->channels.num_allocd == 0);  //Cannot reset if there are still allocated channels
    usb_dwc_ll_hprt_set_port_reset(hal->dev, enable);
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
void usb_dwc_hal_port_enable(usb_dwc_hal_context_t *hal);

/**
 * @brief Disable the host port
 *
 * Exit:
 * - Host port disabled event triggered
 *
 * @param hal Context of the HAL layer
 */
static inline void usb_dwc_hal_port_disable(usb_dwc_hal_context_t *hal)
{
    usb_dwc_ll_hprt_port_dis(hal->dev);
}

/**
 * @brief Suspend the host port
 *
 * @param hal Context of the HAL layers
 */
static inline void usb_dwc_hal_port_suspend(usb_dwc_hal_context_t *hal)
{
    usb_dwc_ll_hprt_set_port_suspend(hal->dev);
}

/**
 * @brief Toggle resume signal on the bus
 *
 * Hosts should hold the resume signal for at least 20ms
 *
 * @note If a remote wakeup event occurs, the resume signal is driven and cleared automatically.
 *
 * @param hal Context of the HAL layer
 * @param enable Enable/disable resume signal
 */
static inline void usb_dwc_hal_port_toggle_resume(usb_dwc_hal_context_t *hal, bool enable)
{
    if (enable) {
        usb_dwc_ll_hprt_set_port_resume(hal->dev);
    } else {
        usb_dwc_ll_hprt_clr_port_resume(hal->dev);
    }
}

/**
 * @brief Check whether the resume signal is being driven
 *
 * If a remote wakeup event occurs, the core will automatically drive and clear the resume signal for the required
 * amount of time. Call this function to check whether the resume signal has completed.
 *
 * @param hal Context of the HAL layer
 * @return true Resume signal is still being driven
 * @return false Resume signal is no longer driven
 */
static inline bool usb_dwc_hal_port_check_resume(usb_dwc_hal_context_t *hal)
{
    return usb_dwc_ll_hprt_get_port_resume(hal->dev);
}

// ---------------- Host Port Scheduling -------------------

/**
 * @brief Sets the periodic scheduling frame list
 *
 * @note This function must be called before attempting configuring any channels to be period via
 *       usb_dwc_hal_chan_set_ep_char()
 *
 * @param hal Context of the HAL layer
 * @param frame_list Base address of the frame list
 * @param frame_list_len Number of entries in the frame list (can only be 8, 16, 32, 64)
 */
static inline void usb_dwc_hal_port_set_frame_list(usb_dwc_hal_context_t *hal, uint32_t *frame_list, usb_hal_frame_list_len_t len)
{
    //Clear and save frame list
    hal->periodic_frame_list = frame_list;
    hal->frame_list_len = len;
}

/**
 * @brief Get the pointer to the periodic scheduling frame list
 *
 * @param hal Context of the HAL layer
 * @return uint32_t* Base address of the periodic scheduling frame list
 */
static inline uint32_t *usb_dwc_hal_port_get_frame_list(usb_dwc_hal_context_t *hal)
{
    return hal->periodic_frame_list;
}

/**
 * @brief Enable periodic scheduling
 *
 * @note The periodic frame list must be set via usb_dwc_hal_port_set_frame_list() should be set before calling this
 *       function
 * @note This function must be called before activating any periodic channels
 *
 * @param hal Context of the HAL layer
 */
static inline void usb_dwc_hal_port_periodic_enable(usb_dwc_hal_context_t *hal)
{
    HAL_ASSERT(hal->periodic_frame_list != NULL);
    usb_dwc_ll_hflbaddr_set_base_addr(hal->dev, (uint32_t)hal->periodic_frame_list);
    usb_dwc_ll_hcfg_set_num_frame_list_entries(hal->dev, hal->frame_list_len);
    usb_dwc_ll_hcfg_en_perio_sched(hal->dev);
    hal->flags.periodic_sched_enabled = 1;
}

/**
 * @brief Disable periodic scheduling
 *
 * Disabling periodic scheduling will save a bit of DMA bandwith (as the controller will no longer fetch the schedule
 * from the frame list).
 *
 * @note Before disabling periodic scheduling, it is the user's responsibility to ensure that all periodic channels have
 *       halted safely.
 *
 * @param hal Context of the HAL layer
 */
static inline void usb_dwc_hal_port_periodic_disable(usb_dwc_hal_context_t *hal)
{
    HAL_ASSERT(hal->flags.periodic_sched_enabled);
    usb_dwc_ll_hcfg_dis_perio_sched(hal->dev);
    hal->flags.periodic_sched_enabled = 0;
}

static inline uint32_t usb_dwc_hal_port_get_cur_frame_num(usb_dwc_hal_context_t *hal)
{
    return usb_dwc_ll_hfnum_get_frame_num(hal->dev);
}

// --------------- Host Port Status/State ------------------

/**
 * @brief Check if a device is currently connected to the host port
 *
 * This function is intended to be called after one of the following events followed by an adequate debounce delay
 * - USB_DWC_HAL_PORT_EVENT_CONN
 * - USB_DWC_HAL_PORT_EVENT_DISCONN
 *
 * @note No other connection/disconnection event will occur again until the debounce lock is disabled via
 *       usb_dwc_hal_disable_debounce_lock()
 *
 * @param hal Context of the HAL layer
 * @return true A device is connected to the host port
 * @return false A device is not connected to the host port
 */
static inline bool usb_dwc_hal_port_check_if_connected(usb_dwc_hal_context_t *hal)
{
    return usb_dwc_ll_hprt_get_conn_status(hal->dev);
}

/**
 * @brief Check the speed of the device connected to the host port
 *
 * @note This function should only be called after confirming that a device is connected to the host port
 *
 * @param hal Context of the HAL layer
 * @return usb_dwc_speed_t Speed of the connected device
 */
static inline usb_dwc_speed_t usb_dwc_hal_port_get_conn_speed(usb_dwc_hal_context_t *hal)
{
    return usb_dwc_ll_hprt_get_speed(hal->dev);
}

/**
 * @brief Disable the debounce lock
 *
 * This function must be called after calling usb_dwc_hal_port_check_if_connected() and will allow connection/disconnection
 * events to occur again. Any pending connection or disconenction interrupts are cleared.
 *
 * @param hal Context of the HAL layer
 */
static inline void usb_dwc_hal_disable_debounce_lock(usb_dwc_hal_context_t *hal)
{
    hal->flags.dbnc_lock_enabled = 0;
    //Clear Conenction and disconenction interrupt in case it triggered again
    usb_dwc_ll_gintsts_clear_intrs(hal->dev, USB_DWC_LL_INTR_CORE_DISCONNINT);
    usb_dwc_ll_hprt_intr_clear(hal->dev, USB_DWC_LL_INTR_HPRT_PRTCONNDET);
    //Reenable the hprt (connection) and disconnection interrupts
    usb_dwc_ll_gintmsk_en_intrs(hal->dev, USB_DWC_LL_INTR_CORE_PRTINT | USB_DWC_LL_INTR_CORE_DISCONNINT);
}

// ----------------------------------------------------- Channel -------------------------------------------------------

// ----------------- Channel Allocation --------------------

/**
 * @brief Allocate a channel
 *
 * @param[in] hal Context of the HAL layer
 * @param[inout] chan_obj Empty channel object
 * @param[in] chan_ctx Context variable for the allocator of the channel
 * @return true Channel successfully allocated
 * @return false Failed to allocate channel
 */
bool usb_dwc_hal_chan_alloc(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj, void *chan_ctx);

/**
 * @brief Free a channel
 *
 * @param[in] hal Context of the HAL layer
 * @param[in] chan_obj Channel object
 */
void usb_dwc_hal_chan_free(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj);

// ---------------- Channel Configuration ------------------

/**
 * @brief Get the context variable of the channel
 *
 * @param[in] chan_obj Channel object
 * @return void* The context variable of the channel
 */
static inline void *usb_dwc_hal_chan_get_context(usb_dwc_hal_chan_t *chan_obj)
{
    return chan_obj->chan_ctx;
}

/**
 * @brief Set the endpoint information for a particular channel
 *
 * This should be called when a channel switches target from one EP to another
 *
 * @note the channel must be in the disabled state in order to change its EP
 *       information
 *
 * @param hal Context of the HAL layer
 * @param chan_obj Channel object
 * @param ep_char Endpoint characteristics
 */
void usb_dwc_hal_chan_set_ep_char(usb_dwc_hal_context_t *hal, usb_dwc_hal_chan_t *chan_obj, usb_dwc_hal_ep_char_t *ep_char);

/**
 * @brief Set the direction of the channel
 *
 * This is a convenience function to flip the direction of a channel without
 * needing to reconfigure all of the channel's EP info. This is used primarily
 * for control transfers.
 *
 * @note This function should only be called when the channel is halted
 *
 * @param chan_obj Channel object
 * @param is_in Whether the direction is IN
 */
static inline void usb_dwc_hal_chan_set_dir(usb_dwc_hal_chan_t *chan_obj, bool is_in)
{
    //Cannot change direction whilst channel is still active or in error
    HAL_ASSERT(!chan_obj->flags.active);
    usb_dwc_ll_hcchar_set_dir(chan_obj->regs, is_in);
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
static inline void usb_dwc_hal_chan_set_pid(usb_dwc_hal_chan_t *chan_obj, int pid)
{
    //Cannot change pid whilst channel is still active or in error
    HAL_ASSERT(!chan_obj->flags.active);
    //Update channel object and set the register
    usb_dwc_ll_hctsiz_set_pid(chan_obj->regs, pid);
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
static inline uint32_t usb_dwc_hal_chan_get_pid(usb_dwc_hal_chan_t *chan_obj)
{
    HAL_ASSERT(!chan_obj->flags.active);
    return usb_dwc_ll_hctsiz_get_pid(chan_obj->regs);
}

// ------------------- Channel Control ---------------------

/**
 * @brief Activate a channel
 *
 * Activating a channel will cause the channel to start executing transfer descriptors.
 *
 * @note This function should only be called on channels that were previously halted
 * @note An event will be generated when the channel is halted
 *
 * @param chan_obj Channel object
 * @param xfer_desc_list A filled transfer descriptor list
 * @param desc_list_len Transfer descriptor list length
 * @param start_idx Index of the starting transfer descriptor in the list
 */
void usb_dwc_hal_chan_activate(usb_dwc_hal_chan_t *chan_obj, void *xfer_desc_list, int desc_list_len, int start_idx);

/**
 * @brief Get the index of the current transfer descriptor
 *
 * @param chan_obj Channel object
 * @return int Descriptor index
 */
static inline int usb_dwc_hal_chan_get_qtd_idx(usb_dwc_hal_chan_t *chan_obj)
{
    return usb_dwc_ll_hcdam_get_cur_qtd_idx(chan_obj->regs);
}

/**
 * @brief Request to halt a channel
 *
 * This function should be called in order to halt a channel. If the channel is already halted, this function will
 * return true. If the channel is still active, this function will return false and users must wait for the
 * USB_DWC_HAL_CHAN_EVENT_HALT_REQ event before treating the channel as halted.
 *
 * @note When a transfer is in progress (i.e., the channel is active) and a halt is requested, the channel will halt
 *       after the next USB packet is completed. If the transfer has more pending packets, the transfer will just be
 *       marked as USB_DWC_HAL_XFER_DESC_STS_NOT_EXECUTED.
 *
 * @param chan_obj Channel object
 * @return true The channel is already halted
 * @return false The halt was requested, wait for USB_DWC_HAL_CHAN_EVENT_HALT_REQ
 */
bool usb_dwc_hal_chan_request_halt(usb_dwc_hal_chan_t *chan_obj);

/**
 * @brief Indicate that a channel is halted after a port error
 *
 * When a port error occurs (e.g., discconect, overcurrent):
 * - Any previously active channels will remain active (i.e., they will not receive a channel interrupt)
 * - Attempting to disable them using usb_dwc_hal_chan_request_halt() will NOT generate an interrupt for ISOC channels
 *   (probalby something to do with the periodic scheduling)
 *
 * However, the channel's enable bit can be left as 1 since after a port error, a soft reset will be done anyways.
 * This function simply updates the channels internal state variable to indicate it is halted (thus allowing it to be
 * freed).
 *
 * @param chan_obj Channel object
 */
static inline void usb_dwc_hal_chan_mark_halted(usb_dwc_hal_chan_t *chan_obj)
{
    chan_obj->flags.active = 0;
}

/**
 * @brief Get a channel's error
 *
 * @param chan_obj Channel object
 * @return usb_dwc_hal_chan_error_t The type of error the channel has encountered
 */
static inline usb_dwc_hal_chan_error_t usb_dwc_hal_chan_get_error(usb_dwc_hal_chan_t *chan_obj)
{
    return chan_obj->error;
}

// -------------------------------------------- Transfer Descriptor List -----------------------------------------------

/**
 * @brief Fill a single entry in a transfer descriptor list
 *
 * - Depending on the transfer type, a single transfer descriptor may corresponds
 *      - A stage of a transfer (for control transfers)
 *      - A frame of a transfer interval (for interrupt and isoc)
 *      - An entire transfer (for bulk transfers)
 * - Check the various USB_DWC_HAL_XFER_DESC_FLAG_ flags for filling a specific type of descriptor
 * - For IN transfer entries, set the USB_DWC_HAL_XFER_DESC_FLAG_IN. The transfer size must also be an integer multiple of
 *   the endpoint's MPS
 *
 * @note Critical section is not required for this function
 *
 * @param desc_list Transfer descriptor list
 * @param desc_idx Transfer descriptor index
 * @param xfer_data_buff Transfer data buffer
 * @param xfer_len Transfer length
 * @param flags Transfer flags
 */
static inline void usb_dwc_hal_xfer_desc_fill(void *desc_list, uint32_t desc_idx, uint8_t *xfer_data_buff, int xfer_len, uint32_t flags)
{
    usb_dwc_ll_dma_qtd_t *qtd_list = (usb_dwc_ll_dma_qtd_t *)desc_list;
    if (flags & USB_DWC_HAL_XFER_DESC_FLAG_IN) {
        usb_dwc_ll_qtd_set_in(&qtd_list[desc_idx],
                           xfer_data_buff, xfer_len,
                           flags & USB_DWC_HAL_XFER_DESC_FLAG_HOC);
    } else {
        usb_dwc_ll_qtd_set_out(&qtd_list[desc_idx],
                            xfer_data_buff,
                            xfer_len,
                            flags & USB_DWC_HAL_XFER_DESC_FLAG_HOC,
                            flags & USB_DWC_HAL_XFER_DESC_FLAG_SETUP);
    }
}

/**
 * @brief Clear a transfer descriptor (sets all its fields to NULL)
 *
 * @param desc_list Transfer descriptor list
 * @param desc_idx Transfer descriptor index
 */
static inline void usb_dwc_hal_xfer_desc_clear(void *desc_list, uint32_t desc_idx)
{
    usb_dwc_ll_dma_qtd_t *qtd_list = (usb_dwc_ll_dma_qtd_t *)desc_list;
    usb_dwc_ll_qtd_set_null(&qtd_list[desc_idx]);
}

/**
 * @brief Parse a transfer decriptor's results
 *
 * @param desc_list Transfer descriptor list
 * @param desc_idx Transfer descriptor index
 * @param[out] xfer_rem_len Remaining length of the transfer in bytes
 * @param[out] xfer_status Status of the transfer
 *
 * @note Critical section is not required for this function
 */
static inline void usb_dwc_hal_xfer_desc_parse(void *desc_list, uint32_t desc_idx, int *xfer_rem_len, int *xfer_status)
{
    usb_dwc_ll_dma_qtd_t *qtd_list = (usb_dwc_ll_dma_qtd_t *)desc_list;
    usb_dwc_ll_qtd_get_status(&qtd_list[desc_idx], xfer_rem_len, xfer_status);
    //Clear the QTD to prevent it from being read again
    usb_dwc_ll_qtd_set_null(&qtd_list[desc_idx]);
}

// ------------------------------------------------- Event Handling ----------------------------------------------------

/**
 * @brief Decode global and host port interrupts
 *
 * - Reads and clears global and host port interrupt registers
 * - Decodes the interrupt bits to determine what host port event occurred
 *
 * @note This should be the first interrupt decode function to be run
 *
 * @param hal Context of the HAL layer
 * @return usb_dwc_hal_port_event_t Host port event
 */
usb_dwc_hal_port_event_t usb_dwc_hal_decode_intr(usb_dwc_hal_context_t *hal);

/**
 * @brief Gets the next channel with a pending interrupt
 *
 * If no channel is pending an interrupt, this function will return NULL. If one or more channels are pending an
 * interrupt, this function returns one of the channel's objects. Call this function repeatedly until it returns NULL.
 *
 * @param hal Context of the HAL layer
 * @return usb_dwc_hal_chan_t* Channel object. NULL if no channel are pending an interrupt.
 */
usb_dwc_hal_chan_t *usb_dwc_hal_get_chan_pending_intr(usb_dwc_hal_context_t *hal);

/**
 * @brief Decode a particular channel's interrupt
 *
 * - Reads and clears the interrupt register of the channel
 * - Returns the corresponding event for that channel
 *
 * @param chan_obj Channel object
 * @note If the host port has an error (e.g., a sudden disconnect or an port error), any active channels will not
 *       receive an interrupt. Each active channel must be manually halted.
 * @return usb_dwc_hal_chan_event_t Channel event
 */
usb_dwc_hal_chan_event_t usb_dwc_hal_chan_decode_intr(usb_dwc_hal_chan_t *chan_obj);

#endif // SOC_USB_OTG_SUPPORTED

#ifdef __cplusplus
}
#endif
