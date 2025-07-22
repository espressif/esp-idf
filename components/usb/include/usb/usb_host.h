/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
Warning: The USB Host Library API is still a beta version and may be subject to change
*/

#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
// Include the other USB Host Library headers as well
#include "usb/usb_helpers.h"
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------- Macros and Types --------------------------------------------------

// ----------------------- Handles -------------------------

/**
 * @brief Handle to a USB Host Library asynchronous client
 *
 * An asynchronous client can be registered using usb_host_client_register()
 *
 * @note Asynchronous API
 */
typedef struct usb_host_client_handle_s *usb_host_client_handle_t;

// ----------------------- Events --------------------------

#define USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS     0x01    /**< All clients have been deregistered from the USB Host Library */
#define USB_HOST_LIB_EVENT_FLAGS_ALL_FREE       0x02    /**< The USB Host Library has freed all devices */

/**
 * @brief The type event in a client event message
 */
typedef enum {
    USB_HOST_CLIENT_EVENT_NEW_DEV,                  /**< A new device has been enumerated and added to the USB Host Library */
    USB_HOST_CLIENT_EVENT_DEV_GONE,                 /**< A device opened by the client is now gone */
} usb_host_client_event_t;

/**
 * @brief Client event message
 *
 * Client event messages are sent to each client of the USB Host Library in order to notify them of various
 * USB Host Library events such as:
 * - Addition of new devices
 * - Removal of existing devices
 *
 * @note The event message structure has a union with members corresponding to each particular event. Based on the event
 *       type, only the relevant member field should be accessed.
 */
typedef struct {
    usb_host_client_event_t event;              /**< Type of event */
    union {
        struct {
            uint8_t address;                    /**< New device's address */
        } new_dev;                              /**< New device info */
        struct {
            usb_device_handle_t dev_hdl;        /**< The handle of the device that was gone */
        } dev_gone;                             /**< Gone device info */
    };
} usb_host_client_event_msg_t;

// ------------------------ Info ---------------------------

/**
 * @brief Current information about the USB Host Library obtained via usb_host_lib_info()
 */
typedef struct {
    int num_devices;    /**< Current number of connected (and enumerated) devices */
    int num_clients;    /**< Current number of registered clients */
} usb_host_lib_info_t;

// ---------------------- Callbacks ------------------------

/**
 * @brief Client event callback
 *
 * - Each client of the USB Host Library must register an event callback to receive event messages from the USB Host
 *   Library.
 * - The client event callback is run from the context of the clients usb_host_client_handle_events() function
 */
typedef void (*usb_host_client_event_cb_t)(const usb_host_client_event_msg_t *event_msg, void *arg);

// -------------------- Configurations ---------------------

/**
 * @brief USB Host Library configuration
 *
 * Configuration structure of the USB Host Library. Provided in the usb_host_install() function
 */
typedef struct {
    bool skip_phy_setup;                        /**< If set, the USB Host Library will not configure the USB PHY thus allowing
                                                     the user to manually configure the USB PHY before calling usb_host_install().
                                                     Users should set this if they want to use an external USB PHY. Otherwise,
                                                     the USB Host Library will automatically configure the internal USB PHY */
    bool root_port_unpowered;                   /**< If set, the USB Host Library will not power on the root port on installation.
                                                     This allows users to power on the root port manually by calling
                                                     usb_host_lib_set_root_port_power(). */
    int intr_flags;                             /**< Interrupt flags for the underlying ISR used by the USB Host stack */
    usb_host_enum_filter_cb_t enum_filter_cb;   /**< Enumeration filter callback. Enable CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK
                                                     to use this feature. Set to NULL otherwise. */
    struct {
        uint32_t nptx_fifo_lines; /**< Required: Number of non-periodic TX FIFO lines.
                                       Must be > 0 to enable custom configuration. */
        uint32_t ptx_fifo_lines;  /**< Optional: Number of periodic TX FIFO lines.
                                       Can be 0 if periodic TX endpoints are not used. */
        uint32_t rx_fifo_lines;   /**< Required: Number of RX FIFO lines.
                                       Must be > 0 to enable custom configuration. */
    } fifo_settings_custom;       /**< Optional custom FIFO configuration (advanced use).
                                       RX and NPTX must be > 0. If all fields are zero,
                                       a default configuration will be selected based on Kconfig bias. */
    unsigned peripheral_map;      /**< Selects the USB peripheral(s) to use.
                                       - On targets with multiple USB peripherals, this field can be used to specify which ones to enable.
                                       - Set to 0 to use the default peripheral.
                                       - On High-Speed capable targets, the default is the High-Speed peripheral.
                                       - On Full-Speed only targets, the default is the Full-Speed peripheral.
                                       - Example: peripheral_map = BIT1; installs USB host on peripheral 1.
                                       - The mapping of bits to specific peripherals is defined in the USB_DWC_LL_GET_HW() macro. */
} usb_host_config_t;

