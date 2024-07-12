/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief Enumerator for various USBH events
 */
typedef enum {
    USBH_EVENT_CTRL_XFER,           /**< A control transfer has completed */
    USBH_EVENT_NEW_DEV,             /**< A new device has been enumerated and added to the device pool */
    USBH_EVENT_DEV_GONE,            /**< A device is gone. Clients should close the device */
    USBH_EVENT_DEV_FREE,            /**< A device has been freed. Its upstream port can now be recycled */
    USBH_EVENT_ALL_FREE,            /**< All devices have been freed */
} usbh_event_t;

/**
 * @brief Event data object for USBH events
 */
typedef struct {
    usbh_event_t event;
    union {
        struct {
            usb_device_handle_t dev_hdl;
            urb_t *urb;
        } ctrl_xfer_data;
        struct {
            uint8_t dev_addr;
        } new_dev_data;
        struct {
            uint8_t dev_addr;
            usb_device_handle_t dev_hdl;
        } dev_gone_data;
        struct {
            unsigned int dev_uid;
            usb_device_handle_t parent_dev_hdl;
            uint8_t port_num;
        } dev_free_data;
    };
} usbh_event_data_t;

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

// ------------------ Requests/Commands --------------------

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
 * @brief Callback used to indicate that the USBH has an event
 *
 * @note This callback is called from within usbh_process()
 */
typedef void (*usbh_event_cb_t)(usbh_event_data_t *event_data, void *arg);

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
    usbh_event_cb_t event_cb;               /**< USBH event callback */
    void *event_cb_arg;                     /**< USBH event callback argument */
} usbh_config_t;

/**
 * @brief USBH device parameters used in usbh_devs_add()
*/
typedef struct {
    unsigned int uid;                       /**< Unique ID assigned to the device */
    usb_speed_t speed;                      /**< Device's speed */
    hcd_port_handle_t root_port_hdl;        /**< Handle of the port that the device is connected to */
    usb_device_handle_t parent_dev_hdl;     /**< Parent's device handle */
    uint8_t parent_port_num;                /**< Parent's port number */
} usbh_dev_params_t;

// ---------------------- USBH Processing Functions ----------------------------

/**
 * @brief Installs the USBH driver
 *
 * - This function will internally install the HCD
 * - This must be called before calling any Hub driver functions
 *
 * @note Before calling this function, the Host Controller must already be un-clock gated and reset. The USB PHY
 *       (internal or external, and associated GPIOs) must already be configured.
 * @param[in] usbh_config USBH driver configuration
 *
 * @return
 *    - ESP_OK: USBH driver installed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: USBH driver is already installed
 *    - ESP_ERR_NO_MEM: Insufficient memory
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
 *
 * @return
 *    - ESP_OK: USBH driver uninstalled successfully
 *    - ESP_ERR_INVALID_STATE: USBH driver is not installed, or has unfinished actions
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
 *
 * @return
 *    - ESP_OK: All devices with pending events have been handled
 *    - ESP_ERR_INVALID_STATE: USBH driver is not installed
 */
esp_err_t usbh_process(void);

// ---------------------- Device Pool Functions --------------------------------

/**
 * @brief Get the current number of devices
 *
 * @note This function can block
 * @param[out] num_devs_ret Current number of devices
 *
 * @return
 *    - ESP_OK: Number of devices obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usbh_devs_num(int *num_devs_ret);

/**
 * @brief Fill list with address of currently connected devices
 *
 * - This function fills the provided list with the address of current connected devices
 * - Device address can then be used in usbh_devs_open()
 * - If there are more devices than the list_len, this function will only fill
 *   up to list_len number of devices.
 *
 * @param[in] list_len Length of empty list
 * @param[inout] dev_addr_list Empty list to be filled
 * @param[out] num_dev_ret Number of devices filled into list
 *
 * @return
 *    - ESP_OK: Address list filled successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usbh_devs_addr_list_fill(int list_len, uint8_t *dev_addr_list, int *num_dev_ret);

/**
 * @brief Create a device and add it to the device pool
 *
 * The created device will not be enumerated where the device's address is 0,
 * device and config descriptor are NULL. The device will still have a default
 * pipe, thus allowing control transfers to be submitted.
 *
 * - Call usbh_devs_open() before communicating with the device
 * - Call usbh_dev_enum_lock() before enumerating the device via the various
 * usbh_dev_set_...() functions.
 *
 * @param[in] params   Device parameters, using for device creation
 *
 * @return
 *    - ESP_OK: Device added to the device pool successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 *    - ESP_ERR_NOT_FINISHED: Adding a device to the device pool not finished
 */
