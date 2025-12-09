/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/usb_utmi_ll.h"
#include "hal/usb_utmi_hal.h"

void _usb_utmi_hal_init(usb_utmi_hal_context_t *hal)
{
    hal->dev = &USB_UTMI;
    _usb_utmi_ll_enable_bus_clock(true);
    _usb_utmi_ll_reset_register();

    /*
    Additional setting to solve missing DCONN event on ESP32P4 (IDF-9953).

    Note: On ESP32P4, the HP_SYSTEM_OTG_SUSPENDM is not connected to 1 by hardware.
    For correct detection of the device detaching, internal signal should be set to 1 by the software.
    */
    usb_utmi_ll_enable_precise_detection(true);
    usb_utmi_ll_configure_ls(hal->dev, true);
}

void _usb_utmi_hal_disable(void)
{
    _usb_utmi_ll_enable_bus_clock(false);
}
