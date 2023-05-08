/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hcd.h"
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------ Types --------------------------------------------------------

// ----------------------- Handles -------------------------

/**
 * @brief Handle of a allocated endpoint
 */
typedef struct usbh_ep_handle_s *usbh_ep_handle_t;

// ----------------------- Events --------------------------

typedef enum {
    USBH_EVENT_DEV_NEW,             /**< A new device has been enumerated and added to the device pool */
    USBH_EVENT_DEV_GONE,            /**< A device is gone. Clients should close the device */
    USBH_EVENT_DEV_ALL_FREE,        /**< All devices have been freed */
} usbh_event_t;

/**
 * @brief Endpoint events
 *
 * @note Optimization: Keep this identical to hcd_pipe_event_t
 */
typedef enum {
    USBH_EP_EVENT_NONE,                     /**< The EP has no events (used to indicate no events when polling) */
    USBH_EP_EVENT_URB_DONE,                 /**< The EP has completed a URB. The URB can be dequeued */
    USBH_EP_EVENT_ERROR_XFER,               /**< The EP encountered excessive errors when transferring a URB i.e., three three consecutive transaction errors (e.g., no ACK, bad CRC etc) */
    USBH_EP_EVENT_ERROR_URB_NOT_AVAIL,      /**< The EP tried to execute a transfer but no URB was available */
    USBH_EP_EVENT_ERROR_OVERFLOW,           /**< The EP received more data than requested. Usually a Packet babble error (i.e., an IN packet has exceeded the EP's MPS) */
    USBH_EP_EVENT_ERROR_STALL,              /**< EP received a STALL response */
} usbh_ep_event_t;

/**
 * @brief Hub driver events for the USBH
 *
 * These events as passed by the Hub driver to the USBH via usbh_hub_pass_event()
 *
 * USBH_HUB_EVENT_PORT_ERROR:
 * - The port has encountered an error (such as a sudden disconnection). The device connected to that port is no longer valid.
 * - The USBH should:
 *      - Trigger a USBH_EVENT_DEV_GONE
 *      - Prevent further transfers to the device
 *      - Trigger the device's cleanup if it is already closed
 *      - When the last client closes the device via usbh_dev_close(), free the device object and issue a USBH_HUB_REQ_PORT_RECOVER request
 *
 * USBH_HUB_EVENT_PORT_DISABLED:
 * - A previous USBH_HUB_REQ_PORT_DISABLE has completed.
 * - The USBH should free the device object
 */
typedef enum {
    USBH_HUB_EVENT_PORT_ERROR,      /**< The port has encountered an error (such as a sudden disconnection). The device
                                         connected to that port should be marked gone. */
    USBH_HUB_EVENT_PORT_DISABLED,   /**< Previous USBH_HUB_REQ_PORT_DISABLE request completed */
} usbh_hub_event_t;

// ------------------ Requests/Commands --------------------

/**
 * @brief Hub driver requests
 *
 * Various requests of the Hub driver that the USBH can make.
 */
typedef enum {
    USBH_HUB_REQ_PORT_DISABLE,      /**< Request that the Hub driver disable a particular port (occurs after a device
                                         has been freed). Hub driver should respond with a USBH_HUB_EVENT_PORT_DISABLED */
    USBH_HUB_REQ_PORT_RECOVER,      /**< Request that the Hub driver recovers a particular port (occurs after a gone
                                         device has been freed). */
} usbh_hub_req_t;

/**
 * @brief Endpoint commands
 *
 * @note Optimization: Keep this identical to hcd_pipe_cmd_t
 */
typedef enum {
    USBH_EP_CMD_HALT,           /**< Halt an active endpoint. Any currently executing URB will be canceled. Enqueued URBs are left untouched */
    USBH_EP_CMD_FLUSH,          /**< Can only be called when halted. Will cause all enqueued URBs to be canceled */
    USBH_EP_CMD_CLEAR,          /**< Causes a halted endpoint to become active again. Any enqueued URBs will being executing.*/
} usbh_ep_cmd_t;

