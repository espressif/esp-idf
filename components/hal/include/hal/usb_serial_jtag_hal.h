/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#if SOC_USB_SERIAL_JTAG_SUPPORTED
#include "hal/usb_serial_jtag_ll.h"
#endif
#include "hal/usb_serial_jtag_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_SERIAL_JTAG_SUPPORTED

/**
 * @brief HAL context type of USJ driver
 */
typedef struct {
    usb_serial_jtag_dev_t *dev;
} usb_serial_jtag_hal_context_t;

/**
 * @brief Initialize the USJ HAL driver
 *
 * @param hal USJ HAL context
 */
void usb_serial_jtag_hal_init(usb_serial_jtag_hal_context_t *hal);

/* ---------------------------- USB PHY Control  ---------------------------- */

#if USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
/**
 * @brief Configure whether USJ is routed to internal/external FSLS PHY
 *
 * @param hal USJ HAL context
 * @param external True if external, False if internal
 */
void usb_serial_jtag_hal_phy_set_external(usb_serial_jtag_hal_context_t *hal, bool external);
#endif // USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED

#endif // SOC_USB_SERIAL_JTAG_SUPPORTED

#ifdef __cplusplus
}
#endif
