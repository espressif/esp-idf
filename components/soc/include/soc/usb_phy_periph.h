/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 Stores a bunch of USB-peripheral data.
*/
typedef struct {
    const uint8_t extphy_vp_in;
    const uint8_t extphy_vm_in;
    const uint8_t extphy_rcv_in;
    const uint8_t extphy_oen_out;
    const uint8_t extphy_vpo_out;
    const uint8_t extphy_vmo_out;
    const uint8_t extphy_suspend_in;
    const uint8_t extphy_speed_in;
    const periph_module_t module;
} usb_phy_signal_conn_t;

extern const usb_phy_signal_conn_t usb_phy_periph_signal;

#ifdef __cplusplus
}
#endif
