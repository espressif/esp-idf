/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Refresh Rate = 48000000/(10+120+120+1024)/(1+20+10+600) = 60Hz
#define MIPI_DSI_DPI_CLK_MHZ  48
#define MIPI_DSI_LCD_H_RES    1024
#define MIPI_DSI_LCD_V_RES    600
#define MIPI_DSI_LCD_HSYNC    10
#define MIPI_DSI_LCD_HBP      120
#define MIPI_DSI_LCD_HFP      120
#define MIPI_DSI_LCD_VSYNC    1
#define MIPI_DSI_LCD_VBP      20
#define MIPI_DSI_LCD_VFP      20

#define TEST_MIPI_DSI_PHY_PWR_LDO_CHAN 3
#define TEST_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV 2500

void test_bsp_enable_dsi_phy_power(void);
void test_bsp_disable_dsi_phy_power(void);

#ifdef __cplusplus
}
#endif
