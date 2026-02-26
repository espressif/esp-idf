/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "esp_cpu.h"

void *__getreent(void)
{
    /*
     * The linker script provides the basic _reent fields
     * used to access errno and stdin/stdout/stderr.
     *
     * Note: if code accesses other fields in struct _reent
     * that are not intended to be "public," data corruption may occur.
     */
    return esp_cpu_get_threadptr();
}
