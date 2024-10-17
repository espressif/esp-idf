/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#define ESP_ROM_GET_CLK_FREQ                (1) // Get clk frequency with rom function `ets_get_cpu_frequency`
#define ESP_ROM_HAS_RVFPLIB                 (1) // ROM has the rvfplib
#define ESP_ROM_HAS_HAL_WDT                 (1) // ROM has the implementation of Watchdog HAL driver
#define ESP_ROM_HAS_HAL_SYSTIMER            (1) // ROM has the implementation of Systimer HAL driver
#define ESP_ROM_HAS_HEAP_TLSF               (1) // ROM has the implementation of the tlsf and multi-heap library
#define ESP_ROM_TLSF_CHECK_PATCH            (1) // ROM does not contain the patch of tlsf_check_pool()
#define ESP_ROM_MULTI_HEAP_WALK_PATCH       (1) // ROM does not contain the patch of multi_heap_walk()
#define ESP_ROM_HAS_LAYOUT_TABLE            (1) // ROM has the layout table
#define ESP_ROM_HAS_SPI_FLASH               (1) // ROM has the implementation of SPI Flash driver
#define ESP_ROM_WITHOUT_REGI2C              (1) // ROM has no regi2c APIs       TODO: IDF-10110 need refactor
#define ESP_ROM_HAS_NEWLIB                  (1) // ROM has newlib (at least parts of it) functions included
#define ESP_ROM_HAS_NEWLIB_NANO_FORMAT      (1) // ROM has the newlib nano version of formatting functions
#define ESP_ROM_HAS_NEWLIB_NANO_PRINTF_FLOAT_BUG  (1) // ROM has the printf float bug with newlib nano version
#define ESP_ROM_HAS_VERSION                 (1) // ROM has version/eco information
#define ESP_ROM_WDT_INIT_PATCH              (1) // ROM version does not configure the clock
#define ESP_ROM_RAM_APP_NEEDS_MMU_INIT      (1) // ROM doesn't init cache MMU when it's a RAM APP, needs MMU hal to init
#define ESP_ROM_HAS_SW_FLOAT                (1) // ROM has libgcc software floating point emulation functions
#define ESP_ROM_USB_OTG_NUM                 (-1) // No USB_OTG CDC in the ROM, set -1 for Kconfig usage.
#define ESP_ROM_HAS_OUTPUT_PUTC_FUNC        (1) // ROM has esp_rom_output_putc (or ets_write_char_uart)
#define ESP_ROM_CACHE_WB_INVLD_LOW_RANGE    (1) // ROM `Cache_WriteBack_Addr` and `Cache_Invalidate_Addr` can only access low vaddr parts
