/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "soc/usb_periph.h"

/*
Note: These IO pins are deprecated. When connecting USB OTG to an external FSLS
PHY, the FSLS Serial Interface signals can be routed to any GPIO via the GPIO
matrix. Thus, this mapping of signals to IO pins is meaningless.

Todo: Remove in IDF v6.0 (IDF-9029)
*/
const usb_iopin_dsc_t usb_periph_iopins[] = {
    {USBPHY_VP_NUM, USB_EXTPHY_VP_IDX, 0, 1},
    {USBPHY_VM_NUM, USB_EXTPHY_VM_IDX, 0, 1},
    {USBPHY_RCV_NUM, USB_EXTPHY_RCV_IDX, 0, 1},
    {USBPHY_OEN_NUM, USB_EXTPHY_OEN_IDX, 1, 1},
    {USBPHY_VPO_NUM, USB_EXTPHY_VPO_IDX, 1, 1},
    {USBPHY_VMO_NUM, USB_EXTPHY_VMO_IDX, 1, 1},
    {GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_IDDIG_IN_IDX, 0, 0},     //connected connector is mini-B
    //connected connector is mini-B
    {GPIO_MATRIX_CONST_ONE_INPUT, USB_SRP_BVALID_IN_IDX, 0, 0},    //HIGH to force USB device mode
    {GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_VBUSVALID_IN_IDX, 0, 0}, //receiving a valid Vbus from host
    {GPIO_MATRIX_CONST_ZERO_INPUT, USB_OTG_AVALID_IN_IDX, 0, 0},
    {-1, -1, 0, 0}
};
