/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_rom_md5.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP MD5 driver operation context
 *
 * Wraps the ROM-supplied md5_context_t with an explicit is_active flag so
 * the driver can detect setup-twice / update-without-setup / use-after-abort
 * without relying on internal field values of the ROM struct.
 */
typedef struct {
    md5_context_t md5_ctx;
    bool is_active;
} esp_md5_hash_operation_t;

#ifdef __cplusplus
}
#endif
