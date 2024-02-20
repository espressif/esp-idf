/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/usb_fsls_phy_ll.h"
#include "hal/usb_fsls_phy_hal.h"

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
        usb_fsls_phy_ll_ext_otg_enable(hal->wrap_dev);
    } else if (phy_target == USB_PHY_TARGET_INT) {
        usb_fsls_phy_ll_usb_wrap_pad_enable(hal->wrap_dev, true);
        usb_fsls_phy_ll_int_otg_enable(hal->wrap_dev);
    }
}

#if SOC_USB_SERIAL_JTAG_SUPPORTED
void usb_fsls_phy_hal_jtag_conf(usb_fsls_phy_hal_context_t *hal, usb_phy_target_t phy_target)
{
    if (phy_target == USB_PHY_TARGET_EXT) {
        usb_fsls_phy_ll_ext_jtag_enable(hal->jtag_dev);
    } else if (phy_target == USB_PHY_TARGET_INT) {
        usb_fsls_phy_ll_int_jtag_enable(hal->jtag_dev);
    }
}
#endif

void usb_fsls_phy_hal_int_load_conf_host(usb_fsls_phy_hal_context_t *hal)
{
    // HOST - upstream: dp_pd = 1, dm_pd = 1
    usb_fsls_phy_ll_int_load_conf(hal->wrap_dev, false, true, false, true);
}

void usb_fsls_phy_hal_int_load_conf_dev(usb_fsls_phy_hal_context_t *hal, usb_phy_speed_t speed)
{
    // DEVICE - downstream
    if (speed == USB_PHY_SPEED_LOW) {
        // LS: dm_pu = 1
        usb_fsls_phy_ll_int_load_conf(hal->wrap_dev, false, false, true, false);
    } else {
        // FS: dp_pu = 1
        usb_fsls_phy_ll_int_load_conf(hal->wrap_dev, true, false, false, false);
    }
}

void usb_fsls_phy_hal_int_mimick_disconn(usb_fsls_phy_hal_context_t *hal, bool disconn)
{
    /*
    We mimick a disconnect by enabling the internal PHY's test mode, then forcing the output_enable to HIGH. This will:
    A HIGH output_enable will cause the received VP and VM to be zero, thus mimicking a disconnection.
    */
    usb_fsls_phy_ll_int_enable_test_mode(hal->wrap_dev, disconn);
}
