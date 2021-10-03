/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "esp_err.h"
#include "usb_private.h"
#include "usbh.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------ Types --------------------------------------------------------

/**
 * @brief Hub driver configuration
 */
typedef struct {
    usb_notif_cb_t notif_cb;            /**< Notification callback */
    void *notif_cb_arg;                 /**< Notification callback argument */
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
 * @return esp_err_t
 */
esp_err_t hub_install(hub_config_t *hub_config);

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
 * @brief Hub driver's processing function
 *
 * Hub driver handling function that must be called repeatdly to process the Hub driver's events. If blocking, the
 * caller can block on the notification callback of source USB_NOTIF_SOURCE_HUB to run this function.
 *
 * @return esp_err_t
 */
esp_err_t hub_process(void);

#ifdef __cplusplus
}
#endif
