/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This is only a dummy USB PHY file for successful linking of ESP32-P4 target
// The internal HS PHY is enabled by default, therefore it needs no configuration
// TODO: Remove this file when proper support of P4 PHYs is implemented IDF-7323
#include "esp_private/usb_phy.h"

esp_err_t usb_new_phy(const usb_phy_config_t *config, usb_phy_handle_t *handle_ret)
{
    return ESP_OK;
}

esp_err_t usb_del_phy(usb_phy_handle_t handle)
{
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
