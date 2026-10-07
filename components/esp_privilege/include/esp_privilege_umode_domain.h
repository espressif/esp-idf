/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_privilege_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The domain code can read the mapped memory. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_READ    1u
/** The domain code can write the mapped memory. Requires ESP_PRIVILEGE_UMODE_DOMAIN_READ. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_WRITE   2u
/** The domain code can execute the mapped memory. Cannot be combined with ESP_PRIVILEGE_UMODE_DOMAIN_WRITE. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE 4u

/** Page size. Virtual addresses, and the addresses and sizes of mapped memory, are multiples of it. */
#define ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE 4096u

#ifdef __cplusplus
}
#endif
