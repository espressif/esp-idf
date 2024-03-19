/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "soc/rtc_cntl_struct.h"
#if SOC_USB_SERIAL_JTAG_SUPPORTED
#include "hal/usb_serial_jtag_ll.h"
#endif
#include "hal/usb_wrap_ll.h"
#include "hal/usb_wrap_hal.h"

void usb_fsls_phy_hal_init(usb_fsls_phy_hal_context_t *hal)
{
    hal->wrap_dev = &USB_WRAP;
#if SOC_USB_SERIAL_JTAG_SUPPORTED
    hal->jtag_dev = &USB_SERIAL_JTAG;
#endif
}

void usb_fsls_phy_hal_otg_conf(usb_fsls_phy_hal_context_t *hal, usb_phy_target_t phy_target)
{
    if (phy_target == USB_PHY_TARGET_EXT) {
        usb_wrap_ll_phy_enable_external(hal->wrap_dev, true);
    } else if (phy_target == USB_PHY_TARGET_INT) {
        usb_wrap_ll_phy_enable_external(hal->wrap_dev, false);
        usb_wrap_ll_phy_enable_pad(hal->wrap_dev, true);
    }
}

#if SOC_USB_SERIAL_JTAG_SUPPORTED
void usb_fsls_phy_hal_jtag_conf(usb_fsls_phy_hal_context_t *hal, usb_phy_target_t phy_target)
{
    if (phy_target == USB_PHY_TARGET_EXT) {
        usb_serial_jtag_ll_phy_enable_external(true);   // USJ uses external PHY
    } else if (phy_target == USB_PHY_TARGET_INT) {
        usb_serial_jtag_ll_phy_enable_external(false);  // USJ uses internal PHY
        usb_serial_jtag_ll_phy_enable_pad(true);        // Enable USB PHY pads
    }
}
#endif

void usb_fsls_phy_hal_int_load_conf_host(usb_fsls_phy_hal_context_t *hal)
{
    // HOST - upstream: dp_pd = 1, dm_pd = 1
    usb_wrap_ll_phy_enable_pull_override(hal->wrap_dev, false, false, true, true);
}

void usb_fsls_phy_hal_int_load_conf_dev(usb_fsls_phy_hal_context_t *hal, usb_phy_speed_t speed)
{
    // DEVICE - downstream
    if (speed == USB_PHY_SPEED_LOW) {
        // LS: dm_pu = 1
        usb_wrap_ll_phy_enable_pull_override(hal->wrap_dev, false, true, false, false);
    } else {
        // FS: dp_pu = 1
        usb_wrap_ll_phy_enable_pull_override(hal->wrap_dev, true, false, false, false);
    }
}

void usb_fsls_phy_hal_int_mimick_disconn(usb_fsls_phy_hal_context_t *hal, bool disconn)
{
    if (disconn) {
        /*
        We mimick a disconnect by enabling the internal PHY's test mode, then forcing the output_enable to HIGH. This will:
        A HIGH output_enable will cause the received VP and VM to be zero, thus mimicking a disconnection.
        */
        usb_wrap_ll_phy_test_mode_set_signals(hal->wrap_dev,
                                            true,     // OEN
                                            false,    // TX D+
                                            false,    // TX D-
                                            false,    // RX D+
                                            false,    // RX D-
                                            false);   // RX RCv
        usb_wrap_ll_phy_enable_test_mode(hal->wrap_dev, true);
    } else {
        usb_wrap_ll_phy_enable_test_mode(hal->wrap_dev, false);
    }
}