/**
 * @brief USB Host Library Client configuration
 *
 * Configuration structure for a USB Host Library client. Provided in usb_host_client_register()
 */
typedef struct {
    bool is_synchronous;        /**< Whether the client is asynchronous or synchronous or not. Set to false for now. */
    int max_num_event_msg;      /**< Maximum number of event messages that can be stored (e.g., 3) */
    union {     // Note: Made into union or future expansion
        struct {
            usb_host_client_event_cb_t client_event_callback;   /**< Client's event callback function */
            void *callback_arg;                                 /**< Event callback function argument */
        } async;                                                /**< Async callback config */
    };
} usb_host_client_config_t;

// ------------------------------------------------ Library Functions --------------------------------------------------

/**
 * @brief Install the USB Host Library
 *
 * - This function should be called only once to install the USB Host Library
 * - This function should be called before any other USB Host Library functions are called
 *
 * @note If skip_phy_setup is set in the install configuration, the user is responsible for ensuring that the underlying
 *       Host Controller is enabled and the USB PHY (internal or external) is already setup before this function is
 *       called.
 * @param[in] config USB Host Library configuration
 *
 * @return
 *    - ESP_OK: USB Host installed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: USB Host Library is not in correct state to be installed
 *      (eg. the library itself of one of it's drivers is already installed)
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t usb_host_install(const usb_host_config_t *config);

/**
 * @brief Uninstall the USB Host Library
 *
 * - This function should be called to uninstall the USB Host Library, thereby freeing its resources
 * - All clients must have been deregistered before calling this function
 * - All devices must have been freed by calling usb_host_device_free_all() and receiving the
 *   USB_HOST_LIB_EVENT_FLAGS_ALL_FREE event flag
 *
 * @note If skip_phy_setup was set when the Host Library was installed, the user is responsible for disabling the
 *       underlying Host Controller and USB PHY (internal or external).
 *
 * @return
 *    - ESP_OK: USB Host uninstalled successfully
 *    - ESP_ERR_INVALID_STATE: USB Host Library is not installed, or has unfinished actions
 */
esp_err_t usb_host_uninstall(void);

/**
 * @brief Handle USB Host Library events
 *
 * - This function handles all of the USB Host Library's processing and should be called repeatedly in a loop
 * - Check event_flags_ret to see if an flags are set indicating particular USB Host Library events
 * - This function should never be called by multiple threads simultaneously
 *
 * @note This function can block
 * @param[in] timeout_ticks Timeout in ticks to wait for an event to occur
 * @param[out] event_flags_ret Event flags that indicate what USB Host Library event occurred.
 *
 * @return
 *    - ESP_OK: No events to be handled
 *    - ESP_ERR_INVALID_STATE: USB Host Library is not installed
 *    - ESP_ERR_TIMEOUT: Semaphore waiting for events has timed out
 */
esp_err_t usb_host_lib_handle_events(TickType_t timeout_ticks, uint32_t *event_flags_ret);

