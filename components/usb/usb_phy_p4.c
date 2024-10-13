/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// TODO: Refactor during the IDF-9198
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "soc/usb_dwc_cfg.h"
#include "hal/usb_utmi_ll.h" // We don't have usb_utmi_hal yet
#include "esp_private/periph_ctrl.h"
// TODO: Remove this file when proper support of P4 PHYs is implemented IDF-7323
#include "esp_private/usb_phy.h"
#include "esp_private/periph_ctrl.h"

#if SOC_RCC_IS_INDEPENDENT
#define USB_UTMI_BUS_CLK_ATOMIC()
#else
#define USB_UTMI_BUS_CLK_ATOMIC()       PERIPH_RCC_ATOMIC()
#endif

esp_err_t usb_new_phy(const usb_phy_config_t *config, usb_phy_handle_t *handle_ret)
{
#if (OTG_HSPHY_INTERFACE != 0)
#if CONFIG_IDF_TARGET_ESP32P4
    USB_UTMI_BUS_CLK_ATOMIC() {
        usb_utmi_ll_enable_bus_clock(true);
        usb_utmi_ll_reset_register();
    }
    /*
    Additional setting to solve missing DCONN event on ESP32P4 (IDF-9953).

    Note: On ESP32P4, the HP_SYSTEM_OTG_SUSPENDM is not connected to 1 by hardware.
    For correct detection of the device detaching, internal signal should be set to 1 by the software.
    */
    usb_utmi_ll_enable_precise_detection(true);
    usb_utmi_ll_configure_ls(&USB_UTMI, true);
#endif // CONFIG_IDF_TARGET_ESP32P4
#endif // (OTG_HSPHY_INTERFACE != 0)
    return ESP_OK;
}

esp_err_t usb_del_phy(usb_phy_handle_t handle)
{
    USB_UTMI_BUS_CLK_ATOMIC() {
        usb_utmi_ll_enable_bus_clock(false);
    }
    return ESP_OK;
}

esp_err_t usb_phy_get_phy_status(usb_phy_target_t target, usb_phy_status_t *status)
{
    return ESP_OK;
}

esp_err_t usb_phy_action(usb_phy_handle_t handle, usb_phy_action_t action)
{
    return ESP_OK;
}
