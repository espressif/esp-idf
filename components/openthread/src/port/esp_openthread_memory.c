/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread.h"
#include "esp_openthread_platform.h"

#include "openthread/platform/memory.h"

#include <stdlib.h>

void *otPlatCAlloc(size_t num, size_t size)
{
    return heap_caps_calloc(num, size, esp_openthread_get_alloc_caps());
}

void otPlatFree(void *ptr)
{
    heap_caps_free(ptr);
}
