/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "soc/interrupts.h"
#include "soc/gpio_sig_map.h"
#include "soc/usb_dwc_periph.h"
#include "soc/usb_dwc_struct.h"

/* -------------------------------- Private --------------------------------- */

static const usb_otg_signal_conn_t dwc_fs_otg_signals = {
    // Inputs
    .iddig          = USB_OTG11_IDDIG_PAD_IN_IDX,
    .avalid         = USB_OTG11_AVALID_PAD_IN_IDX,
    .bvalid         = USB_SRP_BVALID_PAD_IN_IDX,
    .vbusvalid      = USB_OTG11_VBUSVALID_PAD_IN_IDX,
    .sessend        = USB_SRP_SESSEND_PAD_IN_IDX,
    // Outputs
    .idpullup       = USB_OTG11_IDPULLUP_PAD_OUT_IDX,
    .dppulldown     = USB_OTG11_DPPULLDOWN_PAD_OUT_IDX,
    .dmpulldown     = USB_OTG11_DMPULLDOWN_PAD_OUT_IDX,
    .drvvbus        = USB_OTG11_DRVVBUS_PAD_OUT_IDX,
    .chrgvbus       = USB_SRP_CHRGVBUS_PAD_OUT_IDX,
    .dischrgvbus    = USB_SRP_DISCHRGVBUS_PAD_OUT_IDX,
};

static const usb_internal_phy_io_t internal_phy_io = {
    .dp = 27,
    .dm = 26,
};

/* --------------------------------- Public --------------------------------- */

const usb_dwc_info_t usb_dwc_info = {
    .controllers = {
        // High-Speed USB-DWC
        [0] = {
            .fsls_signals = NULL,
            .otg_signals = NULL,
            .internal_phy_io = NULL, // HS PHY is not mapped to any GPIO
            .supported_phys = USB_PHY_INST_UTMI_0,
            .irq = ETS_USB_OTG_INTR_SOURCE,
            .irq_2nd_cpu = ETS_USB_OTG_ENDP_MULTI_PROC_INTR_SOURCE,
        },
        // Full-Speed USB-DWC
        [1] = {
            .fsls_signals = NULL,
            .otg_signals = &dwc_fs_otg_signals,
            .internal_phy_io = &internal_phy_io,
            .supported_phys = USB_PHY_INST_FSLS_INTERN_0,
            .irq = ETS_USB_OTG11_CH0_INTR_SOURCE,
            .irq_2nd_cpu = -1,
        },
    },
};