/**
 * @brief Unblock the USB Host Library handler
 *
 * - This function simply unblocks the USB Host Library event handling function (usb_host_lib_handle_events())
 *
 * @return
 *    - ESP_OK: USB Host library unblocked successfully
 *    - ESP_ERR_INVALID_STATE: USB Host Library is not installed
 */
esp_err_t usb_host_lib_unblock(void);

/**
 * @brief Get current information about the USB Host Library
 *
 * @param[out] info_ret USB Host Library Information
 *
 * @return
 *    - ESP_OK: USB Host Library info obtained successfully
 *    - ESP_ERR_INVALID_STATE: USB Host Library is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_lib_info(usb_host_lib_info_t *info_ret);

/**
 * @brief Power the root port ON or OFF
 *
 * - Powering ON the root port will allow device connections to occur
 * - Powering OFF the root port will disconnect all devices downstream off the root port and prevent
 *   any further device connections.
 *
 * @note If 'usb_host_config_t.root_port_unpowered' was set on USB Host Library installation, users must call this
 *       function to power ON the root port before any device connections can occur.
 *
 * @param[in] enable True to power the root port ON, false to power OFF
 * @return
 *    - ESP_OK: Root port power enabled/disabled
 *    - ESP_ERR_INVALID_STATE: Root port already powered or HUB driver not installed
 */
esp_err_t usb_host_lib_set_root_port_power(bool enable);

// ------------------------------------------------ Client Functions ---------------------------------------------------

/**
 * @brief Register a client of the USB Host Library
 *
 * - This function registers a client of the USB Host Library
 * - Once a client is registered, its processing function usb_host_client_handle_events() should be called repeatedly
 *
 * @param[in] client_config Client configuration
 * @param[out] client_hdl_ret Client handle
 *
 * @return
 *    - ESP_OK: USB Host Library client registered successfully
 *    - ESP_ERR_INVALID_STATE: USB Host Library is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t usb_host_client_register(const usb_host_client_config_t *client_config, usb_host_client_handle_t *client_hdl_ret);

/**
 * @brief Deregister a USB Host Library client
 *
 * - This function deregisters a client of the USB Host Library
 * - The client must have closed all previously opened devices before attempting to deregister
 *
 * @param[in] client_hdl Client handle
 *
 * @return
 *    - ESP_OK: USB Host Library client deregistered successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_client_deregister(usb_host_client_handle_t client_hdl);

/**
 * @brief USB Host Library client processing function
 *
 * - This function handles all of a client's processing and should be called repeatedly in a loop
 * - For a particular client, this function should never be called by multiple threads simultaneously
 *
 * @note This function can block
 * @param[in] client_hdl Client handle
 * @param[in] timeout_ticks Timeout in ticks to wait for an event to occur
 *
 * @return
 *    - ESP_OK: No event to be handled
 *    - ESP_ERR_INVALID_STATE: USB Host Library is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_TIMEOUT: Semaphore waiting for events has timed out
 */
esp_err_t usb_host_client_handle_events(usb_host_client_handle_t client_hdl, TickType_t timeout_ticks);

/**
 * @brief Unblock a client
 *
 * - This function simply unblocks a client if it is blocked on the usb_host_client_handle_events() function.
 * - This function is useful when need to unblock a client in order to deregister it.
 *
 * @param[in] client_hdl Client handle
 *
 * @return
 *    - ESP_OK: Client unblocked successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_client_unblock(usb_host_client_handle_t client_hdl);

// ------------------------------------------------- Device Handling ---------------------------------------------------

/**
 * @brief Open a device
 *
 * - This function allows a client to open a device
 * - A client must open a device first before attempting to use it (e.g., sending transfers, device requests etc.)
 *
 * @param[in] client_hdl Client handle
 * @param[in] dev_addr Device's address
 * @param[out] dev_hdl_ret Device's handle
 *
 * @return
 *    - ESP_OK: Device opened successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_device_open(usb_host_client_handle_t client_hdl, uint8_t dev_addr, usb_device_handle_t *dev_hdl_ret);

/**
 * @brief Close a device
 *
 * - This function allows a client to close a device
 * - A client must close a device after it has finished using the device (claimed interfaces must also be released)
 * - A client must close all devices it has opened before de-registering
 *
 * @note This function can block
 * @param[in] client_hdl Client handle
 * @param[in] dev_hdl Device handle
 *
 * @return
 *    - ESP_OK: Device closed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: The client never opened the device (the device address not found among opened devices)
 *    - ESP_ERR_INVALID_STATE: The client has not released all interfaces from the device
 */