esp_err_t usbh_devs_add(usbh_dev_params_t *params);

/**
 * @brief Indicates to the USBH that a device is gone
 *
 * @param[in] uid Unique ID assigned to the device on creation (see 'usbh_devs_add()')
 *
 * @return
 *    - ESP_OK: Device removed successfully
 *    - ESP_ERR_NOT_FOUND: Device with provided uid not found
 */
esp_err_t usbh_devs_remove(unsigned int uid);

/**
 * @brief Get a device's connection information
 *
 * @note Can be called without opening the device
 *
 * @param[in] uid               Unique ID assigned to the device
 * @param[out] parent_info      Parent device handle
 *
 * @return
 *    - ESP_OK: Device parent info obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: Device with provided uid not found
 */
esp_err_t usbh_devs_get_parent_info(unsigned int uid, usb_parent_dev_info_t *parent_info);

/**
 * @brief Mark that all devices should be freed at the next possible opportunity
 *
 * A device marked as free will not be freed until the last client using the device has called usbh_devs_close()
 *
 * @return
 *    - ESP_OK: There were no devices to free to begin with. Current state is all free
 *    - ESP_ERR_NOT_FINISHED: One or more devices still need to be freed (but have been marked "to be freed")
 */
esp_err_t usbh_devs_mark_all_free(void);

/**
 * @brief Open a device by address
 *
 * A device must be opened before it can be used
 *
 * @param[in] dev_addr Device address
 * @param[out] dev_hdl Device handle
 *
 * @return
 *    - ESP_OK: Device opened successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Device is in invalid state, either already gone (disconnected), or waiting to be freed
 *    - ESP_ERR_NOT_ALLOWED: It is not allowed to open the device, it is locked for the enumeration
 *    - ESP_ERR_NOT_FOUND: Device with provided address not found
 */
esp_err_t usbh_devs_open(uint8_t dev_addr, usb_device_handle_t *dev_hdl);

/**
 * @brief Trigger a USBH_EVENT_NEW_DEV event for the device
 *
 * This is typically called after a device has been fully enumerated.
 *
 * @param[in] dev_hdl Device handle
 *
 * @return
 *    - ESP_OK: USBH_EVENT_NEW_DEV triggered successfully
 *    - ESP_ERR_INVALID_STATE: Device is not in configured state
 */
esp_err_t usbh_devs_new_dev_event(usb_device_handle_t dev_hdl);

// ------------------------ Device Functions -----------------------------------

/**
 * @brief Close a device
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @param[in] dev_hdl Device handle
 *
 * @return
 *    - ESP_OK: Device closed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_ALLOWED: It is not allowed to close the device, it is locked for the enumeration
 */
esp_err_t usbh_dev_close(usb_device_handle_t dev_hdl);

// ------------------------------ Getters --------------------------------------
/**
 * @brief Get a device's address
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @param[in] dev_hdl Device handle
 * @param[out] dev_addr Device's address
 *
 * @return
 *    - ESP_OK: Device's address obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usbh_dev_get_addr(usb_device_handle_t dev_hdl, uint8_t *dev_addr);

/**
 * @brief Get a device's information
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 * @note It is possible that the device has not been enumerated yet, thus some fields may be NULL.
 *
 * @param[in] dev_hdl Device handle
 * @param[out] dev_info Device information
 *
 * @return
 *    - ESP_OK: Device's information obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usbh_dev_get_info(usb_device_handle_t dev_hdl, usb_device_info_t *dev_info);

/**
 * @brief Get a device's device descriptor
 *
 * The device descriptor is cached when the device is created by the Hub driver
 *
 * @note It is possible that the device has not been enumerated yet, thus the device descriptor could be NULL.
 *
 * @param[in] dev_hdl Device handle
 * @param[out] dev_desc_ret Device descriptor
 *
 * @return
 *    - ESP_OK: Device's device descriptor obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usbh_dev_get_desc(usb_device_handle_t dev_hdl, const usb_device_desc_t **dev_desc_ret);

/**
 * @brief Get a device's active configuration descriptor
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 * Simply returns a reference to the internally cached configuration descriptor
 *
 * @note It is possible that the device has not been enumerated yet, thus the configuration descriptor could be NULL.
 *
 * @param[in] dev_hdl Device handle
 * @param[out] config_desc_ret
 *
 * @return
 *    - ESP_OK: Device's active configuration descriptor obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usbh_dev_get_config_desc(usb_device_handle_t dev_hdl, const usb_config_desc_t **config_desc_ret);

// ------------------------------- Setters -------------------------------------

/**
 * @brief Lock a device for enumeration
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * - A device's enumeration lock must be set before any of its enumeration fields
 * (e.g., address, device/config descriptors) can be set/updated.
 * - The caller must be the sole opener of the device (see 'usbh_devs_open()')
 * when locking the device for enumeration.
 *
 * @param[in] dev_hdl Device handle
 *
 * @return
 *    - ESP_OK: Device is locked for enumeration successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Device is in an invalid state and can't be locked for enumeration
 */
