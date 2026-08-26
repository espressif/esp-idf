/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * All functions presented here are stubs for the POSIX/Linux implementation of FReeRTOS.
 * They are meant to allow to compile, but they DO NOT return any meaningful value.
 */

#include <stdlib.h>
#include <stdint.h>
#include "esp_private/system_internal.h"
#include "esp_heap_caps.h"

// dummy, we should never get here on Linux
void esp_restart_noos_dig(void)
{
    abort();
}

uint32_t esp_get_free_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
}

uint32_t esp_get_free_internal_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

uint32_t esp_get_minimum_free_heap_size(void)
{
    return heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
}

const char *esp_get_idf_version(void)
{
    return IDF_VER;
}

void __attribute__((noreturn)) esp_system_abort(const char *details)
{
    exit(1);
}
