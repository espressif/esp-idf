/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_private/sleep_usb.h"

#if SOC_USB_OTG_SUPPORTED && SOC_PM_SUPPORT_CNNT_PD
#include "hal/usb_utmi_ll.h"
#include "hal/usb_dwc_ll.h"

static bool s_usb_utmi_bus_clock_state, s_usb_utmi_stoppclk_state, s_usb_dwc_bvalid_override;

void sleep_usb_otg_phy_backup_and_disable(void)
{
    s_usb_utmi_bus_clock_state = _usb_utmi_ll_bus_clock_is_enabled();
    if (!s_usb_utmi_bus_clock_state) {
        _usb_utmi_ll_enable_bus_clock(true);
    }
    usb_dwc_dev_t *hw = USB_DWC_LL_GET_HW(0);
    // Forcing BVALID low to ignore the hardware-detected VBUS BVALID signal  to suppress USB leakage.
    s_usb_dwc_bvalid_override = usb_dwc_ll_get_bvalid_override(hw);
    usb_dwc_ll_enable_bvalid_override(hw, true);
    s_usb_utmi_stoppclk_state = usb_dwc_ll_get_stoppclk_st(hw);
    usb_dwc_ll_set_stoppclk(hw, true);
}

void sleep_usb_otg_phy_restore(void)
{
    _usb_utmi_ll_enable_bus_clock(true);
    usb_dwc_dev_t *hw = USB_DWC_LL_GET_HW(0);
    usb_dwc_ll_enable_bvalid_override(hw, s_usb_dwc_bvalid_override);
    usb_dwc_ll_set_stoppclk(hw, s_usb_utmi_stoppclk_state);
    if (!s_usb_utmi_bus_clock_state) {
        _usb_utmi_ll_enable_bus_clock(false);
    }
}
#endif