esp_err_t usbh_dev_enum_lock(usb_device_handle_t dev_hdl);

/**
 * @brief Release a device's enumeration lock
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @param[in] dev_hdl Device handle
 *
 * @return
 *    - ESP_OK: Device enumeration lock released successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Device is in an invalid state and enumeration lock can't be released
 */
esp_err_t usbh_dev_enum_unlock(usb_device_handle_t dev_hdl);

/**
 * @brief Set the maximum packet size of EP0 for a device
 *
 * Typically called during enumeration after obtaining the first 8 bytes of the
 * device's descriptor.
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @note The device's enumeration lock must be set before calling this function
 * (see 'usbh_dev_enum_lock()')
 *
 * @param[in] dev_hdl Device handle
 * @param[in] wMaxPacketSize Maximum packet size
 *
 * @return
 *    - ESP_OK: EP0 MPS set successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Device's EP0 MPS can only be updated when in the default state,
 *      pipe is non in a condition to be updated
 *    - ESP_ERR_NOT_ALLOWED: Device's enum_lock must be set before enumeration related data fields can be set
 */
esp_err_t usbh_dev_set_ep0_mps(usb_device_handle_t dev_hdl, uint16_t wMaxPacketSize);

/**
 * @brief Set a device's address
 *
 * Typically called during enumeration after a SET_ADDRESS request has been
 * sent to the device.
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @note The device's enumeration lock must be set before calling this function
 * (see 'usbh_dev_enum_lock()')
 * @param[in] dev_hdl Device handle
 * @param[in] dev_addr Device address to set
 *
 * @return
 *    - ESP_OK: Device's address set successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Device's EP0 MPS can only be updated when in the default state,
 *      pipe is not in a condition to be updated
 *    - ESP_ERR_NOT_ALLOWED: Device's enum_lock must be set before enumeration related data fields can be set
 */
esp_err_t usbh_dev_set_addr(usb_device_handle_t dev_hdl, uint8_t dev_addr);

/**
 * @brief Set a device's device descriptor
 *
 * Typically called during enumeration after obtaining the device's device descriptor
 * via a GET_DESCRIPTOR request.
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @note The device's enumeration lock must be set before calling this function
 * (see 'usbh_dev_enum_lock()')
 *
 * @param[in] dev_hdl Device handle
 * @param[in] device_desc Device descriptor to copy
 *
 * @return
 *    - ESP_OK: Device's device descriptor set successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 *    - ESP_ERR_INVALID_STATE: Device's device descriptor can only be set in the default or addressed state
 *    - ESP_ERR_NOT_ALLOWED: Device's enum_lock must be set before we can set its device descriptor
 */
esp_err_t usbh_dev_set_desc(usb_device_handle_t dev_hdl, const usb_device_desc_t *device_desc);

/**
 * @brief Set a device's configuration descriptor
 *
 * Typically called during enumeration after obtaining the device's configuration
 * descriptor via a GET_DESCRIPTOR request.
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @note The device's enumeration lock must be set before calling this function
 * (see 'usbh_dev_enum_lock()')
 *
 * @param[in] dev_hdl Device handle
 * @param[in] config_desc_full Configuration descriptor to copy
 *
 * @return
 *    - ESP_OK: Device's configuration descriptor set successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 *    - ESP_ERR_INVALID_STATE: Device's config descriptor can only be set when in the addressed state
 *    - ESP_ERR_NOT_ALLOWED: Device's enum_lock must be set before we can set its config descriptor
 */
esp_err_t usbh_dev_set_config_desc(usb_device_handle_t dev_hdl, const usb_config_desc_t *config_desc_full);

