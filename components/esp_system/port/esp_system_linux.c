/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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

static const uint32_t MAGIC_HEAP_SIZE = 47000;

// dummy, we should never get here on Linux
void esp_restart_noos_dig(void)
{
    abort();
}

uint32_t esp_get_free_heap_size( void )
{
    return MAGIC_HEAP_SIZE;
}

uint32_t esp_get_free_internal_heap_size( void )
{
    return MAGIC_HEAP_SIZE;
}

uint32_t esp_get_minimum_free_heap_size( void )
{
    return MAGIC_HEAP_SIZE;
}

const char *esp_get_idf_version(void)
{
    return IDF_VER;
}

void __attribute__((noreturn)) esp_system_abort(const char *details)
{
    exit(1);
}
