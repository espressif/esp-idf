/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "soc/gpio_sig_map.h"
#include "soc/usb_periph.h"
#include "soc/interrupts.h"
#include "soc/usb_dwc_periph.h"

/* -------------------------------- Private --------------------------------- */

static const usb_fsls_serial_signal_conn_t fsls_signals = {
    // Inputs
    .rx_dp          = USB_EXTPHY_VP_IDX,
    .rx_dm          = USB_EXTPHY_VM_IDX,
    .rx_rcv         = USB_EXTPHY_RCV_IDX,
    // Outputs
    .suspend_n      = USB_EXTPHY_SUSPND_IDX,
    .tx_enable_n    = USB_EXTPHY_OEN_IDX,
    .tx_dp          = USB_EXTPHY_VPO_IDX,
    .tx_dm          = USB_EXTPHY_VMO_IDX,
    .fs_edge_sel    = USB_EXTPHY_SPEED_IDX,
};

static const usb_utmi_otg_signal_conn_t otg_signals = {
    // Inputs
    .iddig          = USB_OTG_IDDIG_IN_IDX,
    .avalid         = USB_OTG_AVALID_IN_IDX,
    .bvalid         = USB_SRP_BVALID_IN_IDX,
    .vbusvalid      = USB_OTG_VBUSVALID_IN_IDX,
    .sessend        = USB_SRP_SESSEND_IN_IDX,
    // Outputs
    .idpullup       = USB_OTG_IDPULLUP_IDX,
    .dppulldown     = USB_OTG_DPPULLDOWN_IDX,
    .dmpulldown     = USB_OTG_DMPULLDOWN_IDX,
    .drvvbus        = USB_OTG_DRVVBUS_IDX,
    .chrgvbus       = USB_SRP_CHRGVBUS_IDX,
    .dischrgvbus    = USB_SRP_DISCHRGVBUS_IDX,
};

/* --------------------------------- Public --------------------------------- */

const usb_dwc_info_t usb_dwc_info = {
    .controllers = {
        [0] = {
            .fsls_signals = &fsls_signals,
            .otg_signals = &otg_signals,
            .irq = ETS_USB_INTR_SOURCE,
            .irq_2nd_cpu = -1,
        },
    },
};

/* ------------------------------- Deprecated ------------------------------- */

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
