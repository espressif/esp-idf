/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
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
