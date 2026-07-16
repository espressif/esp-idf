/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>

#include "esp_heap_caps.h"

#include "utils/mem.h"

void *bt_le_ext_malloc(size_t size)
{
#if CONFIG_BT_ISO_HEAP_EXTERNAL_MEMORY
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
    return malloc(size);
#endif
}

void *bt_le_ext_calloc(size_t n, size_t size)
{
#if CONFIG_BT_ISO_HEAP_EXTERNAL_MEMORY
    return heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
    return calloc(n, size);
#endif
}

/* ISO TX/RX data path: force internal DRAM. Plain malloc/calloc follow IDF's
 * default heap policy and could return PSRAM under CONFIG_SPIRAM_USE_MALLOC;
 * MALLOC_CAP_INTERNAL guarantees internal regardless. */
void *bt_le_int_malloc(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
}

void *bt_le_int_calloc(size_t n, size_t size)
{
    return heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
}
