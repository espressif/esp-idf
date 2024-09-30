/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "ext_hub.h"
#include "usb/usb_types_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ext_port_s  *ext_port_hdl_t;

// ------------------------------ Events ---------------------------------------
typedef enum {
    EXT_PORT_CONNECTED = 0x01,      /**< Port has a device connection event */
    EXT_PORT_RESET_COMPLETED,       /**< Port has completed the reset routine */
    EXT_PORT_DISCONNECTED,          /**< Port has a device disconnection event */
} ext_port_event_t;

/**
 * @brief Event data object for External Port driver events
 */
typedef struct {
    ext_port_event_t event;
    union {
        struct {
            ext_hub_handle_t ext_hub_hdl;       /**< Ports' parent external Hub handle */
            usb_device_handle_t parent_dev_hdl; /**< Ports' parent device handle */
            uint8_t parent_port_num;            /**< Ports' parent port number */
        } connected;                            /**< EXT_PORT_CONNECTED event specific data */

        struct {
            usb_device_handle_t parent_dev_hdl; /**< Ports' parent device handle */
            uint8_t parent_port_num;            /**< Ports' parent port number */
        } reset_completed;                      /**< EXT_PORT_RESET_COMPLETED event specific data */

        struct {
            usb_device_handle_t parent_dev_hdl; /**< Ports' parent device handle */
            uint8_t parent_port_num;            /**< Ports' parent port number */
        } disconnected;                         /**< EXT_PORT_DISCONNECTED event specific data */
    };
} ext_port_event_data_t;

// ------------------------------ Callbacks ------------------------------------
/**
 * @brief Callback used to indicate that the External Port Driver requires process callback
 *
 * @note For the Hub Driver only
 */
typedef void (*ext_port_cb_t)(void *user_arg);

/**
 * @brief Callback used to indicate that the External Port driver has an event
 *
 * @note For the Hub Driver only
 */
typedef void (*ext_port_event_cb_t)(ext_port_event_data_t *event_data, void *arg);

// ----------------- External Port Driver configuration ------------------------

/**
 * @brief External Port driver configuration
 */
typedef struct {
    ext_port_cb_t proc_req_cb;                  /**< External Port process callback */
    void *proc_req_cb_arg;                      /**< External Port process callback argument */
    ext_port_event_cb_t event_cb;               /**< External Port event callback */
    void *event_cb_arg;                         /**< External Port event callback argument */
} ext_port_driver_config_t;

/**
 * @brief External Port configuration
 *
 * Structure is used to create new port
 */
typedef struct {
    ext_hub_handle_t ext_hub_hdl;               /**< Ports' parent external Hub handle */
    usb_device_handle_t parent_dev_hdl;         /**< Ports' parent device handle */
    uint8_t parent_port_num;                    /**< Ports' parent port number */
    uint16_t port_power_delay_ms;               /**< Ports' Power on time to Power Good, ms */
} ext_port_config_t;

// -------------------- External Port Processing Functions ---------------------

/**
 * @brief Install the External Port Driver
 *
 * @note This function should only be called from the Hub Driver
 *
 * @param[in] config External Port Driver configuration
 * @return
 *    - ESP_ERR_NOT_ALLOWED:        The Driver was already installed
 *    - ESP_ERR_NO_MEM:             Unable to install the Driver, no memory
 *    - ESP_OK:                     The Driver has been installed successfully
 */
esp_err_t ext_port_install(const ext_port_driver_config_t *config);

/**
 * @brief Uninstall the External Port Driver
 *
 * @note This function should only be called from the Hub Driver
 *
 * @return
 *    - ESP_ERR_NOT_ALLOWED:        The Driver was not installed
 *    - ESP_ERR_INVALID_STATE:      The Driver has ports in the pending list and can't be uninstalled
 *    - ESP_OK:                     The Driver has been uninstall successfully
 */
esp_err_t ext_port_uninstall(void);

/**
 * @brief External Port Driver's process function
 *
 * @note This function should only be called from the Hub Driver
 *
 * External Port Driver process function that must be called repeatedly to process the driver's actions and events.
 * If blocking, the caller can block on the notification callback of source USB_PROC_REQ_SOURCE_HUB
 * to run this function.
 *
 * @return
 *    - ESP_ERR_NOT_ALLOWED:        The Driver was not installed
 *    - ESP_OK:                     The Driver processed completed
 */
esp_err_t ext_port_process(void);

/**
 * @brief Returns External Port Driver's API
 *
 * @note This is a specific API for the External Hub Driver to handle the ports.
 * @return
 *    - NULL:                       The Driver has not been installed
 *    - not NULL:                   Pointer to the External Port Driver API
 */
const ext_hub_port_driver_t *ext_port_get_driver(void);

#ifdef __cplusplus
}
#endif
