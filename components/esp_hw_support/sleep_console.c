/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_private/sleep_console.h"
#include "esp_attr.h"

#if SOC_USB_SERIAL_JTAG_SUPPORTED && !SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP
#include "hal/usb_serial_jtag_ll.h"

static sleep_console_usj_enable_state_t s_usj_state = {0};

void sleep_console_usj_pad_backup_and_disable(void)
{
    s_usj_state.usj_clock_enabled = usb_serial_jtag_ll_module_is_enabled();
    if (!s_usj_state.usj_clock_enabled) {
        // Enable USJ clock and clear reset
        usb_serial_jtag_ll_enable_bus_clock(true);
        usb_serial_jtag_ll_reset_register();
        s_usj_state.usj_pad_enabled = false;
    } else {
        s_usj_state.usj_pad_enabled = usb_serial_jtag_ll_phy_is_pad_enabled();
    }
    // Disable USJ pad & clock
    usb_serial_jtag_ll_phy_enable_pad(false);
    usb_serial_jtag_ll_enable_bus_clock(false);
}

void sleep_console_usj_pad_restore(void)
{
    usb_serial_jtag_ll_enable_bus_clock(true);
    usb_serial_jtag_ll_phy_enable_pad(s_usj_state.usj_pad_enabled);
    if (!s_usj_state.usj_clock_enabled) {
        usb_serial_jtag_ll_enable_bus_clock(false);
    }
}
#endif
