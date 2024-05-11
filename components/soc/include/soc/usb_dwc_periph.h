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
    const uint8_t srp_bvalid_in;
    const uint8_t srp_sessend_in;
    const uint8_t srp_chrgvbus_out;
    const uint8_t srp_dischrgvbus_out;
    const uint8_t otg_iddig_in;
    const uint8_t otg_avalid_in;
    const uint8_t otg_vbusvalid_in;
    const uint8_t otg_idpullup_out;
    const uint8_t otg_dppulldown_out;
    const uint8_t otg_dmpulldown_out;
    const uint8_t otg_drvvbus_out;
    const periph_module_t module;
} usb_phy_signal_conn_t;

extern const usb_phy_signal_conn_t usb_otg_periph_signal;

#ifdef __cplusplus
}
#endif
