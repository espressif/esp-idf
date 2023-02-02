/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_ROM_HAS_CRC_LE              (1) // ROM CRC library supports Little Endian
#define ESP_ROM_HAS_CRC_BE              (1) // ROM CRC library supports Big Endian
#define ESP_ROM_HAS_MZ_CRC32            (1) // ROM has mz_crc32 function
#define ESP_ROM_HAS_JPEG_DECODE         (1) // ROM has JPEG decode library
#define ESP_ROM_HAS_UART_BUF_SWITCH     (1) // ROM has exported the uart buffer switch function
#define ESP_ROM_NEEDS_SWSETUP_WORKAROUND    (1) // ROM uses 32-bit time_t. A workaround is required to prevent printf functions from crashing
#define ESP_ROM_HAS_NEWLIB_NANO_FORMAT  (1) // ROM has the newlib nano version of formatting functions
