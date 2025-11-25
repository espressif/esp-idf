/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/usb_serial_jtag_ll.h"
#include "hal/usb_serial_jtag_hal.h"

void usb_serial_jtag_hal_init(usb_serial_jtag_hal_context_t *hal)
{
    hal->dev = &USB_SERIAL_JTAG;
#if !USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
    usb_serial_jtag_ll_phy_set_defaults();
#endif
}

#if USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
void usb_serial_jtag_hal_phy_set_external(usb_serial_jtag_hal_context_t *hal, bool external)
{
    if (external) {
        usb_serial_jtag_ll_phy_enable_external(true);
    } else {
        usb_serial_jtag_ll_phy_enable_external(false);
        usb_serial_jtag_ll_phy_enable_pad(true);
    }
}
#endif // USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
