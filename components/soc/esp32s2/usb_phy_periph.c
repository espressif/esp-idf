/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/usb_phy_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for USB peripheral: GPIO signals
*/
const usb_phy_signal_conn_t usb_phy_periph_signal = {
	.extphy_vp_in = USB_EXTPHY_VP_IDX,
	.extphy_vm_in = USB_EXTPHY_VM_IDX,
	.extphy_rcv_in = USB_EXTPHY_RCV_IDX,
	.extphy_oen_out = USB_EXTPHY_OEN_IDX,
	.extphy_vpo_out = USB_EXTPHY_VPO_IDX,
	.extphy_vmo_out = USB_EXTPHY_VMO_IDX,
	.extphy_suspend_in = USB_EXTPHY_SUSPND_IDX,
	.extphy_speed_in = USB_EXTPHY_SPEED_IDX,
	.module = PERIPH_USB_MODULE
};
