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
