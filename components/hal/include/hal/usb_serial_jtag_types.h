/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_SERIAL_JTAG_SUPPORTED

/**
 * @brief USJ test mode values
 *
 * Specifies the logic values of each of the USB FSLS Serial PHY interface
 * signals when in test mode.
 */
typedef struct {
    bool dp_pu;     /**< D+ pull-up resistor enable/disable */
    bool dm_pu;     /**< D- pull-up resistor enable/disable */
    bool dp_pd;     /**< D+ pull-down resistor enable/disable */
    bool dm_pd;     /**< D- pull-down resistor enable/disable */
} usb_serial_jtag_pull_override_vals_t;

#endif // SOC_USB_SERIAL_JTAG_SUPPORTED

#ifdef __cplusplus
}
#endif
