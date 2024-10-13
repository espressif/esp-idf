/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_stack.h"

/*
Some tests where the ESP (acting as host) will require that a particular test
device acting as an HID mouse be connected. That test device's information and descriptors are defined in this file.

If you are connecting a different device, please update the descriptors in
dev_hid.c accordingly.
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the test device
 *
 * @note Call this before running tests. This is necessary due to IDF-9886
 */
void dev_hid_init(void);

/**
 * @brief Get the test device's descriptor
 *
 * @param[in] speed Test device's current speed
 * @return Device descriptor
 */
const usb_device_desc_t *dev_hid_get_dev_desc(usb_speed_t speed);

/**
 * @brief Get the test device's configuration descriptor
 *
 * @param[in] speed Test device's current speed
 * @return Configuration descriptor
 */
const usb_config_desc_t *dev_hid_get_config_desc(usb_speed_t speed);

/**
 * @brief Get the test device's HID interface descriptor
 *
 * @param[in] speed Test device's current speed
 * @return HID interface descriptor
 */
const usb_intf_desc_t *dev_hid_get_intf_desc(usb_speed_t speed);

/**
 * @brief Get the test device's HID interrupt IN endpoint descriptor
 *
 * @param[in] speed Test device's current speed
 * @return Interrupt IN endpoint descriptor
 */
const usb_ep_desc_t *dev_hid_get_in_ep_desc(usb_speed_t speed);

/**
 * @brief Get the test device's manufacturer string descriptor
 *
 * @return Manufacturer string descriptor
 */
const usb_str_desc_t *dev_hid_get_str_desc_manu(void);

/**
 * @brief Get the test device's product string descriptor
 *
 * @return Product string descriptor
 */
const usb_str_desc_t *dev_hid_get_str_desc_prod(void);

#ifdef __cplusplus
}
#endif
