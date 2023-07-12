/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define EFUSE_WRITE_OP_CODE 0x5a5a
#define EFUSE_READ_OP_CODE 0x5aa5

#define EFUSE_PKG_VERSION_ESP32C3       0 // QFN32
#define EFUSE_PKG_VERSION_ESP8685       1 // QFN28
#define EFUSE_PKG_VERSION_ESP32C3AZ     2 // QFN32
#define EFUSE_PKG_VERSION_ESP8686       3 // QFN24

#ifdef __cplusplus
}
#endif
