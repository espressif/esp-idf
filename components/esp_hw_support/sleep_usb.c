/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_private/sleep_usb.h"
#include "esp_attr.h"

#if SOC_USB_OTG_SUPPORTED && SOC_PM_SUPPORT_CNNT_PD
#include "hal/usb_utmi_ll.h"
#include "hal/usb_dwc_ll.h"

static bool s_usb_utmi_bus_clock_state, s_usb_utmi_stoppclk_state;

void sleep_usb_otg_phy_backup_and_disable(void)
{
    s_usb_utmi_bus_clock_state = _usb_utmi_ll_bus_clock_is_enabled();
    if (!s_usb_utmi_bus_clock_state) {
        _usb_utmi_ll_enable_bus_clock(true);
    }
    s_usb_utmi_stoppclk_state = usb_dwc_ll_get_stoppclk_st(&USB_DWC_HS);
    usb_dwc_ll_set_stoppclk(&USB_DWC_HS, true);
}

void sleep_usb_otg_phy_restore(void)
{
    _usb_utmi_ll_enable_bus_clock(true);
    usb_dwc_ll_set_stoppclk(&USB_DWC_HS, s_usb_utmi_stoppclk_state);
    if (!s_usb_utmi_bus_clock_state) {
        _usb_utmi_ll_enable_bus_clock(false);
    }
}
#endif
