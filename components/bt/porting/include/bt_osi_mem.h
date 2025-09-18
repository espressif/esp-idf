/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"

void *bt_osi_mem_malloc(size_t size);

void *bt_osi_mem_calloc(size_t n, size_t size);

void *bt_osi_mem_malloc_internal(size_t size);

void *bt_osi_mem_calloc_internal(size_t n, size_t size);

void bt_osi_mem_free(void *ptr);

void bt_osi_mem_free_internal(void *ptr);
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
void bt_osi_mem_count_limit_set(uint16_t count_limit);
#endif // CONFIG_BT_LE_MEM_CHECK_ENABLED

#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
size_t bt_osi_mem_internal_used_size_get(void);
size_t bt_osi_mem_used_size_get(void);
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
