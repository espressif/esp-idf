/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_ROM_HAS_CRC_LE                  (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_CRC_BE                  (1) // ROM CRC library supports Big Endian
#define ESP_ROM_HAS_JPEG_DECODE             (1) // ROM has JPEG decode library
#define ESP_ROM_UART_CLK_IS_XTAL            (1) // UART clock source is selected to XTAL in ROM
#define ESP_ROM_USB_SERIAL_DEVICE_NUM       (3) // UART uses USB_SERIAL_JTAG port in ROM.
#define ESP_ROM_HAS_RETARGETABLE_LOCKING    (1) // ROM was built with retargetable locking
#define ESP_ROM_HAS_ERASE_0_REGION_BUG      (1) // ROM has esp_flash_erase_region(size=0) bug
#define ESP_ROM_GET_CLK_FREQ                (1) // Get clk frequency with rom function `ets_get_cpu_frequency`
#define ESP_ROM_HAS_LAYOUT_TABLE            (1) // ROM has the layout table
#define ESP_ROM_HAS_ETS_PRINTF_BUG          (1) // ROM has ets_printf bug when disable the ROM log either by eFuse or RTC storage register
