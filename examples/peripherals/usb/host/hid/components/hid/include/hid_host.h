/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <wchar.h>
#include <stdint.h>
#include "esp_err.h"
#include <freertos/FreeRTOS.h>

#include "hid.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hid_host_device *hid_host_device_handle_t;     /**< Handle to a HID */
typedef struct hid_interface *hid_host_interface_handle_t;    /**< Handle to a particular HID interface */

/**
 * @brief HID Interface input report callback
 *
 * @param[in] data     Pointer to buffer containing input report data
 * @param[in] length   Data length
 */
typedef void (*hid_input_report_cb_t)(const uint8_t *const data, const int length);

/**
 * @brief USB HID event containing event type and associated device handle
*/
typedef struct {
    enum {
        HID_DEVICE_CONNECTED,       /**< HID device has been connected to the system.*/
        HID_DEVICE_DISCONNECTED,    /**< HID device has been disconnected from the system.*/
    } event;
    union {
        uint8_t address;                /**< Address of connected HID device.*/
        hid_host_device_handle_t handle; /**< HID device handle to disconnected device.*/
    } device;
} hid_host_event_t;

/**
 * @brief USB HID interface event for application logic
*/
typedef enum {
    HID_DEVICE_INTERFACE_INIT = 0x00,       /**< Interface available for application logic */
    HID_DEVICE_INTERFACE_CLAIM,             /**< Interface was claimed */
    HID_DEVICE_INTERFACE_RELEASE,           /**< Interface was released */
    HID_DEVICE_INTERFACE_TRANSFER_ERROR     /**< Interface transfer error occurred */
} hid_interface_event_t;

/**
 * @brief USB HID Host interface event containing event type and associated parameters
*/
typedef struct {
    hid_interface_event_t event;            /**< USB HID Interface event */
    struct interface_event_param {
        uint8_t dev_addr;                   /**< USB Device address */
        uint8_t num;                        /**< USB Interface number */
        hid_protocol_t proto;               /**< USB HID Interface protocol */
    } interface;
} hid_host_interface_event_t;


/**
 * @brief USB HID host event callback.
 *
 * @param[in] event mass storage event
*/
typedef void (*hid_host_event_cb_t)(const hid_host_event_t *event, void *arg);

typedef void (*hid_host_interface_event_cb_t)(const hid_host_interface_event_t *event, void *arg);

/**
 * @brief HID configuration structure.
*/
typedef struct {
    bool create_background_task;    /**< When set to true, background task handling USB events is created.
                                         Otherwise user has to periodically call hid_host_handle_events function */
    size_t task_priority;           /**< Task priority of created background task */
    size_t stack_size;              /**< Stack size of created background task */
    BaseType_t core_id;             /**< Select core on which background task will run or tskNO_AFFINITY  */
    hid_host_event_cb_t callback;   /**< Callback invoked when HID event occurs. Must not be NULL. */
    void *callback_arg;             /**< User provided argument passed to callback */
} hid_host_driver_config_t;

typedef struct {
    uint8_t dev_addr;
    hid_host_interface_event_cb_t iface_event_cb;   /**< Callback invoked when HID event occurs. Must not be NULL. */
    void *iface_event_arg;             /**< User provided argument passed to callback */
} hid_host_device_config_t;

typedef struct {
    hid_protocol_t proto;
    hid_input_report_cb_t callback;
} hid_host_interface_config_t;

/**
 * @brief Install USB Host HID Class driver
 *
 * @param[in] config configuration structure HID to create
 * @return esp_err_r
 */
esp_err_t hid_host_install(const hid_host_driver_config_t *config);

/**
 * @brief Uninstall HID Class driver
 * @return esp_err_t
 */
esp_err_t hid_host_uninstall(void);

/**
 * @brief HID Host USB event handler
 *
 * If HID Host install was made with create_background_task=false configuration,
 * application needs to handle USB Host events itself.
 * Do not used id HID host install was made with create_background_task=true configuration
 *
 * @param[in] arg  Pointer to handler argument
 * @return none
 */
void hid_host_event_handler_task(void *arg);

/**
 * @brief Initialization of HID device.
 *
 * @param[in]  hid_host_dev_config Pointer to HID Host device configuration structure
 * @param[out] hid_device_handle   Pointer to HID device handle
 * @return esp_err_t
 */
esp_err_t hid_host_install_device(const hid_host_device_config_t *hid_host_dev_config,
                                  hid_host_device_handle_t *hid_device_handle);

/**
 * @brief Deinitialization of HID device.
 *
 * @param[in]  hid_device_handle  Device handle obtained from hid_host_install_device function
 * @return esp_err_t
 */
esp_err_t hid_host_uninstall_device(hid_host_device_handle_t hid_device_handle);

/**
 * @brief Print configuration descriptor.
 *
 * @param[in]  hid_device_handle  Device handle obtained from hid_host_install_device function
 * @return esp_err_t
 */
esp_err_t hid_host_print_descriptors(hid_host_device_handle_t hid_device_handle);

/**
 * @brief USB HID Interface claim
 *
 * @param[in]  iface_config  Pointer to Interface configuration structure
 * @param[out] iface_handle  Pointer to Interface handle
 * @param[in] esp_err_t
 */
esp_err_t hid_host_claim_interface(const hid_host_interface_config_t *iface_config,
                                   hid_host_interface_handle_t *iface_handle);

/**
 * @brief USB HID Interface release
 *
 * @param[in] iface_handle   Interface handle obtained from hid_host_claim_interface function
 * @param[in] esp_err_t
 */
esp_err_t hid_host_release_interface(hid_host_interface_handle_t iface_handle);


#ifdef __cplusplus
}
#endif //__cplusplus
