/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "usb_private.h"
#include "usbh.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------- Events ----------------------------------------
typedef enum {
    HUB_EVENT_CONNECTED,            /**< Device has been connected */
    HUB_EVENT_RESET_COMPLETED,      /**< Device reset completed */
    HUB_EVENT_DISCONNECTED,         /**< Device has been disconnected */
} hub_event_t;

typedef struct {
    hub_event_t event;                          /**< HUB event ID */
    union {
        struct {
            unsigned int uid;                   /**< Unique device ID */
        } connected;                            /**< HUB_EVENT_DEV_CONNECTED specific data */

        struct {
            unsigned int uid;                   /**< Unique device ID */
        } reset_completed;                      /**< HUB_EVENT_RESET_COMPLETED specific data */

        struct {
            unsigned int uid;                   /**< Unique device ID */
        } disconnected;                         /**< HUB_EVENT_DEV_DISCONNECTED specific data */
    };
} hub_event_data_t;

// ---------------------- Callbacks ------------------------

/**
 * @brief Callback used to indicate that the USBH has an event
 *
 * @note This callback is called from within usbh_process()
 */
typedef void (*hub_event_cb_t)(hub_event_data_t *event_data, void *arg);

// ------------------------------------------------------ Types --------------------------------------------------------

/**
 * @brief Hub driver configuration
 */
typedef struct {
    usb_proc_req_cb_t proc_req_cb;                  /**< Processing request callback */
    void *proc_req_cb_arg;                          /**< Processing request callback argument */
    hub_event_cb_t event_cb;                        /**< Hub event callback */
    void *event_cb_arg;                             /**< Hub event callback argument */
#ifdef CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK
    usb_host_enum_filter_cb_t enum_filter_cb;       /**< Set device configuration callback */
#endif // CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK
} hub_config_t;

// ---------------------------------------------- Hub Driver Functions -------------------------------------------------

/**
 * @brief Install Hub driver
 *
 * Entry:
 * - USBH must already be installed
 * Exit:
 * - Install Hub driver memory resources
 * - Initializes the HCD root port
 *
 * @param[in] hub_config Hub driver configuration
 * @param[out] client_ret Unique pointer to identify the Hub as a USB Host client
 * @return esp_err_t
 */
esp_err_t hub_install(hub_config_t *hub_config, void **client_ret);

/**
 * @brief Uninstall Hub driver
 *
 * This must be called before uninstalling the USBH
 * Entry:
 * - Must have stopped the root port
 * Exit:
 * - HCD root port deinitialized
 *
 * @return esp_err_t
 */
esp_err_t hub_uninstall(void);

/**
 * @brief Start the Hub driver's root port
 *
 * This will power the root port ON
 *
 * @return esp_err_t
 */
esp_err_t hub_root_start(void);

/**
 * @brief Stops the Hub driver's root port
 *
 * This will power OFF the root port
 *
 * @return esp_err_t
 */
esp_err_t hub_root_stop(void);

/**
 * @brief Indicate to the Hub driver that a device's port can be recycled
 *
 * The device connected to the port has been freed. The Hub driver can now
 * recycled the port.
 *
 * @param[in] parent_dev_hdl
 * @param[in] parent_port_num
 * @param[in] dev_uid Device's unique ID
 * @return
 *     - ESP_OK: Success
 */
esp_err_t hub_port_recycle(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num, unsigned int dev_uid);

/**
 * @brief Reset the port
 *
 *
 * @param[in] parent_dev_hdl
 * @param[in] parent_port_num
 * @return
 *     - ESP_OK: Success
 */
esp_err_t hub_port_reset(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num);

/**
 * @brief Hub driver's processing function
 *
 * Hub driver handling function that must be called repeatedly to process the Hub driver's events. If blocking, the
 * caller can block on the notification callback of source USB_PROC_REQ_SOURCE_HUB to run this function.
 *
 * @return esp_err_t
 */
esp_err_t hub_process(void);

#ifdef __cplusplus
}
#endif