esp_err_t usb_host_device_close(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl);

/**
 * @brief Indicate that all devices can be freed when possible
 *
 * - This function marks all devices as waiting to be freed
 * - If a device is not opened by any clients, it will be freed immediately
 * - If a device is opened by at least one client, the device will be free when the last client closes that device.
 * - Wait for the USB_HOST_LIB_EVENT_FLAGS_ALL_FREE flag to be set by usb_host_lib_handle_events() in order to know
 *   when all devices have been freed
 * - This function is useful when cleaning up devices before uninstalling the USB Host Library
 *
 * @return
 *    - ESP_OK: All devices already freed (i.e., there were no devices)
 *    - ESP_ERR_INVALID_STATE: Client must be deregistered
 *    - ESP_ERR_NOT_FINISHED: There are one or more devices that still need to be freed,
 *      wait for USB_HOST_LIB_EVENT_FLAGS_ALL_FREE event
 */
esp_err_t usb_host_device_free_all(void);

/**
 * @brief Fill a list of device address
 *
 * - This function fills an empty list with the address of connected devices
 * - The Device addresses can then used in usb_host_device_open()
 * - If there are more devices than the list_len, this function will only fill up to list_len number of devices.
 *
 * @param[in] list_len Length of the empty list
 * @param[inout] dev_addr_list Empty list to be filled
 * @param[out] num_dev_ret Number of devices
 *
 * @return
 *    - ESP_OK: Device list filled successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_device_addr_list_fill(int list_len, uint8_t *dev_addr_list, int *num_dev_ret);

// ------------------------------------------------- Device Requests ---------------------------------------------------

// ------------------- Cached Requests ---------------------

/**
 * @brief Get device's information
 *
 * - This function gets some basic information of a device
 * - The device must be opened first before attempting to get its information
 *
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @param[out] dev_info Device information
 *
 * @return
 *    - ESP_OK: Device information obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_device_info(usb_device_handle_t dev_hdl, usb_device_info_t *dev_info);

// ----------------------------------------------- Descriptor Requests -------------------------------------------------

// ----------------- Cached Descriptors --------------------

/**
 * @brief Get device's device descriptor
 *
 * - A client must call usb_host_device_open() first
 * - No control transfer is sent. The device's descriptor is cached on enumeration
 * - This function simple returns a pointer to the cached descriptor
 *
 * @note No control transfer is sent. The device's descriptor is cached on enumeration
 * @param[in] dev_hdl Device handle
 * @param[out] device_desc Device descriptor
 *
 * @return
 *    - ESP_OK: Device descriptor obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_get_device_descriptor(usb_device_handle_t dev_hdl, const usb_device_desc_t **device_desc);

/**
 * @brief Get device's active configuration descriptor
 *
 * - A client must call usb_host_device_open() first
 * - No control transfer is sent. The device's active configuration descriptor is cached on enumeration
 * - This function simple returns a pointer to the cached descriptor
 *
 * @note This function can block
 * @note No control transfer is sent. A device's active configuration descriptor is cached on enumeration
 * @param[in] dev_hdl Device handle
 * @param[out] config_desc Configuration descriptor
 *
 * @return
 *    - ESP_OK: Active configuration descriptor obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_get_active_config_descriptor(usb_device_handle_t dev_hdl, const usb_config_desc_t **config_desc);

/**
 * @brief Get get device's configuration descriptor
 *
 * - The USB Host library only caches a device's active configuration descriptor.
 * - This function reads any configuration descriptor of a particular device (specified by bConfigurationValue).
 * - This function will read the specified configuration descriptor via control transfers, and allocate memory
 *   to store that descriptor.
 * - Users can call usb_host_free_config_desc() to free the descriptor's memory afterwards.
 *
 * @note This function can block
 * @note A client must call usb_host_device_open() on the device first
 * @param[in] client_hdl Client handle - usb_host_client_handle_events() should be called repeatedly in a separate task
 *            to handle client events
 * @param[in] dev_hdl Device handle
 * @param[in] bConfigurationValue Index of device's configuration descriptor to be read
 * @param[out] config_desc_ret Returned configuration descriptor
 * @note bConfigurationValue starts from index 1
 *
 * @return
 *    - ESP_OK: Configuration descriptor obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_SUPPORTED: Invalid bConfigurationValue value (the device does not have this configuration value)
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t usb_host_get_config_desc(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl, uint8_t bConfigurationValue, const usb_config_desc_t **config_desc_ret);

/**
 * @brief Free a configuration descriptor
 *
 * This function frees a configuration descriptor that was returned by usb_host_get_config_desc()
 *
 * @param[out] config_desc Configuration descriptor
 *
 * @return
 *    - ESP_OK: Configuration descriptor freed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_free_config_desc(const usb_config_desc_t *config_desc);

// ----------------------------------------------- Interface Functions -------------------------------------------------

/**
 * @brief Function for a client to claim a device's interface
 *
 * - A client must claim a device's interface before attempting to communicate with any of its endpoints
 * - Once an interface is claimed by a client, it cannot be claimed by any other client.
 *
 * @note This function can block
 * @param[in] client_hdl Client handle
 * @param[in] dev_hdl Device handle
 * @param[in] bInterfaceNumber Interface number
 * @param[in] bAlternateSetting Interface alternate setting number
 *
 * @return
 *    - ESP_OK: Interface claimed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: USB Host is not a correct state to claim an interface
 *    - ESP_ERR_NOT_FOUND: Interface or EP not found
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t usb_host_interface_claim(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl, uint8_t bInterfaceNumber, uint8_t bAlternateSetting);

/**
 * @brief Function for a client to release a previously claimed interface
 *
 * - A client should release a device's interface after it no longer needs to communicate with the interface
 * - A client must release all of its interfaces of a device it has claimed before being able to close the device
 *
 * @note This function can block
 * @param[in] client_hdl Client handle
 * @param[in] dev_hdl Device handle
 * @param[in] bInterfaceNumber Interface number
 *
 * @return
 *    - ESP_OK: Interface released successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Client never opened the USB Device, or interface currently can not be freed
 *    - ESP_ERR_NOT_FOUND: Interface number not found in the list of interfaces
 */
