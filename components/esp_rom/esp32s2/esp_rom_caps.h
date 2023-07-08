/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_ROM_HAS_CRC_LE                  (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_MZ_CRC32                (1) // ROM has mz_crc32 function
#define ESP_ROM_HAS_UART_BUF_SWITCH         (1) // ROM has exported the uart buffer switch function
#define ESP_ROM_NEEDS_SWSETUP_WORKAROUND    (1) // ROM uses 32-bit time_t. A workaround is required to prevent printf functions from crashing
#define ESP_ROM_HAS_REGI2C_BUG              (1) // ROM has the regi2c bug
#define ESP_ROM_HAS_NEWLIB_NANO_FORMAT      (1) // ROM has the newlib nano version of formatting functions
#define ESP_ROM_USB_OTG_NUM                 (3) // The serial port ID (UART, USB, ...) of USB_OTG CDC in the ROM.
#define ESP_ROM_HAS_FLASH_COUNT_PAGES_BUG   (1) // ROM api Cache_Count_Flash_Pages will return unexpected value
