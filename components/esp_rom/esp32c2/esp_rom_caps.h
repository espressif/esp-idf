/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_ROM_HAS_CRC_LE                  (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_CRC_BE                  (1) // ROM CRC library supports Big Endian
#define ESP_ROM_UART_CLK_IS_XTAL            (1) // UART clock source is selected to XTAL in ROM
#define ESP_ROM_HAS_RETARGETABLE_LOCKING    (1) // ROM was built with retargetable locking
#define ESP_ROM_GET_CLK_FREQ                (1) // Get clk frequency with rom function `ets_get_cpu_frequency`
#define ESP_ROM_HAS_RVFPLIB                 (1) // ROM has the rvfplib
#define ESP_ROM_HAS_HAL_WDT                 (1) // ROM has the implementation of Watchdog HAL driver
#define ESP_ROM_HAS_HAL_SYSTIMER            (1) // ROM has the implementation of Systimer HAL driver
#define ESP_ROM_HAS_HEAP_TLSF               (1) // ROM has the implementation of the tlsf and multi-heap library
#define ESP_ROM_TLSF_CHECK_PATCH            (1) // ROM does not contain the patch of tlsf_check()
#define ESP_ROM_HAS_LAYOUT_TABLE            (1) // ROM has the layout table
#define ESP_ROM_HAS_SPI_FLASH               (1) // ROM has the implementation of SPI Flash driver
