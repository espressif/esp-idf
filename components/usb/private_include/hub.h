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

#if CONFIG_USB_HOST_HUBS_SUPPORTED
#define ENABLE_USB_HUBS                     1
#endif // CONFIG_USB_HOST_HUBS_SUPPORTED

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
 * @note This function should only be called from the Host Library task
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
 * The device connected to the port has been freed. The Hub driver can now recycled the port
 *
 * @note This function should only be called from the Host Library task
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
 * @note This function should only be called from the Host Library task
 *
 * @param[in] parent_dev_hdl
 * @param[in] parent_port_num
 * @return
 *     - ESP_OK: Success
 */
esp_err_t hub_port_reset(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num);

/**
 * @brief Activate the port
 *
 * @note This function should only be called from the Host Library task
 *
 * @param[in] parent_dev_hdl    Parent device handle (is used to get the External Hub handle)
 * @param[in] parent_port_num   Parent number (is used to specify the External Port)
 * @return
 *     - ESP_OK: Success
 */
esp_err_t hub_port_active(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num);

/**
 * @brief Disable the port
 *
 * @note This function should only be called from the Host Library task
 *
 * @param[in] parent_dev_hdl    Parent device handle (is used to get the External Hub handle)
 * @param[in] parent_port_num   Parent number (is used to specify the External Port)
 * @return
 * @retval ESP_OK:                  Port has been disabled without error
 * @retval ESP_ERR_INVALID_STATE:   Port can't be disabled in current state
 * @retval ESP_ERR_NOT_SUPPORTED:   This function is not support by the selected port
 */
esp_err_t hub_port_disable(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num);

#if ENABLE_USB_HUBS
/**
 * @brief Notify Hub driver that new device has been attached
 *
 * If device is has a HUB class, then it will be added as External Hub to Hub Driver.
 *
 * @param[in] dev_addr  Device bus address
 * @return
 *     - ESP_OK: Success
 */
esp_err_t hub_notify_new_dev(uint8_t dev_addr);

/**
 * @brief Notify Hub driver that device has been removed
 *
 * If the device was an External Hub, then it will be removed from the Hub Driver.
 *
 * @param[in] dev_addr  Device bus address
 * @return
 *     - ESP_OK: Success
 */
esp_err_t hub_notify_dev_gone(uint8_t dev_addr);

/**
 * @brief Notify Hub driver that all devices should be freed
 *
 * @return
 *     - ESP_OK: Success
 */
esp_err_t hub_notify_all_free(void);
#endif // ENABLE_USB_HUBS

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
