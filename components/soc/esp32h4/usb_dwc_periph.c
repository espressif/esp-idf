/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "soc/gpio_sig_map.h"
#include "soc/interrupts.h"
#include "soc/usb_periph.h"

/* -------------------------------- Private --------------------------------- */

static const usb_otg_signal_conn_t otg_signals = {
    // Inputs
    .iddig = USB_OTG_IDDIG_IN_IDX,
    .avalid = USB_OTG_AVALID_IN_IDX,
    .bvalid = USB_SRP_BVALID_IN_IDX,
    .vbusvalid = USB_OTG_VBUSVALID_IN_IDX,
    .sessend = USB_SRP_SESSEND_IN_IDX,
    // Outputs
    .idpullup = USB_OTG_IDPULLUP_OUT_IDX,
    .dppulldown = USB_OTG_DPPULLDOWN_OUT_IDX,
    .dmpulldown = USB_OTG_DMPULLDOWN_OUT_IDX,
    .drvvbus = USB_OTG_DRVVBUS_OUT_IDX,
    .chrgvbus = USB_SRP_CHRGVBUS_OUT_IDX,
    .dischrgvbus = USB_SRP_DISCHRGVBUS_OUT_IDX,
};

static const usb_internal_phy_io_t internal_phy_io = {
    .dp = 22,
    .dm = 21,
};

/* --------------------------------- Public --------------------------------- */

const usb_dwc_info_t usb_dwc_info = {
    .controllers = {
        [0] = {
            .fsls_signals = NULL,
            .otg_signals = &otg_signals,
            .internal_phy_io = &internal_phy_io,
            .supported_phys = USB_PHY_INST_FSLS_INTERN_0,
            .irq = ETS_USB_OTG11_INTR_SOURCE,
            .irq_2nd_cpu = -1,
        },
    },
};