esp_err_t usb_host_interface_release(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl, uint8_t bInterfaceNumber);

/**
 * @brief Halt a particular endpoint
 *
 * - The device must have been opened by a client
 * - The endpoint must be part of an interface claimed by a client
 * - Once halted, the endpoint must be cleared using usb_host_endpoint_clear() before it can communicate again
 *
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @param[in] bEndpointAddress Endpoint address
 *
 * @return
 *    - ESP_OK: Endpoint halted successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: Endpoint address not found
 *    - ESP_ERR_INVALID_STATE: Endpoint pipe is not in the correct state/condition to execute a command
 */
esp_err_t usb_host_endpoint_halt(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress);

/**
 * @brief Flush a particular endpoint
 *
 * - The device must have been opened by a client
 * - The endpoint must be part of an interface claimed by a client
 * - The endpoint must have been halted (either through a transfer error, or usb_host_endpoint_halt())
 * - Flushing an endpoint will caused an queued up transfers to be canceled
 *
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @param[in] bEndpointAddress Endpoint address
 *
 * @return
 *    - ESP_OK: Endpoint flushed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: Endpoint address not found
 *    - ESP_ERR_INVALID_STATE: Endpoint pipe is not in the correct state/condition to execute a command
 */
esp_err_t usb_host_endpoint_flush(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress);