// ---------------------- Callbacks ------------------------

/**
 * @brief Callback used to indicate completion of control transfers submitted usbh_dev_submit_ctrl_urb()
 * @note This callback is called from within usbh_process()
 */
typedef void (*usbh_ctrl_xfer_cb_t)(usb_device_handle_t dev_hdl, urb_t *urb, void *arg);

/**
 * @brief Callback used to indicate that the USBH has an event
 *
 * @note This callback is called from within usbh_process()
 * @note On a USBH_EVENT_DEV_ALL_FREE event, the dev_hdl argument is set to NULL
 */
typedef void (*usbh_event_cb_t)(usb_device_handle_t dev_hdl, usbh_event_t usbh_event, void *arg);

/**
 * @brief Callback used by the USBH to request actions from the Hub driver
 *
 * The Hub Request Callback allows the USBH to request the Hub actions on a particular port. Conversely, the Hub driver
 * will indicate completion of some of these requests to the USBH via the usbh_hub_event() funtion.
 */
typedef void (*usbh_hub_req_cb_t)(hcd_port_handle_t port_hdl, usbh_hub_req_t hub_req, void *arg);

/**
 * @brief Callback used to indicate an event on an endpoint
 *
 * Return whether to yield or not if called from an ISR. Always return false if not called from an ISR
 */
typedef bool (*usbh_ep_cb_t)(usbh_ep_handle_t ep_hdl, usbh_ep_event_t ep_event, void *arg, bool in_isr);

// ----------------------- Objects -------------------------

/**
 * @brief Configuration for an endpoint being allocated using usbh_ep_alloc()
 */
typedef struct {
    uint8_t bInterfaceNumber;       /**< Interface number */
    uint8_t bAlternateSetting;      /**< Alternate setting number of the interface */
    uint8_t bEndpointAddress;       /**< Endpoint address */
    usbh_ep_cb_t ep_cb;             /**< Endpoint event callback */
    void *ep_cb_arg;                /**< Endpoint callback argument */
    void *context;                  /**< Endpoint context */
} usbh_ep_config_t;

/**
 * @brief USBH configuration used in usbh_install()
 */
typedef struct {
    usb_proc_req_cb_t proc_req_cb;          /**< Processing request callback */
    void *proc_req_cb_arg;                  /**< Processing request callback argument */
    usbh_ctrl_xfer_cb_t ctrl_xfer_cb;       /**< Control transfer callback */
    void *ctrl_xfer_cb_arg;                 /**< Control transfer callback argument */
    usbh_event_cb_t event_cb;               /**< USBH event callback */
    void *event_cb_arg;                     /**< USBH event callback argument */
} usbh_config_t;

// ------------------------------------------------- USBH Functions ----------------------------------------------------

/**
 * @brief Installs the USBH driver
 *
 * - This function will internally install the HCD
 * - This must be called before calling any Hub driver functions
 *
 * @note Before calling this function, the Host Controller must already be un-clock gated and reset. The USB PHY
 *       (internal or external, and associated GPIOs) must already be configured.
 * @param usbh_config USBH driver configuration
 * @return esp_err_t
 */
esp_err_t usbh_install(const usbh_config_t *usbh_config);

/**
 * @brief Uninstall the USBH driver
 *
 * - This function will uninstall the HCD
 * - The Hub driver must be uninstalled before calling this function
 *
 * @note This function will simply free the resources used by the USBH. The underlying Host Controller and USB PHY will
 *       not be disabled.
 * @return esp_err_t
 */
esp_err_t usbh_uninstall(void);

/**
 * @brief USBH processing function
 *
 * - USBH processing function that must be called repeatedly to process USBH events
 * - If blocking, the caller can block until the proc_req_cb() is called with USB_PROC_REQ_SOURCE_USBH as the request
 *   source. The USB_PROC_REQ_SOURCE_USBH source indicates that this function should be called.
 *
 * @note This function can block
 * @return esp_err_t
 */
