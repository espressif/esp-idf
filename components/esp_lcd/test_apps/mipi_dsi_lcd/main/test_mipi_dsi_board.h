/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// FPS = 80000000/(40+140+40+800)/(4+16+16+1280) = 60Hz
#define MIPI_DSI_DPI_CLK_MHZ  80
#define MIPI_DSI_LCD_H_RES    800
#define MIPI_DSI_LCD_V_RES    1280
#define MIPI_DSI_LCD_HSYNC    40
#define MIPI_DSI_LCD_HBP      140
#define MIPI_DSI_LCD_HFP      40
#define MIPI_DSI_LCD_VSYNC    4
#define MIPI_DSI_LCD_VBP      16
#define MIPI_DSI_LCD_VFP      16

#define TEST_MIPI_DSI_PHY_PWR_LDO_CHAN 3
#define TEST_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV 2500

void test_bsp_enable_dsi_phy_power(void);
void test_bsp_disable_dsi_phy_power(void);

#ifdef __cplusplus
}
#endif
