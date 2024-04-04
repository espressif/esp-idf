/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "hal/usb_wrap_ll.h"
#include "hal/usb_wrap_hal.h"

void usb_wrap_hal_init(usb_wrap_hal_context_t *hal)
{
    hal->dev = &USB_WRAP;
#if !USB_WRAP_LL_EXT_PHY_SUPPORTED
    usb_wrap_ll_phy_set_defaults(hal->dev);
#endif
}

#if USB_WRAP_LL_EXT_PHY_SUPPORTED
void usb_wrap_hal_phy_set_external(usb_wrap_hal_context_t *hal, bool external)
{
    if (external) {
        usb_wrap_ll_phy_enable_external(hal->dev, true);
    } else {
        usb_wrap_ll_phy_enable_external(hal->dev, false);
        usb_wrap_ll_phy_enable_pad(hal->dev, true);
    }
}
#endif // USB_WRAP_LL_EXT_PHY_SUPPORTED
