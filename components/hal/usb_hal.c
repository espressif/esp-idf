/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "hal/usb_ll.h"
#include "hal/usb_hal.h"

void usb_hal_init(usb_hal_context_t *usb)
{
    if (usb->use_external_phy) {
        usb_ll_ext_phy_enable();
    } else {
        usb_ll_int_phy_enable();
    }
}
