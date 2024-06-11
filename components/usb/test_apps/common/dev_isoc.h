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
device containing an ISOC endpoint be connected. This header contains
functions to get information and descriptors about that test device.

If you are connecting a different device, please update the descriptors in
dev_isoc.c accordingly.
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the test device's ISOC OUT endpoint descriptor
 *
 * @param[in] speed Test device's current speed
 * @return ISOC OUT endpoint descriptor
 */
const usb_ep_desc_t *dev_isoc_get_out_ep_desc(usb_speed_t speed);

#ifdef __cplusplus
}
#endif