/**
 * @brief Set a device's string descriptor
 *
 * Typically called during enumeration after obtaining one of the device's string
 * descriptor via a GET_DESCRIPTOR request.
 *
 * @note Callers of this function must have opened the device via usbh_devs_open()
 *
 * @note The device's enumeration lock must be set before calling this function
 * (see 'usbh_dev_enum_lock()')
 *
 * @param[in] dev_hdl Device handle
 * @param[in] str_desc String descriptor to copy
 * @param[in] select Select string descriptor. 0/1/2 for Manufacturer/Product/Serial
 * Number string descriptors respectively
 *
 * @return
 *    - ESP_OK: Device's string descriptor set successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 *    - ESP_ERR_INVALID_STATE: Device's string descriptors can only be set when in the default state
 *    - ESP_ERR_NOT_ALLOWED: Device's enum_lock must be set before we can set its string descriptors
 */
esp_err_t usbh_dev_set_str_desc(usb_device_handle_t dev_hdl, const usb_str_desc_t *str_desc, int select);

// ----------------------- Endpoint Functions ----------------------------------

/**
 * @brief Allocate an endpoint on a device
 *
 * This function allows clients to allocate a non-default endpoint (i.e., not EP0) on a connected device
 *
 * - A client must have opened the device using usbh_devs_open() before attempting to allocate an endpoint on the device
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
 *
 * @return
 *    - ESP_OK: Endpoint allocated successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 *    - ESP_ERR_NOT_FOUND: Endpoint with this address has not been found in device's configuration descriptor
 *    - ESP_ERR_INVALID_STATE: USBH is not in a correct state to allocate an endpoint
 *    - ESP_ERR_NOT_SUPPORTED: The pipe's configuration cannot be supported
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
 *
 * @return
 *    - ESP_OK: Endpoint freed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Endpoint's underlying pipe has an in-flight URB
 *    - ESP_ERR_NOT_FOUND: Endpoint with this address has not been allocated on the device
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
 *
 * @return
 *    - ESP_OK: Endpoint handle obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: Endpoint with this address has not been allocated on the device
 */
esp_err_t usbh_ep_get_handle(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress, usbh_ep_handle_t *ep_hdl_ret);

/**
 * @brief Execute a command on a particular endpoint
 *
 * Endpoint commands allows executing a certain action on an endpoint (e.g., halting, flushing, clearing etc)
 *
 * @param[in] ep_hdl Endpoint handle
 * @param[in] command Endpoint command
 *
 * @return
 *    - ESP_OK: Command executed successfully on an endpoint
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: The pipe is not in the correct state/condition too execute the command
 */
esp_err_t usbh_ep_command(usbh_ep_handle_t ep_hdl, usbh_ep_cmd_t command);

/**
 * @brief Get the context of an endpoint
 *
 * Get the context variable assigned to and endpoint on allocation.
 *
 * @note This function can block
 * @param[in] ep_hdl Endpoint handle
 *
 * @return
 *    - Endpoint context
 */
void *usbh_ep_get_context(usbh_ep_handle_t ep_hdl);

// ------------------------- Transfer Functions --------------------------------

/**
 * @brief Submit a control transfer (URB) to a device
 *
 * @param[in] dev_hdl Device handle
 * @param[in] urb URB
 *
 * @return
 *    - ESP_OK: Control transfer submitted successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: The pipe is not in an active state or an URB can't be enqueued
 */
esp_err_t usbh_dev_submit_ctrl_urb(usb_device_handle_t dev_hdl, urb_t *urb);

/**
 * @brief Enqueue a URB to an endpoint
 *
 * The URB will remain enqueued until it completes (successfully or errors out). Use usbh_ep_dequeue_urb() to dequeue
 * a completed URB.
 *
 * @param[in] ep_hdl Endpoint handle
 * @param[in] urb URB to enqueue
 *
 * @return
 *    - ESP_OK: URB enqueued successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: The pipe is not in an active state or an URB can't be enqueued
 */
esp_err_t usbh_ep_enqueue_urb(usbh_ep_handle_t ep_hdl, urb_t *urb);

/**
 * @brief Dequeue a URB from an endpoint
 *
 * Dequeue a completed URB from an endpoint. The USBH_EP_EVENT_URB_DONE indicates that URBs can be dequeued
 *
 * @param[in] ep_hdl Endpoint handle
 * @param[out] urb_ret Dequeued URB, or NULL if no more URBs to dequeue
 *
 * @return
 *    - ESP_OK: URB dequeued successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usbh_ep_dequeue_urb(usbh_ep_handle_t ep_hdl, urb_t **urb_ret);

#ifdef __cplusplus
}
#endif
