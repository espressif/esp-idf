/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "hcd.h"
#include "usbh.h"
#include "usb/usb_types_stack.h"
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_ch11.h"
#include "ext_port.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------- Handles ---------------------------------------

typedef struct ext_hub_s *ext_hub_handle_t;

// ---------------------------- Callbacks --------------------------------------

/**
 * @brief Callback used to indicate that the External Hub Driver requires process callback
 * For Hub Driver only
 */
typedef bool (*ext_hub_cb_t)(bool in_isr, void *user_arg);

/**
 * @brief External Hub Driver configuration
 */
typedef struct {
    ext_hub_cb_t proc_req_cb;                       /**< External Hub process callback */
    void *proc_req_cb_arg;                          /**< External Hub process callback argument */
    const ext_port_driver_api_t* port_driver;       /**< External Port Driver */
} ext_hub_config_t;

// ------------------------------ Driver ---------------------------------------

/**
 * @brief Install External Hub Driver
 *
 * Entry:
 * - should be called within Hub Driver
 *
 * @param[in] config External Hub driver configuration
 *
 * @return
 *    - ESP_OK: External Hub driver successfully installed
 *    - ESP_ERR_NO_MEM: Insufficient memory
 *    - ESP_ERR_INVALID_STATE: External Hub driver already installed
 */
esp_err_t ext_hub_install(const ext_hub_config_t* config);

/**
 * @brief Uninstall External Hub Driver
 *
 * Entry:
 * - should be called within Hub Driver
 *
 * @return
 *    - ESP_OK: External Hub driver successfully uninstalled
 *    - ESP_ERR_INVALID_STATE: External Hub driver is not installed, or the driver has unhandled devices
 */
esp_err_t ext_hub_uninstall(void);

/**
 * @brief External Hub Driver get client pointer
 *
 * Entry:
 * - should be called within Hub Driver
 *
 * @return
 *    - Unique pointer to identify the External Hub as a USB Host client
 */
void *ext_hub_get_client(void);

// -------------------------- External Hub API ---------------------------------

/**
 * @brief Get External Hub device handle by USBH device handle
 *
 * @param[in] dev_hdl       USBH device handle
 * @param[out] ext_hub_hdl  External Hub device handle
 *
 * @return
 *    - ESP_OK: External Hub device handle successfully obtained
 *    - ESP_ERR_INVALID_STATE: External Hub driver is not installed
 *    - ESP_ERR_NOT_FOUND: Device not found
 */
esp_err_t ext_hub_get_handle(usb_device_handle_t dev_hdl, ext_hub_handle_t *ext_hub_hdl);

/**
 * @brief Add new device
 *
 * After attaching new device:
 *  - configure it's parameters (requesting hub descriptor)
 *
 * @param[in] dev_addr      Device bus address
 *
 * @return
 *    - ESP_OK: New device added successfully
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 */
esp_err_t ext_hub_new_dev(uint8_t dev_addr);

/**
 * @brief Device gone
 *
 * After device were detached:
 *  - prepare the device to be freed
 *
 * @param[in] dev_addr       Device bus address
 *
 * @return
 *    - ESP_OK: Device freed successfully
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: Device address not found
 */
esp_err_t ext_hub_dev_gone(uint8_t dev_addr);

/**
 * @brief Marks all devices to be freed
 *
 * Entry:
 * - should be called within Hub Driver when USB Host library need to be uninstalled
 *
 */
void ext_hub_mark_all_free(void);

/**
 * @brief The External Hub or Ports statuses change completed
 *
 * Enables Interrupt IN endpoint to get information about Hub or Ports statuses change
 *
 * @param[in] ext_hub_hdl     External Hub device handle
 *
 * @return
 *    - ESP_OK: External Hub or Ports statuses change completed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: External hub must be in configured state
 */
esp_err_t ext_hub_status_handle_complete(ext_hub_handle_t ext_hub_hdl);

/**
 * @brief External Hub driver's process function
 *
 * External Hub driver process function that must be called repeatedly to process the driver's actions and events.
 * If blocking, the caller can block on the notification callback of source USB_PROC_REQ_SOURCE_HUB
 * to run this function.
 *
 * @return
 *    - ESP_OK: All events handled
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 */
esp_err_t ext_hub_process(void);