/**
 * @brief Clear a halt on a particular endpoint
 *
 * - The device must have been opened by a client
 * - The endpoint must be part of an interface claimed by a client
 * - The endpoint must have been halted (either through a transfer error, or usb_host_endpoint_halt())
 * - If the endpoint has any queued up transfers, clearing a halt will resume their execution
 *
 * @note This function can block
 * @param[in] dev_hdl Device handle
 * @param[in] bEndpointAddress Endpoint address
 *
 * @return
 *    - ESP_OK: Endpoint cleared successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: Endpoint address not found
 *    - ESP_ERR_INVALID_STATE: Endpoint pipe is not in the correct state/condition to execute a command
 */
esp_err_t usb_host_endpoint_clear(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress);

// ------------------------------------------------ Asynchronous I/O ---------------------------------------------------

/**
 * @brief Allocate a transfer object
 *
 * - This function allocates a transfer object
 * - Each transfer object has a fixed sized buffer specified on allocation
 * - The resulting data_buffer_size can be bigger that the requested size. This is to ensure that the data buffer is cache aligned
 * - A transfer object can be re-used indefinitely
 * - A transfer can be submitted using usb_host_transfer_submit() or usb_host_transfer_submit_control()
 *
 * @param[in] data_buffer_size Size of the transfer's data buffer
 * @param[in] num_isoc_packets Number of isochronous packets in transfer (set to 0 for non-isochronous transfers)
 * @param[out] transfer Transfer object
 *
 * @return
 *    - ESP_OK: Transfer object allocated successfully
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t usb_host_transfer_alloc(size_t data_buffer_size, int num_isoc_packets, usb_transfer_t **transfer);

/**
 * @brief Free a transfer object
 *
 * - Free a transfer object previously allocated using usb_host_transfer_alloc()
 * - The transfer must not be in-flight when attempting to free it
 * - If a NULL pointer is passed, this function will simply return ESP_OK
 *
 * @param[in] transfer Transfer object
 *
 * @return
 *    - ESP_OK: Transfer object freed successfully
 */
esp_err_t usb_host_transfer_free(usb_transfer_t *transfer);

/**
 * @brief Submit a non-control transfer
 *
 * - Submit a transfer to a particular endpoint. The device and endpoint number is specified inside the transfer
 * - The transfer must be properly initialized before submitting
 * - On completion, the transfer's callback will be called from the client's usb_host_client_handle_events() function.
 *
 * @param[in] transfer Initialized transfer object
 *
 * @return
 *    - ESP_OK: Transfer submitted successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FINISHED: Transfer already in-flight
 *    - ESP_ERR_NOT_FOUND: Endpoint address not found
 *    - ESP_ERR_INVALID_STATE: Endpoint pipe is not in a correct state to submit transfer
 */
esp_err_t usb_host_transfer_submit(usb_transfer_t *transfer);

/**
 * @brief Submit a control transfer
 *
 * - Submit a control transfer to a particular device. The client must have opened the device first
 * - The transfer must be properly initialized before submitting. The first 8 bytes of the transfer's data buffer should
 *   contain the control transfer setup packet
 * - On completion, the transfer's callback will be called from the client's usb_host_client_handle_events() function.
 *
 * @param[in] client_hdl Client handle
 * @param[in] transfer Initialized transfer object
 *
 * @return
 *    - ESP_OK: Control transfer submitted successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FINISHED: Transfer already in-flight
 *    - ESP_ERR_NOT_FOUND: Endpoint address not found
 *    - ESP_ERR_INVALID_STATE: Endpoint pipe is not in a correct state to submit transfer
 */
esp_err_t usb_host_transfer_submit_control(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer);

#ifdef __cplusplus
}
#endif
