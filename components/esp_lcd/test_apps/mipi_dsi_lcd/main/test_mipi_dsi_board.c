/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "test_mipi_dsi_board.h"
#include "esp_private/esp_ldo.h"

static esp_ldo_unit_handle_t phy_pwr_unit = NULL;

void test_bsp_enable_dsi_phy_power(void)
{
    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
    esp_ldo_unit_init_cfg_t ldo_cfg = {
        .unit_id = TEST_MIPI_DSI_PHY_PWR_LDO_UNIT,
        .cfg = {
            .voltage_mv = TEST_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
        },
    };
    TEST_ESP_OK(esp_ldo_init_unit(&ldo_cfg, &phy_pwr_unit));
    TEST_ESP_OK(esp_ldo_enable_unit(phy_pwr_unit));
}

void test_bsp_disable_dsi_phy_power(void)
{
    TEST_ESP_OK(esp_ldo_disable_unit(phy_pwr_unit));
    TEST_ESP_OK(esp_ldo_deinit_unit(phy_pwr_unit));
}
