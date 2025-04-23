/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forces a string to be removed from the final binary but keeps it in the ELF file
#define ESP_LOG_NOLOAD_STR(str) (__extension__({static const NOLOAD_ATTR char __f[] = (str); (const char *)&__f;}))

#ifdef __cplusplus
}
#endif