// -------------------- External Hub - Port related ----------------------------

/**
 * @brief Indicate to the External Hub driver that a device's port can be recycled
 *
 * The device connected to the port has been freed. The Hub driver can now
 * recycle the port.
 *
 * @param[in] ext_hub_hdl   External Hub handle
 * @param[in] port_num      Port number
 *
 * @return
 *    - ESP_OK: The port can be recycled
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_SIZE: External Hub port number out of the hub's range
 *    - ESP_ERR_NOT_SUPPORTED: External Port Driver has not been installed
 *    - ESP_ERR_INVALID_STATE: External Hub wasn't configured or suspended
 */
esp_err_t ext_hub_port_recycle(ext_hub_handle_t ext_hub_hdl, uint8_t port_num);

/**
 * @brief Indicate to the External Hub driver that a device's port need a reset
 *
 * @param[in] ext_hub_hdl   External Hub handle
 * @param[in] port_num      Port number
 *
 * @return
 *    - ESP_OK: The port can be reset
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_SIZE: External Hub port number out of the hub's range
 *    - ESP_ERR_NOT_SUPPORTED: External Port Driver has not been installed
 */
esp_err_t ext_hub_port_reset(ext_hub_handle_t ext_hub_hdl, uint8_t port_num);

/**
 * @brief Indicate to the External Hub driver that a device's port has a device and device has been enumerated
 *
 * @param[in] ext_hub_hdl   External Hub handle
 * @param[in] port_num      Port number
 *
 * @return
 *    - ESP_OK: Device on the External hub's port has been enumerated
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_SIZE: External Hub port number out of the hub's range
 *    - ESP_ERR_NOT_SUPPORTED: External Port Driver has not been installed
 */
esp_err_t ext_hub_port_active(ext_hub_handle_t ext_hub_hdl, uint8_t port_num);

/**
 * @brief Indicate to the External Hub driver that a device's port should be disabled
 *
 * @param[in] ext_hub_hdl   External Hub handle
 * @param[in] port_num      Port number
 *
 * @return
 *    - ESP_OK: Device on the External hub's port can be disabled
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_SIZE: External Hub port number out of the hub's range
 *    - ESP_ERR_NOT_SUPPORTED: External Port Driver has not been installed
 *    - ESP_ERR_INVALID_STATE: External Hub wasn't configured or suspended
 */
esp_err_t ext_hub_port_disable(ext_hub_handle_t ext_hub_hdl, uint8_t port_num);

/**
 * @brief Returns device speed of the device, attached to the port
 *
 * @param[in] ext_hub_hdl   External Hub handle
 * @param[in] port_num      Port number
 * @param[out] speed        Devices' speed
 *
 * @return
 *    - ESP_OK: The device's speed obtained successfully
 *    - ESP_ERR_NOT_ALLOWED: External Hub driver is not installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_SIZE: External Hub port number out of the hub's range
 *    - ESP_ERR_NOT_SUPPORTED: External Port Driver has not been installed
 *    - ESP_ERR_INVALID_STATE: External Hub wasn't configured or suspended
 */
esp_err_t ext_hub_port_get_speed(ext_hub_handle_t ext_hub_hdl, uint8_t port_num, usb_speed_t *speed);

// --------------------------- USB Chapter 11 ----------------------------------

/**
 * @brief USB Hub Request
 *
 * @note This function designed to be called from the External Port driver
 *
 * @param[in] port_hdl      External Port handle
 * @param[in] data          Request data
 * @param[in] user_arg      User argument
 *
 * @return
 *    - ESP_ERR_NOT_ALLOWED if External Hub Driver is not installed
 *    - ESP_ERR_INVALID_ARG if invalid argument
 *    - ESP_ERR_INVALID_SIZE if port number is out of the hub's range
 *    - ESP_ERR_INVALID_STATE if the hub device wasn't configured
 *    - ESP_ERR_NOT_SUPPORTED if the request is not supported
 *    - ESP_OK if control transfer was successfully submitted
 */
esp_err_t ext_hub_request(ext_port_hdl_t port_hdl, ext_port_parent_request_data_t *data, void *user_arg);

#ifdef __cplusplus
}
#endif
