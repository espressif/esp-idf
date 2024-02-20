/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/usb_serial_jtag_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configures the internal PHY for USB_Serial_JTAG
 *
 * @param hw Start address of the USB Serial_JTAG registers
 */
static inline void usb_fsls_phy_ll_int_jtag_enable(usb_serial_jtag_dev_t *hw)
{
    // USB_Serial_JTAG use internal PHY
    hw->conf0.phy_sel = 0;
    // Disable software control USB D+ D- pullup pulldown (Device FS: dp_pullup = 1)
    hw->conf0.pad_pull_override = 0;
    // Enable USB D+ pullup
    hw->conf0.dp_pullup = 1;
    // Enable USB pad function
    hw->conf0.usb_pad_enable = 1;
}

#ifdef __cplusplus
}
#endif
