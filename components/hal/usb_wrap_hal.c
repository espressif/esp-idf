/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
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
#if !USB_WRAP_LL_EXT_PHY_SUPPORTED
    usb_wrap_ll_phy_set_defaults(hal->wrap_dev);
#endif
}

#if USB_WRAP_LL_EXT_PHY_SUPPORTED
void usb_fsls_phy_hal_otg_conf(usb_fsls_phy_hal_context_t *hal, usb_phy_target_t phy_target)
{
    if (phy_target == USB_PHY_TARGET_EXT) {
        usb_wrap_ll_phy_enable_external(hal->wrap_dev, true);
    } else if (phy_target == USB_PHY_TARGET_INT) {
        usb_wrap_ll_phy_enable_external(hal->wrap_dev, false);
        usb_wrap_ll_phy_enable_pad(hal->wrap_dev, true);
    }
}
#endif // USB_WRAP_LL_EXT_PHY_SUPPORTED

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
    usb_wrap_pull_override_vals_t vals = {
        .dp_pu = false,
        .dm_pu = false,
        .dp_pd = true,
        .dm_pd = true,
    };
    usb_wrap_ll_phy_enable_pull_override(hal->wrap_dev, &vals);
}

void usb_fsls_phy_hal_int_load_conf_dev(usb_fsls_phy_hal_context_t *hal, usb_phy_speed_t speed)
{
    // DEVICE - downstream
    if (speed == USB_PHY_SPEED_LOW) {
        // LS: dm_pu = 1
        usb_wrap_pull_override_vals_t vals = {
            .dp_pu = false,
            .dm_pu = true,
            .dp_pd = false,
            .dm_pd = false,
        };
        usb_wrap_ll_phy_enable_pull_override(hal->wrap_dev, &vals);
    } else {
        // FS: dp_pu = 1
        usb_wrap_pull_override_vals_t vals = {
            .dp_pu = true,
            .dm_pu = false,
            .dm_pd = false,
            .dp_pd = false,
        };
        usb_wrap_ll_phy_enable_pull_override(hal->wrap_dev, &vals);
    }
}

void usb_fsls_phy_hal_int_mimick_disconn(usb_fsls_phy_hal_context_t *hal, bool disconn)
{
    if (disconn) {
        /*
        We mimic a disconnect by enabling the internal PHY's test mode, then forcing the output_enable to HIGH. This will:
        A HIGH output_enable will cause the received VP and VM to be zero, thus mimicking a disconnection.
        */
        usb_wrap_test_mode_vals_t vals = {
            .tx_enable_n = true,
            .tx_dp = false,
            .tx_dm = false,
            .rx_dp = false,
            .rx_dm = false,
            .rx_rcv = false,
        };
        usb_wrap_ll_phy_test_mode_set_signals(hal->wrap_dev, &vals);
        usb_wrap_ll_phy_enable_test_mode(hal->wrap_dev, true);
    } else {
        usb_wrap_ll_phy_enable_test_mode(hal->wrap_dev, false);
    }
}