esp_err_t usbh_process(void);

/**
 * @brief Get the current number of devices
 *
 * @note This function can block
 * @param[out] num_devs_ret Current number of devices
 * @return esp_err_t
 */
esp_err_t usbh_num_devs(int *num_devs_ret);

// ------------------------------------------------ Device Functions ---------------------------------------------------

// --------------------- Device Pool -----------------------

/**
 * @brief Fill list with address of currently connected devices
 *
 * - This function fills the provided list with the address of current connected devices
 * - Device address can then be used in usbh_dev_open()
 * - If there are more devices than the list_len, this function will only fill
 *   up to list_len number of devices.
 *
 * @param[in] list_len Length of empty list
 * @param[inout] dev_addr_list Empty list to be filled
 * @param[out] num_dev_ret Number of devices filled into list
 * @return esp_err_t
 */
esp_err_t usbh_dev_addr_list_fill(int list_len, uint8_t *dev_addr_list, int *num_dev_ret);

/**
 * @brief Open a device by address
 *
 * A device must be opened before it can be used
 *
 * @param[in] dev_addr Device address
 * @param[out] dev_hdl Device handle
 * @return esp_err_t
 */
esp_err_t usbh_dev_open(uint8_t dev_addr, usb_device_handle_t *dev_hdl);

/**
 * @brief CLose a device
 *
 * Device can be opened by calling usbh_dev_open()
 *
 * @param[in] dev_hdl Device handle
 * @return esp_err_t
 */
esp_err_t usbh_dev_close(usb_device_handle_t dev_hdl);

/**
 * @brief Mark that all devices should be freed at the next possible opportunity
 *
 * A device marked as free will not be freed until the last client using the device has called usbh_dev_close()
 *
 * @return
 *  - ESP_OK: There were no devices to free to begin with. Current state is all free
 *  - ESP_ERR_NOT_FINISHED: One or more devices still need to be freed (but have been marked "to be freed")
 */
esp_err_t usbh_dev_mark_all_free(void);

// ------------------- Single Device  ----------------------

/**
 * @brief Get a device's address
 *
 * @note Can be called without opening the device
 *
 * @param[in] dev_hdl Device handle
 * @param[out] dev_addr Device's address
 * @return esp_err_t
 */
esp_err_t usbh_dev_get_addr(usb_device_handle_t dev_hdl, uint8_t *dev_addr);

/**
 * @brief Get a device's information
 *
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @param[out] dev_info Device information
 * @return esp_err_t
 */
esp_err_t usbh_dev_get_info(usb_device_handle_t dev_hdl, usb_device_info_t *dev_info);

/**
 * @brief Get a device's device descriptor
 *
 * - The device descriptor is cached when the device is created by the Hub driver
 *
 * @param[in] dev_hdl Device handle
 * @param[out] dev_desc_ret Device descriptor
 * @return esp_err_t
 */
esp_err_t usbh_dev_get_desc(usb_device_handle_t dev_hdl, const usb_device_desc_t **dev_desc_ret);

/**
 * @brief Get a device's active configuration descriptor
 *
 * Simply returns a reference to the internally cached configuration descriptor
 *
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @param config_desc_ret
 * @return esp_err_t
 */
esp_err_t usbh_dev_get_config_desc(usb_device_handle_t dev_hdl, const usb_config_desc_t **config_desc_ret);

/**
 * @brief Submit a control transfer (URB) to a device
 *
 * @param[in] dev_hdl Device handle
 * @param[in] urb URB
 * @return esp_err_t
 */
esp_err_t usbh_dev_submit_ctrl_urb(usb_device_handle_t dev_hdl, urb_t *urb);

// ----------------------------------------------- Endpoint Functions -------------------------------------------------

