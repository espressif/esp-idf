/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_ROM_HAS_CRC_LE                  (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_CRC_BE                  (1) // ROM CRC library supports Big Endian
#define ESP_ROM_HAS_MZ_CRC32                (1) // ROM has mz_crc32 function
#define ESP_ROM_HAS_JPEG_DECODE             (1) // ROM has JPEG decode library
#define ESP_ROM_UART_CLK_IS_XTAL            (1) // UART clock source is selected to XTAL in ROM
#define ESP_ROM_USB_SERIAL_DEVICE_NUM       (3) // UART uses USB_SERIAL_JTAG port in ROM.
#define ESP_ROM_HAS_RETARGETABLE_LOCKING    (1) // ROM was built with retargetable locking
#define ESP_ROM_HAS_ERASE_0_REGION_BUG      (1) // ROM has esp_flash_erase_region(size=0) bug
#define ESP_ROM_HAS_ENCRYPTED_WRITES_USING_LEGACY_DRV     (1) // `esp_flash_write_encrypted` in ROM has bug.
#define ESP_ROM_GET_CLK_FREQ                (1) // Get clk frequency with rom function `ets_get_cpu_frequency`
#define ESP_ROM_NEEDS_SWSETUP_WORKAROUND    (1) // ROM uses 32-bit time_t. A workaround is required to prevent printf functions from crashing
#define ESP_ROM_HAS_LAYOUT_TABLE            (1) // ROM has the layout table
#define ESP_ROM_HAS_SPI_FLASH               (1) // ROM has the implementation of SPI Flash driver
#define ESP_ROM_HAS_ETS_PRINTF_BUG          (1) // ROM has ets_printf bug when disable the ROM log either by eFuse or RTC storage register
#define ESP_ROM_HAS_NEWLIB_NANO_FORMAT      (1) // ROM has the newlib nano version of formatting functions
#define ESP_ROM_NEEDS_SET_CACHE_MMU_SIZE    (1) // ROM needs to set cache MMU size according to instruction and rodata for flash mmap
#define ESP_ROM_RAM_APP_NEEDS_MMU_INIT      (1) // ROM doesn't init cache MMU when it's a RAM APP, needs MMU hal to init
