/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread.h"

#include "openthread/platform/memory.h"

#include <stdlib.h>

void *otPlatCAlloc(size_t num, size_t size)
{
    return calloc(num, size);
}

void otPlatFree(void *ptr)
{
    free(ptr);
}
