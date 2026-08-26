/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "test_mipi_dsi_board.h"
#include "esp_ldo_regulator.h"

static esp_ldo_channel_handle_t ldo_phy_chan = NULL;

void test_bsp_enable_dsi_phy_power(void)
{
    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
    esp_ldo_channel_config_t ldo_cfg = {
        .chan_id = TEST_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = TEST_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    TEST_ESP_OK(esp_ldo_acquire_channel(&ldo_cfg, &ldo_phy_chan));
}

void test_bsp_disable_dsi_phy_power(void)
{
    TEST_ESP_OK(esp_ldo_release_channel(ldo_phy_chan));
}