/**
 * @brief Allocate an endpoint on a device
 *
 * This function allows clients to allocate a non-default endpoint (i.e., not EP0) on a connected device
 *
 * - A client must have opened the device using usbh_dev_open() before attempting to allocate an endpoint on the device
 * - A client should call this function to allocate all endpoints in an interface that the client has claimed.
 * - A client must allocate an endpoint using this function before attempting to communicate with it
 * - Once the client allocates an endpoint, the client is now owns/manages the endpoint. No other client should use or
 * deallocate the endpoint.
 *
 * @note This function can block
 * @note Default endpoints (EP0) are owned by the USBH. For control transfers, use usbh_dev_submit_ctrl_urb() instead
 *
 * @param[in] dev_hdl Device handle
 * @param[in] ep_config Endpoint configuration
 * @param[out] ep_hdl_ret Endpoint handle
 * @return esp_err_t
 */
esp_err_t usbh_ep_alloc(usb_device_handle_t dev_hdl, usbh_ep_config_t *ep_config, usbh_ep_handle_t *ep_hdl_ret);

/**
 * @brief Free and endpoint on a device
 *
 * This function frees an endpoint previously allocated by the client using usbh_ep_alloc()
 *
 * - Only the client that allocated the endpoint should free it
 * - The client must have halted and flushed the endpoint using usbh_ep_command() before attempting to free it
 * - The client must ensure that there are no more function calls to the endpoint before freeing it
 *
 * @note This function can block
 * @param[in] ep_hdl Endpoint handle
 * @return esp_err_t
 */
esp_err_t usbh_ep_free(usbh_ep_handle_t ep_hdl);

/**
 * @brief Get the handle of an endpoint using its address
 *
 * The endpoint must have been previously allocated using usbh_ep_alloc()
 *
 * @param[in] dev_hdl Device handle
 * @param[in] bEndpointAddress Endpoint address
 * @param[out] ep_hdl_ret Endpoint handle
 * @return esp_err_t
 */
esp_err_t usbh_ep_get_handle(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress, usbh_ep_handle_t *ep_hdl_ret);

/**
 * @brief Enqueue a URB to an endpoint
 *
 * The URB will remain enqueued until it completes (successfully or errors out). Use usbh_ep_dequeue_urb() to dequeue
 * a completed URB.
 *
 * @param[in] ep_hdl Endpoint handle
 * @param[in] urb URB to enqueue
 * @return esp_err_t
 */
esp_err_t usbh_ep_enqueue_urb(usbh_ep_handle_t ep_hdl, urb_t *urb);

/**
 * @brief Dequeue a URB from an endpoint
 *
 * Dequeue a completed URB from an endpoint. The USBH_EP_EVENT_URB_DONE indicates that URBs can be dequeued
 *
 * @param[in] ep_hdl Endpoint handle
 * @param[out] urb_ret Dequeued URB, or NULL if no more URBs to dequeue
 * @return esp_err_t
 */
esp_err_t usbh_ep_dequeue_urb(usbh_ep_handle_t ep_hdl, urb_t **urb_ret);

/**
 * @brief Execute a command on a particular endpoint
 *
 * Endpoint commands allows executing a certain action on an endpoint (e.g., halting, flushing, clearing etc)
 *
 * @param[in] ep_hdl Endpoint handle
 * @param[in] command Endpoint command
 * @return esp_err_t
 */
esp_err_t usbh_ep_command(usbh_ep_handle_t ep_hdl, usbh_ep_cmd_t command);

/**
 * @brief Get the context of an endpoint
 *
 * Get the context variable assigned to and endpoint on allocation.
 *
 * @note This function can block
 * @param[in] ep_hdl Endpoint handle
 * @return Endpoint context
 */
void *usbh_ep_get_context(usbh_ep_handle_t ep_hdl);

// -------------------------------------------------- Hub Functions ----------------------------------------------------

// ------------------- Device Related ----------------------

/**
 * @brief Indicates to USBH that the Hub driver is installed
 *
 * - The Hub driver must call this function in its installation to indicate the the USBH that it has been installed.
 * - This should only be called after the USBH has already be installed
 *
 * @note Hub Driver only
 * @param[in] hub_req_callback Hub request callback
 * @param[in] callback_arg Callback argument
 * @return esp_err_t
 */
