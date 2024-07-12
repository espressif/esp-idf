/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------- Settings & Configuration -----------------------------
#ifdef CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK
#define ENABLE_ENUM_FILTER_CALLBACK                 1
#endif // CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK

// -------------------------- Public Types -------------------------------------

// ---------------------------- Handles ----------------------------------------

/**
 * @brief Handle of enumeration control object
 */
typedef struct enum_ctx_handle_s * enum_ctx_handle_t;

// ------------------------------ Events ---------------------------------------

/**
 * @brief Event data object for Enumerator driver events
 */
typedef enum {
    ENUM_EVENT_STARTED,             /**< Enumeration of a device has started */
    ENUM_EVENT_RESET_REQUIRED,      /**< Enumerating device requires a reset */
    ENUM_EVENT_COMPLETED,           /**< Enumeration of a device has completed */
    ENUM_EVENT_CANCELED,            /**< Enumeration of a device was canceled */
} enum_event_t;

typedef struct {
    enum_event_t event;                         /**< Enumerator driver event */
    union {
        struct {
            unsigned int uid;                   /**< Device unique ID */
            usb_device_handle_t parent_dev_hdl; /**< Parent of the enumerating device */
            uint8_t parent_port_num;            /**< Parent port number of the enumerating device */
        } started;                              /**< ENUM_EVENT_STARTED specific data */

        struct {
            usb_device_handle_t parent_dev_hdl; /**< Parent of the enumerating device */
            uint8_t parent_port_num;            /**< Parent port number of the enumerating device */
        } reset_req;                            /**< ENUM_EVENT_RESET_REQUIRED specific data */

        struct {
            usb_device_handle_t parent_dev_hdl; /**< Parent of the enumerating device */
            uint8_t parent_port_num;            /**< Parent port number of the enumerating device */
            usb_device_handle_t dev_hdl;        /**< Handle of the enumerating device */
            uint8_t dev_addr;                   /**< Address of the enumerating device */
        } complete;                             /**< ENUM_EVENT_COMPLETED specific data */

        struct {
            usb_device_handle_t parent_dev_hdl; /**< Parent of the enumerating device */
            uint8_t parent_port_num;            /**< Parent port number of the enumerating device */
        } canceled;                             /**< ENUM_EVENT_CANCELED specific data */
    };
} enum_event_data_t;

// ---------------------------- Callbacks --------------------------------------

/**
 * @brief Callback used to indicate that the Enumerator has an event
 */
typedef void (*enum_event_cb_t)(enum_event_data_t *event_data, void *arg);

/**
 * @brief Enum driver configuration
 */
typedef struct {
    usb_proc_req_cb_t proc_req_cb;                  /**< Processing request callback */
    void *proc_req_cb_arg;                          /**< Processing request callback argument */
    enum_event_cb_t enum_event_cb;                  /**< Enum event callback */
    void *enum_event_cb_arg;                        /**< Enum event callback argument */
#if ENABLE_ENUM_FILTER_CALLBACK
    usb_host_enum_filter_cb_t enum_filter_cb;       /**< Set device configuration callback */
    void *enum_filter_cb_arg;                       /**< Set device configuration callback argument */
#endif // ENABLE_ENUM_FILTER_CALLBACK
} enum_config_t;

/**
 * @brief Install Enumerator driver
 *
 * Entry:
 * - USBH must already be installed
 * - HUB must already be installed
 *
 * @param[in]  enum_config Enumeration driver configuration
 * @param[out] client_ret Unique pointer to identify Enum Driver as a USB Host client
 *
 * @return
 *    - ESP_OK: Enumeration driver installed successfully
 *    - ESP_ERR_INVALID_STATE: Enumeration driver is already installed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t enum_install(enum_config_t *enum_config, void **client_ret);

/**
 * @brief Uninstall Enumerator driver
 *
 * This must be called before uninstalling the HUB and USBH
 *
 * @return
 *    - ESP_OK: Enumeration driver uninstalled successfully
 *    - ESP_ERR_INVALID_STATE: Enumeration driver is not installed
 */
esp_err_t enum_uninstall(void);

/**
 * @brief Start the enumeration process
 *
 * This will start the enumeration process for the device currently at address 0
 *
 * @param[in] uid  Unique device ID
 *
 * @return
 *    - ESP_OK: Enumeration process started
 *    - ESP_ERR_NOT_FOUND: No device at address 0
 */
esp_err_t enum_start(unsigned int uid);

/**
 * @brief Continue enumeration process
 *
 * This will continue the enumeration process. Typically called after the successful
 * handling of a request from the Enumerator driver (such as ENUM_EVENT_RESET_REQUIRED)
 *
 * @param[in] uid  Unique device ID
 *
 * @return
 *    - ESP_OK: Enumeration continues normally
 *    - ESP_ERR_INVALID_STATE: Enumeration driver not installed, or enumeration process in not active
 */
esp_err_t enum_proceed(unsigned int uid);

/**
 * @brief Cancel the enumeration process
 *
 * This will cancel enumeration process for device object under enumeration
 *
 * @param[in] uid Unique device ID
 *
 * @return
 *    - ESP_OK: Enumeration process canceled successfully
 *    - ESP_ERR_INVALID_STATE: Enumeration driver not installed, or enumeration process in not active
 */
esp_err_t enum_cancel(unsigned int uid);

/**
 * @brief Enumerator processing function
 *
 * Processing function that must be called repeatedly to process enumeration stages
 *
 * @return
 *    - ESP_OK: Enumeration continues normally
 *    - ESP_ERR_INVALID_STATE: Enumeration driver not installed, or enumeration process in not active
 */
esp_err_t enum_process(void);

#ifdef __cplusplus
}
#endif
