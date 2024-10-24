/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// TODO: Remove this file when proper support of P4 PHYs is implemented IDF-11144
#include "hal/usb_utmi_hal.h"
#include "esp_private/usb_phy.h"

#include "soc/soc_caps.h"
#if SOC_RCC_IS_INDEPENDENT
#define USB_UTMI_BUS_CLK_ATOMIC()
#else
#include "esp_private/periph_ctrl.h"
#define USB_UTMI_BUS_CLK_ATOMIC()       PERIPH_RCC_ATOMIC()
#endif

static usb_utmi_hal_context_t s_utmi_hal_context;

esp_err_t usb_new_phy(const usb_phy_config_t *config, usb_phy_handle_t *handle_ret)
{
    USB_UTMI_BUS_CLK_ATOMIC() {
        usb_utmi_hal_init(&s_utmi_hal_context);
    }
    return ESP_OK;
}

esp_err_t usb_del_phy(usb_phy_handle_t handle)
{
    // Note: handle argument is not checked, because we don't have phy_handle for P4 yet
    USB_UTMI_BUS_CLK_ATOMIC() {
        usb_utmi_hal_disable();
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
