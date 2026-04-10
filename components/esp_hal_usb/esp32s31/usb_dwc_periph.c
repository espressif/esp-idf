/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "soc/interrupts.h"
#include "soc/usb_periph.h"

/* --------------------------------- Public --------------------------------- */

const usb_dwc_info_t usb_dwc_info = {
    .controllers = {
        [0] = {
            .fsls_signals = NULL,
            .otg_signals = NULL,
            .internal_phy_io = NULL, // HS PHY is not mapped to any GPIO
            .supported_phys = USB_PHY_INST_UTMI_0,
            .irq = ETS_USB_OTGHS_INTR_SOURCE,
            .irq_2nd_cpu = ETS_USB_OTGHS_ENDP_MULTI_PROC_INTR_SOURCE,
        },
    },
};
