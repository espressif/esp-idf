/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_sig_map.h"
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

static const usb_otg_signal_conn_t otg_signals = {
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

static const usb_internal_phy_io_t internal_phy_io = {
    .dp = 20,
    .dm = 19,
};

/* --------------------------------- Public --------------------------------- */

const usb_dwc_info_t usb_dwc_info = {
    .controllers = {
        [0] = {
            .fsls_signals = &fsls_signals,
            .otg_signals = &otg_signals,
            .internal_phy_io = &internal_phy_io,
            .supported_phys = USB_PHY_INST_FSLS_INTERN_0,
            .irq = ETS_USB_INTR_SOURCE,
            .irq_2nd_cpu = -1,
        },
    },
};
