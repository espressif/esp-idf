/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/usb_dwc_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for USB peripheral: GPIO signals
*/
const usb_phy_signal_conn_t usb_otg_periph_signal = {
    .extphy_vp_in = USB_EXTPHY_VP_IDX,
    .extphy_vm_in = USB_EXTPHY_VM_IDX,
    .extphy_rcv_in = USB_EXTPHY_RCV_IDX,
    .extphy_oen_out = USB_EXTPHY_OEN_IDX,
    .extphy_vpo_out = USB_EXTPHY_VPO_IDX,
    .extphy_vmo_out = USB_EXTPHY_VMO_IDX,
    .extphy_suspend_in = USB_EXTPHY_SUSPND_IDX,
    .extphy_speed_in = USB_EXTPHY_SPEED_IDX,
    .srp_bvalid_in = USB_SRP_BVALID_IN_IDX,
    .srp_sessend_in = USB_SRP_SESSEND_IN_IDX,
    .srp_chrgvbus_out = USB_SRP_CHRGVBUS_IDX,
    .srp_dischrgvbus_out = USB_SRP_DISCHRGVBUS_IDX,
    .otg_iddig_in = USB_OTG_IDDIG_IN_IDX,
    .otg_avalid_in = USB_OTG_AVALID_IN_IDX,
    .otg_vbusvalid_in = USB_OTG_VBUSVALID_IN_IDX,
    .otg_idpullup_out = USB_OTG_IDPULLUP_IDX,
    .otg_dppulldown_out = USB_OTG_DPPULLDOWN_IDX,
    .otg_dmpulldown_out = USB_OTG_DMPULLDOWN_IDX,
    .otg_drvvbus_out = USB_OTG_DRVVBUS_IDX,
    .module = PERIPH_USB_MODULE
};
