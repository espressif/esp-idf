/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
// Please update the following configuration according to your HardWare spec
#define RCV_HOST            SPI2_HOST

#define GPIO_MOSI           CONFIG_BT_BLE_LOG_SPI_OUT_MOSI_IO_NUM
#define GPIO_MISO           -1
#define GPIO_SCLK           CONFIG_BT_BLE_LOG_SPI_OUT_SCLK_IO_NUM
#define GPIO_CS             CONFIG_BT_BLE_LOG_SPI_OUT_CS_IO_NUM