esp_err_t usbh_hub_is_installed(usbh_hub_req_cb_t hub_req_callback, void *callback_arg);

/**
 * @brief Indicates to USBH the start of enumeration for a device
 *
 * - The Hub driver calls this function before it starts enumerating a new device.
 * - The USBH will allocate a new device that will be initialized by the Hub driver using the remaining hub enumeration
 *   functions.
 * - The new device's default pipe handle is returned to all the Hub driver to be used during enumeration.
 *
 * @note Hub Driver only
 * @param[in] port_hdl Handle of the port that the device is connected to
 * @param[in] dev_speed Device's speed
 * @param[out] new_dev_hdl Device's handle
 * @param[out] default_pipe_hdl Device's default pipe handle
 * @return esp_err_t
 */
esp_err_t usbh_hub_add_dev(hcd_port_handle_t port_hdl, usb_speed_t dev_speed, usb_device_handle_t *new_dev_hdl, hcd_pipe_handle_t *default_pipe_hdl);

/**
 * @brief Indicates to the USBH that a hub event has occurred for a particular device
 *
 * @param dev_hdl Device handle
 * @param hub_event Hub event
 * @return esp_err_t
 */
esp_err_t usbh_hub_pass_event(usb_device_handle_t dev_hdl, usbh_hub_event_t hub_event);

// ----------------- Enumeration Related -------------------

/**
 * @brief Assign the enumerating device's address
 *
 * @note Hub Driver only
 * @note Must call in sequence
 * @param[in] dev_hdl Device handle
 * @param dev_addr
 * @return esp_err_t
 */
esp_err_t usbh_hub_enum_fill_dev_addr(usb_device_handle_t dev_hdl, uint8_t dev_addr);

/**
 * @brief Fill the enumerating device's descriptor
 *
 * @note Hub Driver only
 * @note Must call in sequence
 * @param[in] dev_hdl Device handle
 * @param device_desc
 * @return esp_err_t
 */
esp_err_t usbh_hub_enum_fill_dev_desc(usb_device_handle_t dev_hdl, const usb_device_desc_t *device_desc);

/**
 * @brief Fill the enumerating device's active configuration descriptor
 *
 * @note Hub Driver only
 * @note Must call in sequence
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @param config_desc_full
 * @return esp_err_t
 */
esp_err_t usbh_hub_enum_fill_config_desc(usb_device_handle_t dev_hdl, const usb_config_desc_t *config_desc_full);

/**
 * @brief Fill one of the string descriptors of the enumerating device
 *
 * @note Hub Driver only
 * @note Must call in sequence
 * @param dev_hdl Device handle
 * @param str_desc Pointer to string descriptor
 * @param select Select which string descriptor. 0/1/2 for Manufacturer/Product/Serial Number string descriptors respectively
 * @return esp_err_t
 */
esp_err_t usbh_hub_enum_fill_str_desc(usb_device_handle_t dev_hdl, const usb_str_desc_t *str_desc, int select);

/**
 * @brief Indicate the device enumeration is completed
 *
 * This will all the device to be opened by clients, and also trigger a USBH_EVENT_DEV_NEW event.
 *
 * @note Hub Driver only
 * @note Must call in sequence
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @return esp_err_t
 */
esp_err_t usbh_hub_enum_done(usb_device_handle_t dev_hdl);

/**
 * @brief Indicate that device enumeration has failed
 *
 * This will cause the enumerating device's resources to be cleaned up
 * The Hub Driver must guarantee that the enumerating device's default pipe is already halted, flushed, and dequeued.
 *
 * @note Hub Driver only
 * @note Must call in sequence
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @return esp_err_t
 */
esp_err_t usbh_hub_enum_failed(usb_device_handle_t dev_hdl);

#ifdef __cplusplus
}
#endif
